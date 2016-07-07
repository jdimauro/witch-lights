# Witch Lights

The Witch Lights are an interactive light art installation, meant to be seen outdoors at night. 



The documentation and code are currently under construction, so they may change without notice. 

## Overview

This project creates a set of [ghostly lights][vid0], which come out of the woods when you approach them and jump from tree to tree. 

![Animation Demo 1](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/witchlights_firefly2016.gif)

![Animation Demo 2](https://media.giphy.com/media/bTTCN5060PrCU/giphy.gif)

Imagine: as you walk through the unlit path, a tiny purple faerie jumps out of the underbrush, coiling about trees and floating overhead. It zips ahead of you, but pauses, flickering, while you catch up. It then takes off again, remaining always just out of reach, until you reach the end of their territory, where they vanish back into the woods. As you pass by where they disappeared, the lights jump out again, heading back the other way, daring you to chase them.

<iframe src="https://player.vimeo.com/video/153011114" width="500" height="889" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe> 

[Here is a video][vid1] of an installation of the lights using a single five-meter LED strip (taken at twilight so that the hardware is visible):

[vid0]: https://vimeo.com/153011114#t=2s
[vid1]: https://vimeo.com/153011829

![Proof of Concept - Firefly 2015](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-poc-firefly1.gif)

<iframe src="https://player.vimeo.com/video/153011829" width="500" height="281" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe> 

The design of the lighting system is modular, so it can be used with any length of NeoPixel strip. The current configuration of the project uses five 30 pixel-per-meter NeoPixel strips, each one five meters long, for a total of 750 pixels. 

As you can see in the above video, the design uses waterproof conduit, connected to custom, modular connectors, which house the sensors and connect the NeoPixel strips to their power and data lines. 

At the beginning and end of the installation are passive infrared (PIR) motion sensors, which trigger the lights to appear and then zip away in the opposite direction of the triggered sensor. 

## Firefly 2016

The Witch Lights were exhibited at the Firefly Art Festival in Vermont, from June 29th through July 4th 2016. 

[Here is a video of the Witch Lights in action at the festival][vid2]. 

[vid2]: https://vimeo.com/173539041

### Photos

The lights were installed on a winding path through the woods, near the rear entrance to Library Camp. 

![Electronics package and beginning of LED strips](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-firefly2016-0.jpg)

The lights wound up around a tree, and then snaked through a tree stump.

![Lights wrapped around tree stump](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-firefly2016-1.jpg)

The conduit provides structure for the LED strips, and can be used to create spirals and loops in mid-air. 

![Spiral segment of conduit and LEDs](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-firefly2016-2.jpg)

![Spiral and stump](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-firefly2016-3.jpg)

The lights terminated by ducking behind a tree, so that they appear to hide behind a tree and vanish, or come out from behind the tree if activated by the motion sensor on this end (shown in green). 

![Motion sensor and terminus](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-firefly2016-4.jpg)

### Reception

The reaction to the Witch Lights was hugely enthusiastic and positive. Many attendees reported stopping and watching the lights, entranced, on their way through the path. Firefly's location feels like a magical forest, and the addition of a purple faerie that leads you down one of the paths felt like a natural thing to encounter in the dark. 

## Challenges

This is an advanced project. It involves building a weather resistant wire rig with 3D-printed sensor housings, which itself requires a great deal of soldering, assembly, cable pulling, and testing. The modular nature of the design means you will have to take your intended installation environment into account and make your own decisions about the length and number of LED strips and connectors you plan to use. 

### Important Caveat

Even though the rig is designed to be weather resistant, it has not yet been tested in heavy rain or other wet conditions! Please take care and consider that this project uses electricity outdoors. **Proceed at your own risk.** 

## Bill of Materials

Components: 

* [Heat-Shrink, various sizes](https://www.adafruit.com/products/1649)
* [Heat-Shrink, 1/4"](http://www.mouser.com/ProductDetail/3M-Electronic-Specialty/EPS300-1-4-48-Black-Bulk/)
* [Heat-Shrink, 1/2" (optional)](http://www.mouser.com/ProductDetail/3M/FP301-1-2-48-Clear-Bulk/)
* [Ribbon Cable, 100' spool](http://www.mouser.com/ProductDetail/Amphenol-Spectra-Strip/135-2801-010/)
* [Hookup wire, 22AWG, various colors](https://www.adafruit.com/products/1311)
* [M3 socket-head machine screws, 8mm length](http://www.mcmaster.com/#socket-head-cap-screws/=10zugcn "McMaster-Carr") (box 100)
* [Heyco 8452 3/4" conduit](http://amazon.com/Heyco-8452-BLACK-LIQUID-TUBING/dp/B001BQ029G/) (100' spool)
* [PIR Motion Sensor](https://www.adafruit.com/products/189) (2)
* [micro-USB power adapter](https://www.adafruit.com/products/1995)

Arduino Enclosure components:

* [Arduino Due](https://www.adafruit.com/products/1076)
* [Waterproof Enclosure (Large)](https://www.adafruit.com/products/905)
* 3D-Printed `wl-arduino-mount.STL` circuit board mount
* M3 machine screw, 20mm (?)
* ??mm plastic standoff
* [Adafruit Perma-Proto 1/2 board](https://www.adafruit.com/products/1609) 
* [Screw Terminals, 10-pin](https://www.adafruit.com/products/2234) (2)
* [74AHCT125 Logic Level Shifter IC](https://www.adafruit.com/products/1787)
* [16-pin IC socket](https://www.adafruit.com/products/2203)
* [4700uF capacitor](https://www.adafruit.com/products/1589)
* [2.1mm breadboard-friendly DC power jack](https://www.adafruit.com/products/373)
* [2.1mm male DC power adapter](https://www.adafruit.com/products/369)
* [10k ohm resistors](https://www.adafruit.com/products/2784) (2)
* [Adafruit PowerBoost 1000 board](https://www.adafruit.com/products/2465)
* [4400 MAh LiPo battery](https://www.adafruit.com/products/354)
* [Waterproof Panel-Mount on/off switch](https://www.adafruit.com/products/916)

You will need one set of these parts per 3D-printed enclosure (see instructions):

* [Heyco 8404 3/4" conduit fittings](http://smile.amazon.com/Heyco-BLACK-STRAIGHT-CONNECTOR-package/dp/B00LWLMX7O/) (2)
* [Heyco 3261 3/4" waterproof washers](https://amazon.com/Heyco-3261-SANTOPRENE-SEALING-package/dp/B00PKLGC8S/) (2)
* [Adafruit Perma-Proto 1/4 board](https://www.adafruit.com/products/1608)
* [Screw Terminals, 2-pin](https://www.adafruit.com/products/2234) (3)
* [Screw Terminals, 3-pin](https://www.adafruit.com/products/2234) (2)

You will need one set of these parts per NeoPixel strip (see instructions):

* [NeoPixel 30 LED/meter strip, black](https://www.adafruit.com/products/1460), 5 meters
* [4-pin waterproof cable](https://www.adafruit.com/products/744) (2)
* [waterproof cable gland](https://www.adafruit.com/products/762) (2)

Optional NeoPixel Reel Test rig:

(You can optionally build a dedicated Arduino rig to test your NeoPixel strips and keep it handy for field testing, or you can borrow components from the rest of the installation.)

* [Arduino Uno](https://www.adafruit.com/products/50)
* [Adafruit Perma-Proto 1/4 board](https://www.adafruit.com/products/1608)
* [Screw Terminals, 3-pin](https://www.adafruit.com/products/2234) (2)
* [4-pin waterproof cable](https://www.adafruit.com/products/744)

Tools:

* 3D Printer
* Wire Strippers
* Heavy-duty scissors or shears
* Needle-Nose Pliers
* Soldering Iron
* Multimeter
* 2.5mm Hex Wrench
* 1.5mm or 1.3mm flathead screwdriver
* Heat Gun
* 16mm hole saw
* 1" hole saw
* Third-hand tool
* panavise circuit board vise
* label printer (optional)
* cable snake
* wooden dowel (for running cable)
* stick-on velcro dots
* thin velcro cable-management strips

## 3D Printing

For best results, print all components in black or green plastic. 

The enclosures are designed to be easily printable on any commercial 3D printer; they were printed on a first-generation Makerbot Replicator in PLA and ABS plastic, and have been successfully tested on Makerbot Replicator 2, Printrbot, and Lulzbot printers.

![Printing an enclosure on a Makerbot Replicator 2](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-enclosure-printing.jpg)

The purpose of the enclosures is threefold: 

1. House passive infrared (PIR) motion sensors and connect their data lines to a ribbon cable reaching back to the arduino enclosure.
2. Connect NeoPixel strips to their data lines on the same ribbon cable, or to each other (if using multiple strips).
3. Connect NeoPixel strips to the power and ground lines at both their origin and terminus, as recommended in the [Adafruit NeoPixel Uberguide][uber], so that the lights don't dim and turn brown as they extend far away from the arduino enclosure.

[uber]: https://learn.adafruit.com/adafruit-NeoPixel-uberguide

### Printing Enclosures

First, download the files in the `STLs` directory. 

Next, before you jump in to printing and assembling the enclosures, you should make a thumbnail sketch of the installation, so that you know how how many of which components to make, and how they will be assembled. 

For example, here is a diagram of a naive installation, with two PIR sensors and one NeoPixel strip:

    A------P========================================P

![3D Printing Instructions Figure 1](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-3dprint-fig1.png)

This configuration uses the PIR enclosures to connect the NeoPixels to power and data. 

I *do not* recommend you use this configuration, because the PIR sensors have a very, very long range, and so you won't get reliable results. As you approach one end of the strip, the other sensor may trip, and that ruins the illusion of the lights. 

Instead, I recommend a slightly more complex configuration as the simplest possible arrangement: 


    A-------P------N====================================N------------P

![3D Printing Instructions Figure 2](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-3dprint-fig2.png)

To make this configuration, you will need two `wl-pir.stl` enclosures, one `wl-nopir.stl` enclosure, and one `wl-terminus.stl` enclosure. This will space out the sensors and make it so that they trigger at the right times, making the lights chase back and forth between the sensors. 

Please see the second video in the Overview section if you want to see what this configuration looks like in the wild. 

If you want to lengthen the lights, just add another `wl-nopir.stl` enclosure to connect another NeoPixel strip, like so: 

    A-------P------N======================N=====================N------------P

![3D Printing Instructions Figure 3](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-3dprint-fig3.png)

For each installation you build, you will want one `wl-pir.stl` enclosure, one `wl-terminus.stl` enclosure, and one more `wl-nopir.stl` enclosure than the number of NeoPixel strips. 

If you want to make things a bit more complex (why not?), you can space out the NeoPixel strips with areas of blank conduit, which will lengthen the area you can cover with the lights, like so: 

    A-------P------N==================N----------------N==================N------------P

![3D Printing Instructions Figure 4](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-3dprint-fig4.png)

In the end, I recommend sketching your intended installation and then counting the number of each enclosure you will want. Once you do that, start printing your enclosures; they take an average of 5-6 hours each, so while they print you can start figuring out which lids you want to print. 

### Printing Lids

Complexity is the price you pay for a modular system. And this system is very modular, so of course there's an extra step: you have to determine which lids to print. 

Take a look at your sketch from earlier. Let's say you're making a two-strip installation, like so: 

    A-------P------N======================N=====================N------------P

![3D Printing Instructions Figure 3](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-3dprint-fig3.png)

There are three possible lids to print, with zero, one, or two cutouts for waterproof cable glands. That's where the waterproof cables will come out of the enclosures and connect to the NeoPixel strips. 

For each enclosure, look at the diagram. Is there a NeoPixel strip touching the enclosure? If not, print a lid with 0 holes. 

If there's one NeoPixel strip touching the enclosure, print a lid with 1 holes. And if there are two strips touching (like the middle enclosure in the above diagram), print a lid with 2 holes. 

The rule of thumb is, **print a lid with the same number of holes as the number of NeoPixels this enclosure needs to interface with**. 

### Printing Arduino Mount

Print one of the `wl-arduino-mount.STL` file and place it with the components for the arduino enclosure. 

## Wiring and Soldering

The physical layer of this project is composed of the Arduino Due, a power circuit, ribbon cable running through conduit, terminal circuit boards in the enclosures, and waterproof cabling for the NeoPixel strips. 

This part of the project has four steps: 

1. Wire and solder the interface board for the Arduino
2. Solder the power elements to the Arduino interface board
3. Wire and solder the terminal boards
4. Connect the NeoPixels to 4-pin waterproof cables and test them

### Arduino Interface Board

The Arduino Due is a 3.3v microcontroller, but the NeoPixels need 5v on the data line. So we'll need to use a logic level shifter, which will take the data signals sent by pin 6 of the arduino and amp them up to 5 volts. 

And since we're using ribbon cable to send the power and data signals down the length of the installation, we probably want to use screw terminals to connect those wires to the arduino. 

Therefore, you're going to need to make a circuit board for the Arduino Due so it can talk to the NeoPixels and hook up to the ribbon cable. 

![Witch Lights - Arduino Interface Board](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-ard-proto.png)

Use an Adafruit Perma-Proto Half-Size board and solder a pair of 10-pin screw terminals to it, and wire it up as shown in the photograph above. 

The 74AHCT125 chip is installed so that the little "U" cutout faces to the right of the board, matching the little "U" cutout in the socket. 

(TODO: detail shot of 74HCT125 chip)

It's time to connect the Arduino Interface Board with the Arduino Due itself. We're going to use the 20" M3 screw and stand-offs to mount the board underneath the Arduino. 

(TODO: detail shots of the Arduino mounted to the interface board)

Use hookup wire to connect the arduino's headers to the top part of the circuit board as follows:

* Connect the *red* terminal (power) to the "5V" pin
* Connect the *rightmost white* terminal (NeoPixel data) to digital pin 6 (refer to the detail photo above)
* Connect the *green* terminal (PIR sensor 0) to digital pin 3
* Connect the *yellow* terminal (PIR sensor 1) to digital pin 4
* Connect the *black* terminal (ground) to the "GND" pin

**Be certain to wire the data line from pin 6 to the indicated terminal in the photo, and connect the white data line cable to the other indicated terminal on the other side; if you hook them up incorrectly, the NeoPixels will light up erratically or not at all.** 

(TODO: detail shot of the Arduino hooked up to the circuit board)

### Power

Once the Perma-Proto board is wired up like in the photo above, it's time to attach the 4700uF capacitor, and the 2.1mm power connector. 

(TODO: Get current photo of capacitor and power connector on perma-proto board)

Wire up the 5V power supply from the battery board to the 2.1mm connector on the arduino circuit board. Optional power switch installed in the side would go between the power line from the battery board and the arduino circuit board. 

### Terminal Boards

For each 3D-printed enclosure you make, you're going to need a terminal board. These are made using Adafruit Perma Proto 1/4 size boards, 3-pin terminals, and 2-pin terminals.

You'll probably want to strip and install the wires first, like so:

![PIR Terminal Board Detail 1](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-board1.png)

Once those are soldered, use masking tape to hold the terminals on to the board, and solder them on, like so: 

![PIR Terminal Board Detail 2](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-board2.png)

Use the continuity tester feature of your multi-meter to test for short circuits and to ensure that the power, data, and ground lines pass properly through from one side of the board to the other. 

Once you're certain you have a good board, do yourself two small favors:

1. Use a 1.5mm flathead screwdriver to loosen _all_ of the terminals; they ship tightened all the way, and it's far easier to loosen them now than it will be in the field
2. Draw an arrow on the board as shown. This indicates the direction of the data flow away from the Arduino. When it's time to install the boards in their enclosures, you'll install them so that the arrow points away from the Arduino, and that will save you heartache and troubleshooting later. Trust me. 

### NeoPixel Cables

You'll have to solder and heat-shrink one end of a waterproof 4-pin cable to each side of the NeoPixel strip, so that we have a weatherproof way to connect the strips to the 3D-printed enclosures. Once you do that, you'll need to test the strip to make certain there are no problems with your soldering job. 

Make sure you always use the same cable gender for input and output, or else it'll get really confusing. I used a label printer to make an arrow pointing in the direction of the NeoPixel data flow (as indicated by arrows on the NeoPixel Strips), and labeled all the waterproof cables before starting assembly. 

![Arrows on NeoPixel Cables](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-0.png)

Use a third-hand tool to hold the waterproof cable and the end of the NeoPixel strip. Use your wire strippers to take some more insulation off the red, black, and white wires coming out of the waterproof cable, and clip the yellow wire short; we won't be using it. 

Put a length of 1/4" heat shrink tubing on the waterproof cable, and put some thin heat-shrink tubes on the red, white, and black wires coming from the NeoPixel strips. Optionally, slide some 1/2" heat-shrink tubing over the end of the NeoPixel strip itself. 

Tin the wires a little bit by heating them up with a soldering iron and brushing them with a bit of solder. Then twist the black wires together, and use the soldering iron and a bit more solder to splice them. Repeat the process with the red and white wires, like so: 

![Soldering wires](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-1.jpg)

Use the continuity tester on your multimeter to test that you have continuity from the pins on the cable to the bare spliced wires. You may have to use some trial and error to figure out which pin goes to which color of cable; I always forget and have to poke around. 

Then slide the thin heat shrink tubing over the bare soldered wire. Use a heat gun to shrink the tubing over the wires. 

![Heat shrink on the spliced wire](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-2.jpg)

Now, slide the 1/4" heat-shrink tubing over all the colored wires, up to the base of the silicone sheath of the NeoPixel strip.

![Heat-shrink over the colored wire](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-3.jpg)

Use a heat gun to shrink the tubing and seal the wires.

![Heat-shrunk wires](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-4.jpg)

And now, if you have the 1/2" heat shrink over the NeoPixel strips, slide it over the point where the heat-shrink abuts the silicone sheath.

![Optional final heat shrink](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-5.jpg)

Use a heat gun to shrink that down and reinforce the joint. 

Repeat this process on the other end of the NeoPixel strip with the other end of the waterproof cable. And repeat again for each NeoPixel strip you use. 

#### Test NeoPixel Reels

I used an Arduino Uno to create a dedicated test rig for NeoPixel reels after soldering them; you can either do this, or load the `NeoPixel_Reel_Test` sketch on the Arduino Due. Either way, borrow one of the enclosure terminal boards (or make an extra one), and connect the "out" end of a waterproof cable to it, and hook the power and NeoPixel data lines to the Arduino. 

The `NeoPixel_Reel_Test` sketch runs alternating colors down the length of any NeoPixel 5-meter reel connected to it. The purpose of this test is to make certain there are no problems with the solders on the NeoPixel reel and 4-pin waterproof cables. 

Use the reel tester after you solder cable to every NeoPixel strip, then roll up the NeoPixel strip onto its reel and put it away in its silver bag; mark "Tested" and the date on the bag. 


## Software

The current iteration of the software is in the directory marked `witch-lights-classic`. This code successfully handles two PIR sensors and will trigger a different sprite animation when one or the other sensor is tripped. 

### Required Libraries

The Witch Lights run Adafruit NeoPixels, and therefore require the [Adafruit NeoPixel Library][neolib]. Follow the instructions in the README to install the library in your Arduino environment. 

[neolib]: https://github.com/adafruit/Adafruit_NeoPixel

If you have trouble installing the library, or if you've never installed an Arduino library before, please check out the [Adafruit Guide to Arduino Libraries][agal] for in-depth instructions. 

[agal]: https://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use

### Program the Arduino

Download `witch-lights-classic` and use the Arduino IDE to upload the code to your Arduino Due. 

If the Arduino is properly wired to its terminal board, you should be able to use the screw terminals to connect directly to a NeoPixel strip. Power on the Arduino and use a jumper cable to make the green terminal pin go high; you should see the purple faerie lights race away down the NeoPixel strip. 

### Adjust the Code

Once you have an animation running, you can adjust the variables in the code to tune the speed and frequency of the animation. All of the variables you need to adjust are declared at the beginning of the file: 

`N_LEDS`: Each NeoPixel strip has 150 LEDs. Change this value to equal the total number of LEDs in your project. 
IMPORTANT: As you increase the number of LEDs past 150, the animation speed will slow, and you will have to reduce the "interval" variable to compensate.

`interval`: Increase this to make the sprite's animation begin more slowly. Reduce it to make the sprite's animation begin more quickly. As you add LEDs to your project, you will have to adjust this variable.

`minInterval`: If your project has a smaller number of LEDs (150 or 300), you may need to increase this variable to 1 or 2, in order to make the animation appear natural. If you have more than 300 LEDs, leave it at 0. 

`hoverLowerDistance`: This adjusts how frequently the sprite stops and hovers. Decrease it to make the sprite stop more often. Increase it to make the sprite run for longer distances before it pauses to hover. 

`hoverLowerTime`: Adjust this variable, along with `hoverUpperTime`, to change how long the sprite "hovers" for; the higher the values, the longer the sprite will hover. A value of 5 means that the sprite will run through its "sparkle" animation cycle 5 times. The greater the difference between the two values, the greater the random variation in how long the sprite pauses to sparkle for. You will need to tune this value after you've made changes to any of the above variables. 

The process for adjusting the animation goes like this: 

* Set the `N_LEDS` first.
* Let the animation run a few times, watching the sprite as it _starts_. The animation should be a slow but smooth launch. 
* If the speed at start feels slow or choppy, decrease `interval`. If the sprite zips away too quickly, increase `interval`.
* Use the `hoverLowerDistance` variable to tune how often the sprite stops. I recommend laying the strip on the floor at full length, and walking alongside it. You want the sprite to go just a bit faster than you would walk at a natural pace. 
* Fine-tune the `hoverLowerTime` and `hoverUpperTime` variables. This is a matter of feel; you want the amount of time the sprite pauses to feel natural. It's a matter of "you know it when you see it".

TODO: install knobs and map them to the `interval` and `hoverLowerDistance` variables to allow for on-site fine tuning without the need for a laptop.

## Final Assembly and Use

You're ready to put the whole thing together! At this point, you should have:

* 3D-printed enclosures
* 3D-printed arduino mount
* Terminal boards (1 per enclosure)
* Arduino and its terminal board, with the software uploaded and adjusted
* NeoPixel reels with cabling (tested)
* All the remaining components on the Bill of Materials, such as conduit, ribbon cable, etc. 

Here's the process you're going to follow: 

1. Assemble the Arduino Enclosure, and build the power circuit.
2. Cut conduit to the proper lengths, and pull ribbon cable through them for the wiring harness.
3. Install the terminal boards in the 3D-printed housings
4. Install the PIR sensors in the sensor housings
5. Connect the ribbon cable to the terminals on the boards, and test continuity
6. Wire the NeoPixel interface cables into the housings and connect the NeoPixels
7. Run final testing on the assembly

### Arduino Enclosure and Power

In this step, you will assemble the Arduino Enclosure, and construct a power circuit, with battery connector and power switch. 

![Enclosure with battery pack](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-ard-enclosure2.png)

Use a compass to mark a 1" circle on the narrow end of the enclosure, where you want the Heyco connector to go. This will be the "bottom" of the enclosure. (See the photograph above for guidance on where to mark the hole.) Mark the center of the circle. 

Drill a pilot hole with a 1/8" drill bit first, and then use your hole saw to cut the 1" hole in the enclosure. You'll need to use a file or rough sandpaper to open the 1" hole up enough to insert a Heyco connector. 

Slide the half-size perma-proto board into the printed `wl-arduino-mount.STL` piece, then mount it in the enclosure using the M4 screws provided when you unwrapped the enclosure. 

Now for the power circuit. First, we need to place the switch. 

Use a 16mm hole saw to cut a hole in the side of the enclosure; make sure to position it so that the switch's terminals won't interfere with the arduino's proto board. Screw the panel-mount switch into the hole (and don't forget the rubber O-ring). 

Solder leads to the `GND` and `EN` pins on the PowerBoost 1000C, and solder leads to the `5V` and `GND` pins, as well as the `+` and `-` pins. Run `5V` and `GND` down to the power plug on the proto-board, and screw them into the terminals of the male DC adapter. Plug it into the proto-board. 

![Diagram of the on/off panel-mount power switch](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-powerswitch-diagram.jpg)

(See the [product page for the power switch](https://www.adafruit.com/products/482) for details)

Run the `+` and `-` pins to the (outer) LED terminals on the power switch. Make sure to connect them to the `+` and `-` terminals of the switch, respectively. Connect `GND` and `EN` to the terminals labeled `NC` and `C1`. ("Normaly closed" and "Common contact".) 

What you are doing is, you're closing the circuit between `EN` and `GND` on the PowerBoost 1000C. That tells the PowerBoost to turn off. When the on/off switch is pressed, it _opens_ the circuit, and the PowerBoost gets the message to turn on, which supplies power to both the Arduino and the LED ring around the switch itself. 

Connect a battery to the PowerBoost 1000C. When you press the on/off switch, the switch should light up, and the Arduino should power on. If not, check your connections to the terminals on the switch. It's very easy to mix up the `+` and `-` terminals for the switch's LED, and if you do, the PowerBoost won't turn on. 

Position the battery as shown in the photograph above, and carefully use stick-on velcro dots to attach it to the inside of the enclosure. 

At this point, you have a functional brain. You can charge the battery with the lid open by plugging the PowerBoost 1000C into a [micro-USB power adapter](https://www.adafruit.com/products/1995), and the system will continue to run as it charges. 

You should still also have access to the programming port on the Arduino, in case you need to reprogram it for upgrades or bug fixes.  

### Cut Conduit and Pull Cable

This is where you refer back to your plan. How many NeoPixel strips are you going to use? How are they spaced? You should have a thumbnail sketch. 

If you don't, refer back to the **Printing Enclosures** section above, sketch out your length of witch lights, and get started printing the enclosures you're going to need. 

For each NeoPixel strip, you're going to need five meters of conduit. 

If you plan to have a spacer between your NeoPixel strips, you're going to need conduit for that, as well. 

No matter what kind of configuration you sketched out earlier, you're going to put a five meter conduit between the Arduino enclosure and the first PIR sensor. This gives you enough flexibility to find a safe place to install the Arduino and then position the PIR sensor strategically to sense motion at the right place. 

Measure out five meters (16.4 feet) of conduit and cut it with your heavy-duty scissors or shears. Find two Heyco connectors. Unscrew the big dome-like component and slide it over the conduit, then swedge the conduit into the end of the connector. Screw the dome back on, tightly. This seals the conduit onto the connector. Repeat for the other end. 

Congratulations, you've got your first length of conduit ready to go. 

Once you've got your conduit set, it's time to pull ribbon cable through it. You're going to want a friend to help you with the next step. 

Extend your cable snake into the conduit and keep unreeling it until it pokes through the far end of the conduit. Then your friend can pinch the cable and stuff it through the little hole on the end of the cable snake, or use tape to attach it. Now have your friend put your wooden dowel through the hole in the ribbon cable spool, and hold either end of the dowel as you _slowly and carefully_ reel in the cable, until the cable pokes out of the conduit on your end. Pull about 4-5 inches of ribbon past the end of the cable, and then cut the ribbon cable on the other end, also with about 4-5 inches of extra. 

Decide which way you plan to orient the conduit, and then use a label printer or tape and a marker to mark an arrow → pointing _away_ from the Arduino enclosure. 

Since this is the first conduit, connect the PIR sensor housing to the far end. Slide the conduit and ribbon cable through one of the holes on the housing. Doesn't matter which one. Carefully thread the ribbon cable through the hole in one of the silicone washers and the screw-on cap, and use your fingers to tighten those down as hard as you can. You'll probably end up holding the connector in one hand and twisting the dome-like cap on the conduit connector in the other hand. 

Mark an arrow → on the side of the PIR sensor housing, pointing _away_ from the Arduino enclosure. 

For each piece of conduit you connect, repeat these steps, so that every single piece of conduit has a connector on both ends, ribbon cable threaded through it with 4-5" slack on both ends, and a 3D printed housing connected to the "far" end (from the Arduino enclosure). 

The next conduit you want to assemble is the other PIR sensor. Give this one a good five meters as well; the exact length is up to you, but you want enough to give yourself flexibility in positioning and aiming the sensor. 

Now you may see how this is a modular system; these conduit segments may be arranged in any configuration, so long as they form an unbroken chain from the Arduino to the final PIR sensor. 

Next, we will install terminal boards in the 3D-printed enclosures, and we will strip and attach the ribbon cable to the terminals on those boards. 

### Terminal boards

Use 8mm M3 machine screws to attach the terminal boards to the bosses in the enclosures. Be sure to orient them so that the arrow you drew on them points away from the Arduino, in the direction of the data flow down the NeoPixel strips. 

Depending on your 3D printer, the screws *should* bite into the provided holes. If your printer's dimensional accuracy isn't 100% you might end up inserting helicoils. 

![Enclosure](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-enclosure1.png)

In *one* terminal board, you're going to solder an extra 2-wire terminal onto the side. These terminals are a passthrough for the yellow "pir1" sensor line, so set this board aside for use on the first ("pir0") sensor housing. 

![PIR0 Terminal Board Diagram](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir0-fritzing.png)

Here is a diagram showing the first PIR sensor terminal board. Note how the green `PIR0` data line is connected to the left set of terminals, and the yellow `PIR1` data line is connected to the extra 2-wire terminal on the side, as instructed above. 

Most of the other terminal boards will be used to pass through the `PIR1` data line, and to connect the NeoPixel data line and power to the NeoPixel strips. 

![NeoPixel Terminal Board Diagram](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-terminal-fritzing.png)

Here is a diagram showing the first terminal board where a NeoPixel strip is connected to the board (via the 4-pin waterproof cables). Note that the white `NeoPixel` data line connects to the input data pin on the right, and the NeoPixel strip is connected to the output data pins. 

Hopefully this gives you an idea of the model of the system: NeoPixel terminals pass the data line from white line to NeoPixel, or from NeoPixel to NeoPixel. They also connect the NeoPixel strips to power and ground at both ends. That keeps the color of the pixels bright. 

Every single terminal takes the `PIR1` line as an input, and passes it on, until the final "terminus" board, where the `PIR1` sensor connects to the terminal board. 

### PIR sensors 

Two enclosures house PIR passive-infrared motion sensors. Hold off installing the terminal boards in those two housings. 

Press-fit the PIR sensor into the provided slot in the bottom of the 3D-printed "PIR" enclosures. 

![PIR Enclosure side view](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-enclosure2.png)

You may want to then pop the circuit board off, leaving the lens in place, and use hot glue to weatherproof the opening, as well as keep the PIR sensor from popping free, which it will tend to do unless you glue it in. 

Once you've glued the PIR lens into the housing, carefully replace it, and connect the header and leads which came with it in the package.  

![PIR sensor and wires](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-enclosure3.png)

Install a terminal board over the sensor. Screw the power, data, and ground wires into the red, yellow, and black terminals, respectively. 

For the PIR sensor closest to the Arduino, connect the green wire on the ribbon cable to the PIR sensor, and the yellow wire to the special "passthrough" terminal you soldered in the previous step. 

TODO: picture of PIR sensor installed 

### Wiring

TODO: pictures of the wiring process

Start at the arduino enclosure and screw all the matching colored ribbon cable ends to their appropriate terminal. Leave any extras un-stripped, so that in the future you have the option of additional sensors or branching LED strips. 

At each terminal board, hook power and ground into the provided terminals. 

At the first PIR sensor, connect the green ribbon wire to the PIR sensor terminal, and connect the 3 wires from the PIR sensor to the other end of the terminal board. If there is no NeoPixel strip being connected to this enclosure, connect the white wire to the terminals on each side so that the data passes through. 

Connect the yellow wire to the "passthrough" terminal on the side of the terminal board. In all subsequent terminal boards, use the PIR sensor terminals to pass the yellow wire through; this is the wire for the final PIR sensor and it needs an unbroken chain all the way through. 

Once you finish with each terminal board, use your multimeter to check continuity from the terminals in the arduino enclosure. I find it best to coil the conduit into a big loop as I connect terminal boards, so that the terminal board is a meter or so from the arduino enclosure at maximum. 

When you reach a terminal board where a NeoPixel strip is being attached, thread an "output" 4-wire cable through the waterproof cable gland, and connect the red, white, and black cables to the output terminal, so that the data line passes from the white wire on the ribbon cable into the white wire on the 4-wire cable. 

When connecting NeoPixel strips to each other, use a two-cable configuration on the terminal board, so that the NeoPixels get connected to power and ground at each enclosure they touch. (TODO: clarify this)

TODO: instructional photos are probably good here. 

At the last terminal board, connect the yellow wire to the PIR sensor. Do a continuity check between the yellow pin on the arduino to the PIR sensor to make sure there are no breaks in the chain. 

Final checks: 

Check all the ground and power lines to make sure they aren't shorted anywhere down the line. Test the final terminal board against the arduino enclosure. If there are no shorts, turn on the power. Check the power terminals on the final board; it should show 5 volts. 

### Connect NeoPixels

Hook up the NeoPixels to their appropriate 4-pin cables. If you've done continuity checks at each terminal board, you should have an unbroken data line running from beginning to end. Turn on the power and wave your hand at a PIR sensor; you should see a purple sprite animate along the length of the NeoPixel strips. 

This is where you should snap the lids tightly down on the enclosures, and screw the cable glands tight on the 4-pin NeoPixel cables. 

### Install 

Lay the entire length of the lights out in a straight line, if you can. Leave the lids of the enclosures off (for the sensors) or loosely connected (for the NeoPixel interfaces). 

Power on the Arduino and walk alongside the assembly as the sprite animates. Watch for dead pixels, dead strips (probably caused by a bad cable connection on the terminal board), stuttering animation (caused by too high an `interval` value in the software), or other glitches. You want to catch those now, because the next thing you're going to do is install the whole assembly on-site. Test both motion sensors; when you reach the end of the animation loop, you should trip the farthest sensor, causing the sprite to animate back the way it came. If the far sensor doesn't work, you'll have to check continuity for the yellow line on the ribbon cable all the way down the chain. 

Disconnect the NeoPixels from the assembly. Loop the conduit through branches, around trees, along the ground, overhead… whatever path you want the lights to take. 

If your assembly is long enough, you may find it easier to break the length up into two or more pieces. That may allow you to loop around trees and tree branches without dragging the entire assembly along with you. 

Reconnect the NeoPixels. Use velcro loops to bind the NeoPixel strips to the conduit so that there's no bunching or sagging. The Heyco conduit should provide structure for the NeoPixel strips, which tend to sag and droop on their own. 

## Troubleshooting

Most problems with this installation are caused by wiring problems. If you're getting weird behavior, or no pixel illumination:

* Check the voltage at the positive and negative terminals in each enclosure while the assembly is powered on; you should have 5 volts at each terminal. If the voltage is less than 5V, you may have a short circuit in the power line. 

Specific problems I have run into:

### One or more of the NeoPixel strips do not light up (but some do)

Usually this happens because a wire has come loose, or you've got the polarity wrong. One of the NeoPixel strips themselves could be defective. 

* Are the NeoPixel strips oriented correctly? (Check the arrow on the strip; it should be pointed *away* from the Arduino enclosure)
* Use the *NeoPixel Reel Test* assembly to check the first NeoPixel strip that isn't working. 
* is the white data line connected all the way to the first NeoPixel input? Use the continuity check function of your multimeter to be certain. 
* Check any enclosures where the NeoPixels connect to 4-pin waterproof cables; it's possible that the terminals connecting the cables to the wiring harness have come loose as the cables flexed during installation. 
* Check the voltage on the terminal board just before the NeoPixel problem: do you have 5V at the `+` and `-` terminals? 

### None of the NeoPixel strips light up

Either no power is reaching the NeoPixels, or the data signal is not reaching them for some reason. At absolute minimum, even if the PIR sensors both fail, the first NeoPixel should light up green. 

* Does the Arduino have a power light? If not, refer to "The Arduino does not power on" below.
* Use the *NeoPixel Reel Test* assembly to check your first NeoPixel strip. 
* Is the logic level shifter chip installed in the interface board? 
* Does the logic level shifter chip face the correct way? The "U"-shaped cutout should face to the right/top of the board. 
* Is the logic level shifter chip installed in the correct (rightmost) pins of the socket? 
* Check the voltage at the `+` and `-` (red and black) terminals on the arduino interface board. You should have 5 volts. If so, go "downstream" and check the power terminals on all the terminal boards between the NeoPixels and the interface board; find the loose or missing power connections. If not, refer to "The Arduino does not power on" below for troubleshooting steps. 
* Use the continuity tester on your multimeter to make certain the white "data" terminal on the NeoPixel terminal board is connected to the white "data" pin on the Arduino Interface Board. 

### One of the motion sensors isn't working

The PIR sensors work by sending a 3 volt signal down the yellow or the green wires in the ribbon cable. They're powered by the red and black wires on their terminal pins. If the PIR doesn't get power, or if the yellow or green wires don't have perfect continuity all the way down the line from the arduino to the sensor, the sensors won't work. 

* Is the Arduino power light on? If not, see "The Arduino does not power on" below. 
* Does the indicator LED come on when the PIR sensor is tripped? If so, see "None of the NeoPixel strips light up" above. 
* Is the green data line connected all the way to the nearest PIR sensor? Check continuity with your multimeter. 
* Is the yellow data line connected all the way to the farthest PIR sensor? Check continuity with your multimeter. Aren't you glad you didn't screw the lids onto all the 3D-printed enclosures?
* Test the power and ground pins where the PIR sensor connects to the terminal board; you should have 5 volts. If not, check the power and ground pins on the terminal board itself. Got power there? You have a bad terminal board. No power? There's a break somewhere in the red or the black lines running back to the Arduino Interface Board, *or* there is a short circuit somewhere. 
* Check to make certain the wires from the terminal board to the PIR sensor itself have not come loose at either end.

### The Arduino does not power on

Either the battery is dead, the power switch isn't working, the PowerBoost 1000C isn't supplying 5V to the interface board for some reason, or you've got a wiring mistake. 

* Check the leads on the power switch: are any loose? Any short-circuits?
* Is the battery charged? Plug the PowerBoost 1000C into USB. 
* Does the light turn on on the PowerBoost 1000C? The power switch? 
* Test the terminals on the DC power plug. You should have 5V coming into the interface board. If not, check your solders on the PowerBoost 1000C. (And double-check that the PowerBoost is turning on.)
* Double-check the wiring on the Arduino interface board. Make sure the red and blue power rails are connected to the red and black terminals, and that there are no short circuits. 
* Check the wiring on the Arduino: the red terminal on the interface board should connect to the `5V` pin on the Arduino Due. The black terminal on the interface board should connect to the `GND` pin. 

### The NeoPixels light up all weird when I power it on

Most likely you've got a 3-volt signal from the Arduino Due connected to the NeoPixels. Usually this is a wiring mistake. 

* Check to make certain you have pin 6 on the Arduino Due connected to the *second pin from the right* on the "top" of the interface board, and the white ribbon cable wire is connected to the *third pin from the right* on the "bottom" of the board. ("Top" and "Bottom" here are assuming you hold the board with the IC on the _right_ side, and the DC power plug on the _left_.)
* Is the logic level shifter installed correctly? 

TODO: more better troubleshooting 