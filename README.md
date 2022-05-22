# Arduino NS Gamepad for Teensy

USB NS Gamepad provides a Nintendo Switch (NS) compatible gamepad. This is
useful for building an arcade console or to adapt other USB devices for use
with the NS.

This project adds USB HID flight sticks and gamepads to the USBHost_t36
library so they can be used with the NS.

```
	{ 0x0F0D, 0x00c1, HORIPAD, true},   // Hori Horipad NS compatible gamepad
	{ 0x0079, 0x0006, DRAGONRISE, true},// Dragon Rise Fightstick/joystick
	{ 0x046D, 0xC215, EXTREME3D, true}, // Logitech Extreme 3D Pro
	{ 0x044F, 0xB10A, T16000M, true}    // Thrustmaster T.16000M FCS
```

The Teensy 3.6/4.1 acts as a Teensy-in-the-middle between one or more game
controllers and the NS. This can be used for co-pilot mode as well as simulator
control panel/button boxes (car, truck, tractor, etc).

This project also provides additional USB device types for Teensy LC, 3.x and
4.x.

USB Joystick provides a generic joystick. This is known to work with
the Xbox Adaptive Controller (XAC). The XAC is confused by the extra
USB descriptors in the other Teensy USB Joystick types.

USB Serial + Joystick provides a generic joystick with CDC ACM USB serial.

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

## Examples

| Example                           | Description                                                 |
|-----------------------------------|-------------------------------------------------------------|
| [NSGamepad](examples/NSGamepad)   | Convert the Teensy pins to NS inputs                        |
| [NSPassthru](examples/NSPassthru) | Using USB host, pass data from USB controllers to NS inputs |
| [NSMIDI](examples/NSMIDI)         | Using USB host, convert MIDI note on/off data to NS inputs  |

See the example-specific documentation in each folder for reference.
