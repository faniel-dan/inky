# Inky

Inky is a small e-ink display that sits on my desk and shows whatever I'm playing on Spotify: the song, the artist, and the album art in black and white. It has three buttons so I can go back, pause, or skip without switching windows.

I'm building it for the Hack Club Half Life warm-up (Tier 1). It's my first hardware project, so I kept it simple and made sure it doesn't need any soldering. Everything plugs together with jumper wires and a mini breadboard.

## Why it works with free Spotify

Most Spotify display projects I found online have the ESP32 talk to the Spotify Web API over Wi-Fi. That doesn't work for me anymore because Spotify started requiring Premium for developer access in March 2026, and I don't have Premium.

So I went a different way. A small Python script runs on my Mac and asks the Spotify app what's playing. When the song changes, it downloads the album art, converts it to 128x128 black-and-white pixels with dithering (so it still looks like a picture on e-ink), and sends everything to the ESP32 over USB. When I press a button, the ESP32 sends a message back and the script tells Spotify to pause or skip.

```
Spotify app on Mac  <->  now_playing.py  <-- USB -->  ESP32  -->  e-ink screen
                                                       ^
                                                    3 buttons
```

The tradeoff is that my Mac has to be on for Inky to update, but since it lives next to my computer anyway, that's fine.

## Parts

| Part | Qty | Approx. cost | Vendor |
|---|---|---|---|
| WeAct Studio 2.9" e-paper module, black/white | 1 | $11 | AliExpress (WeAct Studio store) |
| ESP32 dev board (38-pin, headers already soldered) | 1 | $5 | AliExpress |
| 6x6 mm tactile push buttons | 1 pack | $2 | AliExpress |
| Mini breadboard | 1 | $2 | AliExpress |
| Dupont jumper wires (female-female and male-female) | 1 set | $2 | AliExpress |
| **Total** | | **~$22 + tax** | Tier 1 limit is $30 |

I already have a USB cable. I first planned on the Waveshare 2.9" display, but it's $21.99 on Waveshare's store, which together with the ESP32 would have used up the whole $30 before buttons, wires, and tax. The WeAct display has the same size and resolution for about half the price, and it's supported by the same e-ink library.

## Wiring

| E-ink module | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA (DIN) | GPIO 23 |
| SCL (CLK) | GPIO 18 |
| CS | GPIO 5 |
| D/C | GPIO 17 |
| RES | GPIO 16 |
| BUSY | GPIO 4 |

Each button connects between its pin and GND. The ESP32's internal pull-up resistors handle the rest.

| Button | ESP32 |
|---|---|
| Previous | GPIO 32 |
| Play/pause | GPIO 33 |
| Next | GPIO 25 |

## Setup

1. Install the ESP32 boards package in Arduino IDE, along with the GxEPD2 and Adafruit GFX libraries. Then open `firmware/spotify_eink/spotify_eink.ino` and upload it.
2. On the Mac:
   ```
   cd computer
   pip3 install -r requirements.txt
   python3 now_playing.py
   ```
   The first time it runs, macOS asks whether Terminal can control Spotify. Click OK.

## What's in this repo

- `firmware/`: the ESP32 code (Arduino)
- `computer/`: the Python script that runs on my Mac
- `images/`: wiring diagram, screenshots, and photos once it's built

## Status

The design is done and I'm waiting on parts. I'll add photos and a demo once it's built.
