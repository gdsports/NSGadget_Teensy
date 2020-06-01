/* Teensy LC/3 becomes a USB NS Gamepad

   Select "NS Gamepad" from the "Tools > USB Type" menu
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

// Configure the number of buttons.  Be careful not
// to use a pin for both a digital button and analog
// axis. The pullup resistor will interfere with
// the analog voltage.
#include <Bounce2.h>

#define NUM_BUTTONS 14
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {23, 22, 21, 20, 7, 18, 6, 19, 8, 12, 9, 13, 11, 10};
#define NUM_DPAD 4
const uint8_t DPAD_PINS[NUM_DPAD] = {2, 3, 4, 5};  // Up, Right, Down, Left
const uint8_t DPAD_MAP[16] = {
                            // LDRU
  NSGAMEPAD_DPAD_CENTERED,  // 0000 All dpad buttons up
  NSGAMEPAD_DPAD_UP,        // 0001 direction UP
  NSGAMEPAD_DPAD_RIGHT,     // 0010 direction RIGHT
  NSGAMEPAD_DPAD_UP_RIGHT,  // 0011 direction UP RIGHT
  NSGAMEPAD_DPAD_DOWN,      // 0100 direction DOWN
  NSGAMEPAD_DPAD_CENTERED,  // 0101 invalid
  NSGAMEPAD_DPAD_DOWN_RIGHT,// 0110 direction DOWN RIGHT
  NSGAMEPAD_DPAD_CENTERED,  // 0111 invalid
  NSGAMEPAD_DPAD_LEFT,      // 1000 direction LEFT
  NSGAMEPAD_DPAD_UP_LEFT,   // 1001 direction UP LEFT
  NSGAMEPAD_DPAD_CENTERED,  // 1010 invalid
  NSGAMEPAD_DPAD_CENTERED,  // 1011 invalid
  NSGAMEPAD_DPAD_DOWN_LEFT, // 1100 direction DOWN LEFT
  NSGAMEPAD_DPAD_CENTERED,  // 1101 invalid
  NSGAMEPAD_DPAD_CENTERED,  // 1110 invalid
  NSGAMEPAD_DPAD_CENTERED,  // 1111 invalid
};

Bounce * buttons = new Bounce[NUM_BUTTONS];
Bounce * dpad = new Bounce[NUM_DPAD];

void setup() {
  // you can print to the Serial1 port while the NSGamepad is active!
  Serial1.begin(115200);
  Serial1.println("NSGamepad setup");
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );  //setup the bounce instance for the current button
    buttons[i].interval(10);                              // interval in ms
  }
  for (int i = 0; i < NUM_DPAD; i++) {
    dpad[i].attach( DPAD_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    dpad[i].interval(10);                                 // interval in ms
  }

  // Sends a clean HID report to the host.
  NSGamepad.begin();
}

uint32_t nextSecond = 0;
uint8_t nextButton = 0;
uint8_t dpad_bits = 0;

typedef struct axis_t {
  uint16_t adcMin;
  uint16_t adcMax;
  const uint8_t toMin;
  const uint8_t toMax;
} axis_t;

axis_t LeftX = {
  128, 1024-128, 0, 255
};
axis_t LeftY = {
  128, 1024-128, 255, 0
};
axis_t RightX = {
  128, 1024-128, 0, 255
};
axis_t RightY = {
  128, 1024-128, 255, 0
};

// Dynamically determine the pot limits because of my craptastic
// analog sticks.
uint8_t axisRead(int analogPin, struct axis_t &ax)
{
  uint16_t x = analogRead(analogPin);
  if (x > ax.adcMax) ax.adcMax = x;
  if (x < ax.adcMin) ax.adcMin = x;
  return map(x, ax.adcMin, ax.adcMax, ax.toMin, ax.toMax);
}

void loop() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    // Update the Bounce instance
    buttons[i].update();
    // Button fell means button pressed
    if ( buttons[i].fell() ) {
      NSGamepad.press(i);
    }
    else if ( buttons[i].rose() ) {
      NSGamepad.release(i);
    }
  }

  for (unsigned i = 0; i < sizeof(DPAD_PINS); i++) {
    // Update the Bounce instance
    dpad[i].update();
    // Button fell means button pressed
    if ( dpad[i].fell() ) {
      dpad_bits |= (1 << i);
    }
    else if ( dpad[i].rose() ) {
      dpad_bits &= ~(1 << i);
    }
  }
  NSGamepad.dPad(DPAD_MAP[dpad_bits]);

  NSGamepad.leftYAxis(axisRead(3, LeftY));
  NSGamepad.leftXAxis(axisRead(2, LeftX));
  NSGamepad.rightYAxis(axisRead(1, RightY));
  NSGamepad.rightXAxis(axisRead(0, RightX));

  NSGamepad.loop();
}
