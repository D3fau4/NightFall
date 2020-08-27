#if __INTELLISENSE__
typedef unsigned int __SIZE_TYPE__;
typedef unsigned long __PTRDIFF_TYPE__;
#define __attribute__(q)
#define __builtin_strcmp(a, b) 0
#define __builtin_strlen(a) 0
#define __builtin_memcpy(a, b) 0
#define __builtin_va_list void *
#define __builtin_va_start(a, b)
#define __extension__
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <stdio.h>
#include <stdlib.h>

#include <borealis.hpp>
#include <string>

#include "Types.hpp"
#include "sm/sm.hpp"
#include "amssu/amssu.h"

int main(int argc, char *argv[])
{
	Result rc = 0;

	// Init the app
	brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
	if (!brls::Application::init("Sys-Updater"))
	{
		brls::Logger::error("Unable to init Borealis application");
		return EXIT_FAILURE;
	}

	/* Check and Initialize ams:su. */
	if (!sm::isRunning("ams:su"))
	{
		if (R_FAILED(rc = amssuInitialize()))
		{
			brls::Application::crash("The software was closed because an error occured:\nSIGABRT (signal 6)");
			sleep(5000);
			return EXIT_FAILURE;
		}
	}

	// Create a sample view
	brls::TabFrame *rootFrame = new brls::TabFrame();
	rootFrame->setTitle("Sys-Updater");
	rootFrame->setIcon(BOREALIS_ASSET("icon/borealis.jpg"));
	// Firmware Tab
	brls::List *mainlist = new brls::List();

	brls::ListItem *UpdateItem = new brls::ListItem("Check Firmwares Updates", "version: 11.0.0");
	UpdateItem->getClickEvent()->subscribe([](brls::View *view) {

	});
	mainlist->addView(UpdateItem);

	// Settings Tab
	brls::List *Settingslist = new brls::List();

	brls::ListItem *MemeItem = new brls::ListItem("Memes");
	brls::SelectListItem *TypeUpdateItem = new brls::SelectListItem(
		"Update",
		{"Nintendo", "Atmosphere"}, 0, "memes");
	Settingslist->addView(MemeItem);
	Settingslist->addView(TypeUpdateItem);
	// add in the root MemeItem the tabs
	rootFrame->addTab("Firmware", mainlist);
	rootFrame->addSeparator();
	rootFrame->addTab("Settings", Settingslist);

	// Add the root view to the stack
	brls::Application::pushView(rootFrame);

	// Run the app
	while (brls::Application::mainLoop())
		;

	// Exit
	return EXIT_SUCCESS;
}