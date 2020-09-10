/*Copyright (c) 2020 D3fau4

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

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
#include "spl/spl.hpp"
#include "amssu/amssu.h"
#include "thread.hpp"

using json = nlohmann::json;
json j;
json Conf;
bool onlineupdate = true;
bool is_patched = false;
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
	if (R_FAILED(ret = smInitialize()))
	{
		return 1;
	}
	return ret;
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
	Network::Net net = Network::Net();
	std::ifstream o("/switch/Sys-Updater/config.json");
	o >> Conf;
	brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
	std::string downloadlink = Conf["URL"].get<std::string>() + "info";
	net.Download(downloadlink, "/switch/Sys-Updater/actual.json");
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
	/* Check if is atmosphere */
	if (sm::isRunning("rnx") || sm::isRunning("tx"))
	{
		brls::Logger::error("The software was closed because only works in atmosphere");
		return EXIT_FAILURE;
	}
	 /*Check if is Ipatched/Mariko */
	if (spl::GetHardwareType() == "Mariko" || spl::HasRCMbug()){
		brls::Logger::error("The software was closed because only works in non-patched/mariko");
		is_patched = true;
	} else {
		brls::Logger::debug("Erista non patched");
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
	UpdateOnlineItem->getClickEvent()->subscribe([](brls::View *view) {
		//download
		brls::StagedAppletFrame *stagedFrame = new brls::StagedAppletFrame();
		stagedFrame->setTitle("System Updater");
		if (onlineupdate == true && is_patched == false)
		{
			Network::Net net = Network::Net();
			std::string download = Conf["URL"].get<std::string>() + j["intfw"].get<std::string>();
			brls::Logger::debug(download);
			net.Download(download, "/switch/Sys-Updater/temp.json");
			stagedFrame->addStage(new PreInstallUpdatePage(stagedFrame, "Download Update"));
			stagedFrame->addStage(new DownloadUpdatePage(stagedFrame));
			stagedFrame->addStage(new InstallUpdate(stagedFrame));
		}
		else
		{
			// añadir pantalla de "sistema actualizado"
		}
		brls::Application::pushView(stagedFrame);
	});
	brls::ListItem *UpdateOfflineItem = new brls::ListItem("System Update Offline");
	UpdateOfflineItem->getClickEvent()->subscribe([](brls::View *view) {
		//download
		brls::StagedAppletFrame *stagedFrame1 = new brls::StagedAppletFrame();
		stagedFrame1->setTitle("System Updater");
		if (onlineupdate == true)
		{
			stagedFrame1->addStage(new InstallUpdate(stagedFrame1));
			//meme.m_Download = true;
		}
		brls::Application::pushView(stagedFrame1);
	});
	mainlist->addView(UpdateOnlineItem);
	mainlist->addView(UpdateOfflineItem);

	// Settings Tab
	/*brls::List *Settingslist = new brls::List();

	brls::ListItem *MemeItem = new brls::ListItem("Memes");
	brls::SelectListItem *TypeUpdateItem = new brls::SelectListItem(
		"Update",
		{"Nintendo", "Atmosphere"}, 0, "memes");
	Settingslist->addView(MemeItem);
	Settingslist->addView(TypeUpdateItem);*/
	// add in the root MemeItem the tabs
	rootFrame->addTab("Firmware", mainlist);
	rootFrame->addSeparator();
	//rootFrame->addTab("Settings", Settingslist);

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