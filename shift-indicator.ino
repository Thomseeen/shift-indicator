#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

#include "Seg7Numbers.h"

template <typename T, size_t N>
constexpr size_t countof(const T (&)[N]) {
  return N;
}

const int Strip_Pin = 10, Strip_Cells = 8, brightness = 15;
Adafruit_NeoPixel RPM_LEDs(Strip_Cells, Strip_Pin, NEO_GRB + NEO_KHZ800);

const int Seg7[] = {6, 5, 4, 3, 2, 7, 8};
const int colorG[] = {200, 120, 40, 0};
const float rpm_ranges[] = {0.60, 0.70, 0.80, 0.90};
const unsigned long blink_time = 75;

float rpm_percentage = 0;
unsigned long last_update = 0, blink_timer = 0;
bool BLINK = 0, FLASH = 0;

void setup() {
  Serial.begin(115200);
  RPM_LEDs.begin();
  RPM_LEDs.setBrightness(brightness);
  RPM_LEDs.clear();
  RPM_LEDs.show();
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);
  for (int i = 0; i < countof(Seg7); i++) {
    pinMode(Seg7[i], OUTPUT);
    digitalWrite(Seg7[i], HIGH);
  }
}

void loop() {
  // get Serial Data, rpm, rpmmax, carspeed, gear with flags rpmdata, speeddata, geardata
  char bufferArray[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  byte gear, d1, d2, rpmdata = 0, speeddata = 0, geardata = 0;
  unsigned int rpm, rpmmax;
  signed int carspeed;

  if (Serial.available() >= 10) {
    last_update = millis();
    digitalWrite(13, LOW);
    for (int i = 0; i < 10; i++) {
      bufferArray[i] = Serial.read();
    }
  } else if ((last_update + 2000) < millis()) {
    last_update = millis();
    digitalWrite(13, HIGH);
    RPM_LEDs.clear();
    RPM_LEDs.show();
    for (int i = 0; i < countof(Seg7); i++) {
      digitalWrite(Seg7[i], HIGH);
    }
  }

  if (bufferArray[0] == 'R') {
    d1 = bufferArray[1];
    d2 = bufferArray[2];
    rpm = ((d1 << 8) + d2);

    d1 = bufferArray[3];
    d2 = bufferArray[4];
    rpmmax = ((d1 << 8) + d2);

    if (rpm && rpmmax && rpmmax > 0) {
      rpmdata = 1;
    }
  }

  if (bufferArray[5] == 'S') {
    d1 = bufferArray[6];
    d2 = bufferArray[7];
    carspeed = ((d1 << 8) + d2);
    speeddata = 1;
  }

  if (bufferArray[8] == 'G') {
    gear = bufferArray[9];
    geardata = 1;
  }

  // Update shown
  // RPM-LEDs
  if (rpmdata == 1) {
    rpm_percentage = rpm / (float)rpmmax;
    if (rpm_percentage < 0.98) {
      FLASH = 0;
      for (int i = 0; i < countof(rpm_ranges); i++) {
        if (rpm_percentage >= rpm_ranges[i]) {
          RPM_LEDs.setPixelColor(i * 2, 255, colorG[i], 0);
        } else {
          RPM_LEDs.setPixelColor(i * 2, 0, 0, 0);
        }
      }
      RPM_LEDs.show();
      for (int i = 0; i < countof(rpm_ranges); i++) {
        if (rpm_percentage >= rpm_ranges[i]) {
          RPM_LEDs.setPixelColor(i * 2 + 1, RPM_LEDs.Color(255, colorG[i], 0));
        } else {
          RPM_LEDs.setPixelColor(i * 2 + 1, RPM_LEDs.Color(0, 0, 0));
        }
      }
      RPM_LEDs.show();
    } else {
      if (BLINK) {
        for (int i = 0; i < Strip_Cells; i++) {
          RPM_LEDs.setPixelColor(i, RPM_LEDs.Color(255, 0, 0));
        }
      } else {
        RPM_LEDs.clear();
      }
      RPM_LEDs.show();
    }
    rpmdata = 0;
  }

  if (blink_timer + blink_time < millis()) {
    blink_timer = millis();
    BLINK = !BLINK;
  }

  // GEAR-7Seg
  if (geardata == 1) {
    switch (gear) {
      case 0:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], neutral[i]);
        }
        break;
      case 1:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], first[i]);
        }
        break;
      case 2:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], second[i]);
        }
        break;
      case 3:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], third[i]);
        }
        break;
      case 4:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], fourth[i]);
        }
        break;
      case 5:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], fifth[i]);
        }
        break;
      case 6:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], sixth[i]);
        }
        break;
      case 7:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], seventh[i]);
        }
        break;
      case 10:
        for (int i = 0; i < countof(Seg7); i++) {
          digitalWrite(Seg7[i], reverse[i]);
        }
        break;
      default:
        break;
    }
    geardata = 0;
  }
}
