#include <cstring>

#include <guisan.hpp>
#include <SDL_ttf.h>
#include <guisan/sdl.hpp>
#include "SelectorEntry.hpp"

#include "sysdeps.h"
#include "options.h"
#include "gui_handling.h"
#include "fsdb_host.h"
#include "uae.h"
#include "zfile.h"
#include "disk.h"

static const char* diskswapper_columns[] =
{
	"#", "Disk Image", "Drive"
};

static const int diskswapper_column_size[] =
{
	20, // #
	420, // Disk image
	SMALL_BUTTON_WIDTH * 2 // Drive
};

static gcn::Label* lblDiskSwapperList[3];
static gcn::Container* diskSwapperListEntry[MAX_SPARE_DRIVES];
static gcn::TextField* diskSwapperListCells[MAX_SPARE_DRIVES][2];
static gcn::Button* cmdDiskSwapperListDrive[MAX_SPARE_DRIVES];
static gcn::Button* cmdDiskSwapperListAdd[MAX_SPARE_DRIVES];
static gcn::ImageButton* cmdDiskSwapperListDelete[MAX_SPARE_DRIVES];
static gcn::Button* cmdDiskSwapperRemoveAll;

static int disk_in_drive(int entry)
{
	for (int i = 0; i < 4; i++) {
		if (_tcslen(changed_prefs.dfxlist[entry]) > 0 && !_tcscmp(changed_prefs.dfxlist[entry], changed_prefs.floppyslots[i].df))
			return i;
	}
	return -1;
}

static int disk_swap(int entry, int mode)
{
	int drv, i, drvs[4] = { -1, -1, -1, -1 };

	for (i = 0; i < MAX_SPARE_DRIVES; i++) {
		drv = disk_in_drive(i);
		if (drv >= 0)
			drvs[drv] = i;
	}
	if ((drv = disk_in_drive(entry)) >= 0) {
		if (mode < 0) {
			changed_prefs.floppyslots[drv].df[0] = 0;
			return 1;
		}

		if (_tcscmp(changed_prefs.floppyslots[drv].df, currprefs.floppyslots[drv].df)) {
			_tcscpy(changed_prefs.floppyslots[drv].df, currprefs.floppyslots[drv].df);
			disk_insert(drv, changed_prefs.floppyslots[drv].df);
		}
		else {
			changed_prefs.floppyslots[drv].df[0] = 0;
		}
		if (drvs[0] < 0 || drvs[1] < 0 || drvs[2] < 0 || drvs[3] < 0) {
			drv++;
			while (drv < 4 && drvs[drv] >= 0)
				drv++;
			if (drv < 4 && changed_prefs.floppyslots[drv].dfxtype >= 0) {
				_tcscpy(changed_prefs.floppyslots[drv].df, changed_prefs.dfxlist[entry]);
				disk_insert(drv, changed_prefs.floppyslots[drv].df);
			}
		}
		return 1;
	}
	for (i = 0; i < 4; i++) {
		if (drvs[i] < 0 && changed_prefs.floppyslots[i].dfxtype >= 0) {
			_tcscpy(changed_prefs.floppyslots[i].df, changed_prefs.dfxlist[entry]);
			disk_insert(i, changed_prefs.floppyslots[i].df);
			return 1;
		}
	}
	_tcscpy(changed_prefs.floppyslots[0].df, changed_prefs.dfxlist[entry]);
	disk_insert(0, changed_prefs.floppyslots[0].df);
	return 1;
}

static void diskswapper_addfile2(struct uae_prefs* prefs, const TCHAR* file)
{
	int list = 0;
	while (list < MAX_SPARE_DRIVES) {
		if (!strcasecmp(prefs->dfxlist[list], file))
			break;
		list++;
	}
	if (list == MAX_SPARE_DRIVES) {
		list = 0;
		while (list < MAX_SPARE_DRIVES) {
			if (!prefs->dfxlist[list][0]) {
				_tcscpy(prefs->dfxlist[list], file);
				fullpath(prefs->dfxlist[list], MAX_DPATH);
				break;
			}
			list++;
		}
	}
}

static void diskswapper_addfile(struct uae_prefs* prefs, const TCHAR* file)
{
	struct zdirectory* zd = zfile_opendir_archive(file, ZFD_ARCHIVE | ZFD_NORECURSE);
	if (zd && zfile_readdir_archive(zd, NULL, true) > 1) {
		TCHAR out[MAX_DPATH];
		while (zfile_readdir_archive(zd, out, true)) {
			struct zfile* zf = zfile_fopen(out, _T("rb"), ZFD_NORMAL);
			if (zf) {
				int type = zfile_gettype(zf);
				if (type == ZFILE_DISKIMAGE)
					diskswapper_addfile2(prefs, out);
				zfile_fclose(zf);
			}
		}
		zfile_closedir_archive(zd);
	}
	else {
		diskswapper_addfile2(prefs, file);
	}
}


class DiskSwapperAddActionListener:public gcn::ActionListener
{
public:
	void action(const gcn::ActionEvent& action_event) override
	{
		for (auto i = 0; i < MAX_SPARE_DRIVES; ++i)
		{
			if (action_event.getSource() == cmdDiskSwapperListAdd[i])
			{
				//---------------------------------------
				// Select disk for drive
				//---------------------------------------
				char tmp[MAX_DPATH];

				if (strlen(changed_prefs.dfxlist[i]) > 0)
					strncpy(tmp, changed_prefs.dfxlist[i], MAX_DPATH);
				else
					strncpy(tmp, current_dir, MAX_DPATH);
				if (SelectFile("Select disk image file", tmp, diskfile_filter))
				{
					if (strncmp(changed_prefs.dfxlist[i], tmp, MAX_DPATH) != 0)
					{
						strncpy(changed_prefs.dfxlist[i], tmp, MAX_DPATH);
						AddFileToDiskList(tmp, 1);
					}
				}
				cmdDiskSwapperListAdd[i]->requestFocus();
			}
		}
		RefreshPanelDiskSwapper();
	}
};

static DiskSwapperAddActionListener* diskSwapperAddActionListener;

class DiskSwapperDelActionListener : public gcn::ActionListener
{
public:
	void action(const gcn::ActionEvent& action_event) override
	{
		for (auto i = 0; i < MAX_SPARE_DRIVES; ++i)
		{
			if (action_event.getSource() == cmdDiskSwapperListDelete[i])
			{
				changed_prefs.dfxlist[i][0] = 0;
			}
		}
		RefreshPanelDiskSwapper();
	}
};

static DiskSwapperDelActionListener* diskSwapperDelActionListener;

class DiskSwapperRemoveAllActionListener : public gcn::ActionListener
{
public:
	void action(const gcn::ActionEvent& action_event) override
	{
		if (action_event.getSource() == cmdDiskSwapperRemoveAll)
		{
			for (auto row = 0; row < MAX_SPARE_DRIVES; ++row)
			{
				changed_prefs.dfxlist[row][0] = 0;
				disk_swap(row, -1);
			}
			RefreshPanelDiskSwapper();
			RefreshPanelFloppy();
			RefreshPanelQuickstart(); 
		}
	}
};

static DiskSwapperRemoveAllActionListener* diskSwapperRemoveAllActionListener;

class DiskSwapperDriveActionListener : public gcn::ActionListener
{
public:
	void action(const gcn::ActionEvent& action_event) override
	{
		for (auto row = 0; row < MAX_SPARE_DRIVES; ++row)
		{
			if (action_event.getSource() == cmdDiskSwapperListDrive[row])
			{
				disk_swap(row, 1);

				AddFileToDiskList(changed_prefs.dfxlist[row], 1);
				RefreshPanelDiskSwapper();
				RefreshPanelFloppy();
				RefreshPanelQuickstart();
			}
		}
		
	}
};

static DiskSwapperDriveActionListener* diskSwapperDriveActionListener;

void InitPanelDiskSwapper(const config_category& category)
{
	int row, column;
	auto posY = DISTANCE_BORDER / 2;
	char tmp[20];

	diskSwapperAddActionListener = new DiskSwapperAddActionListener();
	diskSwapperDelActionListener = new DiskSwapperDelActionListener();
	diskSwapperRemoveAllActionListener = new DiskSwapperRemoveAllActionListener();
	diskSwapperDriveActionListener = new DiskSwapperDriveActionListener();

	for (column = 0; column < 3; ++column)
		lblDiskSwapperList[column] = new gcn::Label(diskswapper_columns[column]);

	for (row = 0; row < MAX_SPARE_DRIVES; ++row)
	{
		diskSwapperListEntry[row] = new gcn::Container();
		diskSwapperListEntry[row]->setSize(category.panel->getWidth() - 2 * DISTANCE_BORDER, SMALL_BUTTON_HEIGHT + 4);
		diskSwapperListEntry[row]->setBaseColor(gui_baseCol);
		diskSwapperListEntry[row]->setBorderSize(0);

		cmdDiskSwapperListAdd[row] = new gcn::Button("...");
		cmdDiskSwapperListAdd[row]->setBaseColor(gui_baseCol);
		cmdDiskSwapperListAdd[row]->setSize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);
		snprintf(tmp, 20, "cmdDiskSwapperAdd%d", row);
		cmdDiskSwapperListAdd[row]->setId(tmp);
		cmdDiskSwapperListAdd[row]->addActionListener(diskSwapperAddActionListener);

		cmdDiskSwapperListDelete[row] = new gcn::ImageButton(prefix_with_application_directory_path("data/delete.png"));
		cmdDiskSwapperListDelete[row]->setBaseColor(gui_baseCol);
		cmdDiskSwapperListDelete[row]->setSize(SMALL_BUTTON_HEIGHT, SMALL_BUTTON_HEIGHT);
		snprintf(tmp, 20, "cmdDiskSwapperDel%d", row);
		cmdDiskSwapperListDelete[row]->setId(tmp);
		cmdDiskSwapperListDelete[row]->addActionListener(diskSwapperDelActionListener);

		for (column = 0; column < 2; ++column)
		{
			diskSwapperListCells[row][column] = new gcn::TextField();
			diskSwapperListCells[row][column]->setSize(diskswapper_column_size[column], SMALL_BUTTON_HEIGHT);
			diskSwapperListCells[row][column]->setEnabled(false);
			diskSwapperListCells[row][column]->setBackgroundColor(colTextboxBackground);
			if (column == 0)
				diskSwapperListCells[row][column]->setText(std::to_string(row + 1));
		}

		cmdDiskSwapperListDrive[row] = new gcn::Button("");
		cmdDiskSwapperListDrive[row]->setSize(SMALL_BUTTON_WIDTH * 2, SMALL_BUTTON_HEIGHT);
		cmdDiskSwapperListDrive[row]->setBaseColor(gui_baseCol);
		snprintf(tmp, 20, "cmdDiskSwapperDrv%d", row);
		cmdDiskSwapperListDrive[row]->setId(tmp);
		cmdDiskSwapperListDrive[row]->addActionListener(diskSwapperDriveActionListener);
	}

	cmdDiskSwapperRemoveAll = new gcn::Button("Remove All");
	cmdDiskSwapperRemoveAll->setBaseColor(gui_baseCol);
	cmdDiskSwapperRemoveAll->setSize(cmdDiskSwapperRemoveAll->getWidth() + 10, BUTTON_HEIGHT);
	cmdDiskSwapperRemoveAll->setId("cmdDiskSwapperRemoveAll");
	cmdDiskSwapperRemoveAll->addActionListener(diskSwapperRemoveAllActionListener);

	auto posX = DISTANCE_BORDER + 2 + SMALL_BUTTON_WIDTH + 34;
	// Labels
	for (column = 0; column < 3; ++column)
	{
		category.panel->add(lblDiskSwapperList[column], posX, posY);
		posX += diskswapper_column_size[column];
	}
	posY += lblDiskSwapperList[0]->getHeight() + 2;

	for (row = 0; row < MAX_SPARE_DRIVES; ++row)
	{
		posX = 1;
		diskSwapperListEntry[row]->add(cmdDiskSwapperListAdd[row], posX, 2);
		posX += cmdDiskSwapperListAdd[row]->getWidth() + 4;
		diskSwapperListEntry[row]->add(cmdDiskSwapperListDelete[row], posX, 2);
		posX += cmdDiskSwapperListDelete[row]->getWidth() + 8;
		for (column = 0; column < 2; ++column)
		{
			diskSwapperListEntry[row]->add(diskSwapperListCells[row][column], posX, 2);
			posX += diskswapper_column_size[column];
		}
		diskSwapperListEntry[row]->add(cmdDiskSwapperListDrive[row], posX, 2);
		category.panel->add(diskSwapperListEntry[row], DISTANCE_BORDER, posY);
		posY += diskSwapperListEntry[row]->getHeight() + 5;
	}

	posY += DISTANCE_NEXT_Y / 2;
	category.panel->add(cmdDiskSwapperRemoveAll, DISTANCE_BORDER, posY);

	RefreshPanelDiskSwapper();
}

void ExitPanelDiskSwapper()
{
	for (auto& column : lblDiskSwapperList)
		delete column;

	for (auto row = 0; row < MAX_SPARE_DRIVES; ++row)
	{
		delete cmdDiskSwapperListAdd[row];
		delete cmdDiskSwapperListDelete[row];
		for (auto column = 0; column < 2; ++column)
			delete diskSwapperListCells[row][column];
		delete cmdDiskSwapperListDrive[row];
		delete diskSwapperListEntry[row];
	}

	delete cmdDiskSwapperRemoveAll;

	delete diskSwapperAddActionListener;
	delete diskSwapperDelActionListener;
	delete diskSwapperRemoveAllActionListener;
	delete diskSwapperDriveActionListener;
}

void RefreshPanelDiskSwapper()
{
	TCHAR tmp[10]{};
	TCHAR tmp2[MAX_DPATH]{};

	for (auto row = 0; row < MAX_SPARE_DRIVES; row++)
	{
		if (changed_prefs.dfxlist[row][0] != 0)
		{
			_tcscpy(tmp2, changed_prefs.dfxlist[row]);
			auto j = _tcslen(tmp2) - 1;
			if (j < 0)
				j = 0;
			while (j > 0) {
				if ((tmp2[j - 1] == '\\' || tmp2[j - 1] == '/')) {
					if (!(j >= 5 && (tmp2[j - 5] == '.' || tmp2[j - 4] == '.')))
						break;
				}
				j--;
			}
			diskSwapperListCells[row][1]->setText(tmp2 + j);
			const int drv = disk_in_drive(row);
			tmp[0] = 0;
			if (drv >= 0)
				_stprintf(tmp, _T("DF%d:"), drv);
			cmdDiskSwapperListDrive[row]->setCaption(tmp);
		}
		else
		{
			diskSwapperListCells[row][1]->setText("");
			cmdDiskSwapperListDrive[row]->setCaption("");
		}
	}

}

bool HelpPanelDiskSwapper(std::vector<std::string>& helptext)
{
	helptext.clear();
	helptext.emplace_back("There are 10 slots available in the Disk Swapper. You can insert a disk image on each slot,");
	helptext.emplace_back("and then use the built-in hotkey combinations to swap them on the fly:");
	helptext.emplace_back(" ");
	helptext.emplace_back("Hotkey for slots 1-10: End & 1 to 0 keys");
	//helptext.emplace_back("Hotkey for slots 11-20: End & Shift & 1 to 0 keys");
	return true;
}