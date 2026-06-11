# Cheat-Sheet operativa — Flipper Zero (physical access pentest)

> 1 pagina. Riferimento rapido in ingaggio. Dettagli/scenari: [APP_CATALOG.md](APP_CATALOG.md).
> Uso esclusivo in test **autorizzati** (Dognet Technologies).

## LF 125 kHz — badge prossimità
| App | Cosa fa | Flusso lampo |
|---|---|---|
| **125 kHz RFID** | leggi/clona/emula EM4100, HID Prox, Indala | Read → Save → Write(T5577) / Emulate |
| **EM4100 Fuzzer** | emula ID EM4100 sequenziali (sonda vicini) | imposta FC+ID → OK = sweep |

## HF 13.56 MHz — NFC
| App | Cosa fa | Flusso lampo |
|---|---|---|
| **NFC** | MIFARE/NTAG/DESFire: leggi/clona/emula | Read → Save → Emulate |
| **mfkey** | recupera chiavi MIFARE Classic dai nonces | NFC → Detect Reader → mfkey |
| **nfc_magic** | scrive su magic card (UID incluso) | NFC Read orig → nfc_magic Write |
| **Picopass** | HID iClass/Picopass leggi/clona/scrivi | Read → Save → Emulate/Write |
| **Seader** | iClass SE/SEOS (oltre il legacy) | Read via SAM/Secure |

## Altri canali
| App | Cosa fa | Flusso lampo |
|---|---|---|
| **Sub-GHz** | telecomandi 300–928 MHz cattura/replay (cancelli, sbarre) | Read / Read RAW → Save → Send |
| **iButton** | chiavi contatto Dallas (citofoni) | Read → Save → Emulate |
| **Infrared** | IR telecomandi + pulsanti uscita REX | Read → Save → Send |
| **Bad KB** | BadUSB: tastiera USB esegue script Ducky | scegli payload → collega al PC |

## ESP32 WiFi (devboard)
| App | Cosa fa |
|---|---|
| **WiFi Marauder** | scan AP/client, deauth, sniffing, wardriving |
| **Evil Portal** | captive portal fasullo → credential harvesting |

## Promemoria operativi
- LF/NFC: **range pochi cm**, serve contatto col lettore.
- Lettori reali: **debounce/anti-passback** → dwell ≥ 300–400 ms in emulazione.
- **Bad KB**: imposta **layout IT** o i simboli si rompono; testa su finestra dedicata.
- Tutto **entro scope e autorizzazione** dell'ingaggio.
