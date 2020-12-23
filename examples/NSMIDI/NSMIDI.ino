/*
 * Select Board: Teensy 3.6
 * Select NS Gamepad on the "Tools > USB Type" menu.
 */
/*
 * MIT License
 *
 * Copyright (c) 2020 gdsports625@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <USBHost_t36.h>

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);
KeyboardController keyboard1(myusb);

const uint8_t DPAD_OFFSET = 64;
const uint8_t LEFT_STICK_LEFT = 128;
const uint8_t LEFT_STICK_RIGHT = 129;
const uint8_t LEFT_STICK_UP = 130;
const uint8_t LEFT_STICK_DOWN = 131;
const uint8_t RIGHT_STICK_LEFT = 132;
const uint8_t RIGHT_STICK_RIGHT = 133;
const uint8_t DPAD_LEFT = NSGAMEPAD_DPAD_LEFT + DPAD_OFFSET;
const uint8_t DPAD_UP = NSGAMEPAD_DPAD_UP + DPAD_OFFSET;
const uint8_t DPAD_RIGHT = NSGAMEPAD_DPAD_RIGHT + DPAD_OFFSET;
const uint8_t DPAD_DOWN = NSGAMEPAD_DPAD_DOWN + DPAD_OFFSET;

const uint8_t NOTE2BUTTON[] = {
    255,    // Note 0
    255,    // Note 1
    255,    // Note 2
    255,    // Note 3
    255,    // Note 4
    255,    // Note 5
    255,    // Note 6
    255,    // Note 7
    255,    // Note 8
    255,    // Note 9
    255,    // Note 10
    255,    // Note 11
    255,    // Note 12
    255,    // Note 13
    255,    // Note 14
    255,    // Note 15
    255,    // Note 16
    255,    // Note 17
    255,    // Note 18
    255,    // Note 19
    255,    // Note 20
    255,    // Note 21
    255,    // Note 22
    255,    // Note 23
    255,    // Note 24
    255,    // Note 25
    255,    // Note 26
    255,    // Note 27
    255,    // Note 28
    255,    // Note 29
    255,    // Note 30
    255,    // Note 31
    255,    // Note 32
    255,    // Note 33
    255,    // Note 34
    255,    // Note 35
    255,    // Note 36
    255,    // Note 37
    255,    // Note 38
    255,    // Note 39
    255,    // Note 40
    255,    // Note 41
    255,    // Note 42
    255,    // Note 43
    255,    // Note 44
    255,    // Note 45
    255,    // Note 46
    255,    // Note 47
    255,    // Note 48
    255,    // Note 49
    255,    // Note 50
    255,    // Note 51
    255,    // Note 52
    NSButton_LeftTrigger,    // W Note 53
    NSButton_Minus,          // B Note 54
    DPAD_LEFT,// W Note 55
    NSButton_Capture,        // B Note 56
    DPAD_UP,// W Note 57
    NSButton_LeftThrottle,   // B Note 58
    DPAD_RIGHT,// W Note 59
    DPAD_DOWN, // W Note 60
    LEFT_STICK_DOWN,         // B Note 61 down
    LEFT_STICK_LEFT,         // W Note 62 left
    LEFT_STICK_UP,           // B Note 63 up
    LEFT_STICK_RIGHT,        // W Note 64 right
    255,    // W Note 65
    255,    // B Note 66
    255,    // W Note 67
    255,    // B Note 68
    255,    // W Note 69
    255,    // B Note 70
    255,    // W Note 71
    255,    // W Note 72
    255,    // B Note 73
    255,    // W Note 74
    255,    // B Note 75
    255,    // W Note 76
    NSButton_B,             // W Note 77
    NSButton_RightThrottle, // B Note 78
    NSButton_Y,             // W Note 79
    NSButton_Home,          // B Note 80
    NSButton_X,             // W Note 81
    NSButton_Plus,          // B Note 82
    NSButton_A,             // W Note 83
    NSButton_RightTrigger,  // W Note 84
    255,    // Note 85
    255,    // Note 86
    255,    // Note 87
    255,    // Note 88
    255,    // Note 89
    255,    // Note 90
    255,    // Note 91
    255,    // Note 92
    255,    // Note 93
    255,    // Note 94
    255,    // Note 95
};

#define NSGAMEPAD_DPAD_CENTERED 0xF
#define NSGAMEPAD_DPAD_UP 0
#define NSGAMEPAD_DPAD_UP_RIGHT 1
#define NSGAMEPAD_DPAD_RIGHT 2
#define NSGAMEPAD_DPAD_DOWN_RIGHT 3
#define NSGAMEPAD_DPAD_DOWN 4
#define NSGAMEPAD_DPAD_DOWN_LEFT 5
#define NSGAMEPAD_DPAD_LEFT 6
#define NSGAMEPAD_DPAD_UP_LEFT 7
const uint8_t BITS2DIRECTION[16] = {
//                             RLDU
  NSGAMEPAD_DPAD_CENTERED,  // 0000
  NSGAMEPAD_DPAD_UP,        // 0001
  NSGAMEPAD_DPAD_DOWN,      // 0010
  NSGAMEPAD_DPAD_CENTERED,  // 0011
  NSGAMEPAD_DPAD_LEFT,      // 0100
  NSGAMEPAD_DPAD_UP_LEFT,   // 0101
  NSGAMEPAD_DPAD_DOWN_LEFT, // 0110
  NSGAMEPAD_DPAD_CENTERED,  // 0111
  NSGAMEPAD_DPAD_RIGHT,     // 1000
  NSGAMEPAD_DPAD_UP_RIGHT,  // 1001
  NSGAMEPAD_DPAD_DOWN_RIGHT,// 1010
  NSGAMEPAD_DPAD_CENTERED,  // 1011
  NSGAMEPAD_DPAD_CENTERED,  // 1100
  NSGAMEPAD_DPAD_CENTERED,  // 1101
  NSGAMEPAD_DPAD_CENTERED,  // 1110
  NSGAMEPAD_DPAD_CENTERED,  // 1111
};

// Indexed by value returned by getOemKey().
// See USB HID Usage Table
const uint8_t KEYCODE2BUTTON[] = {
  255,  // 0
  255,  // 1
  255,  // 2
  255,  // 3
  LEFT_STICK_LEFT,  // 4 a
  255,  // 5 b
  255,  // 6 c
  NSButton_X, // 7 d
  255,  // 8 e
  NSButton_Y, // 9 f
  255,  // 10 g
  255,  // 11 h
  255,  // 12 i
  NSButton_B, // 13 j
  NSButton_A, // 14 k
  RIGHT_STICK_LEFT, // 15 l
  255,  // 16 m
  255,  // 17 n
  255,  // 18 o
  255,  // 19 p
  255,  // 20 q
  255,  // 21 r
  LEFT_STICK_RIGHT, // 22 s
  255,  // 23 t
  255,  // 24 u
  255,  // 25 v
  255,  // 26 w
  255,  // 27 x
  255,  // 28 y
  255,  // 29 z
  NSButton_LeftThrottle,  // 30 1
  NSButton_LeftTrigger,   // 31 2
  NSButton_LeftStick,     // 32 3
  NSButton_Minus,         // 33 4
  NSButton_Capture,       // 34 5
  NSButton_Home,          // 35 6
  NSButton_Plus,          // 36 7
  NSButton_RightStick,    // 37 8
  NSButton_RightTrigger,  // 38 9
  NSButton_RightThrottle, // 39 0
  NSButton_A,             // 40 ENTER
  255,  // 41 ESCAPE
  NSButton_B,             // 42 BACKSPACE
  255,  // 43 TAB
  NSButton_RightThrottle, // 44 SPACE
  255,  // 45 - _
  255,  // 46 = +
  255,  // 47 [ {
  255,  // 48 ] }
  255,  // 49 \ |
  255,  // 50 # ~
  RIGHT_STICK_RIGHT,  // 51 ; :
  255,  // 52 ' "
  255,  // 53 ` ~
  255,  // 54 , <
  255,  // 55 . >
  255,  // 56 / ?
  255,  // 57 Caps Lock
  255,  // 58 F1
  255,  // 59 F2
  255,  // 60 F3
  255,  // 61 F4
  255,  // 62 F5
  255,  // 63 F6
  255,  // 64 F7
  255,  // 65 F8
  255,  // 66 F9
  255,  // 67 F10
  255,  // 68 F11
  255,  // 69 F12
  255,  // 70 PrtScr
  255,  // 71 Scoll Lock
  255,  // 72 Pause
  255,  // 73 Insert
  255,  // 74 Home
  255,  // 75 PageUp
  255,  // 76 DELETE
  255,  // 77 End
  255,  // 78 PageDown
  DPAD_RIGHT, // 79 RightArrow
  DPAD_LEFT,  // 80 LeftArrow
  DPAD_DOWN,  // 81 DownArrow
  DPAD_UP,    // 82 UpArrow
  255,  // 83
  255,  // 84
  255,  // 85
  255,  // 86
  255,  // 87
  255,  // 88
  255,  // 89
  255,  // 90
  255,  // 91
  255,  // 92
  255,  // 93
  255,  // 94
  255,  // 95
  255,  // 96
  255,  // 97
  255,  // 98
  255,  // 99
  255,  // 100
  255,  // 101
  255,  // 102
  255,  // 103
  255,  // 104
  255,  // 105
  255,  // 106
  255,  // 107
  255,  // 108
  255,  // 109
  255,  // 110
  255,  // 111
  255,  // 112
  255,  // 113
  255,  // 114
  255,  // 115
  255,  // 116
  255,  // 117
  255,  // 118
  255,  // 119
  255,  // 120
  255,  // 121
  255,  // 122
  255,  // 123
  255,  // 124
  255,  // 125
  255,  // 126
  255,  // 127
  255,  // 128
  255,  // 129
  255,  // 130
  255,  // 131
  255,  // 132
  255,  // 133
  255,  // 134
  255,  // 135
  255,  // 136
  255,  // 137
  255,  // 138
  255,  // 139
  255,  // 140
  255,  // 141
  255,  // 142
  255,  // 143
  255,  // 144
  255,  // 145
  255,  // 146
  255,  // 147
  255,  // 148
  255,  // 149
  255,  // 150
  255,  // 151
  255,  // 152
  255,  // 153
  255,  // 154
  255,  // 155
  255,  // 156
  255,  // 157
  255,  // 158
  255,  // 159
  255,  // 160
  255,  // 161
  255,  // 162
  255,  // 163
  255,  // 164
  255,  // 165
  255,  // 166
  255,  // 167
  255,  // 168
  255,  // 169
  255,  // 170
  255,  // 171
  255,  // 172
  255,  // 173
  255,  // 174
  255,  // 175
  255,  // 176
  255,  // 177
  255,  // 178
  255,  // 179
  255,  // 180
  255,  // 181
  255,  // 182
  255,  // 183
  255,  // 184
  255,  // 185
  255,  // 186
  255,  // 187
  255,  // 188
  255,  // 189
  255,  // 190
  255,  // 191
  255,  // 192
  255,  // 193
  255,  // 194
  255,  // 195
  255,  // 196
  255,  // 197
  255,  // 198
  255,  // 199
  255,  // 200
  255,  // 201
  255,  // 202
  255,  // 203
  255,  // 204
  255,  // 205
  255,  // 206
  255,  // 207
  255,  // 208
  255,  // 209
  255,  // 210
  255,  // 211
  255,  // 212
  255,  // 213
  255,  // 214
  255,  // 215
  255,  // 216
  255,  // 217
  255,  // 218
  255,  // 219
  255,  // 220
  255,  // 221
  255,  // 222
  255,  // 223
  255,  // 224
  255,  // 225
  255,  // 226
  255,  // 227
  255,  // 228
  255,  // 229
  255,  // 230
  255,  // 231
  255,  // 232
  255,  // 233
  255,  // 234
  255,  // 235
  255,  // 236
  255,  // 237
  255,  // 238
  255,  // 239
  255,  // 240
  255,  // 241
  255,  // 242
  255,  // 243
  255,  // 244
  255,  // 245
  255,  // 246
  255,  // 247
  255,  // 248
  255,  // 249
  255,  // 250
  255,  // 251
  255,  // 252
  255,  // 253
  255,  // 254
  255,  // 255
};

void setup() {
  NSGamepad.begin();

  // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  myusb.begin();

  keyboard1.attachRawPress(myKeyOn);
  keyboard1.attachRawRelease(myKeyOff);
  midi1.setHandleNoteOn(myNoteOn);
  midi1.setHandleNoteOff(myNoteOff);
}

void loop() {
  // The handler functions are called when midi1 reads data.  They
  // will not be called automatically.  You must call midi1.read()
  // regularly from loop() for midi1 to actually read incoming
  // data and run the handler functions as messages arrive.
  myusb.Task();
  midi1.read();
  NSGamepad.loop();
}

uint8_t Dpad_bits;

void buttonPress(uint8_t button)
{
  if (button < DPAD_OFFSET) {
    NSGamepad.press(button);
  }
  else {
    switch (button) {
      case LEFT_STICK_DOWN:
        NSGamepad.leftYAxis(255);
        break;
      case LEFT_STICK_UP:
        NSGamepad.leftYAxis(0);
        break;
      case LEFT_STICK_LEFT:
        NSGamepad.leftXAxis(0);
        break;
      case LEFT_STICK_RIGHT:
        NSGamepad.leftXAxis(255);
        break;
      case RIGHT_STICK_LEFT:
        NSGamepad.rightXAxis(0);
        break;
      case RIGHT_STICK_RIGHT:
        NSGamepad.rightXAxis(255);
        break;
      case DPAD_UP:
        Dpad_bits |= 1 << 0;
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      case DPAD_DOWN:
        Dpad_bits |= 1 << 1;
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      case DPAD_LEFT:
        Dpad_bits |= 1 << 2;
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      case DPAD_RIGHT:
        Dpad_bits |= 1 << 3;
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      default:
        break;
    }
  }
}

void buttonRelease(uint8_t button)
{
  if (button < DPAD_OFFSET) {
    NSGamepad.release(button);
  }
  else {
    switch (button) {
      case LEFT_STICK_DOWN:
      case LEFT_STICK_UP:
        NSGamepad.leftYAxis(128);
        break;
      case LEFT_STICK_LEFT:
      case LEFT_STICK_RIGHT:
        NSGamepad.leftXAxis(128);
        break;
      case RIGHT_STICK_LEFT:
      case RIGHT_STICK_RIGHT:
        NSGamepad.rightXAxis(128);
        break;
      case DPAD_UP:
        Dpad_bits &= ~(1 << 0);
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      case DPAD_DOWN:
        Dpad_bits &= ~(1 << 1);
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      case DPAD_LEFT:
        Dpad_bits &= ~(1 << 2);
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      case DPAD_RIGHT:
        Dpad_bits &= ~(1 << 3);
        NSGamepad.dPad(BITS2DIRECTION[Dpad_bits]);
        break;
      default:
        break;
    }
  }
}

void myKeyOn(uint8_t oemkey)
{
  if (oemkey > sizeof(KEYCODE2BUTTON)) return;
  buttonPress(KEYCODE2BUTTON[oemkey]);
}

void myKeyOff(uint8_t oemkey)
{
  if (oemkey > sizeof(KEYCODE2BUTTON)) return;
  buttonRelease(KEYCODE2BUTTON[oemkey]);
}

void myNoteOn(byte channel, byte note, byte velocity)
{
  if (note >= sizeof(NOTE2BUTTON)) return;
  buttonPress(NOTE2BUTTON[note]);
}

void myNoteOff(byte channel, byte note, byte velocity)
{
  if (note >= sizeof(NOTE2BUTTON)) return;
  buttonRelease(NOTE2BUTTON[note]);
}
