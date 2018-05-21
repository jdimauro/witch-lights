# Progress summary 2018-05-20

[Algorithmic fading trails using FastLED libraries][vid1]

[vid1]: https://vimeo.com/271001287

## DimTrail() function

## Algorithmic braking

Now that I've done some pixel animation of how I want the faerie sprites to move, I've worked out that I like to have them slow down and stop over a range of about 4-8 pixels, with the trails fading as they slow. I'm now working on slowing the sprites down by manipulating the framerate (which is how they accelerate). 

I just put in some movement logic where the sprites calculate their distance from their destination pixel, and when that is within a certain percentage of the overall travel distance, it starts braking. 

However, currently deceleration is handled by a single factor; how many milliseconds do you add or subtract to the `updateInterval` on each call of `updateTravel()`? 

For acceleration from stop, it's simple enough to start at about 40ms, and subtract 1ms per interval as you travel. The result is a reasonably natural enough acceleration towards "cruising speed". 

For making the sprite slow down to a stop, however, that doesn't really result in a pleasing deceleration curve. 

[Algorithmic brake at 15% of total distance travelled][vid2]

[vid2]: https://vimeo.com/271006221

So, one thing I need to look into: a more graceful way to manipulate `updateInterval` (as in, number of ms per animation frame) for deceleration and a slow stop and transition into idle. 

## Algorithmic idle animation

After the faerie sprite in the above video slows to a stop, it idles across 3 pixels, back and forth. I sketched this in terms of pixel values from the `colorPalette` array in my new notebook, and my friend [Scott Longely][sl] translated it into a set of simple algorithms for each pixel last night. 

[sl]: http://s-a-w-s.blogspot.com

That isn't the full desired result, though. What I need to do next is to write a `colorFade()` function to transition smoothly between pixel values. 

I've found example code as gists on GitHub. Both use FastLED functions for the fade, which have so far resulted in good animation performance, so I'm game to see how they go. 

* [Example One](https://gist.github.com/kriegsman/1f7ccbbfa492a73c015e)
* [Example Two](https://gist.github.com/kriegsman/d0a5ed3c8f38c64adcb4837dafb6e690)

I have so far not looked at the code at all to see how suitible it may be. That's for tomorrow, I believe. 

## Transition from idle to travel

There's a visual discontinuity at the moment where the animation transitions from `isIdling` to `false`; at that moment, it chooses a new destination, and draws the `travel` sprite. Unfortunately, the last frame of the idle animation and the travel sprite do not match perfectly. 

So one my my next action items is to fix that. 

## Faeries go both ways

