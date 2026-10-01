#!/usr/bin/env python3
"""Sends what the Spotify app on this Mac is playing to the e-ink display over USB,
and turns the display's button presses into play/pause/skip.

Works with a free Spotify account because it talks to the Spotify desktop app,
not the Spotify Web API.

Usage: python3 now_playing.py [serial-port]
"""
import io
import subprocess
import sys
import time
import urllib.request

import serial
from serial.tools import list_ports
from PIL import Image

ART_SIZE = 128
POLL_SECONDS = 2

NOW_PLAYING_SCRIPT = '''
if application "Spotify" is running then
    tell application "Spotify"
        if player state is stopped then return ""
        set t to current track
        return (name of t) & linefeed & (artist of t) & linefeed & (artwork url of t) & linefeed & (player state as string)
    end tell
end if
return ""
'''

# Button name sent by the display -> Spotify AppleScript command
COMMANDS = {
    "PLAYPAUSE": "playpause",
    "NEXT": "next track",
    "PREV": "previous track",
}


def osascript(script):
    result = subprocess.run(["osascript", "-e", script], capture_output=True, text=True)
    return result.stdout.strip()


def now_playing():
    """Returns (title, artist, artwork_url, state), or None if nothing is playing."""
    lines = osascript(NOW_PLAYING_SCRIPT).split("\n")
    if len(lines) < 4:
        return None
    return tuple(lines[:4])


def album_art_hex(url):
    """Downloads the album art and turns it into 128x128 black-and-white pixels, as hex."""
    if not url:
        return ""
    with urllib.request.urlopen(url, timeout=10) as response:
        image = Image.open(io.BytesIO(response.read()))
    # Mode "1" uses dithering so photos still look like photos on e-ink
    bw = image.convert("L").resize((ART_SIZE, ART_SIZE)).convert("1")
    # Pillow uses 1 = white, the display draws 1 = black, so flip every bit
    return bytes(b ^ 0xFF for b in bw.tobytes()).hex()


def to_ascii(text):
    # The display's fonts only have plain ASCII characters
    return text.replace("\n", " ").encode("ascii", "replace").decode()


def find_port():
    for port in list_ports.comports():
        if any(name in port.device for name in ("usbserial", "usbmodem", "SLAB", "wchusbserial")):
            return port.device
    sys.exit("Couldn't find the display. Plug it in, or pass the port: python3 now_playing.py /dev/cu.xxx")


def send(ser, track, art_hex):
    title, artist, _, state = track
    for line in (f"T{to_ascii(title)}", f"A{to_ascii(artist)}", f"S{state}", f"I{art_hex}", "D"):
        ser.write((line + "\n").encode())


def main():
    port = sys.argv[1] if len(sys.argv) > 1 else find_port()
    ser = serial.Serial(port, 115200, timeout=0.1)
    print(f"Connected to {port}")
    time.sleep(2)  # the board restarts when the port opens

    last_sent = None
    art_cache = {"url": None, "hex": ""}
    last_poll = 0.0

    while True:
        message = ser.readline().decode(errors="ignore").strip()
        if message in COMMANDS:
            osascript(f'tell application "Spotify" to {COMMANDS[message]}')
            last_poll = 0.0  # update the screen right away
        elif message == "HELLO":
            last_sent = None  # display restarted, resend everything

        if time.time() - last_poll < POLL_SECONDS:
            continue
        last_poll = time.time()

        track = now_playing() or ("Nothing playing", "", "", "paused")
        if track == last_sent:
            continue

        url = track[2]
        if url != art_cache["url"]:
            try:
                art_cache["hex"] = album_art_hex(url)
            except Exception as error:
                print(f"Couldn't get album art: {error}")
                art_cache["hex"] = ""
            art_cache["url"] = url

        send(ser, track, art_cache["hex"])
        last_sent = track
        print(f"{track[3]}: {track[0]} - {track[1]}")


if __name__ == "__main__":
    main()
