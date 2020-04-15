Hello!

I made a "patch" for PinBox - a homebrew for 3DS

What included in this "patch":
- added c-Stick callibration
What functionality lost in this "patch":
- screen capture not work anymore

# Known issues
1) screen capture not work anymore
2) your wifi access point should have internet connection, else connection will be unstable
3) if 3ds not connecting to server or if no buttons work at all 
- restart server and 3ds app, try restarting your pc and check your wifi

# Installation
1) Use latest release of PinBox from
https://github.com/namkazt/PinBox
2) Google and install "ViGEmBus_Setup_1.16.116" - or newer version if exist
3) Download "Pinbox_Server_r0.2.4_patch_PinBoxServer_noScreen_Calibrated_cStick" 
extract to "Pinbox_Server_r0.2.4_test1" folder
and replace file "input.cfg" 
4) run PinBoxServer_noScreen_Calibrated_cStick.exe

# How to callibrate c-Stick
1) open "input.cfg"
You will see this - change values as you wish 
(this is config for my new3ds, where c-Stick shifting "down" was unresponsive and I added plus 80 percent acceleraion on it)

// c-stick calibration in percent - 
//example - 
//"20" -plus 20 percent power
//"-20" - minus 20 percent power

cStickUp = 50;
cStickDown = 80;
cStickRight = 50;
cStickLeft = 50;

2) restart server and 3ds client applications