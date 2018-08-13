/*
Proof of concept code for interfacing an Arduino with the frontpanel of a DT150.

The used PT6312 chip needs 5V logic, tested with Arduino Nano.

Connections for front panel connector (red line is pin 1):

 1: 3.3V
 2: NC (KEYS)
 3: (not used yet) IR
 4: D3 (SCK)
 5: D4 (DATA, half-duplex SPI)
 6: D2 (STB)
 7: (not used yet) LOCK LED (pin has to be configured as input on Arduino for LED to light up)
 8: NC (STBY)
 9: GND
10: 5V
*/
const int PIN_STB = 2;
const int PIN_SCK = 3;
const int PIN_DATA = 4;

const byte COMMAND1_DISPLAY_MODE = 0x00;
const byte COMMAND2_DATA = 0x40;
const byte COMMAND3_ADDRESS = 0xc0;
const byte COMMAND4_DISPLAY_CTRL = 0x80;

const byte DISPLAY_ON = 0b1111;
const byte DISPLAY_MODE = 0b111;

const byte font[] = {
/* - */ 0x01,
/* . */ 0x04,
/* 0 */ 0xfa,
/* 1 */ 0x82,
/* 2 */ 0xb9,
/* 3 */ 0xab,
/* 4 */ 0xc3,
/* 5 */ 0x6b,
/* 6 */ 0x7b,
/* 7 */ 0xe2,
/* 8 */ 0xfb,
/* 9 */ 0xeb,
/* a */ 0xf3,
/* b */ 0x5b,
/* c */ 0x19,
/* d */ 0x9b,
/* e */ 0x79,
/* f */ 0x71,
/* g */ 0x7a,
/* h */ 0x53,
/* i */ 0x02,
/* j */ 0x9a,
/* k */ 0x73,
/* l */ 0x58,
/* m */ 0xf2,
/* n */ 0x13,
/* o */ 0x1b,
/* p */ 0xf1,
/* q */ 0xe3,
/* r */ 0x11,
/* s */ 0x6a,
/* t */ 0x59,
/* u */ 0x1a,
/* v */ 0xda,
/* w */ 0xdb,
/* x */ 0xd3,
/* y */ 0xcb,
/* z */ 0xb8,
};

void sendCommand(byte command) {
  digitalWrite(PIN_STB, LOW);
  shiftOut(PIN_DATA, PIN_SCK, LSBFIRST, command);
  digitalWrite(PIN_STB, HIGH);
}

void writeByte(byte command, byte data) {
  digitalWrite(PIN_STB, LOW);
  shiftOut(PIN_DATA, PIN_SCK, LSBFIRST, command);
  shiftOut(PIN_DATA, PIN_SCK, LSBFIRST, data);
  digitalWrite(PIN_STB, HIGH);  
}

void writeLetters(char* letters) {
  sendCommand(COMMAND2_DATA);
  writeByte(COMMAND3_ADDRESS+0x00, getLetter(letters[0]));
  writeByte(COMMAND3_ADDRESS+0x02, getLetter(letters[1]));
  writeByte(COMMAND3_ADDRESS+0x04, getLetter(letters[2]));
  writeByte(COMMAND3_ADDRESS+0x06, getLetter(letters[3]));
}

byte getLetter(char ch) {
  if (ch == 0x2d || ch == 0x2e) {
    return font[ch-0x2d];
  }
  if (ch >= 0x30 && ch <= 0x39) {
    return font[ch-0x2e];
  }
  if (ch >= 0x41 && ch <= 0x5a) {
    return font[ch-0x35];
  }
  if (ch >= 0x61 && ch <= 0x7a) {
    return font[ch-0x55];
  }
  return 0;
}

void setup() {
  pinMode(PIN_STB, OUTPUT);
  pinMode(PIN_SCK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  digitalWrite(PIN_STB, HIGH);

  delay(200);
  sendCommand(COMMAND2_DATA);
  for (int addr=0; addr < 0x16; addr++) {
    writeByte(COMMAND3_ADDRESS | addr, 0);
  }
  sendCommand(COMMAND1_DISPLAY_MODE | DISPLAY_MODE);
  sendCommand(COMMAND4_DISPLAY_CTRL | DISPLAY_ON);
}

int pos = 0;
const char* alphabet = "-.0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIKLMNOPQRSTUVWXYZ";

void loop() {
  char word[] = {0, 0, 0, 0,};
  for (int i = 0; i < 4; i++) {
    word[i] = alphabet[(pos+i) % 64];
  }
  writeLetters(word);
  if (++pos >= 64) {
    pos = 0;
  }
  delay(500);
}

