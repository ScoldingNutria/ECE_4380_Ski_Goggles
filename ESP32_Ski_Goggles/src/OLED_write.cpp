#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SDA_PIN 12
#define SCL_PIN 13

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static void mirrorDisplayHorizontal() {
  uint8_t* buf = display.getBuffer();
  int pages = SCREEN_HEIGHT / 8;

  for (int page = 0; page < pages; page++) {
    for (int x = 0; x < SCREEN_WIDTH / 2; x++) {
      int leftIndex  = page * SCREEN_WIDTH + x;
      int rightIndex = page * SCREEN_WIDTH + (SCREEN_WIDTH - 1 - x);

      uint8_t tmp     = buf[leftIndex];
      buf[leftIndex]  = buf[rightIndex];
      buf[rightIndex] = tmp;
    }
  }
}

void oled_init() {
  Serial.println("oled_init()");
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR, false, false)) {
    Serial.println("OLED init failed");
    while (1) { delay(100); }
  }

  Serial.println("OLED init OK");
  display.clearDisplay();
  display.display();
}

void oled_show_speed(float IMU_roll) {
  Serial.print("oled_show_speed(): ");
  Serial.println(IMU_roll);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f ", IMU_roll);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (SCREEN_WIDTH  - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y);
  display.print(buf);

  mirrorDisplayHorizontal();
  display.display();
}



static const uint8_t* getGlyph(char c) {
  static const uint8_t unknown[3] = {0x04,0x0E,0x04};
  static const uint8_t sp[3]      = {0x00,0x00,0x00};

  static const uint8_t digits[10][3] = {
    {0x1F,0x11,0x1F},{0x00,0x1F,0x00},{0x1D,0x15,0x17},{0x11,0x15,0x1F},{0x07,0x04,0x1F},
    {0x17,0x15,0x1D},{0x1F,0x15,0x1D},{0x01,0x01,0x1F},{0x1F,0x15,0x1F},{0x17,0x15,0x1F}
  };

  static const uint8_t upperAtoZ[26][3] = {
    {0x1F,0x05,0x1F},{0x1F,0x15,0x0A},{0x1F,0x11,0x11},{0x1F,0x11,0x0E},{0x1F,0x15,0x11},
    {0x1F,0x05,0x01},{0x1F,0x11,0x1D},{0x1F,0x04,0x1F},{0x11,0x1F,0x11},{0x10,0x10,0x0F},
    {0x1F,0x04,0x1B},{0x1F,0x10,0x10},{0x1F,0x06,0x1F},{0x1F,0x02,0x1F},{0x1F,0x11,0x1F},
    {0x1F,0x05,0x03},{0x1F,0x11,0x2F},{0x1F,0x0D,0x16},{0x17,0x15,0x1D},{0x01,0x1F,0x01},
    {0x0F,0x10,0x1F},{0x07,0x18,0x07},{0x1F,0x0C,0x1F},{0x1B,0x04,0x1B},{0x03,0x1C,0x03},{0x19,0x15,0x13}
  };

  if (c == ' ') return sp;
  if (c >= '0' && c <= '9') return digits[c - '0'];
  if (c >= 'A' && c <= 'Z') return upperAtoZ[c - 'A'];
  if (c >= 'a' && c <= 'z') return upperAtoZ[c - 'a'];
  if (c == '-') { static const uint8_t m[3]={0x04,0x04,0x04}; return m; }
  if (c == '.') { static const uint8_t p[3]={0x00,0x10,0x00}; return p; }
  return unknown;
}

static void drawTinyString(int16_t x, int16_t y, const char* s) {
  const uint8_t glyphW = 3;
  const uint8_t glyphH = 5;
  const uint8_t glyphStride = glyphW + 1;

  for (size_t i = 0; s[i]; i++) {
    const uint8_t* g = getGlyph(s[i]);
    for (uint8_t col = 0; col < glyphW; col++) {
      uint8_t bits = g[col];
      for (uint8_t row = 0; row < glyphH; row++) {
        if (bits & (1 << row)) {
          display.drawPixel(x + col, y + row, SSD1306_WHITE);
        }
      }
    }
    x += glyphStride;
  }
}

static int16_t tinyWidth(const char* s) {
  const uint8_t glyphStride = 4;
  size_t n = strlen(s);
  if (n == 0) return 0;
  return n * glyphStride - 1;
}



void oled_show_three_lines(const char* lineTop, const char* lineMid, const char* lineBottom) {
  display.clearDisplay();

  bool topIsBig = (strlen(lineTop) <= 3);   

  int16_t y = 0;

  if (topIsBig) {
    
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);

    int16_t x1, y1; uint16_t w, h;
    display.getTextBounds(lineTop, 0, 0, &x1, &y1, &w, &h);

    int16_t x = (SCREEN_WIDTH - w) / 2;
    display.setCursor(x, 0);
    display.print(lineTop);

    y = 17;  
  } else {
    
    int16_t wTop = tinyWidth(lineTop);
    int16_t xTop = (SCREEN_WIDTH - wTop) / 2;
    drawTinyString(xTop, 0, lineTop);
    y = 8;
  }

  
  int16_t wMid = tinyWidth(lineMid);
  int16_t xMid = (SCREEN_WIDTH - wMid) / 2;
  drawTinyString(xMid, y, lineMid);

  
  int16_t wBottom = tinyWidth(lineBottom);
  int16_t xBottom = (SCREEN_WIDTH - wBottom) / 2;
  drawTinyString(xBottom, y + 8, lineBottom);

  mirrorDisplayHorizontal();
  display.display();
}

void oled_show_three_lines(float a, float b, float c) {
  char A[16], B[16], C[16];
  snprintf(A, sizeof(A), "%.1f", a);
  snprintf(B, sizeof(B), "%.1f", b);
  snprintf(C, sizeof(C), "%.1f", c);
  oled_show_three_lines(A, B, C);
}
