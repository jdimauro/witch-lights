# Mystery Crash

I've spent several hours in the last couple days trying to debug a weird crash in the Witch Lights when writing to global variables.

Here's the memory report during the `setup()` function:

		Dynamic ram used: 0
		Program static ram used 7404
		Stack ram used 80

		My guess at free mem: 90820



		Dynamic ram used: 1188
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94492
		
		Loop Count: 0
		
		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		Loop Count: 1

During `loop()` running the test pattern, memory usage stays totally static: 

		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		Loop Count: 738
		
		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		Loop Count: 739

The crash happens on loop cycle 741, each time (I've run several tests):

		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		Loop Count: 740

So that's loop number 740. On 741, the contents of memory change:
		
		Dynamic ram used: 1348
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94332

Three things jump out at me.

* Dynamic RAM (the "heap") dropped from 1380 to 1348, a difference of 32 bytes
* The free memory estimate has changed from 94300 to 94332, which basically mirrors the change in the heap
* It crashes when it tries to access `counter`, which is a global int

I have previously had a crash just like this, when I was trying to access a global int array for the `noIdle` feature. I found that moving the value I was trying to access into the FaerieSprite class definition fixed the crash, and wrote it off as a scope issue to be debugged later. But what if this was the cause instead?

While I was trying to debug noIdle, I used the `debug(n)` function to light up LEDs during various stages of various method calls, to see where the crash occurred. What I found at the time was, the foremost test pattern sprite failed when `currentPixel == NUM_LEDS - 1`, after its `Update()` method called `MarkDone()` successfully. When that happens, the `isDone` bool is set to TRUE, and `SpriteManager` deletes that sprite's `Sprite` pointer from the `spriteVector` pointer array. 

Which seems to have happened, given the 32 bytes freed up from the heap. All righty, then. 

So, `SpriteManager` deletes a sprite, and then the exact *moment* we next try to access the `counter` global int, boom. We crash.

Or not. 