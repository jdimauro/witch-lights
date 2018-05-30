# Progress: Slow, stop, go

Next up in terms of goals is to see how to make the sprite slow and stop naturally, and then accelerate back up to travel speed. 

[Witch Lights Animation Test 2: Slow, stop, go][video]:

[video]: https://vimeo.com/267676592

I've spend a few iterations tuning this. The deceleration curve is fairly decent, but it doesn't look right when it starts up from a hard stop. I think the sprite should "wind up" before going, so that means backing up a bit. I'll have to play with the timing to make it look right. 

Once I have these dialed in, I can break them up into "intro" and "outro" animations, and create some loop animations to put in between. 