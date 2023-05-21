#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"


MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long timestart = 0;
static const unsigned char PROGMEM heartsmall[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,
  0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
  0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
  0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,
};

static const unsigned char PROGMEM heartbig[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
  0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
  0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
  0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
  0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
  0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
  0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
  0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00
};

int a, b, c, d, e;
int BPM;

void setup() {

  Serial.begin(115200);

  Wire.setClock(400000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(26, 10);
  display.println("Measure your");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 30);
  display.println("HeartRate");
  display.display();
  delay(5000);
}

void loop() {
  bpm();
  checkbpm();
}

void bpm()
{
  long irValue = particleSensor.getIR();

  if (irValue > 50000)
  {

    if (checkForBeat(irValue) == true)
    {
      long delta = millis() - lastBeat;
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);
      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;

        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
  }
  else { }
}

void checkbpm()
{
  long irValue = particleSensor.getIR();
  unsigned long currentmillis = millis() / 1000;
  if (currentmillis - timestart >= 1 ) {
    timestart = millis() / 1000;
    if (irValue < 50000) {
      currentmillis = 0;
      timestart = 0;
      Serial.println("reset");

      display.clearDisplay();
      display.drawBitmap(45, 10, heartbig, 32, 32, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(16, 50);
      display.println("Please waiting...");
      display.display();

    }
  }
  switch (timestart)
  {
    case 15:
      a =  beatAvg;

      Serial.println(a);

      display.clearDisplay();
      display.drawBitmap(45, 10, heartbig, 32, 32, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(16, 50);
      display.println("Please waiting...");
      display.display();

      break;

    case 20:
      b = beatAvg;

      Serial.println(b);

      display.clearDisplay();
      display.drawBitmap(50, 15, heartsmall, 24, 21, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(16, 50);
      display.println("Please waiting...");
      display.display();

      break;

    case 25:
      c = beatAvg;

      Serial.println(c);

      display.clearDisplay();
      display.drawBitmap(45, 10, heartbig, 32, 32, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(16, 50);
      display.println("Please waiting...");
      display.display();

      break;

    case 30:
      d = beatAvg;

      Serial.println(d);

      display.clearDisplay();
      display.drawBitmap(50, 15, heartsmall, 24, 21, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(16, 50);
      display.println("Please waiting...");
      display.display();

      break;

    case 35:
      e = beatAvg;

      Serial.println(e);

      display.clearDisplay();
      display.drawBitmap(45, 10, heartbig, 32, 32, 1);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(16, 50);
      display.println("Please waiting...");
      display.display();

      break;

    case 37:
      int sum = (a + b + c + d + e) / 5;
      Serial.println(sum);
      Serial.println("done....");

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(18, 25);
      display.println("BPM :");
      display.setCursor(86, 25);
      display.println(sum);
      display.display();
      delay(10000);

      break;
  }
}
