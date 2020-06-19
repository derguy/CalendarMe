# CalendarMe
This is just a project that I created for my son. At this moment (2020) he hasn't got his own mobilephone and I want to give him the oppertunity to see upcoming appointments. 

## Parts

### Adafruit TPL5110 Low Power Timer Breakout
<https://www.adafruit.com/product/3435>
This is a nice timer that is placed right in front of the Arduino-Board. It powers up the arduino at an interval (up to 2 hours) that can be changed by a poti. With this in place there is no need for a deep-sleep, which is - especially for the NodeMCU - to battery-consuming. 

For me the TPL5110 only works with the NodeMCU by soldering a diode from GND to DONE. 

### Waveshare 1.54inch e-Paper Module (B)
<https://www.waveshare.com/wiki/1.54inch_e-Paper_Module>
I bought the version with red color, but for this project there is no need for the color. My red color is so light that I doesn't want to use it. Probably that is a hardware-defect. If you have the BW-Version you have to use another constructor for it in the Code. 

### NodeMCU Lua Amica ESP8266
<https://www.az-delivery.de/en/products/nodemcu>

## Connect
I had some problems to get the right GPIO-Setup for the TPL5110 and e-Paper Module. Especially the DONE-Pin on the TPL only works on some GPIOs and the RST-Pin on the e-paper does not fit to every NodeMCU-pin. I think this is because the NodeMCU has a HIGH-status on some pins while powering up. 

![friting-diagramm](./CalendarMe.png)

## 3D-printed case
<https://www.thingiverse.com/thing:4483551>