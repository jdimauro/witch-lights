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

## Wiring and Soldering


## 3D Printing

The housings are designed to be easily printable on any commercial 3D printer; they were printed on a first-generation Makerbot Replicator in PLA and ABS plastic, and have been successfully tested on Cura, Printrbot, and Lulzbot printers.

## Software

The current iteration of the software is in the directory marked `witch-lights-classic`. This code is inelegant but functional; it successfully handles two PIR sensors and will trigger a different sprite animation when one or the other sensor is tripped. 

It does _not_ allow for the triggering of more than one sprite at a time, and does not respond to sensor trigger events while a sprite is being animated. 

## Final Assembly and Use