
A bit intro to the ocasion which forced me to make this project :-) I very much love moOde audio player and I'm using it almost since it has born. I've made a couple of players and I'm been listening to them for a days and nights but since then I'm strugelling for a decent remote control. Have tryed a lot of DIY projects but no one ot them covered my points.

link to youtube movie: https://www.youtube.com/watch?v=-xvVthnFq_s

Advanced IR remote control receiver for moOde audio player is for Arduino Pro Micro 32u4 board, and it is able even to control Vol+ and Vol- but more important is that it is capable to make PowerOff, Reset and PowerOn safely to the SdCard files, it gives command to the RPi to make PowerOff and to close all opened files, then it shuts down the power supply to the RPi. My device is acting as a media keyboard via USB HID and uses same interface to send commands for Powering Off and On the device.

Power Off/Reset commands work by default but for using Vol +/- you have to enable "USB Volume Knob Setting" in Audio settings. You can add some more options by modifying "/etc/triggerhappy/triggers.d/media.conf". 

My credits go to:
1. The briliant Tim Curtis and his moOde audio player: https://github.com/moodeaudio
2. The magician of software Nico Hood, I'll be very happy one day I completely undestand how his HID-Projects work: https://github.com/NicoHood/HID
3. Same as the famous HID-Projects Nico Hood has a very nice IR library as well: https://github.com/NicoHood/IRLremote
4. Thanks to Paul Stoffregen and his very usefull library for reading Rotaty Encoders: http://www.pjrc.com/teensy/td_libs_Encoder.html  
5. A couple of nice and handy libraries for buttons and leds: https://github.com/SethSenpai/singleLEDLibrary , https://arduinogetstarted.com/tutorials/arduino-button-library

Feel free to use it on your own risk , all the schematics and necessary explanation are included in schematics folder.
