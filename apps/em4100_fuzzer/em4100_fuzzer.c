/*
 * EM4100 ID Fuzzer  --  Dognet Technologies
 *
 * Emulatore sequenziale di ID EM4100 (125 kHz) per physical access pentest
 * AUTORIZZATI. Tiene fisso il facility code a 8 bit e scorre il card-id a
 * 32 bit (candidate = base + direction*i, i in [0,range)), emulando ciascun
 * candidato per un dwell-time configurabile. Serve a sondare se un lettore
 * concede l'accesso a badge "vicini" emessi sequenzialmente nello stesso sito.
 *
 * Teoria del frame EM4100: 64 bit Manchester @125 kHz (9 header + 10 nibble
 * con parita' di riga + 4 parita' di colonna + stop). Il payload utile sono
 * 40 bit = 5 byte: [facility][id31..id0]. Il LFRFIDWorker gestisce
 * Manchester+parita' verso il campo RF; noi forniamo solo i 5 byte decodificati.
 *
 * --- Contratto del LFRFIDWorker (verificato sui sorgenti) ---
 *  - lfrfid_worker_emulate_start() fa furi_check(mode_index == Idle).
 *  - lfrfid_worker_stop() e' ASINCRONO: setta solo il flag StopMode.
 *  - Il loop di una modalita' (emulate) esce SOLO su StopMode
 *    (check_for_stop controlla solo quel flag), NON su StopThread.
 *  - lfrfid_worker_stop_thread() e' SINCRONO (furi_thread_join), ma il join
 *    si completa solo se la modalita' e' gia' uscita -> va preceduto da stop().
 * Quindi il re-arm sincrono e race-free e':
 *    stop() [StopMode: rompe il loop] -> stop_thread() [join] -> start_thread().
 *
 * Concorrenza: il modello (condiviso con la draw callback) e' protetto da
 * FuriMutex. Le operazioni sul worker (che includono un join bloccante) sono
 * eseguite FUORI dal mutex: la fase "logic" muta il modello sotto lock e
 * restituisce un "effect"; il main loop esegue l'effect senza lock. Il
 * FuriTimer si limita ad accodare un evento TICK (niente worker dal timer).
 */

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lfrfid/lfrfid_worker.h>
#include <lfrfid/protocols/lfrfid_protocols.h>
#include <toolbox/protocols/protocol_dict.h>

#define TAG "EM4100Fuzzer"

#define EM4100_DATA_SIZE (5u) // 40 bit decodificati
#define DWELL_MIN_MS (100u)
#define DWELL_MAX_MS (3000u)
#define DWELL_STEP_MS (100u)
#define RANGE_MIN (1u)
#define RANGE_MAX (5000u)

// Campi editabili, nell'ordine di scorrimento con <- / ->
typedef enum {
    FieldFacility = 0,
    FieldCardBase,
    FieldDwell,
    FieldRange,
    FieldDirection,
    FieldCount,
} FuzzerField;

typedef struct {
    uint8_t facility; // 8 bit facility/version, tenuto fisso durante lo sweep
    uint32_t card_base; // card-id 32 bit di partenza
    uint32_t range; // numero di candidati da emulare
    uint32_t index; // offset corrente nello sweep [0, range)
    uint32_t emitted; // contatore candidati gia' emulati (telemetria UI)
    uint16_t dwell_ms; // tempo di emulazione per candidato
    int8_t direction; // +1 (incrementa) / -1 (decrementa)
    FuzzerField field; // campo selezionato per l'editing
    bool running; // sweep attivo
} FuzzerModel;

typedef enum {
    EventTypeInput,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} FuzzerEvent;

// Azione sul worker decisa dalla "logic" (sotto mutex) ed eseguita dal main
// loop FUORI dal mutex.
typedef enum {
    EffectNone,
    EffectArm, // (ri)avvia l'emulazione su (facility, card)
    EffectStop, // ferma l'emulazione, worker -> Idle
} FuzzerEffect;

typedef struct {
    FuzzerEffect effect;
    uint8_t facility;
    uint32_t card;
    bool quit;
} FuzzerOutcome;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* queue;
    FuriMutex* mutex;
    FuriTimer* timer;

    ProtocolDict* dict;
    LFRFIDWorker* worker;

    FuzzerModel model;
} FuzzerApp;

/* ---- EM4100: serializzazione del payload a 5 byte (big-endian) ---- */
static void em4100_pack(uint8_t facility, uint32_t card, uint8_t out[EM4100_DATA_SIZE]) {
    out[0] = facility;
    out[1] = (uint8_t)(card >> 24);
    out[2] = (uint8_t)(card >> 16);
    out[3] = (uint8_t)(card >> 8);
    out[4] = (uint8_t)(card);
}

// Candidato corrente: base + direction*index, con wrap naturale a 32 bit.
static uint32_t fuzzer_current_card(const FuzzerModel* m) {
    return m->card_base + (uint32_t)((int32_t)m->direction * (int32_t)m->index);
}

/* =====================  EFFECTS (worker, FUORI dal mutex)  ===================== */

// Porta il worker a Idle in modo SINCRONO e race-free.
// stop() rompe il loop di modalita' (StopMode); stop_thread() fa il join;
// start_thread() rimette il thread in attesa con mode_index = Idle.
static void fuzzer_worker_reset_idle(FuzzerApp* app) {
    lfrfid_worker_stop(app->worker); // StopMode: fa uscire il loop di emulate
    lfrfid_worker_stop_thread(app->worker); // StopThread + join (sincrono)
    lfrfid_worker_start_thread(app->worker); // riparte -> Idle
}

// Re-arm: reset a Idle, carica il nuovo payload, avvia l'emulazione.
static void fuzzer_worker_arm(FuzzerApp* app, uint8_t facility, uint32_t card) {
    uint8_t data[EM4100_DATA_SIZE];
    em4100_pack(facility, card, data);
    fuzzer_worker_reset_idle(app);
    protocol_dict_set_data(app->dict, LFRFIDProtocolEM4100, data, EM4100_DATA_SIZE);
    lfrfid_worker_emulate_start(app->worker, LFRFIDProtocolEM4100);
}

static void fuzzer_apply_effect(FuzzerApp* app, const FuzzerOutcome* out) {
    switch(out->effect) {
    case EffectArm:
        fuzzer_worker_arm(app, out->facility, out->card);
        break;
    case EffectStop:
        fuzzer_worker_reset_idle(app);
        break;
    case EffectNone:
    default:
        break;
    }
}

/* =====================  LOGIC (modello, SOTTO mutex)  ===================== */

static FuzzerOutcome fuzzer_sweep_start(FuzzerApp* app) {
    FuzzerModel* m = &app->model;
    m->index = 0;
    m->emitted = 1;
    m->running = true;
    furi_timer_start(app->timer, furi_ms_to_ticks(m->dwell_ms));
    return (FuzzerOutcome){
        .effect = EffectArm, .facility = m->facility, .card = fuzzer_current_card(m)};
}

static FuzzerOutcome fuzzer_sweep_stop(FuzzerApp* app) {
    FuzzerModel* m = &app->model;
    m->running = false;
    furi_timer_stop(app->timer);
    return (FuzzerOutcome){.effect = EffectStop};
}

// Avanza di un candidato; a fine sweep si ferma.
static FuzzerOutcome fuzzer_sweep_tick(FuzzerApp* app) {
    FuzzerModel* m = &app->model;
    if(!m->running) return (FuzzerOutcome){.effect = EffectNone};

    if(m->index + 1u >= m->range) {
        return fuzzer_sweep_stop(app);
    }
    m->index++;
    m->emitted++;
    return (FuzzerOutcome){
        .effect = EffectArm, .facility = m->facility, .card = fuzzer_current_card(m)};
}

/* ---- Editing dei campi (solo a sweep fermo) ---- */
static void fuzzer_edit(FuzzerModel* m, int dir) {
    switch(m->field) {
    case FieldFacility:
        m->facility = (uint8_t)((int)m->facility + dir); // wrap 8 bit
        break;
    case FieldCardBase:
        m->card_base += (uint32_t)dir; // wrap 32 bit
        break;
    case FieldDwell: {
        int32_t v = (int32_t)m->dwell_ms + dir * (int32_t)DWELL_STEP_MS;
        if(v < (int32_t)DWELL_MIN_MS) v = DWELL_MIN_MS;
        if(v > (int32_t)DWELL_MAX_MS) v = DWELL_MAX_MS;
        m->dwell_ms = (uint16_t)v;
        break;
    }
    case FieldRange: {
        int32_t v = (int32_t)m->range + dir;
        if(v < (int32_t)RANGE_MIN) v = RANGE_MIN;
        if(v > (int32_t)RANGE_MAX) v = RANGE_MAX;
        m->range = (uint32_t)v;
        break;
    }
    case FieldDirection:
        m->direction = (m->direction >= 0) ? -1 : 1;
        break;
    default:
        break;
    }
}

// Gestione input; muta il modello e restituisce l'effetto + eventuale quit.
static FuzzerOutcome fuzzer_handle_input(FuzzerApp* app, const InputEvent* input) {
    FuzzerModel* m = &app->model;

    if(input->key == InputKeyBack && input->type == InputTypeShort) {
        FuzzerOutcome out = m->running ? fuzzer_sweep_stop(app) :
                                         (FuzzerOutcome){.effect = EffectNone};
        out.quit = true;
        return out;
    }

    if(input->type != InputTypeShort && input->type != InputTypeRepeat) {
        return (FuzzerOutcome){.effect = EffectNone};
    }

    switch(input->key) {
    case InputKeyOk:
        if(input->type == InputTypeShort) {
            return m->running ? fuzzer_sweep_stop(app) : fuzzer_sweep_start(app);
        }
        break;
    case InputKeyLeft:
        if(!m->running) m->field = (FuzzerField)((m->field + FieldCount - 1) % FieldCount);
        break;
    case InputKeyRight:
        if(!m->running) m->field = (FuzzerField)((m->field + 1) % FieldCount);
        break;
    case InputKeyUp:
        if(!m->running) fuzzer_edit(m, +1);
        break;
    case InputKeyDown:
        if(!m->running) fuzzer_edit(m, -1);
        break;
    default:
        break;
    }
    return (FuzzerOutcome){.effect = EffectNone};
}

/* =====================  RENDERING  ===================== */

static void fuzzer_draw_line(Canvas* canvas, int y, bool selected, const char* text) {
    if(selected) {
        canvas_draw_str(canvas, 2, y, ">");
    }
    canvas_draw_str(canvas, 10, y, text);
}

static void fuzzer_draw_callback(Canvas* canvas, void* ctx) {
    FuzzerApp* app = ctx;
    furi_check(furi_mutex_acquire(app->mutex, FuriWaitForever) == FuriStatusOk);
    const FuzzerModel* m = &app->model;
    char buf[40];

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 9, "EM4100 ID Fuzzer");

    canvas_set_font(canvas, FontSecondary);

    snprintf(buf, sizeof(buf), "FC  0x%02X", m->facility);
    fuzzer_draw_line(canvas, 20, m->field == FieldFacility, buf);

    snprintf(
        buf,
        sizeof(buf),
        "ID  %08lX  %lu",
        (unsigned long)m->card_base,
        (unsigned long)m->card_base);
    fuzzer_draw_line(canvas, 29, m->field == FieldCardBase, buf);

    snprintf(buf, sizeof(buf), "Dwell  %u ms", (unsigned)m->dwell_ms);
    fuzzer_draw_line(canvas, 38, m->field == FieldDwell, buf);

    snprintf(buf, sizeof(buf), "Range  %lu", (unsigned long)m->range);
    fuzzer_draw_line(canvas, 47, m->field == FieldRange, buf);

    snprintf(buf, sizeof(buf), "Dir  %s", (m->direction >= 0) ? "+ up" : "- down");
    fuzzer_draw_line(canvas, 56, m->field == FieldDirection, buf);

    if(m->running) {
        snprintf(
            buf,
            sizeof(buf),
            "RUN %lu/%lu  %08lX",
            (unsigned long)(m->index + 1u),
            (unsigned long)m->range,
            (unsigned long)fuzzer_current_card(m));
    } else {
        snprintf(buf, sizeof(buf), "OK=run  <>field  ^v edit");
    }
    canvas_draw_str(canvas, 2, 64, buf);

    furi_mutex_release(app->mutex);
}

static void fuzzer_input_callback(InputEvent* input_event, void* ctx) {
    FuzzerApp* app = ctx;
    FuzzerEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(app->queue, &event, FuriWaitForever);
}

static void fuzzer_timer_callback(void* ctx) {
    FuzzerApp* app = ctx;
    FuzzerEvent event = {.type = EventTypeTick};
    furi_message_queue_put(app->queue, &event, 0); // non bloccare nel contesto timer
}

/* =====================  ALLOC / FREE  ===================== */

static FuzzerApp* fuzzer_app_alloc(void) {
    FuzzerApp* app = malloc(sizeof(FuzzerApp));

    app->model = (FuzzerModel){
        .facility = 0x00,
        .card_base = 0x00000001,
        .range = 100,
        .index = 0,
        .emitted = 0,
        .dwell_ms = 400,
        .direction = +1,
        .field = FieldCardBase,
        .running = false,
    };

    app->queue = furi_message_queue_alloc(8, sizeof(FuzzerEvent));
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->timer = furi_timer_alloc(fuzzer_timer_callback, FuriTimerTypePeriodic, app);

    // LF RFID: dizionario protocolli + worker su thread dedicato (resta Idle).
    app->dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    app->worker = lfrfid_worker_alloc(app->dict);
    lfrfid_worker_start_thread(app->worker);

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, fuzzer_draw_callback, app);
    view_port_input_callback_set(app->view_port, fuzzer_input_callback, app);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

static void fuzzer_app_free(FuzzerApp* app) {
    furi_timer_stop(app->timer);
    // stop() (StopMode) prima di stop_thread(): se il worker stesse emulando,
    // garantisce che il loop di modalita' esca e il join si completi.
    lfrfid_worker_stop(app->worker);
    lfrfid_worker_stop_thread(app->worker);
    lfrfid_worker_free(app->worker);
    protocol_dict_free(app->dict);

    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(app->view_port);

    furi_timer_free(app->timer);
    furi_mutex_free(app->mutex);
    furi_message_queue_free(app->queue);
    free(app);
}

/* =====================  ENTRY POINT  ===================== */

int32_t em4100_fuzzer_app(void* p) {
    UNUSED(p);
    FuzzerApp* app = fuzzer_app_alloc();
    FURI_LOG_I(TAG, "avviato (uso pentest autorizzato)");

    bool running = true;
    FuzzerEvent event;
    while(running) {
        if(furi_message_queue_get(app->queue, &event, FuriWaitForever) != FuriStatusOk) {
            continue;
        }

        // Fase LOGIC: muta il modello sotto lock, decide l'effetto.
        furi_check(furi_mutex_acquire(app->mutex, FuriWaitForever) == FuriStatusOk);
        FuzzerOutcome outcome = {.effect = EffectNone};
        if(event.type == EventTypeInput) {
            outcome = fuzzer_handle_input(app, &event.input);
        } else if(event.type == EventTypeTick) {
            outcome = fuzzer_sweep_tick(app);
        }
        furi_mutex_release(app->mutex);

        // Fase EFFECT: operazioni sul worker FUORI dal lock (join bloccante).
        fuzzer_apply_effect(app, &outcome);

        view_port_update(app->view_port);
        if(outcome.quit) running = false;
    }

    fuzzer_app_free(app);
    return 0;
}
