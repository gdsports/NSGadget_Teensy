# Arduino NS Gamepad for Teensy

This project adds a USB mode to the Teensy LC, 3.x, and 4.x development
boards that allows them to act as a Nintendo Switch (NS) compatible gamepad.
This is useful for building an arcade console or to adapt other USB devices
for use with the NS.

In addition to the NS USB mode, this project modifies the [`USBHost_t36` library](https://github.com/PaulStoffregen/USBHost_t36)
to add support for USB HID flight sticks and gamepads so that they can be used with the NS.

```cpp
	{ 0x0F0D, 0x00c1, HORIPAD, true},    // Hori Horipad NS compatible gamepad
	{ 0x0079, 0x0006, DRAGONRISE, true}, // Dragon Rise Fightstick/joystick
	{ 0x046D, 0xC215, EXTREME3D, true},  // Logitech Extreme 3D Pro
	{ 0x044F, 0xB10A, T16000M, true}     // Thrustmaster T.16000M FCS
```

The Teensy 3.6/4.x acts as a Teensy-in-the-middle between one or more game
controllers and the NS. This can be used for co-pilot mode as well as simulator
control panel/button boxes (car, truck, tractor, etc).

This project also provides additional USB device types for Teensy LC, 3.x and
4.x:

* `Joystick` provides a generic USB joystick. This is known to work with
the Xbox Adaptive Controller (XAC). The XAC is confused by the extra
USB descriptors in the other Teensy USB Joystick types.

* `Joystick + Serial` provides a generic USB joystick with CDC ACM USB serial.

This project has not had much testing so watch out for potholes.

If you prefer Python, see the Raspberry Pi version of NSGadget. The Pi has
enough GPIOs for NS buttons and the model B has 4 USB host ports. The Pi Zero
is the cheapest option but it is hard to find. See
https://github.com/gdsports/NSGadget_Pi.

## Install

Install the [Arduino IDE 1.8.13](https://www.arduino.cc/en/Main/Software) and
[Teensyduino 1.53](https://www.pjrc.com/teensy/td_download.html). I highly
recommend extracting the IDE zip or tar in a separate directory from the
default Arduino directory.

On a Linux system, the following instructions will install the IDE into
`~/nsg/arduino-1.8.13`. Creating the portable directory ensures the sketches and
libraries are stored separately from the default Arduino sketches and
libraries.

```
cd
mkdir nsg
cd nsg
tar xf ~/Downloads/arduino-1.8.13-linux64.tar.xz
cd arduino-1.8.13
mkdir portable
```

Run the Teensyduino installer. Make sure to install in the arduino-1.8.13
directory created above.

Copy the files in this repo's hardware directory to the arduino-1.8.13/hardware
directory. This will overwrite Teensyduino files.

Start the IDE with all changes. The examples should build without changes. If
compile errors occur, be sure the Board is set to "Teensy 3.6" and the USB Type
is set to "NS Gamepad".

```
cd ~/nsg/arduino-1.8.13
./arduino&
```

## Usage

To use the NS USB mode, select "NS Gamepad" from the "Tools > USB Type" menu.

For maximum compatibility with the NS, the USB serial and other USB endpoints
are not present. The USB descriptors are as much as possible the same as the
Hori gamepad controller. One consequence is the Teensy auto upload does not
work. But this is easy to fix. Use the IDE Upload function. After the upload
fails, press the "Prog" button on the Teensy to start the upload.

Gamepad output is managed by the `NSGamepad` instance:

```cpp
	NSGamepad.begin();            // initialize the class
	NSGamepad.press(NSButton_A);  // set the 'A' button state
	NSGamepad.write();            // write to USB
```

### D-Pad

The direction buttons have eight possible states:

```
NSGAMEPAD_DPAD_CENTERED
NSGAMEPAD_DPAD_UP
NSGAMEPAD_DPAD_UP_RIGHT
NSGAMEPAD_DPAD_RIGHT
NSGAMEPAD_DPAD_DOWN_RIGHT
NSGAMEPAD_DPAD_DOWN
NSGAMEPAD_DPAD_DOWN_LEFT
NSGAMEPAD_DPAD_LEFT
NSGAMEPAD_DPAD_UP_LEFT
```

These can be set using the `NSGamepad.dPad()` function:

```cpp
	NSGamepad.dPad(NSGAMEPAD_DPAD_UP);
```

Note that the direction buttons (Up, Right, Down, Left) are reported as an 8
direction D-pad so some combinations cannot be reported.

### Buttons

There are 14 buttons on the NS gamepad:

```
NSButton_Y 
NSButton_B
NSButton_A
NSButton_X
NSButton_LeftTrigger
NSButton_RightTrigger
NSButton_LeftThrottle
NSButton_RightThrottle
NSButton_Minus
NSButton_Plus
NSButton_LeftStick
NSButton_RightStick
NSButton_Home
NSButton_Capture
```

Buttons can be set using the `press()` and `release()` functions:

```cpp
	NSGamepad.press(NSButton_A);
	NSGamepad.release(NSButton_A);
```

Note that the NS thumbsticks are clickable so they also count as buttons. See
`LeftStick` and `RightStick` above.

All buttons can be set at once using the `buttons()` function. This function
takes a bitfield of button states as its argument.

### Joysticks

There are two joysticks on the NS gamepad: left and right. Each axis can be
set using its respective function:

```
leftXAxis(uint8_t)
leftYAxis(uint8_t)
rightXAxis(uint8_t)
rightYAxis(uint8_t)
```

The joystick values are in the range 0-255, with 127 being centered.

## Examples

| Example                           | Description                                                 |
|-----------------------------------|-------------------------------------------------------------|
| [NSGamepad](examples/NSGamepad)   | Convert the Teensy pins to NS inputs                        |
| [NSPassthru](examples/NSPassthru) | Using USB host, pass data from USB controllers to NS inputs |
| [NSMIDI](examples/NSMIDI)         | Using USB host, convert MIDI note on/off data to NS inputs  |

See the example-specific documentation in each folder for reference.

## License

The original Teensy core files and their modified versions are licensed under a modified version of the permissive [MIT license](https://opensource.org/licenses/MIT). Newly contributed files are licensed under the MIT license with no additional stipulations.

See the [LICENSE](LICENSE.txt) file for more information.
