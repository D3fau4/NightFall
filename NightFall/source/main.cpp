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
#include "psm/psm.hpp"
#include "amssu/amssu.h"
#include "thread.hpp"

using json = nlohmann::json;
json j;
json Conf;
namespace i18n = brls::i18n;	// for loadTranslations() and getStr()
using namespace i18n::literals; // for _i18n

bool onlineupdate = false;
bool is_patched = false;
bool HasEmummc = false;
std::string fwstring = "";
brls::SelectListItem *selectfirmoff;

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
	if (R_FAILED(ret = psmInitialize()))
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
	psmExit();
}

void deletetemp()
{
	brls::Logger::debug("Deleting temporal folders");
	FS::DeleteDir("/switch/NightFall/temp/");
	FS::DeleteFile("/switch/NightFall/temp.json");
}

void Chain(){
	//no Sleep
	appletSetCpuBoostMode(ApmCpuBoostMode_FastLoad);
	appletSetAutoSleepDisabled(true);
	appletSetAutoSleepTimeAndDimmingTimeEnabled(false);
	appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
}

void UnChain(){
	//Normal
	appletSetCpuBoostMode(ApmCpuBoostMode_Normal);
	appletSetAutoSleepDisabled(false);
	appletSetAutoSleepTimeAndDimmingTimeEnabled(true);
	appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleep);
}

void InitFolders()
{
	if (R_SUCCEEDED(FS::createdir("/switch/NightFall/")))
		brls::Logger::debug("se ha creado la carpeta");
	deletetemp();
	if (R_SUCCEEDED(FS::createdir("/switch/NightFall/temp/")))
		brls::Logger::debug("se ha creado la carpeta");
	if (R_SUCCEEDED(FS::createdir("/switch/NightFall/Firmwares/")))
		brls::Logger::debug("se ha creado la carpeta");
}

void WriteConfig()
{
	brls::Logger::debug("Create Config");
	std::ifstream o("romfs:/config.json");
	o >> Conf;
	o.close();
}

void CheckJson()
{
	if (Conf["DeleteFolder"].is_null())
	{
		brls::Logger::debug("OLD JSON");
		Conf.clear();
		FS::DeleteFile("/switch/NightFall/config.json");
		WriteConfig();
	}
	if (Conf["Exfat"].is_null())
	{
		brls::Logger::debug("OLD JSON");
		Conf.clear();
		FS::DeleteFile("/switch/NightFall/config.json");
		WriteConfig();
	}
}

void CheckHardware()
{
	/*Check if is Ipatched/Mariko */
	if (spl::GetHardwareType() == "Iowa" || spl::GetHardwareType() == "Hoag" || spl::GetHardwareType() == "Calcio" || spl::HasRCMbugPatched())
	{
		brls::Logger::error("Mariko or patched unit detected");
		is_patched = true;
	}
	else
	{
		brls::Logger::debug("Erista non patched");
	}
	if (spl::HasEmummc())
	{
		brls::Logger::debug("Have Emummc");
		HasEmummc = true;
		is_patched = false;
	}
	else
	{
		brls::Logger::debug("Doesnt have Emummc");
	}
}

int main(int argc, char *argv[])
{
	// init
	apmInitialize();
	Chain();

	// Init the app
	InitFolders();
	i18n::loadTranslations();
	
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
		WriteConfig();
	}
	else
	{
		std::ifstream o("/switch/NightFall/config.json");
		o >> Conf;
		o.close();
		CheckJson();
	}

	/* Initialize Services */
	if (R_FAILED(Init_Services()))
	{
		brls::Application::crash("main/crash/service_start"_i18n);
		//return EXIT_FAILURE;
	}
	/* Check if is atmosphere */
	if (sm::isRunning("rnx") || sm::isRunning("tx"))
	{
		brls::Logger::error("main/crash/service_TX"_i18n);
		return EXIT_FAILURE;
	}
	// Check some shit
	CheckHardware();

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
			std::snprintf(firmwarever, sizeof(firmwarever), "%s: %s", "main/tabs/Firmware/update/current_fw"_i18n.c_str(), ver.display_version);
			onlineupdate = false;
		}
		else
		{
			std::snprintf(firmwarever, sizeof(firmwarever), "main/tabs/Firmware/update/update_required"_i18n.c_str());
			onlineupdate = true;
		}
	}
	else
	{
		std::snprintf(firmwarever, sizeof(firmwarever), "%s: %s", "main/tabs/update/Firmware/current_fw"_i18n.c_str(), ver.display_version);
		onlineupdate = false;
	}

	brls::Logger::debug(j["Firmwver"].get<std::string>());
	brls::Logger::debug(ver.display_version);

	brls::ListItem *UpdateOnlineItem = new brls::ListItem("main/tabs/Firmware/update/title"_i18n.c_str(), firmwarever);
	UpdateOnlineItem->getClickEvent()->subscribe([](brls::View *view) {
		//download
		brls::StagedAppletFrame *stagedFrame = new brls::StagedAppletFrame();
		stagedFrame->setTitle("main/tabs/Firmware/update/title"_i18n.c_str());
		if (onlineupdate == true && is_patched == false && psm::GetBatteryState() >= 15)
		{
			Chain();
			Network::Net net = Network::Net();
			std::string download = Conf["URL"].get<std::string>() + j["intfw"].get<std::string>();
			brls::Logger::debug(download);
			net.Download(download, "/switch/NightFall/temp.json");
			stagedFrame->addStage(new PreInstallUpdatePage(stagedFrame, "main/tabs/Firmware/update/update_download"_i18n.c_str()));
			stagedFrame->addStage(new DownloadUpdatePage(stagedFrame));
			stagedFrame->addStage(new InstallUpdate(stagedFrame, j["Firmwver"].get<std::string>().c_str()));
		}
		else
		{
			// añadir pantalla de "sistema actualizado"
			if (psm::GetBatteryState() <= 15 && onlineupdate == false)
			{
				stagedFrame->addStage(new UpToDate(stagedFrame, "main/tabs/Firmware/update/update_lowbattery"_i18n.c_str()));
			}
			else if (is_patched == false)
			{
				stagedFrame->addStage(new UpToDate(stagedFrame, "main/tabs/Firmware/update/update_uptodate"_i18n.c_str()));
			}
			else
			{
				stagedFrame->addStage(new UpToDate(stagedFrame, "main/tabs/Firmware/update/update_mariko"_i18n.c_str()));
			}
		}
		brls::Application::pushView(stagedFrame);
	});

	mainlist->addView(UpdateOnlineItem);

	std::vector<std::string> FirmwareOfflist = FS::getDirectories("/switch/NightFall/Firmwares/");
	if (!FirmwareOfflist.empty())
	{
		// list firmwares
		selectfirmoff = new brls::SelectListItem(
			"main/tabs/Firmware/update/update_select"_i18n.c_str(),
			FirmwareOfflist, 0);

		brls::ListItem *UpdateOfflineItem = new brls::ListItem("main/tabs/Firmware/update/title_offline"_i18n.c_str());
		UpdateOfflineItem->getClickEvent()->subscribe([](brls::View *view) {
			Chain();
			// algo
			brls::StagedAppletFrame *stagedFrame = new brls::StagedAppletFrame();
			stagedFrame->setTitle("main/tabs/Firmware/update/title_offline"_i18n.c_str());
			stagedFrame->addStage(new PreOfflineInstallPage(stagedFrame, "main/tabs/Firmware/update/update_install"_i18n.c_str()));
			stagedFrame->addStage(new InstallUpdate(stagedFrame, fwstring, "/switch/NightFall/Firmwares/" + fwstring + "/"));
			brls::Application::pushView(stagedFrame);
		});

		mainlist->addView(UpdateOfflineItem);
		mainlist->addView(selectfirmoff);
	}

	// Settings Tab
	brls::List *Settingslist = new brls::List();

	brls::SelectListItem *wantExfat = new brls::SelectListItem(
		"Exfat",
		{"main/tabs/Settings/exfat_options/no"_i18n.c_str(), "main/tabs/Settings/exfat_options/yes"_i18n.c_str()}, 0, "main/tabs/Settings/exfat_options/subtitle"_i18n.c_str());
	wantExfat->setSelectedValue(Conf["Exfat"].get<int>());

	brls::SelectListItem *DeletePostInstall = new brls::SelectListItem(
		"main/tabs/Settings/delete_options/title"_i18n.c_str(),
		{"main/tabs/Settings/delete_options/no"_i18n.c_str(), "main/tabs/Settings/delete_options/yes"_i18n.c_str()}, 0, "main/tabs/Settings/delete_options/subtitle"_i18n.c_str());
	DeletePostInstall->setSelectedValue(Conf["DeleteFolder"].get<int>());

	brls::ListItem *Serverurl = new brls::ListItem("main/tabs/Settings/change_url/title"_i18n.c_str(), "main/tabs/Settings/change_url/subtitle"_i18n.c_str());
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
	Settingslist->addView(DeletePostInstall);
	// add in the root MemeItem the tabs
	rootFrame->addTab("main/tabs/Firmware/title"_i18n.c_str(), mainlist);
	rootFrame->addSeparator();
	rootFrame->addTab("main/tabs/Settings/title"_i18n.c_str(), Settingslist);

	// Add the root view to the stack
	brls::Application::pushView(rootFrame);
	UnChain();
	// Run the app
	while (brls::Application::mainLoop())
	{
		if (!FirmwareOfflist.empty())
			fwstring = FirmwareOfflist[selectfirmoff->getSelectedValue()];
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
		if (DeletePostInstall->getSelectedValue() != Conf["DeleteFolder"].get<int>())
		{
			Conf["DeleteFolder"] = DeletePostInstall->getSelectedValue();
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