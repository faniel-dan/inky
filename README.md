# Inky: Spotify e-ink display

A little e-ink screen for my desk that shows what I'm playing on Spotify (song, artist, and dithered black-and-white album art) with buttons for previous, play/pause, and next. Built for the Hack Club Half Life warm-up (Tier 1).

**No soldering needed:** everything connects with jumper wires and a mini breadboard.

## How it works

```
Spotify app on Mac  <->  now_playing.py  <-- USB -->  ESP32  -->  e-ink screen
                                                       ^
                                                    3 buttons
```

- `computer/now_playing.py` asks the Spotify desktop app what's playing every 2 seconds. When the song changes, it downloads the album art, turns it into 128x128 black-and-white pixels, and sends everything to the ESP32 over USB.
- The ESP32 (`firmware/spotify_eink/`) draws it on the e-ink screen. When a button is pressed, it sends `PREV`, `PLAYPAUSE` or `NEXT` back to the Mac, and the script tells Spotify.
- This talks to the Spotify **app** instead of the Spotify Web API, so it works with a **free** Spotify account. The Web API has required Premium since March 2026.

## Bill of materials

| Part | Qty | Approx. cost | Notes |
|---|---|---|---|
| ESP32 dev board (DevKitC / DevKit V1) | 1 | $7 | Buy one with **headers already soldered** |
| Waveshare 2.9" e-Paper Module, black/white (V2) | 1 | $18 | Comes with a jumper-wire cable, no soldering |
| Tactile push buttons (6 mm) | 3 | $1 | |
| Mini breadboard + jumper wires (female-female, male-female) | 1 set | $4 | |
| **Total** | | **~$30** | Tier 1 limit is $30. If it's over, use the 2.13" display (~$13) instead |

USB cable: I already have one.

## Wiring

| E-ink module pin | ESP32 pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| DIN | GPIO 23 |
| CLK | GPIO 18 |
| CS | GPIO 5 |
| DC | GPIO 17 |
| RST | GPIO 16 |
| BUSY | GPIO 4 |

| Button | ESP32 pin | Other leg |
|---|---|---|
| Previous | GPIO 32 | GND |
| Play/pause | GPIO 33 | GND |
| Next | GPIO 25 | GND |

## Setup

1. **ESP32:** in Arduino IDE, install the ESP32 boards package plus the **GxEPD2** and **Adafruit GFX** libraries. Open `firmware/spotify_eink/spotify_eink.ino` and upload it.
2. **Mac:**
   ```
   cd computer
   pip3 install -r requirements.txt
   python3 now_playing.py
   ```
   The first time, macOS asks to let Terminal control Spotify. Click **OK**.

## Folders

- `firmware/`: ESP32 code (Arduino)
- `computer/`: the Mac script
- `images/`: wiring diagram, screenshots, demo
- `JOURNAL.md`: design journal for Half Life
