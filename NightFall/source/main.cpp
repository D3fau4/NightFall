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
bool onlineupdate = false;
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
	if (R_FAILED(ret = hiddbgInitialize()))
	{
		return 1;
	}
	if (R_FAILED(ret = nifmInitialize(NifmServiceType_Admin)))
	{
		return 1;
	}
	return ret;
}

void close_Services()
{
	brls::Logger::debug("Closing Services");
	setsysExit();
	amssuExit();
	splExit();
	smExit();
	hiddbgExit();
	nifmExit();
}

void deletetemp()
{
	brls::Logger::debug("Deleting temporal folders");
	FS::DeleteDir("/switch/NightFall/temp/");
	FS::DeleteFile("/switch/NightFall/temp.json");
}

void InitFolders()
{
	if (R_SUCCEEDED(FS::createdir("/switch/NightFall/")))
		brls::Logger::debug("se ha creado la carpeta");
	deletetemp();
	if (R_SUCCEEDED(FS::createdir("/switch/NightFall/temp/")))
		brls::Logger::debug("se ha creado la carpeta");
}

int main(int argc, char *argv[])
{
	// Init the app
	InitFolders();
	// init
	BackGround::BackgroundTasks meme;
	Network::Net net = Network::Net();

	brls::Logger::setLogLevel(brls::LogLevel::DEBUG);
	if (!brls::Application::init("NightFall"))
	{
		brls::Logger::error("Unable to init Borealis application");
		return EXIT_FAILURE;
	}

	// get config
	if (R_SUCCEEDED(FS::checkFile("/switch/NightFall/config.json")))
	{
		brls::Logger::debug("Create Config");
		std::ifstream o("romfs:/config.json");
		o >> Conf;
		o.close();
	}
	else
	{
		std::ifstream o("/switch/NightFall/config.json");
		o >> Conf;
		o.close();
	}

	/* Initialize Services */
	if (R_FAILED(Init_Services()))
	{
		brls::Application::crash("The services cant start");
		return EXIT_FAILURE;
	}
	/* Check if is atmosphere */
	if (sm::isRunning("rnx") || sm::isRunning("tx"))
	{
		brls::Logger::error("The software was closed because only works in atmosphere");
		return EXIT_FAILURE;
	}
	/*Check if is Ipatched/Mariko */
	if (spl::GetHardwareType() == "Mariko" || spl::HasRCMbug())
	{
		brls::Logger::error("The software was closed because only works in non-patched/mariko");
		is_patched = true;
	}
	else
	{
		brls::Logger::debug("Erista non patched");
	}

	// get server info
	if (net.HasInternet() == true)
	{
		std::string downloadlink = Conf["URL"].get<std::string>() + "info";
		brls::Logger::debug(downloadlink);
		net.Download(downloadlink, "/switch/NightFall/actual.json");
		std::ifstream i("/switch/NightFall/actual.json");
		i >> j;
		i.close();
	}

	// Create a view
	brls::TabFrame *rootFrame = new brls::TabFrame();
	rootFrame->setTitle("NightFall");
	rootFrame->setIcon(BOREALIS_ASSET("icon/NightFall.jpg"));

	// Firmware Tab
	brls::List *mainlist = new brls::List();

	Result ret = 0;
	SetSysFirmwareVersion ver;
	if (R_FAILED(ret = setsysGetFirmwareVersion(&ver)))
	{
		return NULL;
	}
	char firmwarever[0x43];
	if (j["Firmwver"].empty() == false)
	{
		if (j["Firmwver"].get<std::string>() == ver.display_version)
		{
			std::snprintf(firmwarever, sizeof(firmwarever), "Current system version: %s", ver.display_version);
			onlineupdate = false;
		}
		else
		{
			std::snprintf(firmwarever, sizeof(firmwarever), "Update required");
			onlineupdate = true;
		}
	}
	else
	{
		std::snprintf(firmwarever, sizeof(firmwarever), "Current system version: %s", ver.display_version);
		onlineupdate = false;
	}

	brls::ListItem *UpdateOnlineItem = new brls::ListItem("System Update", firmwarever);
	UpdateOnlineItem->getClickEvent()->subscribe([](brls::View *view) {
		//download
		brls::StagedAppletFrame *stagedFrame = new brls::StagedAppletFrame();
		stagedFrame->setTitle("System Update");
		if (onlineupdate == true && is_patched == false)
		{
			Network::Net net = Network::Net();
			std::string download = Conf["URL"].get<std::string>() + j["intfw"].get<std::string>();
			brls::Logger::debug(download);
			net.Download(download, "/switch/NightFall/temp.json");
			stagedFrame->addStage(new PreInstallUpdatePage(stagedFrame, "Download Update"));
			stagedFrame->addStage(new DownloadUpdatePage(stagedFrame));
			stagedFrame->addStage(new InstallUpdate(stagedFrame));
		}
		else
		{
			// añadir pantalla de "sistema actualizado"
			if (is_patched == false)
			{
				stagedFrame->addStage(new UpToDate(stagedFrame, "You are up to date."));
			}
			else
			{
				stagedFrame->addStage(new UpToDate(stagedFrame, "Sorry i dont support patched units :("));
			}
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
	//mainlist->addView(UpdateOfflineItem);

	// Settings Tab
	brls::List *Settingslist = new brls::List();

	brls::SelectListItem *wantExfat = new brls::SelectListItem(
		"Exfat",
		{"No", "Yes"}, 0, "Do you want exfat updates?");
	wantExfat->setSelectedValue(Conf["Exfat"].get<int>());

	brls::ListItem *Serverurl = new brls::ListItem("Server", "Change the URL");
	Serverurl->getClickEvent()->subscribe([](brls::View *view) {
		SwkbdConfig kbd;
		char tmpoutstr[50] = {0};
		Result rc = swkbdCreate(&kbd, 0);
		if (R_SUCCEEDED(rc))
		{
			swkbdConfigMakePresetDefault(&kbd);
			swkbdConfigSetInitialText(&kbd, Conf["URL"].get<std::string>().c_str());
			rc = swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));

			if (R_SUCCEEDED(rc))
			{
				brls::Logger::debug(tmpoutstr);
				Conf["URL"] = tmpoutstr;
			}

			swkbdClose(&kbd);
		}
	});
	Serverurl->setValue(Conf["URL"].get<std::string>());

	Settingslist->addView(Serverurl);
	Settingslist->addView(wantExfat);
	// add in the root MemeItem the tabs
	rootFrame->addTab("Firmware", mainlist);
	rootFrame->addSeparator();
	rootFrame->addTab("Settings", Settingslist);

	// Add the root view to the stack
	brls::Application::pushView(rootFrame);

	// Run the app
	while (brls::Application::mainLoop())
	{
		if (R_SUCCEEDED(FS::checkFile("/switch/NightFall/config.json")))
		{
			brls::Logger::debug("Create Config");
			std::ofstream out("/switch/NightFall/config.json");
			out << Conf;
			out.close();
		}
		if (wantExfat->getSelectedValue() != Conf["Exfat"].get<int>())
		{
			Conf["Exfat"] = wantExfat->getSelectedValue();
			std::ofstream out("/switch/NightFall/config.json");
			out << Conf;
			out.close();
		}
		if (Serverurl->getValue() != Conf["URL"].get<std::string>())
		{
			Serverurl->setValue(Conf["URL"].get<std::string>());
			std::ofstream out("/switch/NightFall/config.json");
			out << Conf;
			out.close();

			if (net.HasInternet() == true)
			{
				std::string downloadlink = Conf["URL"].get<std::string>() + "info";
				brls::Logger::debug(downloadlink);
				net.Download(downloadlink, "/switch/NightFall/actual.json");
				std::ifstream i("/switch/NightFall/actual.json");
				i >> j;
				i.close();
			}
		}
	}
	close_Services();
	deletetemp();
	// Exit
	return EXIT_SUCCESS;
}