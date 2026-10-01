---
title: "Inky"
github: "https://github.com/faniel-dan/inky"
description: "A desk e-ink display that shows what I'm playing on Spotify, with buttons to skip and pause. Works with free Spotify."
created_at: "2026-10-01"
---

# October 1st: Picked my project (and got around Spotify's paywall)

Today was all about figuring out what to build for the warm-up and planning it out.

My first idea was a macropad, since that's basically the classic Hack Club starter project. But it needs soldering and I don't own a soldering iron, and Tier 1 says you shouldn't need tools you don't already have. So I switched to something I'd seen on social media: a little e-ink display that shows what you're playing on Spotify. I'm calling it **Inky**.

The plan was going to be the usual setup from projects like [Spotify-Diy-Thing](https://github.com/witnessmenow/Spotify-Diy-Thing) and [ESP32-Spotify](https://github.com/sleepy-dave/ESP32-Spotify), where the ESP32 connects to Wi-Fi and asks the Spotify Web API what's playing. Then I found out Spotify changed their rules in March 2026 and [developer access now requires Premium](https://techcrunch.com/2026/02/06/spotify-changes-developer-mode-api-to-require-premium-accounts-limits-test-users/). I don't have Premium, so that whole approach was dead.

Here's how I got around it:

- I found [esp32-spotify-tft](https://github.com/ravnhzl/esp32-spotify-tft), which gets the song from the computer over USB instead of from the internet
- The Spotify desktop app on Mac can be controlled with AppleScript, and you can ask it for the song name, artist, album art link, and whether it's playing. That works on free accounts!
- So a Python script on my Mac reads Spotify and sends everything to the ESP32 over USB. When I press a button on Inky, the ESP32 sends a message back and the script tells Spotify to pause or skip

```
Spotify app on Mac  <->  now_playing.py  <-- USB -->  ESP32  -->  e-ink screen
                                                       ^
                                                    3 buttons
```

The other tricky part was album art. E-ink is black and white only, so the script shrinks the cover to 128x128 and uses dithering (lots of tiny black and white dots) so it still looks like a picture instead of a black blob.

Parts I picked (about $30, all no-solder):
- ESP32 dev board with pre-soldered headers
- Waveshare 2.9" e-paper display (comes with a jumper cable)
- 3 push buttons for previous / play-pause / next
- Mini breadboard + jumper wires

I also planned out all the wiring (it's in the [README](README.md)) and set up this repo. I used AI to help write the first version of the firmware and the Mac script, and I'm going to go through it and make sure I understand every part before the parts arrive.

<!-- TODO: add a screenshot here, e.g. ![GitHub repo](images/repo.png) -->

**tl;dr:** Spotify's API needs Premium now, so Inky gets the song from the Spotify app on my Mac over USB instead. Works with free Spotify!

**Total time spent: 1.5h**
