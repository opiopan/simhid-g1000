SimHID G1000
===
<p align="center">
<img alt="description" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/movie.gif">
</p>

SimHID G1000 is a input device that imitates a Garmin G1000 glass cockpit system for controlling flight simulators.<br>
This device is designed to utilise an iPad as a display.
iPad, iPad Air or iPad Pro's with a display from 10.2 inches to 11 inches can be used with this device.<br>

The SimHID G1000 not only has 41 push buttons and 14 axes rotary encoders like the actual Garmin G1000,
but also has a total 6 input switches on both sides of the device.
These additional switches allow you to switch one device between PFD and MFD modes.<br>

The SimHID G1000 behaves as a USB virtual serial device, and communicates with the host using a proprietary protocol named SimHID protocol.
In order to recognize this protocol and to control switching between PFD and MFD, I made a Windows application called [fsmapper](https://github.com/opiopan/fsmapper).


## How to build
<p align="center">
<img alt="description" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/assembly.jpg" width=750>
</p>

The above image shows all parts that make up the SimHID G1000.<br>
Most parts are 3D printed by FDM printing, but some parts are cut out and engraved from a 2mm thick acrylic plate using a laser cutter.
The model data for 3D printing and laser cutting are published at [here on Thingiverse](https://www.thingiverse.com/thing:4966493).
In this Github repository, the PCB design and firmware source codes are hosted.<br>
Regarding the PCB design including manufacturing details, please refer to me [here](pcb).
<br>

Once all parts of the SimHID G1000 are prepared, please assemble them according to [this procedure](docs/how-to-assemble.md).

## Compatible iPad
<img alt="board image" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/ipad.jpg" width=400 align="right">

SimHID G1000 uses an iPad as display. 
This is done using a application like [Duet Display](https://duetdisplay.com). This allows the iPad to act as a secondary display and shows the PFD or MFD images as rendered 
by a flight simulator.
<br>
An iPad can be easily inserted from the top of the SimHID G1000 along the guide as shown on the right.<br>
To date, I have confirmed a fit with the iPad Air gen 3 but all iPad models from 10.2 inches to 11 inches should fit. 


The iPad list compatible with SimHID G1000 is below (as of Sep. 2021).

|Screen Size | iPad Model                         |
|:----------:|------------------------------------|
|10.2″       | iPad Gen 7<br>iPad Gen 8<br>iPad Gen 9|
|10.5″       | iPad Pro Gen 2<br>iPad Air Gen 3     |
|10.9″       | iPad Air Gen 4                      |
|11″         | iPad Pro 11″ Gen 1<br>iPad Pro 11″ Gen 2<br>iPad Pro 11″ Gen 3|



## Compiling firmware
1. **Requirements**<br>
The [GNU Arm Embedded Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
must be installed in a directory indicated by `PATH` environment variable.

2. **Downloading Source Codes**<br>
    ```shell
    $ git clone https://github.com/opiopan/simhid-g1000.git
    ```

3. **Compiling**
    ```shell
    $ cd simhid-g1000/firmware
    $ make all

## Programming Firmware
Once compiled as the above steps, the following firmware binary files are made.
- ```firmware/build/simhid-g1000.elf``` : ELF format binary
- ```firmware/build/simhid-g1000.bin``` : raw binary

You can choose two ways to program the firmware to SimHID G1000.<br>
One is programming the firmware via the SWD by using a debugger.  The SimHID G1000 PCB exports a SWD I/F and you can access that from the backside of this device.<br>
The other method is programming it via the USB port in DFU mode. You can switch to the SimHID G1000 in DFU mode by inserting it to the USB port with the button at right side held down.

### Programming via Debugger
1. **Preparing Debugger**<br>
    Make sure that a debugger is connected with SWD port and debugger controlling software which acts as a GDB server with OpenOCD is running.

2. **Programming**<br>
    You can program a firmware by making ```flash``` target with ```DEBUGSERVER``` parameter.
    If debugger controlling software is running on the same PC, specify ```localhost``` for ```DEBUGSERVER``` parameter.

    ```shell
    $ make DEBUGSERVER=localhost flash
    ```
### Programming via USB
<img alt="board image" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/dfu.jpg" width=300 align="right">

1. **Install required software**<br>
    Make sure that software with programming function via the DFU is installed.
    The following programs are popular to use for this purpose.
    - [STM32CubeMXProgrammer](https://www.st.com/ja/development-tools/stm32cubeprog.html)
    - [dfu-util](http://dfu-util.sourceforge.net)

2. **Switching SimHID G1000 in DFU mode**<br>
    Hold down the button on the right side of theSimHID G1000 and connect with a USB port on PC.

3. **Programming**<br>
    In case using dfu-util, you can program just by making ```dfuflash``` target.

    ```shell
    $ make dfuflash
    ```

    Otherwise, follow the instructions for the software you are using.

4. **Exsiting from DFU mode**<br>
    Pull out the SimHID G1000 from USB port on PC, then re-connect that to USB port.

## SimHID protocol
The SimHID G1000 communicates with the PC using a proprietary protocol named SimHID protocol via a USB virtual serial port.<br>
Initially, I was considering that the SimHID G1000 behaves as USB HID device which can be handled by a Windows standard USB gaming controller device driver.
However in the end, I decided to implement a proprietary protocol because of two issues.<br>

One issue is the distinguishably of each operatible elements.<br>
The SimHID G1000 consists of 47 binary switches (push buttons) and 14 axes of rotary encoders. When each rotary encoder has 2 bianry buttons, such as for clockwise and for counter clockwise, SimHID G1000 is recognized as a device that consists of 75 buttons. In case handling this device by the standard USB HID gaming controller device driver, all buttons are distinguished only by their serial number. For example, the push button on top of the rotary encoder for setting the navigation frequency is #12, the switch representing the left of joystick for map panning is #62.<br>
This situation is very complicated for me when I setting up the mappings between SimHID G1000 operation and aircraft control. 
I wanted to assign each operatible element a descriptive name.

The other issue was actually based on my misunderstanding regarding rotary encoder performance.<br>
I estimated that multiple clicks bya  rotary encoders rotation can occur during the communication interval with host. It means that the click event may be doroped if each click event is expressed as a simple push button.<br>
So, I tried to demonstrate each axis of the rotary encoders as a analog axis of a USB gaming controller device.
However, I soon realized that this plan doesn't work becase the Windows standard USB gaming controller device driver limits the number of analog axes to a maximum of 8
even though there is no limit in the USB HID specification.<br>
In practice, only one click event occurs during the communication interval with host in normal usage, and there is no problem even if each rotation click event is demonstrated as a push button down and up event.<br>

In summary, I decided to design a new proprietary protocol and implement that due to the former issue.

SimHID protocol is designed to be both human readable and machine readable.
You can observe communication data between a SimHID device and a host by using general terminal emulation software with serial port connectivity such as screen command.<br>
All commands to the device, responses from the device, and asynchronous events occured by device operation are expressed with a one line text ending with end of line mark. The end of line mark consists combination of CR and LF, in other words, ```0Dh``` and ```0Ah``` byte sequences or ```"\r\n"``` C string.

The following is a example of data sent from SimHID G1000 when the 100-feet unit altitude dial is rotated clockwise 3 clicks then rotated counterclockwise 2 clicks.
```
S EC4X 1
S EC4X 1
S EC4X 1
S EC4X -1
S EC4X -1
```

The following is a case of pushing the MENU button then releasing it.
```
S SW28 1
S SW28 0
```

The above examples shows the case of notification from the device to the host.
On the other hand, the SimHID protocol also assumes that the host sends data to a device in order to change status of a element of a device, such as turning on a LED.<br>
This upstream communication will be done by same format data, "```S <element-name> <value> \r\n```".

Regarding the details of the SimHID protocol specification, please refer to [this document](docs/simhid-protocol.md).

## Companion software
As mentioned above, I made a software called [fsmapper](https://github.com/opiopan/fsmapper), to control a flight simulator by using the SimHID G1000.<br>
[fsmapper](https://github.com/opiopan/fsmapper) can:
- Invoke Microsoft FS2020 events, such as turning autopilot on, according to the operation of SimHID devices or USB gaming controller device
- Change location, size, and/or visibility of pop-out windows of FS2020 such as G1000 PFD according to the operation of SimHID device of USB gaming controller device
- Change the mapping rule between SimHID device or USB gaming controller device operation and FS2020 events when device is operated or aircraft is changed
- Remap SimHID device or USB gaming controller device operation to virtual joystick device operation using vJoy driver

The above functions are configuread flexibly by writing a Lua scrip.

I believe that these functionalities must be already realised by existing software, and I also beleive that this my personal project must be reinventing the wheel.<br>
However I couldn't find the suitable solution which has all the functions listed above since I'm not familiar with flight simulator ecosystem. 
I still did it becuase I am a Flight Simulator hobbyist who was with the FS2020 screenshot and installe Windows to my PC for the first time in 10 years. 

Anyway, I made it and am using it with SimHID G1000.

I hope you have fun with it too. 
