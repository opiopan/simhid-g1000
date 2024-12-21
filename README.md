SimHID G1000
===
<p align="center">
<img alt="description" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/movie.gif">
</p>

SimHID G1000 is a input device that imitates Garmin G1000 glass cockpit system for controlling flight simulators.<br>
This device is designed to utilize a iPad as a display.
iPad, iPad Air or iPad Pro with a display from 10.2 inches to 11 inches can be applied to this device.<br>

SimHID G1000 not only has 41 push buttons and 14 axes rotary encoders like the actual Garmin G1000,
but also has a total 6 input switches on both side of left and right sides.
These additional switches allow you to switch one device between PFD and MFD for both purposes.<br>

SimHID G1000 behaves as a USB virtual serial device, and communicate with the host using proprietary protocol named SimHID protocol.
In order to recognize this protocol and to control switching between PFD and MFD, I made a Windows application called [fsmapper](https://opiopan.github.io/fsmapper).


## How to build
<p align="center">
<img alt="description" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/assembly.jpg" width=750>
</p>

The above image shows all parts that make up SimHID G1000.<br>
Most parts are 3D printed by FDM, but some parts are cut out and engraved from a 2mm thick acrylic plate using a laser cutter.
The model data for 3D printer and laser cutter are published at [Thingiverse](https://www.thingiverse.com/thing:4966493). Regarding these models, please refer [here](https://www.thingiverse.com/thing:4966493).

In this Github repository, PCB design and firmware source codes are hosted.<br>
Regarding PCB design related information include manufacturing, please refer README at [here](pcb).<br>

Once alll parts of SimHID G1000 are prepared, please assemble those according to [this procedure](docs/how-to-assemble.md).

## Compatible iPad
<img alt="board image" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/ipad.jpg" width=400 align="right">

SimHID G1000 uses an iPad as display. 
I assume that an iPad behave as second display of Windows PC by using an application such as [Duet Display](https://duetdisplay.com),
and show PFD or MFD images rendered by a flight simulator.<br>
An iPad can be easily inserted from the top of the SimHID G1000 along the guide like as the right image.<br>
I've confirmed to fit with SimHID G1000 only in case of iPad Air gen 3, 
but I believe that all iPad families with display from 10.2 inches to 11 inches can be fit.

The iPad list compatible with SimHID G1000 is below (as of Sep. 2021).

|Screen Size | iPad Model                         |
|:----------:|------------------------------------|
|10.2″       | iPad Gen 7<br>iPad Gen 8<br>iPad Gen 9|
|10.5″       | iPad Pro Gen 2<br>iPad Air Gen 3     |
|10.9″       | iPad Air Gen 4                      |
|11″         | iPad Pro 11″ Gen 1<br>iPad Pro 11″ Gen 2<br>iPad Pro 11″ Gen 3|



## Compiling firmware
**Note**: |
:----|
The pre-built firmware can be downloaded from [here](https://github.com/opiopan/simhid-g1000/releases/latest/). If you haven’t modified the firmware code, using this version will be easier.


1. **Requirements**<br>
    Ensure that the [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) is installed and its bin folder is added to the `PATH` environment variable.
    Note that you need to install the toolchain for **AArch32 bare-metal** (`arm-none-eabi` variant).

    The Makefile for this firmware code is written with the assumption that it will be built in a Unix-like environment.
    If you are building on Windows, I strongly recommend using the Linux version of the Arm GNU Toolchain on WSL2.


2. **Downloading Source Codes**<br>
    ```shell
    $ git clone https://github.com/opiopan/simhid-g1000.git
    ```

3. **Compiling**
    ```shell
    $ cd simhid-g1000/firmware
    $ make all

## Programming Firmware
Once compiling as the above steps is done, following firmware binary files are made.
- ```firmware/build/simhid-g1000.elf``` : ELF format binary
- ```firmware/build/simhid-g1000.bin``` : raw binary

You can choose two way to program firmware to SimHID G1000.<br>
One is programming firmware via SWD by using debugger.  SimHID G1000 PCB exports a SWD I/F and you can access that from backside of this device.<br>
The other one is programming it via USB port in DFU mode. You can switch SimHID G1000 in DFU mode by inserting that to USB port with the button at right side pressed.

### Programming via Debugger
1. **Preparing Debugger**<br>
    Make sure that a debugger is connected with swd port and debugger controlling software which behave as GDB server such as OpenOCD is running.

2. **Programming**<br>
    You can program a firmware by making ```flash``` target with ```DEBUGSERVER``` parameter.
    If debugger controlling software is running same PC, specify ```localhost``` for ```DEBUGSERVER``` parameter.

    ```shell
    $ make DEBUGSERVER=localhost flash
    ```
### Programming via USB
<img alt="board image" src="https://raw.githubusercontent.com/wiki/opiopan/simhid-g1000/images/dfu.jpg" width=300 align="right">

1. **Install required software**<br>
    Make sure that software with programming function via DFU is installed.
    Following softwares are populor to use for this purpose.
    - [STM32CubeMXProgrammer](https://www.st.com/ja/development-tools/stm32cubeprog.html)
    - [dfu-util](http://dfu-util.sourceforge.net)

2. **Switching SimHID G1000 in DFU mode**<br>
    Hold down the button on the right side of SimHID G1000 and connect that with USB port on PC.

3. **Programming**<br>
    In case using dfu-util, you can program just by making ```dfuflash``` target.

    ```shell
    $ make dfuflash
    ```

    Othrewise, follow the instructions for the software you are using.

4. **Exsiting from DFU mode**<br>
    Pull out SimHID G1000 from USB port on PC, then re-connect that to USB port.

## SimHID protocol
SimHID G1000 communicate with PC using a proprietary protocol named SimHID protocol via USB virtual serial port.<br>
Initially, I was considering that SimHID G1000 behaves as USB HID device which can be handled by Windows standard USB gaming controller device driver.
However in the end, I decided to implement a proprietary protocol because of two issues.<br>

One issue is distinguishability of each operable elements.<br>
SimHID G1000 consists of 47 binary switches (push buttons) and 14 axes rotary encoders. when each rotary encoders is expressed 2 push buttons, such as for clockwise and for counter clockwise, SimHID G1000 is recognized as a device that consists of 75 buttons. In case handling this device by the standard USB HID gaming controller device driver, all buttons are distinguished only by their serial number. For example, the push button on top of the rotary encoder for setting the navigation frequency is #12, the switch representing the left of joystick for map panning is #62.<br>
This situation is very complicated for me when I setting up the mappings between SimHID G1000 operation and aircraft control. 
I wanted to assign each operable elements a descriptive name.

The other one issue was actually based on my misunderstanding regarding rotary encoder performance.<br>
I estimated that multiple clicks by rotary encoder's rotation can occur during the communication interval with host. It means that click event may be dropped if each click event is expressed as a simple push button.<br>
So, I tried to express each axes of rotary encoder as a analog axis of USB gaming controller device.
However, I soon realized that this plan doesn't work because Windows standard USB gaming controller device driver limits the number of analog axes to a maximum of 8
even though there is no limit in the USB HID specification.<br>
In practice, only one click event occurs during the communication interval with host in normal usage, and there is no problem even if each rotation click event is expressed as push button down and up event.<br>

In summary, I decided to design a new proprietary protocol and implement that due to the former issue.

SimHID protocol is designed to be both human readable and machine readable.
You can observe communication data between a SimHID device and a host by using general terminal emulation software with serial port connectivity such as screen command.<br>
All commands to the device, responses from the device, and asynchronous events occurred by device operation are expressed one line text ending with end of line mark. The end of line mark consists combination of CR and LF, in other words, ```0Dh``` and ```0Ah``` byte sequences or ```"\r\n"``` C string.

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

The above examples shows the cases of notification from the device to the host.
On the other hand, SimHID protocol also assumes tha cases that the host send data to a device in order to change status of a element of a device, such as turning on a LED.<br>
This upstream communication will be done by same format data, "```S <element-name> <value> \r\n```".

Regarding the details of SimHID protocol specification, please refer [this document](docs/simhid-protocol.md).

## Companion software
As mentioned above, I made a windows application, [fsmapper](https://opiopan.github.io/fsmapper), to control a flight simulator by using SimHID G1000.

fsmapper not only allows you to use the SimHID G1000 as a Garmin G1000, but it can also recreate virtual instrument panels for various aircraft on a touch display, and integrate common control devices such as joysticks and throttles.
For examples of operating various aircraft using fsmapper and the SimHID G1000 combination, refer to the following video.

[![Showcase](https://github.com/user-attachments/assets/79c6737c-9762-48b7-a1d1-e438c90e13ff)](https://youtu.be/rIp1M2r1_ko)

[![Integrating multiple instruments](https://github.com/user-attachments/assets/c6628980-0b5b-4b8c-8cbf-01d03524c85b)](https://youtu.be/tjl9g2BJMeQ)
