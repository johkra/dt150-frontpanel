#include "Arduino.h"

class dt150 {
  private:
    dt150();
  
    const byte COMMAND1_DISPLAY_MODE = 0x00;
    const byte COMMAND2_DATA = 0x40;
    const byte COMMAND3_ADDRESS = 0xc0;
    const byte COMMAND4_DISPLAY_CTRL = 0x80;
    const byte DISPLAY_ON = 0b1111;
    const byte DISPLAY_MODE = 0b111;
    const byte WRITE_LED = 0b01;
    const byte LED2 = 0b0010;
    const byte KEY_DATA = 0x10;
    const byte SW_DATA = 0x11;

    const byte font[38] = {
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

    const byte pin_stb;
    const byte pin_sck;
    const byte pin_data;
    const byte pin_lock;
   
  void sendCommand(byte command) {
    digitalWrite(pin_stb, LOW);
    shiftOut(pin_data, pin_sck, LSBFIRST, command);
    digitalWrite(pin_stb, HIGH);
  }

  void writeByte(byte command, byte data) {
    digitalWrite(pin_stb, LOW);
    shiftOut(pin_data, pin_sck, LSBFIRST, command);
    shiftOut(pin_data, pin_sck, LSBFIRST, data);
    digitalWrite(pin_stb, HIGH);  
  }
  
//  byte readByte(byte command) {
//    digitalWrite(pin_stb, LOW);
//    shiftOut(pin_data, pin_sck, LSBFIRST, command);
//    delay(1);
//    pinMode(pin_data, INPUT);
//    byte data = shiftIn(pin_data, pin_sck, LSBFIRST);
//    pinMode(pin_data, OUTPUT);
//    digitalWrite(pin_stb, HIGH);
//    return data;
//  }
//  
//  void readBytes(byte command, byte* data, byte len) {
//    digitalWrite(pin_stb, LOW);
//    shiftOut(pin_data, pin_sck, LSBFIRST, command);
//    delay(1);
//    pinMode(pin_data, INPUT);
//    for (int i = 0; i < len; i++) {
//      data[i] = shiftIn(pin_data, pin_sck, LSBFIRST);
//    }
//    pinMode(pin_data, OUTPUT);
//    digitalWrite(pin_stb, HIGH);
//  }
  
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
  
  public:
    dt150(byte pin_stb, byte pin_sck, byte pin_data, byte pin_lock) : pin_stb(pin_stb), pin_sck(pin_sck), pin_data(pin_data), pin_lock(pin_lock) {
      pinMode(pin_stb, OUTPUT);
      pinMode(pin_sck, OUTPUT);
      pinMode(pin_data, OUTPUT);
      pinMode(pin_lock, OUTPUT);

      delay(200);
      sendCommand(COMMAND2_DATA);
      for (int addr=0; addr < 0x16; addr++) {
        writeByte(COMMAND3_ADDRESS | addr, 0);
      }
      sendCommand(COMMAND1_DISPLAY_MODE | DISPLAY_MODE);
      sendCommand(COMMAND4_DISPLAY_CTRL | DISPLAY_ON);

      lockOff();
      standbyOff();
    }

    const byte STANDBY = 0x1;
    const byte PROGUP = 0x2;
    const byte PROGDOWN = 0x4;

    // TODO: Handle dots, e.g. "107.7".
    void writeLetters(char* letters) {
      sendCommand(COMMAND2_DATA);
      writeByte(COMMAND3_ADDRESS+0x00, getLetter(letters[0]));
      writeByte(COMMAND3_ADDRESS+0x02, getLetter(letters[1]));
      writeByte(COMMAND3_ADDRESS+0x04, getLetter(letters[2]));
      writeByte(COMMAND3_ADDRESS+0x06, getLetter(letters[3]));
    }
    
    void lockOn() {
      digitalWrite(pin_lock, LOW);
    }
    
    void lockOff() {
      digitalWrite(pin_lock, HIGH);
    }
    
    void standbyOn() {
      writeByte(COMMAND2_DATA | WRITE_LED, LED2);
    }
    
    void standbyOff() {
      writeByte(COMMAND2_DATA | WRITE_LED, 0);
    }
};


