LightBright
===========

Rainbow LED Sneakers with Heel Sensor


This project is a modified version of the 'Firewalker' LED sneaker tutorial available from Adafruit. https://learn.adafruit.com/firewalker-led-sneakers/overview

A few changes to note:

1) I did not use a Neopixel LED array because Adafruit was sold out of the white 60 LED per meter strips, and the rubber on my sneakers is white. Instead, I bought an addressable, waterproofed RGB LED strip (with white component board) from another vendor.

2) I did not use the same pair of shoes. Instead I used a pair of obnoxiously rainbow high top skateboard sneakers that had a thick tongue and metal enforced vents on the insides of each shoe.

3) Instead of sewing the Flora to the outside of the shoe, I sewed it to the inside of the tongue. I found it easier to route the wires this way, since the LED wires fit perfectly through the side vents.

4) Instead of the fire-hued wave animation of the Firewalker project, I wanted a nice rainbow of balanced hues that would turn on in a wave from heel to toe, then turn off in a wave from heel to toe. Much of the original code used the intensity of the sensor reading to determine which firey hue was used, so I rewrote a lot of it in favor of rainbow making code.

5) The LED layout and sensor readings for my right foot were a bit different than my left, which is the one I used while developing. To compensate for this, I copypasta'd the code for my left foot, modified as needed, and saved it as a separate file.

