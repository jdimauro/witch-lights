When we create a generic metaclass for the customizeable sprites, we plan to have different Update methods, and a decision metric for the Update method to decide which to call.

One of those methods should be a Travel method, which is our term for moving from point to point and leaving a trail behind the sprite. 

Right now, in order to specify the travel destination, we call the `SetNextInflection()` method, which changes the target pixel for when the sprite enters travel mode. 

I'm thinking we should also specify a couple of other factors. 

So, `SetNextInflection()` gives us a target pixel of 149, and we're on pixel 20. 

When we created the sprite, we should have specified a few factors: 

`DefaultTravelIntervalAccelerationFactor` is the linear term for how much we change the interval while accelerating. `-1` is the current default. So, each interval, we subtract 1 from the `updateInterval` int. 

OR we could do dynamic acceleration factors based on how far we have to travel?

`DefaultTravelStartInterval` is the interval we always start with when traveling from pixel a to pixel b from a stop. `40ms` is the current default interval

`DefaultTravelMinimumInterval` basically defines our top speed assuming that V is constant. `0ms` is the current default

`DefaultTravelMaximumInterval` would define the slowest the sprite is allowed to move before stopping. 

(This may not be necessary, I'm envisioning an edge case where for some reason the interval gets set to 6 seconds between frames and we can't tell if it crashed or not.)

### Accelerating and Braking

I can envision 2 ways of handling this. 

One way, for each move, we specify a travel to one destination pixel with a positive acceleration factor, and when we reach that pixel, we specify a much shorter travel to a much closer pixel, with a more aggressive negative acceleration factor, which would smoothly slow the sprite down for an "ease in". 

Another way, we specify that traveling is a two-part process, with acceleration and braking. 

So if we want to travel 30 pixels, we'd accelerate for 20 pixels, and brake for 10, or 25 and 5, or whatever ends up looking best according to testing. The braking factor would usually be higher than the acceleration factor. 

So we could specify stuff for each sprite to give it a profile for acceleration and braking, like:

`DefaultTravelIntervalBrakeRatio` is an implementation guess, where we say "take the total number of pixels you have to travel, and multiply by this number. The result is the number of pixels back from the destination pixel you have to brake for. So accelerate until that pixel, then when you reach it, start braking." So if `DefaultTravelIntervalBrakeRatio` was `.1` then given a total number of 120 pixels to travel, we'd accelerate for 108 pixels, and brake for 12.

You'd want to round up, so that you always have at least 1 pixel of braking. 

Brake factor: do we specify this as a factor in sprite creation? Or do we do something like:

* take current interval and subtract it from `DefaultTravelMaximumInterval`: that's how much braking needs to happen over the brake distance
* divide the result by the number of pixels before the target pixel

(That's a linear brake curve, we might actually want a parabolic curve instead?)

I mean, let's assume there's no dumb assumptions in that pseudo-algorithm, what I mean is, given travel of `n` pixels, we always use `n * DefaultTravelIntervalBrakeRatio` pixels to brake, and always brake down to the same (maximum) interval. 

### Talking through the example: linear acceleration factor

So if we are traveling 120 pixels, with a default acceleration factor of -1, default brake ratio of .1, and a `DefaultTravelStartInterval` of 40ms, a `DefaultTravelMinimumInterval` of 5ms, and `DefaultTravelMaximumInterval` of 60ms, then:

* We'd multiply 120 by .1 and get 12
* 120-12 = 108; we would start accelerating towards pixel `currentPixel + 108`
* Using a linear acceleration factor of -1ms, we'd accelerate over 35 intervals, traveling 35 pixels
* Over the remaining 73 pixels, the sprite will travel at a constant maximum velocity
* The brake factor is calculated from the `DefaultTravelMaximumInterval`, current interval, and the calculated brake distance: in this case, we have 12 pixels to go from 5ms to 60ms. A naive implementation would be `(DefaultTravelMaximumInterval - CurrentInterval)/brake pixels`, or `55/12` = 4.58ms. Rounding up to the next highest int, we get 5ms. 
* So, over the 12 pixels, at each interval, we'd add 5ms to the current interval, maxing out at 60ms. 

For a much shorter move, let's say we want to move 10 pixels. 

* multiply 10 by .1, we get 1 (which isn't a great thing)
* we have 9 pixels to accelerate; starting at our default interval of 40ms, at pixel `currentPixel + 9`, `currentInterval` will be 31ms
* the brake factor calculates out at `29/1`, or 29ms in a single step

This is why I think we should be able to specify factors for shorter moves. 

### UpdateTravel method

I'm thinking that, when we transition from scanner to travel, we set the `isScanning` bool to `false` and use a `StartTravelWithAcceleration()` method to define the target pixel, the acceleration factor, the brake factor, and I'm thinking we should also be able to specify the `TravelMaximumInterval` to determine what speed we're braking down to? That way we can totally set ourselves up for a seamless transition into a pre-rendered pixel animation. 

So if we want to move n pixels in either direction, you'd specify:

* Target distance (negative moves "backwards")
* Acceleration Factor
* Brake Ratio
* Maximum Interval

At a guess, when moving 10 pixels, I would set acceleration to -4, brake ratio to .25 or .33, and maximum interval to maybe 40-50ms? And start tuning from there. 

That way, we start at 40ms, and accelerate to 12ms over 7 pixels, then over 3 pixels, the brake factor is `38/3 = 12.66`, rounded to 13, so we'd brake by 13ms each interval to a maximum of 50ms at the target pixel.  

### Calculating smooth acceleration over distance?

Do we want to try, instead of using a fixed linear acceleration factor, to calculate that factor based on the distance we're accelerating over? 

This could be an optional thing for travel moves?

It'd work like the brake calculation: figure out how many pixels we're accelerating for, then divide the difference between `currentInterval` and `TravelMinimumInterval` over those pixels, and the result is how many ms to accelerate by each interval? 

So, for travel moves, you'd say "here's my start speed, here's my maximum speed, here's my minimum speed, here's my brake distance ratio, here's the pixel I'm traveling to" and it would calculate out the acceleration factors for you.