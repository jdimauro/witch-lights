# Mystery Crash

I've spent several hours in the last couple days trying to debug a weird crash in the Witch Lights when writing to global variables. 

The TL;DR of it is, depending on various factors, sometimes when I try to read global `bool` mode values and modify them in SRAM, the Arduino Due stops responding shortly after its first sprite reaches pixel `NUM_LEDS`, i.e. has gone off the end of the strip. The following is a log of Arduino Memory Kremlinology, where I attempt to interpret the behavior of RAM on an Arduino Due by checking who stands next to Stalin in the May Day Parade. 
=======
I've spent several hours in the last couple days trying to debug a weird crash in the Witch Lights when writing to global variables. The short version is, when I define *some*, but not *all* global variables, and then try to read or write to those globals, the Arduino freezes up after the test pattern reaches the end of the LED strip. 

I can work around the issue by either commenting out the specific globals in question (and any code that references them), or by commenting out other globals that get loaded into RAM, such as pre-rendered raster animations. Which, the first thing I thought--in fact, the first thing anyone who I talk to about this thinks--is that I've run out of memory somehow. This kind of thing is exactly what happens when you're up at the limit of your available SRAM. That is where I began looking. 

Here is a memory map of the Arduino Due:

		0x0008 0000 - 0x000B FFFF   256 KiB flash bank 0
		0x000C 0000 - 0x000F FFFF   256 KiB flash bank 1
		                            Both banks above provide 512 KiB of contiguous flash memory
		0x2000 0000 - 0x2000 FFFF   64 KiB SRAM0
		0x2007 0000 - 0x2007 FFFF   64 KiB mirrored SRAM0, so that it's consecutive with SRAM1
		0x2008 0000 - 0x2008 7FFF   32 KiB SRAM1
		0x2010 0000 - 0x2010 107F   4224 bytes of NAND flash controller buffer

One key takeaway is that the Due has a contiguous address space, despite having separate 64K and 32K banks. That address space ranges from `0x2007 0000` to `0x2008 7FFF`. 

Because the Due is basically a weird experiment that escaped into the wild, the usual Arduino instructions for viewing available RAM don't work. [Fortunately, I found instructions here](https://forum.arduino.cc/index.php?topic=182759.0). The memory report code is looking at the contiguous RAM address space I just mentioned, like so:

		char *ramstart=(char *)0x20070000;
		char *ramend=(char *)0x20088000;

The code calculates 4 things:

* Dynamic RAM used (the "heap", which grows from the "top" of the static area, "up")
* Static RAM used (globals and static variables, in a reserved space "under" the heap)
* Stack RAM used (local variables, interrupts, function calls are stored here, starting at the "top" of the SRAM address space and growing "down" towards the heap; when functions complete, their local variables and pointers are cleaned up, and the stack shrinks)
* "Guess at free mem" (which is complicated)

The "free mem" calculation is `stack_ptr - heapend + mi.fordblks`

Which, in theory, is subtracting the totaly amount of unallocated memory blocks in the range below the stack? I think? I'm not sure. I'm reading the internet and interpreting.

Here's the memory report during the `setup()` function:

		Dynamic ram used: 0
		Program static ram used 7404
		Stack ram used 80

		My guess at free mem: 90820

OK, so total free RAM is reporting at roughly 88K out of 96K, not bad. 

		Dynamic ram used: 1188
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94492
		
		Loop Count: 0
		
Now, at this point we're executing the main `loop()` function for the first time, and here we see something interesting. The total amount of RAM used and the guess at free RAM no longer add up. What gives? 

Well, what happens here is that memory in the *heap*, the dynamic RAM reported up top, has been freed up, but because more stuff is sitting on "top" of it in memory address space, the memory isn't really actually free to be used. The 
		
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

So, here's a hypothesis: `SpriteManager` deletes a sprite, and then the exact *moment* we next try to access the `counter` global int, boom. We crash.

The problem is, if so, finding the root cause of this is hard. 

So I'm not going to. 

Instead, I'm going to take the booleans *out* of globals entirely, and change them into functions, which will query the mode pin when they are called. Memory used by functions is stored in the stack, completely on the other side of the memory address space from where we're allocating and deleting sprites from memory. 

That's the plan, anyway. I'll report back when I run the first experiments. 