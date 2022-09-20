# USB NS Gamepad

Teensy LC impersonates a USB NS Gamepad. This should also work on Teensy
3 and Teensy 4.

Select "NS Gamepad" from the "Tools > USB Type" menu.

Pushbuttons should be connected between the digital pins and ground.

For maximum compatibility with the NS, the USB serial and other USB endpoints
are not present. The USB descriptors are as much as possible the same as the
Hori gamepad controller. One consequence is the Teensy auto upload does not
work. But this is easy to fix. Use the IDE Upload function. After the upload
fails, press the Prog button on the Teensy to start the upload.

## Teensy LC pin out with button and axis assignments

For a graphic Teensy LC pin out diagram see the [following](https://www.pjrc.com/teensy/teensyLC.html).
Teensy 3.x/4.x should also work but the LC is sufficient.

|Function       |LC Pin |LC Pin |Function      |
|---------------|-------|-------|--------------|
|               |GND    |Vin    |              |
|RX1            |0      |GND    |              |
|TX1            |1      |3.3V   |              |
|Up             |2      |23/A9  |Y             |
|Right          |3      |22/A8  |B             |
|Down           |4      |21/A7  |A             |
|Left           |5      |20/A6  |X             |
|Left throttle  |6      |19/A5  |Right throttle|
|Left trigger   |7      |18/A4  |Right trigger |
|Minus          |8      |17/A3  |Left X axis   |
|Left stick     |9      |16/A2  |Left Y axis   |
|Capture        |10     |15/A1  |Right X axis  |
|Home           |11     |14/A0  |Right Y axis  |
|Plus           |12     |13     |Right stick   |

Serial1 (RX1/TX1) pins are available for debug output.

For graphic joy-con diagrams see the [following](https://en-americas-support.nintendo.com/app/answers/detail/a_id/22634/~/joy-con-controller-diagram).

Note 1: The NS thumbsticks are clickable so they also count as buttons. See
"Left Stick" and "Right Stick" above.

Note 2: The direction buttons (Up, Right, Down, Left) are reported as an 8
direction D-pad so some combinations cannot be reported.
