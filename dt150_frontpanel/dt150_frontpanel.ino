/*
Proof of concept code for interfacing an Arduino with the frontpanel of a DT150.

The used PT6312 chip needs 5V logic, tested with Arduino Nano.

Connections for front panel connector (red line is pin 1):

 1: 3.3V
 2: NC (KEYS)
 3: (not used yet) IR
 4: D3 (SCK)
 5: D2 (STB)
 6: D4 (DATA, half-duplex SPI)
 7: D5 LOCK LED
 8: NC (STBY)
 9: GND
10: 5V
*/

#include "dt150fp.h"

dt150 *frontpanel;

void setup() {
  frontpanel = new dt150(2, 3, 4, 5);
}

byte pos = 0;
boolean lock_state = false;
const char* alphabet = "-.0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIKLMNOPQRSTUVWXYZ";

byte hexChar(byte val) {
  if (val < 10 ) {
    return val + 0x30;
  }
  return val + 0x37;
}

void loop() {
  char word[] = {0, 0, 0, 0,};
  for (int i = 0; i < 4; i++) {
    word[i] = alphabet[(pos+i) % 64];
  }
  frontpanel->writeLetters(word);
  if (++pos >= 64) {
    pos = 0;
  }
  lock_state = !lock_state;
  if (lock_state) {
    frontpanel->lockOn();
    frontpanel->standbyOff();
  } else {
    frontpanel->lockOff();
    frontpanel->standbyOn();
  }
  delay(500);
}

