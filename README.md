# Witch Lights

The Witch Lights are an interactive light art installation, meant to be seen outdoors at night. 

The documentation and code are currently under construction, so they may change without notice. 

## Overview

This project creates a set of ghostly lights, which come out of the woods when you approach them and jump from tree to tree. 

Imagine: as you walk through the unlit path, a tiny purple faerie jumps out of the underbrush, coiling about trees and floating overhead. It zips ahead of you, but pauses, flickering, while you catch up. It then takes off again, remaining always just out of reach, until you reach the end of their territory, where they vanish back into the woods. As you pass by where they disappeared, the lights jump out again, heading back the other way, daring you to chase them.

<iframe src="https://player.vimeo.com/video/153011114" width="500" height="889" frameborder="0" webkitallowfullscreen mozallowfullscreen allowfullscreen></iframe> 

Here is a video of an installation of the lights using a single five-meter LED strip (taken at twilight so that the hardware is visible):

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

To make this configuration, you will need two PIR enclosures, and two "nopir" enclosures. This will space out the sensors and make it so that they trigger at the right times, making the lights chase back and forth between the sensors. 

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

There are three possible lids to print, with zero, one, or two cutouts for waterproof cable glands. 

## Wiring and Soldering

### Arduino Enclosure

### Terminal Boards

### Pulling Cable

## Software

The current iteration of the software is in the directory marked `witch-lights-classic`. This code is inelegant but functional; it successfully handles two PIR sensors and will trigger a different sprite animation when one or the other sensor is tripped. 

It does _not_ allow for the triggering of more than one sprite at a time, and does not respond to sensor trigger events while a sprite is being animated. 

## Final Assembly and Use

### 