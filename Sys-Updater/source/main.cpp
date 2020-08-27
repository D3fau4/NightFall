#if __INTELLISENSE__
typedef unsigned int __SIZE_TYPE__;
typedef unsigned long __PTRDIFF_TYPE__;
#define __attribute__(q)
#define __builtin_strcmp(a,b) 0
#define __builtin_strlen(a) 0
#define __builtin_memcpy(a,b) 0
#define __builtin_va_list void*
#define __builtin_va_start(a,b)
#define __extension__
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <switch.h>
#include <iostream>

int main(int argc, char* argv[])
{
	consoleInit(NULL);

	// Other initialization goes here. As a demonstration, we print hello world.
	std::cout << "Hello World!\n" << std::endl;

	// Main loop
	while (appletMainLoop())
	{
		// Scan all the inputs. This should be done once for each frame
		hidScanInput();

		// hidKeysDown returns information about which buttons have been
		// just pressed in this frame compared to the previous one
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_A)
			std::cout << "Pressed A button!\n" << std::endl;

		if (kDown & KEY_PLUS)
			break; // break in order to return to hbmenu

		// Your code goes here

		// Update the console, sending a new frame to the display
		consoleUpdate(NULL);
	}

	// Deinitialize and clean up resources used by the console (important!)
	consoleExit(NULL);
	return 0;
}