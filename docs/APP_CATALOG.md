# Catalogo App — Flipper Zero / Momentum mntm-012

> Uso esclusivo in **penetration test fisici autorizzati** (Dognet Technologies).
> Ogni voce: **cosa fa · come si usa · scenario d'ingaggio**.
> Device: Flipper Zero "Ererl1y" — 262 app installate. Qui sono catalogate quelle
> rilevanti per l'assessment di accessi fisici. Le altre (Games/Media/Tools) sono
> raggiungibili da **Apps**.

---

## 🔑 125 kHz — LF RFID (badge prossimità low-frequency)

### `125 kHz RFID` (integrata)
- **Cosa fa:** legge/emula/scrive badge LF: EM4100/EM4102, HID Prox, Indala, AWID, Paradox, Pyramid, Jablotron, Noralsy, ecc. Scrive su carte riscrivibili **T5577/EM4305**.
- **Uso:** `Read` → avvicina al badge → `Save`. `Saved` → seleziona → `Emulate` (impersona) o `Write` (clona su T5577).
- **Scenario:** badge dipendente lasciato incustodito → `Read`, `Save`, poi `Write` su T5577 → tessera fisica clonata per test del tornello/porta.

### `EM4100 ID Fuzzer` (custom, `/ext/apps/RFID/em4100_fuzzer.fap`)
- **Cosa fa:** dato un ID EM4100 base (facility + card-id 32 bit), **emula ID sequenziali** (`base ± i`) con dwell-time configurabile.
- **Uso:** `←→` campo, `↑↓` valore, `OK` start/stop. Imposta FC+ID di un badge letto, `Range`, avvicina al lettore.
- **Scenario:** badge emessi in sequenza nel sito → sonda se il lettore concede l'accesso a ID vicini (altri dipendenti) senza possederne il badge fisico.

---

## 📶 13.56 MHz — NFC (smart card / badge high-frequency)

### `NFC` (integrata)
- **Cosa fa:** legge/emula/clona MIFARE Classic/Ultralight/DESFire, NTAG, ISO14443. Recupero chiavi MIFARE Classic (dictionary + nested), scrittura.
- **Uso:** `Read` → `Save`. Per MIFARE Classic protette: `Detect Reader` cattura nonces → vedi `mfkey`.
- **Scenario:** badge aziendale MIFARE Classic → lettura, recupero chiavi, emulazione/clone per test accesso.

### `Picopass` (`/ext/apps/NFC/picopass.fap`)
- **Cosa fa:** legge/emula/scrive **HID iClass / Picopass** (legacy e Elite). Recupero chiavi, write su carte iClass.
- **Uso:** `Read` la carta iClass → `Save` → `Emulate` o `Write`.
- **Scenario:** molti accessi corporate usano iClass legacy → clone della tessera per impersonare l'accesso.

### `mfkey` / `mfkey32` (`/ext/apps/NFC/mfkey.fap`)
- **Cosa fa:** recupera le **chiavi MIFARE Classic** dai nonces catturati (attacco mfkey32 / nested).
- **Uso:** prima `NFC → Detect Reader` (cattura nonces avvicinando il Flipper al lettore reale), poi `mfkey` calcola le chiavi → tornano in `NFC` per leggere/clonare.
- **Scenario:** non hai il badge ma hai accesso al **lettore** → catturi i nonces quando un dipendente passa, recuperi le chiavi, cloni.

### `nfc_magic` (`/ext/apps/NFC/nfc_magic.fap`)
- **Cosa fa:** scrive su **magic card** (Gen1a/Gen2/UFUID) incluso il blocco 0 (UID) — clone 1:1 di MIFARE Classic.
- **Uso:** leggi l'originale in `NFC`, poi `nfc_magic` → `Write` sulla magic card.
- **Scenario:** clone fisico fedele (UID incluso) di un badge MIFARE per i sistemi che validano l'UID.

### `Seader` (`/ext/apps/NFC/seader.fap`)
- **Cosa fa:** interagisce con **HID iClass SE/SEOS** via canale SAM/Secure (oltre il legacy di Picopass).
- **Scenario:** siti con iClass SEOS moderno → analisi/emulazione dove Picopass legacy non arriva.

---

## 📻 Sub-GHz (300–928 MHz)

### `Sub-GHz` (integrata)
- **Cosa fa:** cattura/replay/brute di telecomandi: cancelli, sbarre, garage, campanelli, alcuni sistemi d'allarme. Region-free su Momentum.
- **Uso:** `Read`/`Read RAW` → `Save` → `Send`. `Frequency Analyzer` per trovare la frequenza di un telecomando attivo.
- **Scenario:** sbarra/cancello del parcheggio perimetrale → cattura del telecomando di servizio → replay per accesso veicolare.

---

## 🔘 iButton (Dallas 1-Wire) — `iButton` (integrata)
- **Cosa fa:** legge/emula/scrive chiavi di contatto Dallas (DS1990), Cyfral, Metakom (citofoni/accessi).
- **Scenario:** chiave di contatto del citofono/portoneria → lettura ed emulazione per accesso androne.

---

## 🔴 Infrared — `Infrared` (integrata)
- **Cosa fa:** cattura/replay IR. Librerie universali (TV, AC) + **lettori IR di uscita / pulsanti REX** e alcuni controlli accessi.
- **Scenario:** pulsante di uscita IR ("request-to-exit") lato interno → in alcuni varchi attivabile da fuori attraverso fessure/vetri.

---

## ⌨️ BadUSB — `Bad KB` (integrata)
- **Cosa fa:** emula tastiera/mouse USB-HID, esegue script **Ducky** al collegamento. BLE-HID anche wireless.
- **Uso:** carica uno script `.txt` Ducky → collega a una postazione → esecuzione automatica.
- **Scenario:** workstation sbloccata e incustodita durante il giro → payload di raccolta/POC in pochi secondi.

---

## 📡 ESP32 / WiFi (devboard esterna, categoria GPIO/ESP)

### `WiFi Marauder` (`/ext/apps/GPIO/ESP/esp32_wifi_marauder.fap`)
- **Cosa fa:** front-end per ESP32 Marauder: scan AP/client, **deauth**, beacon spam, sniffing, wardriving.
- **Scenario:** ricognizione WiFi del perimetro, test resilienza rete ospiti, deauth controllato in scope.

### `Evil Portal` (`/ext/apps/GPIO/ESP/evil_portal.fap`)
- **Cosa fa:** captive portal fasullo su ESP32 per **credential harvesting** (test di awareness).
- **Scenario:** finto portale "WiFi Ospiti Azienda" → verifica se il personale inserisce credenziali.

### Altre ESP utili
- `ghost_esp.fap`, `wifi_scanner.fap`, `wardriver.fap`, `esp_flasher.fap` (flash firmware sull'ESP).

---

## 🔵 Bluetooth (categoria Bluetooth)
- `ble_spam.fap` — spam advertising BLE (test resilienza/awareness dispositivi).
- `findmy.fap` — emula/analizza tag tipo AirTag (tracking awareness).

---

## 🔐 `U2F` (integrata)
- **Cosa fa:** il Flipper agisce da token FIDO/U2F hardware. (Difensivo, non offensivo.)

---

### Note operative
- LF/NFC hanno **range di pochi cm**: contatto col lettore necessario.
- I lettori reali fanno **debounce/anti-passback**: dwell-time adeguati negli strumenti di emulazione.
- Tutto va eseguito **entro lo scope e l'autorizzazione scritta** dell'ingaggio.
