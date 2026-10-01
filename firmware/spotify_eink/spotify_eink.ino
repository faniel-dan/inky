// Inky: Spotify e-ink display
// Shows the song my Mac is playing (sent over USB by computer/now_playing.py)
// and sends button presses back to control playback.
// Board: ESP32 DevKit. Libraries: GxEPD2, Adafruit GFX.
#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

// WeAct Studio 2.9" black/white (296x128, SSD1680).
// For the Waveshare 2.9" V2 instead, swap GxEPD2_290_BS for GxEPD2_290_T94_V2.
GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(
    GxEPD2_290_BS(/*CS=*/5, /*DC=*/17, /*RST=*/16, /*BUSY=*/4));

const int ART_SIZE = 128;
const int ART_BYTES = ART_SIZE * ART_SIZE / 8;
const int TEXT_X = ART_SIZE + 10;

const int NUM_BUTTONS = 3;
const int BUTTON_PINS[NUM_BUTTONS] = {32, 33, 25};
const char *BUTTON_COMMANDS[NUM_BUTTONS] = {"PREV", "PLAYPAUSE", "NEXT"};
const unsigned long DEBOUNCE_MS = 50;

uint8_t art[ART_BYTES];
bool hasArt = false;
String title = "Waiting for Mac...";
String artist = "";
bool playing = false;

// Longest line is the album art: 'I' + 2 hex chars per byte
char line[2 * ART_BYTES + 8];
size_t lineLen = 0;

bool lastPressed[NUM_BUTTONS];
unsigned long lastChange[NUM_BUTTONS];

// Prints text at TEXT_X, cutting it short with "..." if it would run off the screen
void printFitted(String text, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  int maxWidth = display.width() - TEXT_X - 4;
  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  bool cut = false;
  while (w > maxWidth && text.length() > 0) {
    text.remove(text.length() - 1);
    cut = true;
    display.getTextBounds(text + "...", 0, y, &x1, &y1, &w, &h);
  }
  display.setCursor(TEXT_X, y);
  display.print(cut ? text + "..." : text);
}

void drawScreen() {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    if (hasArt) {
      display.drawBitmap(0, 0, art, ART_SIZE, ART_SIZE, GxEPD_BLACK);
    }
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    printFitted(title, 34);
    display.setFont(&FreeSans9pt7b);
    printFitted(artist, 62);
    printFitted(playing ? "> Playing" : "|| Paused", 118);
  } while (display.nextPage());
}

uint8_t hexValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

// Messages from the Mac, one per line:
//   T<title>  A<artist>  S<playing|paused>  I<album art as hex, or empty>  D (draw)
void handleLine() {
  line[lineLen] = '\0';
  const char *data = line + 1;
  switch (line[0]) {
    case 'T': title = data; break;
    case 'A': artist = data; break;
    case 'S': playing = strcmp(data, "playing") == 0; break;
    case 'I':
      hasArt = (lineLen - 1 == 2 * ART_BYTES);
      if (hasArt) {
        for (int i = 0; i < ART_BYTES; i++) {
          art[i] = (hexValue(data[2 * i]) << 4) | hexValue(data[2 * i + 1]);
        }
      }
      break;
    case 'D': drawScreen(); break;
  }
}

void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (lineLen > 0) handleLine();
      lineLen = 0;
    } else if (c != '\r' && lineLen < sizeof(line) - 1) {
      line[lineLen++] = c;
    }
  }
}

void readButtons() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool pressed = digitalRead(BUTTON_PINS[i]) == LOW;
    if (pressed != lastPressed[i] && millis() - lastChange[i] > DEBOUNCE_MS) {
      lastPressed[i] = pressed;
      lastChange[i] = millis();
      if (pressed) Serial.println(BUTTON_COMMANDS[i]);
    }
  }
}

void setup() {
  Serial.setRxBufferSize(8192);  // album art arrives in one big burst
  Serial.begin(115200);
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);  // buttons connect the pin to GND
  }
  display.init(0, true, 2, false);  // 0 = no debug text on Serial, it would confuse the Mac script
  display.setRotation(1);           // landscape, 296x128
  drawScreen();
  Serial.println("HELLO");  // tells the Mac script to resend the current song
}

void loop() {
  readSerial();
  readButtons();
}
