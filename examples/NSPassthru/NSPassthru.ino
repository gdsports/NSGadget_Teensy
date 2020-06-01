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


//=============================================================================
// loop
//=============================================================================
void loop()
{
  myusb.Task();
  PrintDeviceListChanges();

  static uint8_t dragonFirst = 255;
  for (int joystick_index = 0; joystick_index < COUNT_JOYSTICKS; joystick_index++) {
    if (joysticks[joystick_index].available()) {
      uint64_t axis_mask = joysticks[joystick_index].axisMask();
      uint64_t axis_changed_mask = joysticks[joystick_index].axisChangedMask();
      uint32_t buttons = joysticks[joystick_index].getButtons();
      static uint16_t dragonButtons = 0;
      JoystickController::joytype_t joystickType = joysticks[joystick_index].joystickType();
      if (JOY_DEBUG) Serial1.printf("Joystick(%d): buttons = %x", joystick_index, buttons);
      if (joystickType == JoystickController::DRAGONRISE) {
        if (joystick_index < dragonFirst) dragonFirst = joystick_index;
        if (dragonFirst == joystick_index) {
          dragonButtons = (dragonButtons & ~(0x1F << 9)) | ((buttons & 0x1F) << 9);
          NSGamepad.dPad(DPAD_MAP[(buttons >> 5) & 0xF]);
        }
        else {
          dragonButtons = (dragonButtons & ~0x1FF) | (buttons & 0x1FF);
        }
        NSGamepad.buttons(dragonButtons);
      }
      else {
        NSGamepad.buttons((uint16_t)buttons);
      }
      for (uint8_t i = 0; axis_mask != 0; i++, axis_mask >>= 1) {
        if (axis_mask & 1) {
          int ax = joysticks[joystick_index].getAxis(i);
          if (JOY_DEBUG) Serial1.printf(" %d:%d", i, ax);
          switch (joystickType) {
            case JoystickController::HORIPAD:
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
              break;
            case JoystickController::EXTREME3D: // Logitech Extreme 3D Pro
              switch (i) {
                case 0:
                  // Big stick X axis
                  NSGamepad.leftXAxis(map(ax, 0, 0x3FF, 0, 255));
                  break;
                case 1:
                  // Bit stick Y axis
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
              break;
            case JoystickController::T16000M: // Thrustmaster T.16000M
              switch (i) {
                case 0:
                  // Big stick X axis
                  NSGamepad.leftXAxis(map(ax, 0, 0x3FFF, 0, 255));
                  break;
                case 1:
                  // Bit stick Y axis
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
              break;
            case JoystickController::DRAGONRISE:
              switch (i) {
                case 0:
                  if (dragonFirst == joystick_index)
                    NSGamepad.leftXAxis(ax);
                  else
                    NSGamepad.rightXAxis(ax);
                  break;
                case 1:
                  if (dragonFirst == joystick_index)
                    NSGamepad.leftYAxis(ax);
                  else
                    NSGamepad.rightYAxis(ax);
                  break;
                default:
                  break;
              }
              break;
            default:
              break;
          } /* switch joystickType */
        } /* if (axis_mask */
      } /* for axis_mask */
      if (JOY_DEBUG) Serial1.println();
      joysticks[joystick_index].joystickDataClear();
    } /* if joystick available */
  } /* for joystick_index */
  NSGamepad.loop();
}

//=============================================================================
// Show when devices are added or removed
//=============================================================================
void PrintDeviceListChanges() {
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
