# Catalogo completo app — Flipper Zero / Momentum mntm-012 (262 app)

> Inventario reale del device "Ererl1y" (fonte: `tools/device_apps.txt`).
> Per ogni app: **cosa fa** + **scenario** (ottica physical access / red team).
> ⚠️ = funzione non confermata al 100%, da verificare. Uso solo in test **autorizzati**.
> Rapido: [CHEATSHEET.md](CHEATSHEET.md) · core con dettaglio: [APP_CATALOG.md](APP_CATALOG.md).

---

## 📶 NFC — 13.56 MHz (20)

| App | Cosa fa | Scenario |
|---|---|---|
| **nfc** | App principale: leggi/emula/clona MIFARE, NTAG, DESFire, ISO14443 | Badge aziendale: lettura → emulazione/clone |
| **picopass** | HID iClass/Picopass (legacy+Elite): leggi/emula/scrivi | Accessi corporate iClass: clone tessera |
| **seader** | iClass SE/SEOS via SAM/Secure channel | Siti con iClass moderno oltre il legacy |
| **seos** | HID SEOS: lettura/analisi | Badge SEOS nuova generazione |
| **mfkey** / **mfkey32** | Recupero chiavi MIFARE Classic dai nonces | Hai il lettore non il badge: nonces → chiavi |
| **mfc_editor** | Editor dei dump MIFARE Classic | Modifica blocchi/valori prima del clone |
| **mifare_fuzzer** | Emula/fuzza UID MIFARE in sequenza | Sonda lettori che validano per UID |
| **nfc_magic** | Scrive su magic card (Gen1a/2/UFUID), UID incluso | Clone 1:1 di MIFARE Classic |
| **nfc_apdu_runner** | Invia APDU custom a smartcard | Probing smartcard/EMV in test |
| **metroflip** | Legge/decodifica carte trasporto pubblico | Analisi tessere metro/abbonamenti |
| **passy** | Legge ePassport/MRTD (ICAO 9303) con MRZ | Verifica leggibilità documenti |
| **saflip** | ⚠️ Saflok — analisi keycard alberghiere | Test serrature hotel |
| **ami_tool** / **weebo** | Amiibo: lettura/emulazione NTAG215 | Test NTAG / uso ludico |
| **nfc_maker** | Genera tag NFC (URL, WiFi, testo, contatti) | Crea tag per test/social engineering |
| **nfc_playlist** | Emula in sequenza più tag salvati | Prova rapida di una lista di badge |
| **nfc_login** | Login al PC via tag NFC | Test di accesso senza-password su workstation |
| **nfc_eink** | Scrive su tag e-ink NFC | Gestione cartellini e-paper NFC |
| **cyborg_detector** | ⚠️ Rileva impianti NFC sottocutanei | Screening anti-impianto |

## 🔑 RFID — 125 kHz LF + UHF (9)

| App | Cosa fa | Scenario |
|---|---|---|
| **lfrfid** | App principale LF: EM4100/HID Prox/Indala… leggi/emula/scrivi T5577 | Badge LF: clone su T5577 |
| **em4100_fuzzer** | *(nostra)* emula ID EM4100 sequenziali | Sonda badge "vicini" sul lettore |
| **em4100_generator** | Genera ID EM4100 validi | Crea badge di test |
| **fuzzer_rfid** | Fuzzer LF multi-protocollo | Brute/sweep su vari protocolli LF |
| **key_generator** | Genera chiavi/ID LF | Preparazione carte di test |
| **t5577_writer** | Scrive su carte T5577 | Clone singolo badge LF |
| **t5577_multiwriter** | Scrive in batch più T5577 | Produzione multipla di cloni di test |
| **simultaneous_rfid_reader** | ⚠️ Lettura multipla/UHF | Lettura tag in volume |
| **uhf_rfid** | UHF 860–960 MHz (richiede modulo): legge tag long-range | Tag logistici/varchi UHF a distanza |

## 📻 Sub-GHz — 300–928 MHz (20)

| App | Cosa fa | Scenario |
|---|---|---|
| **subghz_fap** | App principale Sub-GHz (Momentum) | Cattura/replay telecomandi cancelli/sbarre |
| **subghz_bruteforcer** | Brute force codici fissi (garage, barriere) | Apertura cancello a codice fisso |
| **subghz_remote** | Telecomando custom multi-pulsante | Telecomando "universale" da segnali salvati |
| **subghz_playlist** / **_creator** | Esegue sequenze di trasmissioni | Catena di replay programmata |
| **subghz_scheduler** | Trasmissioni temporizzate | Replay a orari prestabiliti |
| **protoview** | Visualizza/decodifica segnali raw | Reverse di un protocollo sconosciuto |
| **sub_analyzer** / **spectrum_analyzer** | Analisi spettro/segnale | Trova frequenza/canale di un telecomando |
| **radio_scanner** | Scanner di frequenze attive | Mappa l'attività RF del perimetro |
| **rolling_flaws** | Analisi rolling code (KeeLoq…) | Valuta debolezze di telecomandi rolling |
| **tpms** | Legge sensori pressione pneumatici | Tracking veicoli via TPMS |
| **weather_station** | Legge sensori meteo wireless | Ricognizione sensori 433/868 |
| **pocsag_pager** | Decodifica/invia POCSAG | Intercetta cercapersone (ospedali/industria) |
| **meal_pager** | Spoof pager ristorante | Test pager coda/locali |
| **esubghz_chat** | Chat cifrata su Sub-GHz | Comms di team in ingaggio |
| **tpms**, **weather_station** già sopra | — | — |
| **chief_cooker** | ⚠️ generatore/cooker di segnali | Da verificare |
| **hc11_modem** | Modulo radio HC-11 | Link dati seriale RF |
| **flipper_share** / **fmf_to_sub** | Trasferimento/conversione file .sub | Gestione libreria segnali |

## 🔴 Infrared (13)

| App | Cosa fa | Scenario |
|---|---|---|
| **infrared** | App principale IR: cattura/replay telecomandi | Pulsanti uscita REX, controlli IR |
| **ir_scope** | Analizza/visualizza segnali IR raw | Reverse di un protocollo IR ignoto |
| **xremote** / **flipper_xremote** / **ir_remote** | Telecomandi universali estesi | Controllo dispositivi IR sul posto |
| **ir_intervalometer** | Trigger temporizzato fotocamere | Scatti automatici |
| **lidar_emulator** | ⚠️ Emula segnali LiDAR IR | Test sensori di prossimità |
| **hitachi/midea/mitsubishi_ac_remote** | Telecomandi condizionatori | Controllo HVAC |
| **xbox_controller** | ⚠️ controllo IR Xbox | Uso ludico |
| **flame_rng** | ⚠️ RNG basato su sensore | Curiosità/test |

## 🔘 iButton — Dallas 1-Wire (3)

| App | Cosa fa | Scenario |
|---|---|---|
| **ibutton** | Leggi/emula/scrivi chiavi Dallas/Cyfral/Metakom | Chiavi citofono/androne: clone |
| **fuzzer_ibtn** | Fuzzer ID iButton (sweep) | Sonda lettori di contatto |
| **ibutton_converter** | Converte formati chiave iButton | Interoperabilità tra formati |

## 🔵 Bluetooth (5)

| App | Cosa fa | Scenario |
|---|---|---|
| **ble_spam** | Spam advertising BLE (popup iOS/Android/Win) | Test resilienza/awareness, disturbo |
| **findmy** | Emula beacon Apple FindMy/AirTag | Awareness tracking / stalking-test |
| **hid_ble** | Tastiera/mouse via BLE-HID | Input remoto wireless su PC sbloccato |
| **bt_trigger** | Trigger remoto via BT | Innesco azioni a distanza |
| **pc_monitor** | Mostra statistiche PC via BT | Monitor di sistema |

## 🔌 USB (8)

| App | Cosa fa | Scenario |
|---|---|---|
| **bad_kb** *(anche in Tools)* | BadUSB/BadBLE: esegue script Ducky | Esecuzione codice su workstation incustodita |
| **hid_usb** | Tastiera/mouse USB remoti | Input controllato su PC collegato |
| **bc_scanner** | Emula scanner barcode (USB HID) | Iniezione "scansioni" in gestionali |
| **mass_storage** | Espone la SD come chiavetta USB | Trasporto/consegna file payload |
| **u2f** | Token FIDO/U2F hardware | (Difensivo) 2FA |
| **usb_ccb** | ⚠️ USB chip card bridge/CCID | Lettore smartcard via USB |
| **xinput_controller** / **ldtoypad** / **portal_of_flipper** | Emula controller Xbox / toypad Lego / portale Skylanders | Uso ludico / emulazione periferiche |

## 🛠️ Tools (30)

| App | Cosa fa | Scenario |
|---|---|---|
| **bad_kb** | BadUSB (vedi USB) | Payload HID |
| **magspoof** *(vedi GPIO)* | — | — |
| **key_copier** | Da foto di una chiave fisica → profilo di taglio | Duplicazione chiavi meccaniche |
| **combo_cracker** | ⚠️ Brute combinazioni (lucchetti/PIN) | Attacco serrature a combinazione |
| **can_tools** | Bus CAN (automotive) via GPIO | Test reti veicolari/centraline |
| **dtmf_dolphin** | Genera toni DTMF | Phreaking/telefonia, citofoni a toni |
| **nfc_rfid_detector** | Rileva campi RFID/NFC di lettori | Trova/identifica lettori nascosti |
| **barcode_app** / **qrcode** | Genera barcode/QR | Badge/etichette fasulle, payload QR |
| **totp** | Generatore 2FA TOTP | (Difensivo) codici OTP |
| **passgen** | Generatore password | Utility |
| **flipbip** / **flip_crypt** | Wallet BIP39 / cifratura | Gestione segreti |
| **caesar_cipher** / **multi_converter** / **programmercalc** / **hex_editor** / **hex_viewer** / **text_viewer** / **iconedit** / **calculator** / **resistors** / **voltcalc_app** / **counter** / **cntdown_tim** / **tasks** / **flipp_pomodoro** / **nightstand** / **tone_gen** / **quac** / **upython** | Utility varie (editor, calcolatrici, convertitori, micropython, **quac**=launcher macro) | Supporto operativo / scripting on-device |

## ⚡ GPIO / hardware (86)

### Attacco / hardware hacking
| App | Cosa fa | Scenario |
|---|---|---|
| **magspoof** | Emula banda magnetica (carte mag) via bobina | Test lettori a striscia magnetica |
| **gpio_sentry_safe** | Apre casseforti SentrySafe via GPIO | Test casseforti elettroniche |
| **gpio_reader_a/b** / **gpio_explorer_app** / **gpio_controller** / **gpio** / **gpio_badge** | Lettura/controllo pin GPIO | Sniffing/pilotaggio segnali su PCB |
| **uart_terminal** / **flipper_spi_terminal** | Terminale UART / SPI | Console di debug su dispositivi target |
| **signal_generator** / **flipperscope** | Generatore segnali / oscilloscopio | Bench analisi segnali |
| **wire_tester** / **wii_ec_anal** | Test cavi / analisi Wii nunchuk | Diagnostica hardware |

### Debug / dump firmware
| App | Cosa fa | Scenario |
|---|---|---|
| **spi_mem_manager** | Dump/scrittura flash SPI (25xx) | **Estrazione firmware** da chip su PCB |
| **swd_probe** | Debug/dump ARM via SWD | Dump firmware/RAM da MCU ARM |
| **flip_tdi** | JTAG TDI | Accesso JTAG su target |
| **dap_link** | CMSIS-DAP debugger | Programmazione/debug MCU |
| **avr_isp** | Programmatore AVR ISP | Flash/dump microcontrollori AVR |
| **i2ctools** / **ina_meter** / **eth_troubleshooter** | I2C / corrente / ethernet | Diagnostica bus e alimentazione |

### NRF24 (2.4 GHz)
| App | Cosa fa | Scenario |
|---|---|---|
| **nrf24mousejacker** | Mousejack: inietta tasti in dongle wireless | HID injection su mouse/tastiere 2.4 GHz |
| **nrf24sniff** / **nrf24scan** / **nrf24channelscanner** | Sniff/scan canali NRF24 | Ricognizione periferiche wireless |
| **nrf24batch** | Operazioni batch NRF24 | Automazione test 2.4 GHz |

### ESP32/8266 (devboard WiFi)
| App | Cosa fa | Scenario |
|---|---|---|
| **esp32_wifi_marauder** / **mayhem_marauder** | Marauder: scan/deauth/sniff/wardriving | Attacco/ricognizione WiFi perimetro |
| **ghost_esp** | Suite attacchi ESP | Toolkit WiFi/BLE esteso |
| **evil_portal** | Captive portal fasullo | Credential harvesting (awareness) |
| **esp8266_deauther** / **esp8266_wifi_deauther_v2** | Deauth WiFi | Test resilienza rete |
| **wifi_scanner** / **wardriver** | Scan/mappatura WiFi+GPS | Wardriving del sito |
| **esp_flasher** | Flash firmware sull'ESP | Setup/aggiornamento devboard |
| **camera_suite** / **mayhem_camera** / **mayhem_nannycam** / **mayhem_motion** / **mayhem_qrcode** / **mayhem_morseflash** | ESP32-CAM: foto/motion/QR/morse | Sorveglianza/ricognizione visiva |
| **esp8266_ifttt_virtual_button** | Trigger IFTTT | Automazioni cloud |

### FlipperHTTP (devboard WiFi)
| App | Cosa fa | Scenario |
|---|---|---|
| **flip_wifi** | Gestione WiFi/credenziali devboard | Setup connettività |
| **web_crawler** / **flip_downloader** / **flip_library** | Crawler/download via WiFi | OSINT/raccolta dati on-device |
| **flip_social** / **flip_telegram** / **flip_trader** / **flip_weather** / **flip_map** / **flip_world** / **free_roam** | Social/telegram/borsa/meteo/mappe/gioco online | Connettività e utility (uso vario) |

### Sensori / GPS / moduli
| App | Cosa fa | Scenario |
|---|---|---|
| **gps_nmea** / **nearby_files** | GPS via UART / file per posizione | Geolocalizzazione, wardriving |
| **flipper_geiger** | Contatore Geiger (radiazioni) | Rilevazione radiazioni ambientali |
| **co2_logger** / **unitemp** / **lightmeter** / **uv_meter_as7331** / **hc_sr04** / **radar_scanner** | CO2/temp/luce/UV/distanza/radar | Misure ambientali/prossimità |
| **flipper_atomicdiceroller** | RNG da rumore | Entropia/curiosità |

### Altri GPIO / accessori
| App | Cosa fa | Scenario |
|---|---|---|
| **air_mouse** / **vgm_air_mouse** | Mouse a giroscopio | Input gestuale |
| **fm_radio** / **fmtx_app** | Ricevitore / trasmettitore FM | Ascolto/diffusione FM (test) |
| **flashlight** / **timelapse** / **longwave_clock** / **gpio_badge** | Torcia / timelapse / orologio LF / badge LED | Utility |
| **flipboard_keyboard** / **_signal** / **_blinky** / **_simon** | App per macro-tastiera FlipBoard | Macro hardware |
| **malveke_\*** | Tool cartucce/emulatore Game Boy (board MALVEKE) | Uso ludico/retro |
| **video_game_module_tool** | Tool per il VGM ufficiale | Gestione modulo video |
| **coleco** / **pokemon** / **blackhat** | ⚠️ emulatori/giochi vari | Uso ludico / da verificare |

## 🎮 Games (40) — intrattenimento / distrazione (social engineering)
`doom, tetris, snake20, arkanoid, air_arkanoid, asteroids, flappy_bird, pinball0, t_rex_runner, jetpack_joyride, zombiez, tanks, scorched_tanks, laser_tag, bomberduck, heap_defence, yapinvaders, geometry_flip, quadrastic, roots_of_life, gameoflife, 4inrow, chess, reversi, tictactoe, minesweeper_redux, solitaire, blackjack, videopoker, slotmachine, game_2048, game15, simon_says, color_guess, multi_dice, yatzee, rubiks_cube_scrambler, tama_p1 (tamagotchi), umpire_indicator, flipper_pong`
→ Scenario: nessuno operativo; utile al più come **copertura/distrazione** durante un pretexting.

## 🎵 Media (15) — audio/grafica
`music_player, wav_player, video_player, image_viewer, paint, etch, morse_code, metronome, bpm_tapper, tuning_fork, usb_midi, flizzer_tracker, text2sam (TTS), fmatrix, fzspground`
→ Scenario: prevalentemente ludico/utility; `morse_code`/`text2sam` talvolta utili in test segnali/audio.

## ⚙️ Settings / sistema (assets, 13)
`about, passport, momentum_app (impostazioni Momentum), js_app (runtime JavaScript), bt_settings, clock_settings, desktop_settings, expansion_settings, input_settings, notification_settings, power_settings, storage_settings, system_settings`
→ Non sono "app" operative: sono le voci di **Settings** + il runtime JS. `js_app` è interessante: esegue script `.js` on-device (automazioni).

---

### Highlight operativi (capability che ampliano il raggio d'azione)
- **magspoof** — emulazione banda magnetica (carte/badge mag).
- **gpio_sentry_safe** — apertura casseforti SentrySafe.
- **nrf24mousejacker** — HID injection su tastiere/mouse wireless 2.4 GHz.
- **spi_mem_manager / swd_probe / flip_tdi / avr_isp** — dump/flash firmware da chip e MCU.
- **saflip / passy / metroflip / seos** — keycard hotel, ePassport, trasporti, SEOS.
- **key_copier** — duplicazione chiavi meccaniche da foto.
- **can_tools** — bus CAN automotive.
- **uhf_rfid** — tag UHF a lunga distanza.
- **js_app / quac** — automazione on-device (script JS / launcher macro): base per le "pipeline" che valuteremo.
