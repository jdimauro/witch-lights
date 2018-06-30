Trying to figure this out. 

I've added a number of modes and logic to the Witch Lights. They started as a set of global booleans at the top of the code, and to toggle them I'd change them from false to true and recompile and re-upload. 

I'd rather not have to do that when on site doing an installation, so I defined `INPUT_PULLUP` digital pins for each boolean, and can activate the various modes by attaching a jumper and connecting the relevant pin to ground. 

So, for example, in my `setup()` block, I've got:

		digitalRead(SPAWN_LURKERS_PIN) == LOW ? spawnLurkers = true : spawnLurkers = false;

So if I want lurkers to spawn randomly, I bridge pin 52 to ground. I tested that and it worked fine. So I added the rest of the lines to `setup()`, and the Witch Lights boot, run a test pattern, and then crash. 

Curious. 

As it turns out, only *some* of the `digitalRead()` statements cause the crash. The culprits:

* BURN_NIGHT_PIN
* DEBUG_INTERVAL
* DEBUG_INFLECTION

So, was it crashing when trying to read the pins, or trying to set the booleans? I'll spare you the description of my test process: it's the booleans. If you put `debugInterval = true;` as a standalone line of code, the Witch Lights crash after the test pattern. 

Your next idea might be, ok, what does `debugInterval` *do* when it's called in your code? And the answer: nothing. It's not referenced anywhere yet. I'm putting the plumbing in to set the bool first, and then I'm going to put the debug logic into the sprite methods. 

Like, this is literally a value in RAM that is not referenced anywhere, except to be set true or false depending on a pin reading. 

And so are the others that crash. *And*, so are some of the others which do *not* crash. Like, being an un-referenced boolean value isn't causing the crash. 

Right. OK. 

All the global bools are declared at the very top of the program. 

		bool debugMode = true;			
		bool spawnLurkers = false;	
		bool randomInflection = false
		bool spawnFaeries = false;	
		bool placeLurkers = false;	
		bool placeTrees = false;		
		bool placeNoIdle = false;		
		bool partyMode = false;
		bool videoMode = false;
		bool debugInterval = false;
		bool debugInflection = false;

Some, like `debugMode`, `spawnLurkers`, and `spawnFaeries`, trigger behavior when true or false. Others are there to be used once I've implemented the mode pins. 

And there's no real rhyme or reason as to which crash. The crashers are `partyMode`, `debugInterval`, and `debugInflection`. They're near the end of the list, but `videoMode` doesn't crash, and it's like in the middle of the set of bools there. ¯/\\_(ツ)_/¯

---
  
### 11:00 PM — Tuesday; June 26, 2018 ###

I got it to work, by commenting out all the `CRGB` and `char` globals and all the pre-rendered animation. 

For some reason, reducing the size of the static storage for globals fixes the problem?

¯\\_(ツ)_/¯