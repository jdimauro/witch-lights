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

The design of the lighting system is modular, so it can be used with any length of NeoPixel strip. The current configuration of the project uses five 30 pixel-per-meter NeoPixel strips, each one five meters long, for a total of 750 pixels. 

As you can see in the above video, the design uses waterproof conduit, connected to custom, modular connectors, which house the sensors and connect the NeoPixel strips to their power and data lines. 

At the beginning and end of the installation are passive infrared (PIR) motion sensors, which trigger the lights to appear and then zip away in the opposite direction of the triggered sensor. 

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
* [Heyco 8452 3/4" conduit](http://amazon.com/Heyco-8452-BLACK-LIQUID-TUBING/dp/B001BQ029G/ "Amazon.com: Heyco 8452 HF2 3/4-inch FLEX II BLACK LIQUID TIGHT TUBING (each): Industrial &amp; Scientific") (100' spool)
* [PIR Motion Sensor](https://www.adafruit.com/products/189) (2)

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

* [Heyco 8404 3/4" conduit fittings](http://smile.amazon.com/Heyco-BLACK-STRAIGHT-CONNECTOR-package/dp/B00LWLMX7O/ "Amazon.com: Heyco 8404 HFC 3/4 BLACK HI FLEX STRAIGHT CONNECTOR WITH NUT (package of 10): Industrial &amp; Scientific") (2)
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

For example, here is a diagram of the simplest possible installation, with two PIR sensors and one NeoPixel strip:

    A------P========================================P

This configuration uses the PIR enclosures to connect the NeoPixels to power and data. 

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

If you want to make things a bit more complex (why not?), you can space out the NeoPixel strips with areas of blank conduit, which will lengthen the area you can cover with the lights, like so: 

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

### Printing Arduino Mount

Print one of the `wl-arduino-mount.STL` file and place it with the components for the arduino enclosure. 

## Wiring and Soldering

The physical layer of this project is composed of the Arduino Due, a power circuit, ribbon cable running through conduit, terminal circuit boards in the enclosures, and waterproof cabling for the NeoPixel strips. 

### Arduino Enclosure

The Arduino Due is a 3.3v microcontroller, but the NeoPixels need 5v on the data line. So we'll need to use a logic level shifter, which will take the data signals sent by pin 6 of the arduino and amp them up to 5 volts. 

And since we're using ribbon cable to send the power and data signals down the length of the installation, we probably want to use screw terminals to connect those wires to the arduino. 

Therefore, you're going to need to make a circuit board for the Arduino Due so it can talk to the NeoPixels and hook up to the ribbon cable. 

![Witch Light Arduino Perma-Proto Board](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-ard-proto.png)

Use an Adafruit Perma-Proto Half-Size board and solder a pair of 10-pin screw terminals to it, and wire it up as shown in the photograph above. 

The 74AHCT125 chip is installed so that the little "U" cutout faces to the right of the board, matching the little "U" cutout in the socket. 

(TODO: detail shot of 74HCT125 chip)

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

(TODO: close-up diagram shot of perma-proto boards)

Use the continuity tester feature of your multi-meter to test for short circuits and to ensure that the power, data, and ground lines pass properly through from one side of the board to the other. 

### NeoPixel Cables

You'll have to solder and heat-shrink one end of a waterproof 4-pin cable to each side of the NeoPixel strip, so that we have a weatherproof way to connect the strips to the 3D-printed enclosures. Once you do that, you'll need to test the strip to make certain there are no problems with your soldering job. 

Make sure you always use the same cable gender for input and output, or else it'll get really confusing. I used a label printer to make an arrow pointing in the direction of the NeoPixel data flow (as indicated by arrows on the NeoPixel Strips), and labeled all the waterproof cables before starting assembly. 

(TODO: Picture of labeled waterproof cables)

Use a third-hand tool to hold the waterproof cable and the end of the NeoPixel strip. Use your wire strippers to take some more insulation off the red, black, and white wires coming out of the waterproof cable, and clip the yellow wire short; we won't be using it. 

Put a length of 1/4" heat shrink tubing on the waterproof cable, and put some thin heat-shrink tubes on the red, white, and black wires coming from the NeoPixel strips. Optionally, slide some 1/2" heat-shrink tubing over the end of the NeoPixel strip itself. 

Tin the wires a little bit by heating them up with a soldering iron and brushing them with a bit of solder. Then twist the black wires together, and use the soldering iron and a bit more solder to splice them. Repeat the process with the red and white wires, like so: 

![Soldering wires](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-neopixel-cables-1.jpg)


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

### Pulling Cable

Use a cable snake to pull ribbon cable through the heyco conduit. Oh yeah, cut the conduit to length first. 

## Software

The current iteration of the software is in the directory marked `witch-lights-classic`. This code successfully handles two PIR sensors and will trigger a different sprite animation when one or the other sensor is tripped. 

Download `witch-lights-classic` and use the Arduino IDE to upload the code to your Arduino Due. 

## Final Assembly and Use

### Arduino Enclosure and Power

Cut a hole in the enclosure for the heyco conduit thingy, and optionally a hole for the power button, unless you want to just unplug the battery when you want to turn it off. 

### PIR sensors 

Press-fit the PIR sensor into the provided slot in the bottom of the 3D-printed "PIR" enclosure.

![PIR Enclosure side view](https://raw.githubusercontent.com/jdimauro/witch-lights/master/docs/wl-pir-enclosure2.png)

You may want to then pop the circuit board off, leaving the lens in place, and use hot glue to weatherproof the opening, as well as keep the PIR sensor from popping free, which it will tend to do unless you glue it in. 

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

Disconnect the NeoPixels temporarily, and loop the conduit through branches, around trees, along the ground, overhead… whatever path you want the lights to take. Then reconnect the NeoPixel cables at each end, and use velcro loops to bind the NeoPixel strips to the conduit so that there's no bunching or sagging. 

## Troubleshooting

Most problems with this installation are caused by wiring problems. If you're getting weird behavior, or no pixel illumination, check:

* are there any short circuits? 
* is the white data line connected all the way to the first NeoPixel input? Use the continuity check function of your multimeter to be certain. 
* Is the yellow data line connected all the way to the farthest PIR sensor? Check continuity with your multimeter. 
* Check any enclosures where the NeoPixels connect to 4-pin waterproof cables; it's possible that the terminals connecting the cables to the wiring harness have come loose as the cables flexed during installation. 
* Are the NeoPixel strips oriented correctly? (Check the arrow on the strip; it should be pointed *away* from the Arduino enclosure)
* Is the Arduino power light on? Does the indicator LED come on when the PIR sensor is tripped? 
* Check the voltage at the positive and negative terminals in each enclosure while the assembly is powered on; you should have 5 volts at each terminal. If the voltage is less than 5V, you may have a short circuit in the power line. 

TODO: more better troubleshooting 