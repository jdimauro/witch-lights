# Witch Lights

The Witch Lights are an interactive light art installation, meant to be seen outdoors at night. 

The documentation and code are currently under construction, so they may change without notice. 

## Overview

This project creates a set of [ghostly lights][vid0], which come out of the woods when you approach them and jump from tree to tree. 

Imagine: as you walk through the unlit path, a tiny purple faerie jumps out of the underbrush, coiling about trees and floating overhead. It zips ahead of you, but pauses, flickering, while you catch up. It then takes off again, remaining always just out of reach, until you reach the end of their territory, where they vanish back into the woods. As you pass by where they disappeared, the lights jump out again, heading back the other way, daring you to chase them.

<iframe src="https://player.vimeo.com/video/153011114" width="500" height="889" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe> 

[Here is a video][vid1] of an installation of the lights using a single five-meter LED strip (taken at twilight so that the hardware is visible):

[vid0]: https://vimeo.com/153011114#t=2s
[vid1]: https://vimeo.com/153011829

<iframe src="https://player.vimeo.com/video/153011829" width="500" height="281" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe> 

The design of the lighting system is modular, so it can be used with any length of neopixel strip. The current configuration of the project uses five 30 pixel-per-meter NeoPixel strips, each one five meters long, for a total of 750 pixels. 

As you can see in the above video, the design uses waterproof conduit, connected to custom, modular connectors, which house the sensors and connect the NeoPixel strips to their power and data lines. 

At the beginning and end of the installation are passive infrared (PIR) motion sensors, which trigger the lights to appear and then zip away in the opposite direction of the triggered sensor. 

## Challenges

This is an advanced project. It involves building a weather resistant wire rig with 3D-printed sensor housings, which itself requires a great deal of soldering, assembly, cable pulling, and testing. The modular nature of the design means you will have to take your intended installation environment into account and make your own decisions about the length and number of LED strips and connectors you plan to use. 

### Important Caveat

Even though the rig is designed to be weather resistant, it has not yet been tested in heavy rain or other wet conditions! Please take care and consider that this project uses electricity outdoors. Proceed at your own risk. 

## 3D Printing

The enclosures are designed to be easily printable on any commercial 3D printer; they were printed on a first-generation Makerbot Replicator in PLA and ABS plastic, and have been successfully tested on Cura, Printrbot, and Lulzbot printers.

The purpose of the enclosures is threefold: 

1. house passive infrared (PIR) motion sensors and connect their data lines to a ribbon cable reaching back to the arduino enclosure
2. connect neopixel strips to their data lines on the same ribbon cable, or to each other (if using multiple strips)
3. connect neopixel strips to the power and ground lines at both their origin and terminus, as recommended in the [Adafruit NeoPixel Uberguide][1], so that the lights don't dim and turn yellow as they extend far away from the arduino enclosure

[1]: https://learn.adafruit.com/adafruit-neopixel-uberguide

### Printing Enclosures

First, download the files in the `STLs` directory. 

Next, before you jump in to printing and assembling the enclosures, you should make a thumbnail sketch of the installation, so that you know how how many of which components to make, and how they will be assembled. 

For example, here is a diagram of the simplest possible installation, with two PIR sensors and one NeoPixel strip:

    A------P========================================P

This configuration uses the PIR enclosures to connect the neopixels to power and data. 

I *do not* recommend you use this configuration, because the PIR sensors have a very, very long range, and so you won't get reliable results. As you approach one end of the strip, the other sensor may trip, and that ruins the illusion of the lights. 

Instead, I recommend a slightly more complex configuration: 


    A-------P------N====================================N------------P

(TODO: scan actual drawing of the configurations)

To make this configuration, you will need two PIR enclosures, one "nopir" enclosure, and one "nopir terminal" connector. This will space out the sensors and make it so that they trigger at the right times, making the lights chase back and forth between the sensors. 

Please see the second video in the Overview section if you want to see what this configuration looks like in the wild. 

If you want to lengthen the lights, just add another "nopir" enclosure to connect another NeoPixel strip, like so: 

    A-------P------N======================N=====================N------------P

(TODO: scan actual drawing of the configurations)

Basically, for each installation you build, you will want two PIR sensor enclosures, and one more "nopir" enclosure than the number of NeoPixel strips. 

If you want to make things a bit more complex (why not?), you can space out the neopixel strips with areas of blank conduit, which will lengthen the area you can cover with the lights, like so: 

    A-------P------N==================N----------------N==================N------------P

(TODO: scan actual drawing of the configurations)

In the end, I recommend sketching your intended installation and then counting the number of each enclosure you will want. Once you do that, start printing your enclosures; they take an average of 5-6 hours each, so while they print you can start figuring out which lids you want to print. 

### Printing Lids

Complexity is the price you pay for a modular system. And this system is very modular, so of course there's an extra step: you have to determine which lids to print. 

Take a look at your sketch from earlier. Let's say you're making a two-strip installation, like so: 

    A-------P------N======================N=====================N------------P

(TODO: scan actual drawing of the configurations)

There are three possible lids to print, with zero, one, or two cutouts for waterproof cable glands. That's where the waterproof cables will come out of the enclosures and connect to the NeoPixel strips. 

For each enclosure, look at the diagram. Is there a NeoPixel strip touching the enclosure? If not, print a lid with 0 holes. 

If there's one NeoPixel strip touching the enclosure, print a lid with 1 holes. And if there are two strips touching (like the middle enclosure in the above diagram), print a lid with 2 holes. 

The rule of thumb is, print a lid with the same number of holes as the number of NeoPixels this enclosure needs to interface with. 

## Wiring and Soldering

The physical layer of this project is composed of the Arduino Due, a power circuit, ribbon cable running through conduit, terminal circuit boards in the enclosures, and waterproof cabling for the NeoPixel strips. 

### Arduino Enclosure

The Arduino Due is a 3.3v microcontroller, but the NeoPixels need 5v on the data line. So we'll need to use a logic level shifter, which will take the data signals sent by pin 6 of the arduino and amp them up to 5 volts. 

And since we're using ribbon cable to send the power and data signals down the length of the installation, we probably want to use screw terminals to connect those wires to the arduino. 

Therefore, you're going to need to make a circuit board for the arduino due so it can talk to the neopixels and hook up to the ribbon cable. 

![Witch Light Arduino Perma-Proto Board](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-ard-proto.png)

Use an Adafruit Perma-Proto Half-Size board and solder a pair of 10-pin screw terminals to it, and wire it up as shown in the photograph above. 

The 74AHCT125 chip is installed so that the little "U" cutout faces to the right of the board, matching the little "U" cutout in the socket. 

Be certain to wire the data line from pin 6 to the indicated terminal in the photo, and connect the white data line cable to the other indicated terminal on the other side; if you hook them up incorrectly, the NeoPixels will light up erratically or not at all. 

### Power

Once the Perma-Proto board is wired up like in the photo above, it's time to attach the 4700uF capacitor, and the 2.1mm power connector. 

(TODO: Get current photo of capacitor and power connector on perma-proto board)

Wire up the 5V power supply from the battery board to the 2.1mm connector on the arduino circuit board. Optional power switch installed in the side would go between the power line from the battery board and the arduino circuit board. 

### Terminal Boards

For each 3D-printed enclosure you make, you're going to need a terminal board. These are made using Adafruit Perma Proto 1/4 size boards, 3-pin terminals, and 2-pin terminals.

You'll probably want to strip and install the wires first, like so:

![PIR Terminal Board Detail 1](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-board1.jpg)

Once those are soldered, use masking tape to hold the terminals on to the board, and solder them on, like so: 

(TODO: close-up diagram shot of perma-proto boards)

Use the continuity tester feature of your multi-meter to test for short circuits and to ensure that the power, data, and ground lines pass properly through from one side of the board to the other. 

### NeoPixel Cables

You'll have to solder and heat-shrink one end of a waterproof 4-pin cable to each side of the strip. Make sure you always use the same cable gender for input and output, or else it'll get really confusing. 

### Pulling Cable

Use a cable snake to pull ribbon cable through the heyco conduit. Oh yeah, cut the conduit to length first. 

## Software

The current iteration of the software is in the directory marked `witch-lights-classic`. This code is inelegant but functional; it successfully handles two PIR sensors and will trigger a different sprite animation when one or the other sensor is tripped. 

It does _not_ allow for the triggering of more than one sprite at a time, and does not respond to sensor trigger events while a sprite is being animated. 

The animation is currently unfinished, but was good enough for the initial proof of concept. More work needs to be done on it. 

## Final Assembly and Use

### Arduino Enclosure and Power

Cut a hole in the enclosure for the heyco conduit thingy, and optionally a hole for the power button, unless you want to just unplug the battery when you want to turn it off. 

### PIR sensors 

Press-fit the PIR sensor into the provided slot. You may want to then pop the circuit board off, leaving the lens in place, and use hot glue to weatherproof the opening, as well as keep the PIR sensor from popping free, which it will tend to do unless you glue it in. 

### Terminal boards

Use 8mm M3 machine screws to attach the terminal boards to the bosses in the enclosures. Depending on your 3D printer, the screws *should* bite into the provided holes. If your printer's dimensional accuracy isn't 100% you might end up inserting helicoils. 

![Enclosure](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-enclosure1.png)


### Conduit to Connectors

Swedge the conduit onto connectors and put them into the enclosures; pull the ribbon cable through the waterproof washer and screw the conduit onto the enclosure at both ends, leaving the ribbon loose inside the connectors. 

### Wiring

Start at the arduino enclosure and screw all the matching colored ribbon cable ends to their appropriate terminal. Leave any extras un-stripped, so that in the future you have the option of additional sensors or branching LED strips. 

At each terminal board, hook power and ground into the provided terminals. 

At the first PIR sensor, connect the green ribbon wire to the PIR sensor terminal, and connect the 3 wires from the PIR sensor to the other end of the terminal board. If there is no NeoPixel strip being connected to this enclosure, connect the white wire to the terminals on each side so that the data passes through. 

Connect the yellow wire to the "passthrough" terminal on the side of the terminal board. In all subsequent terminal boards, use the PIR sensor terminals to pass the yellow wire through; this is the wire for the final PIR sensor and it needs an unbroken chain all the way through. 

Once you finish with each terminal board, use your multimeter to check continuity from the terminals in the arduino enclosure. I find it best to coil the conduit into a big loop as I connect terminal boards, so that the terminal board is a meter or so from the arduino enclosure at maximum. 

When you reach a terminal board where a neopixel strip is being attached, thread an "output" 4-wire cable through the waterproof cable gland, and connect the red, white, and black cables to the output terminal, so that the data line passes from the white wire on the ribbon cable into the white wire on the 4-wire cable. 

When connecting neopixel strips to each other, use a two-cable configuration on the terminal board, so that the neopixels get connected to power and ground at each enclosure they touch. (TODO: clarify this)

TODO: instructional photos are probably good here. 

At the last terminal board, connect the yellow wire to the PIR sensor. Do a continuity check between the yellow pin on the arduino to the PIR sensor to make sure there are no breaks in the chain. 

Final checks: 

Check all the ground and power lines to make sure they aren't shorted anywhere down the line. Test the final terminal board against the arduino enclosure. If there are no shorts, turn on the power. Check the power terminals on the final board; it should show 5 volts. 

### Connect NeoPixels

Hook up the NeoPixels to their appropriate 4-pin cables. If you've done continuity checks at each terminal board, you should have an unbroken data line running from beginning to end. Turn on the power and wave your hand at a PIR sensor; you should see a purple sprite animate along the length of the NeoPixel strips. 

This is where you should snap the lids tightly down on the enclosures, and screw the cable glands tight on the 4-pin NeoPixel cables. 

### Install 

Disconnect the neopixels temporarily, and loop the conduit through branches, around trees, along the ground, overhead… whatever path you want the lights to take. Then reconnect the NeoPixel cables at each end, and use velcro loops to bind the neopixel strips to the conduit so that there's no bunching or sagging. 

## Troubleshooting

Most problems with this installation are caused by wiring problems. If you're getting weird behavior, or no pixel illumination, check:

* are there any short circuits? 
* is the white data line connected all the way to the first NeoPixel input? 
* Are the 4-pin connectors connected right on the inside? 
* Are the NeoPixel strips oriented correctly? (Check the arrow on the strip; it should be pointed *away* from the Arduino enclosure)
* Is the Arduino power light on? Does the indicator LED come on when the PIR sensor is tripped? 

TODO: more better troubleshooting 