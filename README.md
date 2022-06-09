
# LedSkates
This is a system of many wireless LEDs strips for roller skates or any other purpose. They are controlled by web page.

## Physical diagram
System is made of two [Wemos D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini_lite.html) boars. One of them is an Access Point, the second is a station connected to AP. They communicate using UDP. 

![Physical diagram](https://raw.githubusercontent.com/kristopalka/LED-Skates/main/gitresources/diagram.png)

## Web page
The Access Point is also a web server, which host a simple webpage to control LEDs.

![Web page](https://raw.githubusercontent.com/kristopalka/LED-Skates/main/gitresources/image2.png)

## Modes 
There are many modes for LEDs that you can choose.

![](https://github.com/kristopalka/LED-Skates/blob/main/gitresources/mode1.gif?raw=true)
![](https://github.com/kristopalka/LED-Skates/blob/main/gitresources/mode4.gif?raw=true)
![](https://github.com/kristopalka/LED-Skates/blob/main/gitresources/mode2.gif?raw=true)
![](https://github.com/kristopalka/LED-Skates/blob/main/gitresources/mode3.gif?raw=true)

## Running
I suggest opening repository as a [PlatformIO](https://platformio.org/) project, and upload it to board using platformio.ini file
