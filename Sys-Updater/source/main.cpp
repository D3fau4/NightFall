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
#include <fstream>

#include <borealis.hpp>
#include <string>
#include "UI/UI.hpp"
#include "json.hpp"
#include "Types.hpp"
#include "net/net.hpp"
#include "FS/FS.hpp"
#include "sm/sm.hpp"
#include "amssu/amssu.h"
#include "thread.hpp"

using json = nlohmann::json;
json j;
bool onlineupdate = true;
/* Var */
static constexpr size_t UpdateTaskBufferSize = 0x100000;
AsyncResult m_prepare_result;

std::string Updatepath;
UpdateState m_UpdateState = UpdateState::NothingToDo;

/* Var */
Result Init_Services(void)
{
	Result ret = 0;

	if (R_FAILED(ret = setsysInitialize()))
	{
		return 1;
	}

	if (R_FAILED(ret = splInitialize()))
	{
		return 1;
	}
	if (R_FAILED(ret = amssuInitialize()))
	{
		return 1;
	}
	return ret;
}

void installUpdate(char *path)
{
	if (m_UpdateState == UpdateState::NeedsSetup)
	{
		if (R_FAILED(amssuSetupUpdate(nullptr, UpdateTaskBufferSize, path, true)))
		{
			brls::Application::crash("Fallo al hacer el setup del update");
		}
		m_UpdateState = UpdateState::NeedsPrepare;
	}
	else if (m_UpdateState == UpdateState::NeedsPrepare)
	{
		if (R_FAILED(amssuRequestPrepareUpdate(&m_prepare_result)))
		{
			brls::Application::crash("Fallo al preparar el update");
		}
	}
	//else if (m_UpdateState == UpdateState::NeedsPrepare)
}

void close_Services()
{
	setsysExit();
	amssuExit();
	splExit();
}

void deletetemp()
{
	FS::DeleteDir("/switch/Sys-Updater/temp/");
	FS::DeleteFile("/switch/Sys-Updater/temp.json");
}

void InitFolders()
{
	if (R_SUCCEEDED(FS::createdir("/switch/Sys-Updater/")))
		brls::Logger::debug("se ha creado la carpeta");
	if (R_SUCCEEDED(FS::createdir("/switch/Sys-Updater/temp/")))
		brls::Logger::debug("se ha creado la carpeta");
}

int main(int argc, char *argv[])
{
	// Init the app
	InitFolders();
	// init
	BackGround::BackgroundTasks meme;
	//meme.BackgroundTasks();
	Network::Net net = Network::Net();
	brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
	net.Download("http://192.168.1.128/info", "/switch/Sys-Updater/actual.json");
	std::ifstream i("/switch/Sys-Updater/actual.json");
	i >> j;
	if (!brls::Application::init("Sys-Updater"))
	{
		brls::Logger::error("Unable to init Borealis application");
		return EXIT_FAILURE;
	}

	/* Initialize Services */
	if (R_FAILED(Init_Services()))
	{
		brls::Application::crash("The software was closed because an error occured:\nSIGABRT (signal 6)");
		return EXIT_FAILURE;
	}

	// Create a view
	brls::TabFrame *rootFrame = new brls::TabFrame();
	rootFrame->setTitle("Sys-Updater");
	rootFrame->setIcon(BOREALIS_ASSET("icon/borealis.jpg"));

	// Firmware Tab
	brls::List *mainlist = new brls::List();

	Result ret = 0;
	SetSysFirmwareVersion ver;
	if (R_FAILED(ret = setsysGetFirmwareVersion(&ver)))
	{
		return NULL;
	}
	char firmwarever[0x43];
	if (j["Firmwver"].get<std::string>() == ver.display_version)
	{
		std::snprintf(firmwarever, sizeof(firmwarever), "Current system version: %s", ver.display_version);
	}
	else
	{
		std::snprintf(firmwarever, sizeof(firmwarever), "Se necesita update");
	}

	brls::ListItem *UpdateOnlineItem = new brls::ListItem("System Update", firmwarever);
	brls::ListItem *UpdateOfflineItem = new brls::ListItem("System Update Offline");
	UpdateOnlineItem->getClickEvent()->subscribe([](brls::View *view) {
		//download
		brls::StagedAppletFrame *stagedFrame = new brls::StagedAppletFrame();
		stagedFrame->setTitle("System Updater");
		if (onlineupdate == true)
		{
			Network::Net net = Network::Net();
			std::string download = "http://192.168.1.128/" + j["intfw"].get<std::string>();
			brls::Logger::debug(download);
			net.Download(download, "/switch/Sys-Updater/temp.json");
			stagedFrame->addStage(new InstallUpdatePage(stagedFrame, "Go to step 2"));
			stagedFrame->addStage(new DownloadUpdatePage(stagedFrame));
			//meme.m_Download = true;
		}
		else
		{
			// añadir pantalla de "sistema actualizado"
		}
		brls::Application::pushView(stagedFrame);
	});
	UpdateOfflineItem->getClickEvent()->subscribe([](brls::View *view) {
		//installUpdate("/switch/Sys-Updater/temp/");
	});
	mainlist->addView(UpdateOnlineItem);
	mainlist->addView(UpdateOfflineItem);

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
	close_Services();
	brls::Logger::debug("iniciando Finalizado del thread");

	brls::Logger::debug("Finalizado thread");
	deletetemp();
	// Exit
	return EXIT_SUCCESS;
}