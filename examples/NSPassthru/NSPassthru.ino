/* Teensy 3.6 USB NS gamepad pass through
 *
 * Based on the PJRC joystick.ino  * example program.
 *
 * Select "NS Gamepad" from the "Tools > USB Type" menu
 *
 * Convert/adapt various USB joysticks/controllers connected to the Teensy USB
 * host port to USB NS gamepad on the Teensy USB device port.
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

#define JOY_DEBUG 0

#include "USBHost_t36.h"

USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);
USBHIDParser hid4(myusb);
#define COUNT_JOYSTICKS 4
JoystickController joysticks[COUNT_JOYSTICKS](myusb);

USBDriver *drivers[] = {&hub1, &joysticks[0], &joysticks[1], &joysticks[2], &joysticks[3], &hid1, &hid2, &hid3, &hid4};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "joystick[0D]", "joystick[1D]", "joystick[2D]", "joystick[3D]",  "HID1", "HID2", "HID3", "HID4"};
bool driver_active[CNT_DEVICES] = {false, false, false, false};

USBHIDInput *hiddrivers[] = {&joysticks[0], &joysticks[1], &joysticks[2], &joysticks[3]};
#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
const char * hid_driver_names[CNT_DEVICES] = {"joystick[0H]", "joystick[1H]", "joystick[2H]", "joystick[3H]"};
bool hid_driver_active[CNT_DEVICES] = {false};

uint8_t joystick_left_trigger_value[COUNT_JOYSTICKS] = {0};
uint8_t joystick_right_trigger_value[COUNT_JOYSTICKS] = {0};
uint64_t joystick_full_notify_mask = (uint64_t) - 1;

// Convert direction pad numbers to (X,Y) values. X and Y range from 0..255.
// The direction pad returns 0..F where 0xF is the idle/centered position.
// X=128, Y=128 center position.
const struct dPad2dir {
  uint8_t xDir;
  uint8_t yDir;
} dPad2dir_table[16] = {
  {128, 0},   /* 0 Up */
  {255, 0},   /* 1 Up Right */
  {255, 128}, /* 2 Right */
  {255, 255}, /* 3 Down Right */
  {128, 255}, /* 4 Down */
  {0, 255},   /* 5 Down Left */
  {0, 128},   /* 6 Left */
  {0, 0},     /* 7 Up Left */
  {128, 128}, /* 8 Invalid */
  {128, 128}, /* 9 Invalid */
  {128, 128}, /* 10 Invalid */
  {128, 128}, /* 11 Invalid */
  {128, 128}, /* 12 Invalid */
  {128, 128}, /* 13 Invalid */
  {128, 128}, /* 14 Invalid */
  {128, 128}  /* 15 Centered */
};

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

//=============================================================================
// Setup
//=============================================================================
void setup()
{
  // Use Serial1 UART because USB Serial not supported
  Serial1.begin(115200);
  // Sends a clean HID report to the NS.
  Serial1.println("\n\nNS Gamepad");
  NSGamepad.begin();
  Serial1.println("\n\nUSB Host Joystick");
  myusb.begin();
}

// Process input events from a Hori Horipad Nintendo Switch compatible gamepad.
// This is mostly just a pass through since the USB device port emulates the
// same device.
void handle_horipad(int joystick_index)
{
  uint64_t axis_mask = joysticks[joystick_index].axisMask();
  uint32_t buttons = joysticks[joystick_index].getButtons();

  for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
    if (axis_mask & 1) {
      int ax = joysticks[joystick_index].getAxis(i);
      switch (i) {
        case 0:
          NSGamepad.leftXAxis(ax);
          break;
        case 1:
          NSGamepad.leftYAxis(ax);
          break;
        case 2:
          NSGamepad.rightXAxis(ax);
          break;
        case 5:
          NSGamepad.rightYAxis(ax);
          break;
        case 9:
          NSGamepad.dPad(ax);
          break;
        default:
          break;
      }
    }
  }

  NSGamepad.buttons((uint16_t)buttons);
}

// Process input events from Logitech Extreme 3D Pro flight control stick
//
// The Logitech Extreme 3D Pro joystick (also known as a flight stick)
// has a large X,Y,twist joystick with an 8-way hat switch on top.
// This maps the large X,Y axes to the gamepad right thumbstick and
// the hat switch to the gamepad left thumbstick. There are six
// buttons on the top of the stick and six on the base. The twist
// used to control the stick buttons. Each gamepad thumbstick is
// also a button. For example, clicking the right thumbstick enables
// stealth mode in Zelda:BOTW.
//
// Map LE3DP button numbers to NS gamepad buttons
//    LE3DP buttons
//    0 = front trigger
//    1 = side thumb rest button
//    2 = top large left
//    3 = top large right
//    4 = top small left
//    5 = top small right
//
// Button array (2 rows, 3 columns) on base
//
//    7 9 11
//    6 8 10
void handle_le3dp(int joystick_index)
{
  static const uint8_t BUTTON_MAP[12] = {
    NSButton_A,             // Front trigger
    NSButton_B,             // Side thumb trigger
    NSButton_X,             // top large left
    NSButton_Y,             // top large right
    NSButton_LeftTrigger,   // top small left
    NSButton_RightTrigger,  // top small right
    NSButton_Minus,
    NSButton_Plus,
    NSButton_Capture,
    NSButton_Home,
    NSButton_LeftThrottle,
    NSButton_RightThrottle
  };

  uint64_t axis_mask = joysticks[joystick_index].axisMask();

  for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
    if (axis_mask & 1) {
      int ax = joysticks[joystick_index].getAxis(i);
      switch (i) {
        case 0:
          // Big stick X axis
          NSGamepad.leftXAxis(map(ax, 0, 0x3FF, 0, 255));
          break;
        case 1:
          // Big stick Y axis
          NSGamepad.leftYAxis(map(ax, 0, 0x3FF, 0, 255));
          break;
        case 5:
          //Twist
          break;
        case 6:
          //Slider
          break;
        case 9:
          // direction pad (hat switch)
          // Convert direction to X,Y for right thumbstick
          NSGamepad.rightXAxis(dPad2dir_table[ax].xDir);
          NSGamepad.rightYAxis(dPad2dir_table[ax].yDir);
          break;
        default:
          break;
      }
    }
  }

  uint32_t buttons = joysticks[joystick_index].getButtons();
  for (uint8_t i = 0; i < sizeof(BUTTON_MAP); i++) {
    if (buttons & (1 << i)) {
      NSGamepad.press(BUTTON_MAP[i]);
    }
    else {
      NSGamepad.release(BUTTON_MAP[i]);
    }
  }
}

// Process input events from Thrustermaster T.16000 flight control stick
//
// Map T16K button numbers to NS gamepad buttons
// The Thrustmaster T.16000M ambidextrous joystick (also known as a flight stick)
// has a large X,Y,twist joystick with an 8-way hat switch on top.
// This function maps the large X,Y axes to the gamepad right thumbstick and
// the hat switch to the gamepad left thumbstick. There are four
// buttons on the top of the stick and 12 on the base. The twist
// used to control the stick buttons. Each gamepad thumbstick is
// also a button. For example, clicking the right thumbstick enables
// stealth mode in Zelda:BOTW.
//
//    Map T16K button numbers to NS gamepad buttons
//    T16K buttons
//    0 = trigger
//    1 = top center
//    2 = top left
//    3 = top right
//
//    Button array on base, left side
//
//    4
//    9 5
//      8 6
//        7
//
//    Button array on base, right side
//
//          10
//       11 15
//    12 14
//    13
void handle_t16k(int joystick_index)
{
  static const uint8_t BUTTON_MAP[16] = {
    NSButton_A,             // Trigger
    NSButton_B,             // Top center
    NSButton_X,             // Top Left
    NSButton_Y,             // Top Right
    NSButton_LeftTrigger,   // Base left 4
    NSButton_RightTrigger,  // Base left 5
    NSButton_Minus,         // Base left 6
    NSButton_Plus,          // Base left 7
    NSButton_Capture,       // Base left 8
    NSButton_Home,          // Base left 9
    NSButton_LeftStick,     // Base right 10
    NSButton_RightStick,    // Base right 11
    NSButton_LeftThrottle,  // Base right 12
    NSButton_RightThrottle, // Base right 13
    NSButton_Reserved1,     // Base right 14
    NSButton_Reserved2      // Base right 15
  };
  uint64_t axis_mask = joysticks[joystick_index].axisMask();

  for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
    if (axis_mask & 1) {
      int ax = joysticks[joystick_index].getAxis(i);
      switch (i) {
        case 0:
          // Big stick X axis
          NSGamepad.leftXAxis(map(ax, 0, 0x3FFF, 0, 255));
          break;
        case 1:
          // Big stick Y axis
          NSGamepad.leftYAxis(map(ax, 0, 0x3FFF, 0, 255));
          break;
        case 5:
          //Twist
          break;
        case 6:
          //Slider
          break;
        case 9:
          // direction pad (hat switch)
          // Convert direction to X,Y for right thumbstick
          NSGamepad.rightXAxis(dPad2dir_table[ax].xDir);
          NSGamepad.rightYAxis(dPad2dir_table[ax].yDir);
          break;
        default:
          break;
      }
    }
  }

  uint32_t buttons = joysticks[joystick_index].getButtons();
  for (uint8_t i = 0; i < sizeof(BUTTON_MAP); i++) {
    if (buttons & (1 << i)) {
      NSGamepad.press(BUTTON_MAP[i]);
    }
    else {
      NSGamepad.release(BUTTON_MAP[i]);
    }
  }
}

// Process input events from two DragonRise joysticks. Each joystick has
// one stick and up to 12 buttons. Two are required to create a gamepad.
//
// Map two Dragon Rise arcade joysticks to one NS gamepad The Dragon Rise
// arcade joystick has 1 stick and up to 10 buttons. Two are required to make 1
// gamepad with 2 sticks plus 18 buttons. The dragonFirst flag determines which
// joystick is the left or right side of the gamepad.
//
void handle_dragonrise(int joystick_index)
{
  // Left side of gamepad
  static const uint8_t BUTTON_MAP_LEFT[12] = {
    NSButton_LeftThrottle,
    NSButton_LeftTrigger,
    NSButton_Minus,
    255,    // DPAD Up
    255,    // DPAD Right
    255,    // DPAD Down
    255,    // DPAD Left
    NSButton_LeftStick,
    NSButton_Capture,
    NSButton_Reserved1,
    NSButton_Reserved1,
    NSButton_Reserved1
  };
  // Right side of gamepad
  static const uint8_t BUTTON_MAP_RIGHT[12] = {
    NSButton_RightThrottle,
    NSButton_RightTrigger,
    NSButton_Plus,
    NSButton_A,
    NSButton_B,
    NSButton_X,
    NSButton_Y,
    NSButton_RightStick,
    NSButton_Home,
    NSButton_Reserved2,
    NSButton_Reserved2,
    NSButton_Reserved2
  };

  static uint8_t dragonFirst = 255;
  static uint8_t dpad_bits = 0;
  uint64_t axis_mask = joysticks[joystick_index].axisMask();

  if (joystick_index < dragonFirst) dragonFirst = joystick_index;
  for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
    if (axis_mask & 1) {
      int ax = joysticks[joystick_index].getAxis(i);
      switch (i) {
        case 0:
          if (dragonFirst == joystick_index)
            NSGamepad.rightXAxis(ax);
          else
            NSGamepad.leftXAxis(ax);
          break;
        case 1:
          if (dragonFirst == joystick_index)
            NSGamepad.rightYAxis(ax);
          else
            NSGamepad.leftYAxis(ax);
          break;
        default:
          break;
      }
    }
  }

  uint32_t buttons = joysticks[joystick_index].getButtons();
  const uint8_t *button_map_ptr;
  size_t button_map_size;
  uint8_t button_out;
  if (joystick_index == dragonFirst) {
    button_map_size = sizeof(BUTTON_MAP_RIGHT);
    button_map_ptr = BUTTON_MAP_RIGHT;
  }
  else {
    button_map_size = sizeof(BUTTON_MAP_LEFT);
    button_map_ptr = BUTTON_MAP_LEFT;
  }
  for (uint8_t i = 0; i < button_map_size; i++) {
    button_out = button_map_ptr[i];
    if (button_out == 255) {  // direction pad button
      if (buttons & (1 << i)) {
        dpad_bits |= (1 << (i - 3));
      }
      else {
        dpad_bits &= ~(1 << (i - 3));
      }
      NSGamepad.dPad(DPAD_MAP[dpad_bits]);
    }
    else {
      if (buttons & (1 << i)) {
        NSGamepad.press(button_out);
      }
      else {
        NSGamepad.release(button_out);
      }
    }
  }
}

//=============================================================================
// loop
//=============================================================================
void loop()
{
  myusb.Task();
  PrintDeviceListChanges();

  for (int joystick_index = 0; joystick_index < COUNT_JOYSTICKS; joystick_index++) {
    if (joysticks[joystick_index].available()) {
      JoystickController::joytype_t joystickType = joysticks[joystick_index].joystickType();
      switch (joystickType) {
        case JoystickController::HORIPAD:
          handle_horipad(joystick_index);
          break;
        case JoystickController::EXTREME3D: // Logitech Extreme 3D Pro
          handle_le3dp(joystick_index);
          break;
        case JoystickController::T16000M: // Thrustmaster T.16000M
          handle_t16k(joystick_index);
          break;
        case JoystickController::DRAGONRISE:
          handle_dragonrise(joystick_index);
          break;
        default:
          break;
      }
      if (JOY_DEBUG) Serial1.println();
      joysticks[joystick_index].joystickDataClear();
    } /* if joystick available */
  } /* for joystick_index */
  NSGamepad.loop();
}

//=============================================================================
// Show when devices are added or removed
//=============================================================================
void PrintDeviceListChanges()
{
  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial1.printf("*** Device %s - disconnected ***", driver_names[i]);
        Serial1.println();
        driver_active[i] = false;
      } else {
        Serial1.printf("*** Device %s %x:%x - connected ***", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        Serial1.println();
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) {
          Serial1.printf("  manufacturer: %s", psz);
          Serial1.println();
        }
        psz = drivers[i]->product();
        if (psz && *psz) {
          Serial1.printf("  product: %s", psz);
          Serial1.println();
        }
        psz = drivers[i]->serialNumber();
        if (psz && *psz) {
          Serial1.printf("  Serial: %s", psz);
          Serial1.println();
        }
      }
    }
  }

  for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) {
    if (*hiddrivers[i] != hid_driver_active[i]) {
      if (hid_driver_active[i]) {
        Serial1.printf("*** HID Device %s - disconnected ***", hid_driver_names[i]);
        Serial1.println();
        hid_driver_active[i] = false;
      } else {
        Serial1.printf("*** HID Device %s %x:%x - connected ***", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        Serial1.println();
        hid_driver_active[i] = true;

        const uint8_t *psz = hiddrivers[i]->manufacturer();
        if (psz && *psz) {
          Serial1.printf("  manufacturer: %s", psz);
          Serial1.println();
        }
        psz = hiddrivers[i]->product();
        if (psz && *psz) {
          Serial1.printf("  product: %s", psz);
          Serial1.println();
        }
        psz = hiddrivers[i]->serialNumber();
        if (psz && *psz) {
          Serial1.printf("  Serial: %s", psz);
          Serial1.println();
        }
      }
    }
  }
}
