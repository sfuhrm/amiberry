#include <cstdio>
#include <cstring>

#include <guisan.hpp>
#include <SDL_ttf.h>
#include <guisan/sdl.hpp>
#include <guisan/sdl/sdltruetypefont.hpp>

#include "SelectorEntry.hpp"

#include "sysdeps.h"
#include "options.h"
#include "gui_handling.h"
#include "inputdevice.h"
#include "amiberry_input.h"

static gcn::Window* grpPort;
static gcn::RadioButton* optPort0;
static gcn::RadioButton* optPort1;
static gcn::RadioButton* optPort2;
static gcn::RadioButton* optPort3;

static gcn::Window* grpFunction;
static gcn::RadioButton* optMultiNone;
static gcn::RadioButton* optMultiSelect;

static gcn::Label* lblCustomAction[SDL_CONTROLLER_BUTTON_MAX];
static gcn::DropDown* cboCustomAction[SDL_CONTROLLER_BUTTON_MAX];

static gcn::Label* lblPortInput;
static gcn::TextField* txtPortInput;
static gcn::Label* lblRetroarch;

static gcn::CheckBox* chkAnalogRemap;

static int SelectedPort = 1;
static int SelectedFunction = 0;


class string_list_model : public gcn::ListModel
{
private:
	std::vector<std::string> values{};
public:
	string_list_model(const char* entries[], const int count)
	{
		for (auto i = 0; i < count; ++i)
			values.emplace_back(entries[i]);
	}

	int getNumberOfElements() override
	{
		return values.size();
	}

	int add_element(const char* elem) override
	{
		values.emplace_back(elem);
		return 0;
	}

	void clear_elements() override
	{
		values.clear();
	}

	int swap_first_element(const char* Elem)
	{
		values.erase(values.begin());
		values.emplace(values.begin(), Elem);
		return 0;
	}

	std::string getElementAt(int i) override
	{
		if (i < 0 || i >= static_cast<int>(values.size()))
			return "---";
		return values[i];
	}
};

static string_list_model CustomEventList(nullptr, 0);
static string_list_model CustomEventList_HotKey(nullptr, 0);
static string_list_model CustomEventList_Menu(nullptr, 0);
static string_list_model CustomEventList_Quit(nullptr, 0);
static string_list_model CustomEventList_Reset(nullptr, 0);

const string label_button_list[] = {
	"South:", "East:", "West:", "North:", "Select:", "Guide:", "Start:", "L.Stick:", "R.Stick:",
	"L.Shoulder:", "R.Shoulder:", "DPad Up:", "DPad Down:", "DPad Left:", "DPad Right:",
	"Misc1:", "Paddle1:", "Paddle2:", "Paddle3:", "Paddle4:", "Touchpad:"
};

const string label_axis_list[] = {
	"Left X:", "Left Y:", "Right X:", "Right Y:", "L.Trigger:", "R.Trigger:"
};

class GroupActionListener : public gcn::ActionListener
{
public:
	void action(const gcn::ActionEvent& actionEvent) override
	{
		if (actionEvent.getSource() == optPort0)
			SelectedPort = 0;
		else if (actionEvent.getSource() == optPort1)
			SelectedPort = 1;
		else if (actionEvent.getSource() == optPort2)
			SelectedPort = 2;
		else if (actionEvent.getSource() == optPort3)
			SelectedPort = 3;

		else if (actionEvent.getSource() == optMultiNone)
			SelectedFunction = 0;
		else if (actionEvent.getSource() == optMultiSelect)
			SelectedFunction = 1;
		else if (actionEvent.getSource() == chkAnalogRemap)
			changed_prefs.input_analog_remap = chkAnalogRemap->isSelected();

		RefreshPanelCustom();
	}
};

static GroupActionListener* grpActionListener;

class CustomActionListener : public gcn::ActionListener
{
public:
	void action(const gcn::ActionEvent& actionEvent) override
	{
		for (auto t = 0; t < SDL_CONTROLLER_BUTTON_MAX; t++)
		{
			if (actionEvent.getSource() == cboCustomAction[t])
			{
				std::array<int, SDL_CONTROLLER_BUTTON_MAX> tempmap{};

				// get map
				switch (SelectedFunction)
				{
				case 0:
					tempmap = changed_prefs.jports[SelectedPort].amiberry_custom_none;
					break;
				case 1:
					tempmap = changed_prefs.jports[SelectedPort].amiberry_custom_hotkey;
					break;
				default:
					break;
				}

				// get the selected action from the drop-down, and 
				// push it into the 'temp map'
				tempmap[t] = remap_event_list[cboCustomAction[t]->getSelected() - 1];

				// push map back into changed_prefs
				switch (SelectedFunction)
				{
				case 0:
					changed_prefs.jports[SelectedPort].amiberry_custom_none = tempmap;
					break;
				case 1:
					changed_prefs.jports[SelectedPort].amiberry_custom_hotkey = tempmap;
					break;
				default:
					break;
				}

				// and here, we will scroll through the custom-map and 
				// push it into the currprefs config file
				inputdevice_updateconfig(nullptr, &changed_prefs);
				RefreshPanelCustom();
			}
		}
	}
};

static CustomActionListener* customActionListener;

void InitPanelCustom(const config_category& category)
{
	int i;
	char tmp[255];

	if (CustomEventList.getNumberOfElements() == 0)
	{
		CustomEventList.add_element("None");

		for (int idx = 0; idx < remap_event_list_size; idx++)
		{
			const auto* const ie = inputdevice_get_eventinfo(remap_event_list[idx]);
			snprintf(tmp, 255, "%s", ie->name);
			CustomEventList.add_element(tmp);
		}
	}

	CustomEventList_HotKey = CustomEventList;
	CustomEventList_Menu = CustomEventList;
	CustomEventList_Quit = CustomEventList;
	CustomEventList_Reset = CustomEventList;

	CustomEventList_HotKey.swap_first_element("None (HotKey)");
	CustomEventList_Menu.swap_first_element("None (Menu)");
	CustomEventList_Quit.swap_first_element("None (Quit)");
	CustomEventList_Reset.swap_first_element("None (Reset)");

	customActionListener = new CustomActionListener();
	grpActionListener = new GroupActionListener();

	optPort0 = new gcn::RadioButton("0: Mouse", "radioportgroup");
	optPort0->setId("0: Mouse");
	optPort0->addActionListener(grpActionListener);
	optPort1 = new gcn::RadioButton("1: Joystick", "radioportgroup");
	optPort1->setId("1: Joystick");
	optPort1->addActionListener(grpActionListener);
	optPort2 = new gcn::RadioButton("2: Parallel 1", "radioportgroup");
	optPort2->setId("2: Parallel 1");
	optPort2->addActionListener(grpActionListener);
	optPort3 = new gcn::RadioButton("3: Parallel 2", "radioportgroup");
	optPort3->setId("3: Parallel 2");
	optPort3->addActionListener(grpActionListener);

	optMultiNone = new gcn::RadioButton("None", "radiomultigroup");
	optMultiNone->setId("None");
	optMultiNone->addActionListener(grpActionListener);
	optMultiSelect = new gcn::RadioButton("HotKey", "radiomultigroup");
	optMultiSelect->setId("HotKey");
	optMultiSelect->addActionListener(grpActionListener);

	chkAnalogRemap = new gcn::CheckBox("Remap DPad to left axis");
	chkAnalogRemap->setId("chkAnalogRemap");
	chkAnalogRemap->addActionListener(grpActionListener);
	chkAnalogRemap->setEnabled(true);

	grpPort = new gcn::Window("Joystick Port");
	grpPort->setPosition(DISTANCE_BORDER, DISTANCE_BORDER);
	grpPort->setMovable(false);
	grpPort->add(optPort0, 10, 10);
	grpPort->add(optPort1, optPort0->getX() + optPort0->getWidth() + DISTANCE_NEXT_X, optPort0->getY());
	grpPort->add(optPort2, optPort1->getX() + optPort1->getWidth() + DISTANCE_NEXT_X, optPort0->getY());
	grpPort->add(optPort3, optPort2->getX() + optPort2->getWidth() + DISTANCE_NEXT_X, optPort0->getY());
	grpPort->setSize(category.panel->getWidth() - DISTANCE_BORDER * 2, TITLEBAR_HEIGHT + optPort0->getHeight() * 2 + 5);
	grpPort->setTitleBarHeight(TITLEBAR_HEIGHT);
	grpPort->setBaseColor(gui_baseCol);

	category.panel->add(grpPort);

	grpFunction = new gcn::Window("Function Key");
	grpFunction->setPosition(DISTANCE_BORDER, grpPort->getY() + grpPort->getHeight() + DISTANCE_NEXT_Y);
	grpFunction->setMovable(false);
	grpFunction->add(optMultiNone, 10, 10);
	grpFunction->add(optMultiSelect, optMultiNone->getX() + optMultiNone->getWidth() + DISTANCE_NEXT_X, optMultiNone->getY());

	grpFunction->setSize(grpPort->getWidth(), grpPort->getHeight());
	grpFunction->setTitleBarHeight(TITLEBAR_HEIGHT);
	grpFunction->setBaseColor(gui_baseCol);

	category.panel->add(grpFunction);

	// the input-device should be listed
	lblPortInput = new gcn::Label("Input Device:");
	lblPortInput->setAlignment(gcn::Graphics::RIGHT);

	txtPortInput = new gcn::TextField();
	txtPortInput->setEnabled(false);
	txtPortInput->setBaseColor(gui_baseCol);
	txtPortInput->setBackgroundColor(colTextboxBackground);

	lblRetroarch = new gcn::Label("[-]");
	lblRetroarch->setAlignment(gcn::Graphics::LEFT);

	txtPortInput->setSize(
		grpFunction->getWidth() - (lblPortInput->getWidth() + DISTANCE_NEXT_X * 2 + lblRetroarch->getWidth()),
		TEXTFIELD_HEIGHT);

	for (i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
		lblCustomAction[i] = new gcn::Label(label_button_list[i]);
	
	for (i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
	{
		lblCustomAction[i]->setSize(lblCustomAction[14]->getWidth(), lblCustomAction[14]->getHeight());
		lblCustomAction[i]->setAlignment(gcn::Graphics::RIGHT);

		cboCustomAction[i] = new gcn::DropDown(&CustomEventList);
		cboCustomAction[i]->setSize(cboCustomAction[i]->getWidth() * 2, cboCustomAction[i]->getHeight());
		cboCustomAction[i]->setBaseColor(gui_baseCol);
		cboCustomAction[i]->setBackgroundColor(colTextboxBackground);

		snprintf(tmp, 20, "cboCustomAction%d", i);
		cboCustomAction[i]->setId(tmp);
		cboCustomAction[i]->addActionListener(customActionListener);
	}

	auto posY = grpFunction->getY() + grpFunction->getHeight() + DISTANCE_NEXT_Y;
	category.panel->add(lblPortInput, DISTANCE_BORDER, posY);
	category.panel->add(txtPortInput, lblPortInput->getX() + lblPortInput->getWidth() + DISTANCE_NEXT_X, posY);
	category.panel->add(lblRetroarch, txtPortInput->getX() + txtPortInput->getWidth() + DISTANCE_NEXT_X, posY);
	posY = txtPortInput->getY() + txtPortInput->getHeight() + DISTANCE_NEXT_Y;
	
	for (i = 0; i < SDL_CONTROLLER_BUTTON_MAX / 2; i++)
	{
		category.panel->add(lblCustomAction[i], 5, posY);
		category.panel->add(cboCustomAction[i], lblCustomAction[i]->getX() + lblCustomAction[i]->getWidth() + 4, posY);
		posY = posY + DROPDOWN_HEIGHT + 6;
	}

	posY = txtPortInput->getY() + txtPortInput->getHeight() + DISTANCE_NEXT_Y;
	auto posX = cboCustomAction[0]->getX() + cboCustomAction[0]->getWidth() + 4;
	for (i = SDL_CONTROLLER_BUTTON_MAX / 2; i < SDL_CONTROLLER_BUTTON_MAX; i++)
	{
		category.panel->add(lblCustomAction[i], posX, posY);
		category.panel->add(cboCustomAction[i], lblCustomAction[i]->getX() + lblCustomAction[i]->getWidth() + 4, posY);
		posY = posY + DROPDOWN_HEIGHT + 6;
	}

	category.panel->add(chkAnalogRemap, DISTANCE_BORDER + lblCustomAction[0]->getWidth(), posY);
	posY += chkAnalogRemap->getHeight() + DISTANCE_NEXT_Y;

	RefreshPanelCustom();
}

void ExitPanelCustom()
{
	delete optPort0;
	delete optPort1;
	delete optPort2;
	delete optPort3;
	delete grpPort;

	delete optMultiNone;
	delete optMultiSelect;

	delete grpFunction;
	delete chkAnalogRemap;

	for (auto& i : lblCustomAction)
		delete i;

	for (auto& i : cboCustomAction)
		delete i;

	delete lblPortInput;
	delete txtPortInput;
	delete lblRetroarch;

	delete grpActionListener;
	delete customActionListener;
}

void RefreshPanelCustom()
{
	optPort0->setSelected(SelectedPort == 0);
	optPort1->setSelected(SelectedPort == 1);
	optPort2->setSelected(SelectedPort == 2);
	optPort3->setSelected(SelectedPort == 3);

	optMultiNone->setSelected(SelectedFunction == 0);
	optMultiSelect->setSelected(SelectedFunction == 1);

	chkAnalogRemap->setSelected(changed_prefs.input_analog_remap);

	// Refresh the drop-down section here
	// get map
	std::array<int, SDL_CONTROLLER_BUTTON_MAX> tempmap{};
	switch (SelectedFunction)
	{
	case 0:
		tempmap = changed_prefs.jports[SelectedPort].amiberry_custom_none;
		break;
	case 1:
		tempmap = changed_prefs.jports[SelectedPort].amiberry_custom_hotkey;
		break;
	default:
		break;
	}

	// update the joystick port and disable those not available
	if (changed_prefs.jports[SelectedPort].id >= JSEM_JOYS
		&& changed_prefs.jports[SelectedPort].id < JSEM_MICE - 1)
	{
		const auto host_joy_id = changed_prefs.jports[SelectedPort].id - JSEM_JOYS;
		didata* did = &di_joystick[host_joy_id];
		
		for (auto n = 0; n < SDL_CONTROLLER_BUTTON_MAX; ++n)
		{
			const auto temp_button = did->mapping.button[n];

			// disable unmapped buttons
			cboCustomAction[n]->setEnabled(temp_button > -1);
			lblCustomAction[n]->setEnabled(temp_button > -1);

			// set hotkey/quit/reset/menu on NONE field (and disable hotkey)
			if (temp_button == did->mapping.hotkey_button
				&& temp_button != -1)
			{
				cboCustomAction[n]->setListModel(&CustomEventList_HotKey);
				cboCustomAction[n]->setEnabled(false);
				lblCustomAction[n]->setEnabled(false);
			}

			else if (temp_button == did->mapping.quit_button
				&& temp_button != -1
				&& SelectedFunction == 1
				&& changed_prefs.use_retroarch_quit)
			{
				cboCustomAction[n]->setListModel(&CustomEventList_Quit);
				cboCustomAction[n]->setEnabled(false);
				lblCustomAction[n]->setEnabled(false);
			}

			else if (temp_button == did->mapping.menu_button
				&& temp_button != -1
				&& SelectedFunction == 1
				&& changed_prefs.use_retroarch_menu)
			{
				cboCustomAction[n]->setListModel(&CustomEventList_Menu);
				cboCustomAction[n]->setEnabled(false);
				lblCustomAction[n]->setEnabled(false);
			}

			else if (temp_button == did->mapping.reset_button
				&& temp_button != -1
				&& SelectedFunction == 1
				&& changed_prefs.use_retroarch_reset)
			{
				cboCustomAction[n]->setListModel(&CustomEventList_Reset);
				cboCustomAction[n]->setEnabled(false);
				lblCustomAction[n]->setEnabled(false);
			}

			else
				cboCustomAction[n]->setListModel(&CustomEventList);

			if (tempmap[n] > 0)
			{
				// Custom mapping found
				const auto x = find_in_array(remap_event_list, remap_event_list_size, tempmap[n]);
				cboCustomAction[n]->setSelected(x + 1);
			}
			// Retroarch mapping is not the same, so we skip this to avoid incorrect actions showing in the GUI
			else if (did->mapping.is_retroarch)
			{
				cboCustomAction[n]->setSelected(0);
			}
			else
			{
				// Default mapping
				const auto evt = default_mapping[temp_button];
				const auto x = find_in_array(remap_event_list, remap_event_list_size, evt);
				cboCustomAction[n]->setSelected(x + 1);
			}
		}

		if (did->mapping.number_of_hats > 0 || changed_prefs.input_analog_remap == true)
		{
			for (int i = SDL_CONTROLLER_BUTTON_DPAD_UP; i <= SDL_CONTROLLER_BUTTON_DPAD_RIGHT; i++)
			{
				cboCustomAction[i]->setEnabled(true);
				lblCustomAction[i]->setEnabled(true);
			}
		}

		if (did->mapping.is_retroarch)
			lblRetroarch->setCaption("[R]");
		else
			lblRetroarch->setCaption("[N]");
		
		txtPortInput->setText(did->name);
	}

	else
	{
		for (auto& n : cboCustomAction)
		{
			n->setListModel(&CustomEventList);
			n->setEnabled(false);
		}
		for (auto& n : lblCustomAction)
		{
			n->setEnabled(false);
		}

		lblRetroarch->setCaption("[_]");
		const std::string text = "Not a valid Input Controller for Joystick Emulation.";
		txtPortInput->setText(text);
	}
}

bool HelpPanelCustom(std::vector<std::string>& helptext)
{
	helptext.clear();
	helptext.emplace_back("Set up Custom input actions for each Amiga port, such as Keyboard remapping,");
	helptext.emplace_back("or emulator functions.");
	helptext.emplace_back(" ");
	helptext.emplace_back("Select the port which you wish to re-map with 'Joystick Port'.");
	helptext.emplace_back("The currently selected Input Device will then be displayed under 'Input Device'.");
	helptext.emplace_back(" ");
	helptext.emplace_back("Buttons which are not available on this device (detected with RetroArch ");
	helptext.emplace_back("configuration files) are unavailable to remap.");
	helptext.emplace_back(" ");
	helptext.emplace_back("The HotKey button (used for secondary functions) is also unavailable for custom options. ");
	helptext.emplace_back("The actions performed by pressing the HotKey with other buttons can also be remapped.");
	helptext.emplace_back("Pre-defined functions such as Quit/Reset/Menu will be displayed as the 'default' option.");
	helptext.emplace_back(" ");
	helptext.emplace_back("The Function of the individual buttons are selectable via the marked drop-down boxes.");
	helptext.emplace_back(" ");
	helptext.emplace_back("You can also remap the game controllers D-Pad to act like the Left Analog stick.");
	return true;
}
