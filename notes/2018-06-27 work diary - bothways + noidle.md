# Mystery: noIdle fails when travelDirection = -1

Here's the memory report during the `setup()` function:

		Dynamic ram used: 0
		Program static ram used 7404
		Stack ram used 80

		My guess at free mem: 90820



		Dynamic ram used: 1188
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94492
		
		0
		
		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		1

During `loop()` running the test pattern, memory usage stays totally static: 

		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		738
		
		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		739

The crash:

		Dynamic ram used: 1380
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94300
		
		740
		
		Dynamic ram used: 1348
		Program static ram used 7404
		Stack ram used 104

		My guess at free mem: 94332

