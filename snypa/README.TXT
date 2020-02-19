Hi, this is my Linux Eclipse C++,  Xorg/OpenGL Demo Application. It has been assembled from tutorials and code snippits found on the internet.
It could be useful for an intermediate programmer getting started in Xorg/OpenGL.

You should be able to import it directy into Eclipse C++ IDE.

I've been slowly coding this since 2010 or so.  

Feel free to use it as you please.

It has joystick input (dual axis). You may need to remap the axis to suit your joystick. 

Keyboard Shortcuts:
	- Escape 	- Quit
	- F1-F10	- Mode Cycle
	- F12 		- Screenshot
	- Alt I 	- Toggle Info
	- Alt F 	- Toggle Fullscreen
	- Alt R 	- Toggle Rendering
	- Alt K 	- Toggle Keyboard Info
	- Alt M 	- Toggle Mouse Info
	- Alt J 	- Toggle Joystick Info
	- Alt B		- Toggle Blending
	- Alt D 	- Toggle Depth Testing 
	- Alt P 	- Print Video Modes
	- Alt V 	- Toggle VSync
	- +			- Increase Rate
	- - 		- Decrease Rate
	
Also the program has console output, so best to run from a terminal.

Dependencies:

	- pthread
	- GL
	- boost_system
	- boost_filesystem
	- GLU
	- X11
	- Xxf86vm

So you will need add the packages to your system, to compile.

Also, it uses the stb_image.h, stb_image_write.h, and stb_truetype.h libraries, for loading/saving textures and fonts.
	
Contact me (John Becker) on snypaproject@gmail.com for questions/comments.