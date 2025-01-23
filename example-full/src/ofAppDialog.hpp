/*

	ofAppDialog.cpp

	ofApp Dialog functions for the ofxWinDialog full example
	In a separate file to avoid complication of ofApp code

	This file shows detail of how to Create dialogs and to 
	create and use callback functions to respond to ofxWinDialog

*/

#pragma once

#include "ofApp.h"

//
// ================= DIALOG 1 ===================
//
//
// Add controls and design the main dialog
//
void ofApp::CreateAppDialog()
{
	//
	// If controls are added one below the other,
	// setting the y position makes it easier to
	// adjust their relative vertical positions.
	//
	int ypos = 20;

	//
	// Static text
	//
	// Default style left aligned (SS_LEFT)
	// Additional styles can be specified
	//	  SS_CENTER - centered
	//	  SS_RIGHT  - right aligned
	//	  WS_BORDER - outlined
	//	  SS_SUNKEN - sunken edge
	// Static text is not a control and has not title
	// Text can be multi-line. Allow sufficient height for the number of lines.
	std::string text = "- - - - -  ofxWinDialog  - - - - -\n";
	dialog->AddText(text, 25, ypos, 345, 70, SS_CENTER | WS_BORDER);
	ypos += 30;

	//
	// Hyperlink
	//
	// This is static text modified with the SS_NOTIFY style so that
	// it is detected by the event handler.
	//
	// The first argument, the title, is displayed blue in the dialog.
	// The second argumnent, the text, indicates the action to be taken.
	// This is usually a URL but can be anything recognized
	// by Windows as a command, such as a program path.
	// In this example, the title is the same as the text to show the url.
	//
	dialog->AddHyperlink("https://spout.zeal.co/", "https://spout.zeal.co/", 25, ypos, 345, 30);

	// Here the text outline encloses the hyperlink line.
	// Increment the y position for the rest of the controls
	ypos += 90;

	//
	// Checkbox
	//
	// Checkbox, radio and push buttons have a title to identify the control
	// and text to display on the control. Title and text can have different names.
	// If the text is empty, the title is used for display.
	// The text alignment can be : BS_LEFT, BS_CENTER or BS_RIGHT. Default BS_LEFT.
	// The button can be at the right instead of the left : BS_RIGHTBUTTON
	// Add the style as an optional last argument
	dialog->AddCheckBox("Show graphics", "", 35, ypos, 160, 25, bShowGraphics);

	//
	// Spin control
	//
	// An up/down spin control increments or decrements
	// a value that is shown in a a buddy text window
	// and immediately returns it to ofApp.
	//    x, y, width, height - position and dimensions
	//    min, max - range. index - starting value
	// Style is the same as for static text but can include alignment
	// of the spin control : UDS_ALIGNLEFT or UDS_ALIGNRIGHT (default)
	// A border provides best visibility.
	dialog->AddSpin("Spin 1", 200, ypos, 45, 25, 0, 10, 0, WS_BORDER | SS_CENTER);
	ypos += 35;

	//
	// Radio buttons
	//
	// Radio buttons are organized into a group.
	// Add a group before adding the buttons in the group
	// Multiple groups can be made in this way.
	// The buttons are usually aligned vertically but in this
	// example they are positioned horizontally to save space.
	// Text and button position styles are the same as for checkbox.
	//
	// Group - Graphics shape
	dialog->AddRadioGroup();
	dialog->AddRadioButton("Circle", "",     35, ypos, 100, 25, true); // Active radio button
	dialog->AddRadioButton("Square", "",    135, ypos, 100, 25, false);
	dialog->AddRadioButton("Rectangle", "", 235, ypos, 120, 25, false);
	ypos += 35;

	//
	// Slider mode
	//
	// Add a button for the slider to inform ofApp when the position
	// has changed or after the mouse is released
	// This button informs ofApp to call an ofxWinDialog function
	// (SliderMode) to change the slider mode of operation.
	dialog->AddCheckBox("One click", "",      35, ypos, 100, 25, bOnce);
	// A help button is useful for this example.
	dialog->AddButton("One click help", "?", 150, ypos,  25, 25);
	ypos += 30;

	//
	// Slider control
	//
	// The position value is returned to ofApp *100 to allow for slider ranges 0-1
	//
	// AddSlider : Name, xpos, ypos, width, height, minimum, maximum, bShow, interval
	//
	// bShow (default true)
	//    If true adds a value text indicator to the right of the slider
	//    Set false if not required
	// tickinterval (default 0)
	//    If tick marks are required, add both the bShow and tick interval arguments.
	//        In this example the slider range is 0-255 (256) and an interval
	//        of 16 gives 16 ticks. The slider depth should be greater than
	//        25 for tick marks to be visible.
	//
	dialog->AddSlider("Slider 1", 90, ypos, 210, 30, 0, 255, red, true, 16);
	// Add a text label to the left of the slider (1 pixel down to centre on the control)
	// Labels are independent of the slider title. The label is not
	// a control and the name will not conflict with other controls
	dialog->AddText("Red",        35, ypos,  70, 25);
	// Leave a spacing > 30 to show the slider outline when selected
	ypos += 30;
	dialog->AddSlider("Slider 2", 90, ypos, 210, 30, 0, 255, green, true, 16);
	dialog->AddText("Green",      35, ypos,  70, 25);
	ypos += 30;
	dialog->AddSlider("Slider 3", 90, ypos, 210, 30, 0, 255, blue, true, 16);
	dialog->AddText("Blue",       35, ypos,  70, 25);
	ypos += 30;
	// Alpha is 0.0 - 1.0 as an example
	// Ten ticks = 0.1 interval
	// Openframeworks requires 0-255
	// convert the starting value to 0.0-1.0 for this example
	float start = (float)alpha/255.0;
	dialog->AddSlider("Slider 4", 90, ypos, 210, 30, 0.0, 1.0, start, true, 0.1);

	// Text label to the left of the control
	dialog->AddText("Alpha",   35, ypos,  70, 25);
	ypos += 40;

	//
	// Combo box
	//
	// Prepare items for the combo list box
	// Test for successful load from Windows/Fonts
	if (tahoma.isLoaded()) {
		comboItems.push_back("Tahoma");  // Item 0
		comboFonts.push_back(tahoma);
	}
	if (verdana.isLoaded()) {
		comboItems.push_back("Verdana"); // Item 1 etc
		comboFonts.push_back(verdana);
	}
	if (georgia.isLoaded()) {
		comboItems.push_back("Georgia");
		comboFonts.push_back(georgia);
	}
	if (trebuchet.isLoaded()) {
		comboItems.push_back("Trebuchet");
		comboFonts.push_back(trebuchet);
	}
	if (jokerman.isLoaded()) {
		comboItems.push_back("Jokerman");
		comboFonts.push_back(jokerman);
	}
	if (staccato.isLoaded()) {
		comboItems.push_back("Staccato");
		comboFonts.push_back(staccato);
	}
	fontNumber = 0; // starting index 0 (Tahoma)
	comboFont = comboFonts[0]; // starting font

	dialog->AddCombo("Combo 1", 90, ypos, 200, 400, comboItems, 0);
	// Label for the combo box (2 pixels down to centre on the control)
	ypos += 2;
	dialog->AddText("Font",     35, ypos,  50, 25);
	ypos += 35;

	//
	// Edit control for text entry
	//
	// Label for the text control
	dialog->AddText("Text",   35, ypos, 70, 30);
	// The edit control
	fontText = "Example text";
	dialog->AddEdit("Edit 1", 90, ypos, 200, 30, fontText);
	ypos += 2;
	// Button to submit the text entry
	dialog->AddButton("Edit 1 button", ">", 295, ypos, 30, 25);
	ypos += 70;

	//
	// Group box
	//
	// Enclose all controls in a group box
	// A group box name will not conflict with other controls
	dialog->AddGroup("Options", 25, 105, 345, ypos-120);

	//
	// Push buttons
	//
	// Reset, Save, Load
	//
	dialog->AddButton("Reset button", "Reset", 35, ypos, 100, 30);
	// Control status can be saved to and loaded from a configuration file
	// using dialog->Save and dialog->Load
	dialog->AddButton("Save button", "Save",  140, ypos, 100, 30);
	dialog->AddButton("Load button", "Load",  245, ypos, 100, 30);
	ypos += 45;

	// OK, Cancel, Help
	dialog->AddButton("OK button", "OK",          35, ypos, 100, 30, BS_DEFPUSHBUTTON);
	dialog->AddButton("Cancel button", "Cancel", 140, ypos, 100, 30);
	dialog->AddButton("Help button", "Help",     245, ypos, 100, 30);

	//
	// Option
	//
	// Initialization file sections
	//
	// The default section name for a control is the Type :
	//   "CheckBox", "Radio", "Edit", "Slider", "Combo"
	// A different name can be used to identify controls
	// with a common grouping independent of control type.
	// This takes effect when the controls are saved with
	// dialog->Save and separate sections can be specified
	// for dialog->Load
	dialog->SetSection("Show graphics", "Graphics");
	dialog->SetSection("Spin 1", "Graphics");
	dialog->SetSection("Circle", "Graphics");
	dialog->SetSection("Square", "Graphics");
	dialog->SetSection("Rectangle", "Graphics");
	dialog->SetSection("One click", "Colour");
	dialog->SetSection("Slider 1", "Colour");
	dialog->SetSection("Slider 2", "Colour");
	dialog->SetSection("Slider 3", "Colour");
	dialog->SetSection("Slider 4", "Colour");
	dialog->SetSection("Combo 1", "Font");
	dialog->SetSection("Edit 1", "Font");
	dialog->SetSection("Edit 1 button", "Font");

	//
	// Visual styles
	//
	// Visual Styles for Version 6 common controls change the
	// appearance of dialog controls compared to the Classic look.
	// Visual Styles can be disabled if the classic appearance is preferable.
	// For example, slider tick marks are more visible if Visual Styles are disabled.
	//
	// After dialog open and the window and control handles are known
	//     void DisableTheme(HWND hwndDialog, HWND hwndControl = NULL);
	//
	// Before dialog open
	//		void DisableTheme(std::string type, std::string title="");
	//
	// For example :
	// An individal control
	//     dialog->DisableTheme("Slider", "Alpha");
	//
	// All controls of one type
	// Slider, Button, Checkbox, Radio, Combo, Edit etc.
	dialog->DisableTheme("Slider");
	//
	// All controls
	//     dialog->DisableTheme();

	//
	// Position
	//
	// The dialog window can be opened after the controls
	// are added and after the main window has opened.
	//
	// The width and height depend on the design here
	//   o Width - allow for the width of the group box
	//   o Height - add about 50 at the top and 50 at the bottom
	//
	// Opening position :
	//   o If x and y are both positive, that position is used
	//	 o If x and y are both zero, centre on the main window
	//   o if y is zero, offset from the centre by the x amount
	//   o If x and y are both negative, centre on the desktop
	//   o if x is CW_USEDEFAULT the system selects x and ignores y
	// For this example, position to the left of the main window.
	// The dialog is opened/closed by right mouse click.
	//
	dialog->SetPosition(-410, 0, 410, 625);


}

//
// Dialog1 callback function
//
// This function is called by ofxWinDialog when a control is changed.
// The title is checked to test the value and update the required variable.
//
void ofApp::ofxWinDialogFunction(std::string title, std::string text, int value)
{
	if (title == "WM_DESTROY") {
		// For multiple dialogs, "value" is returned
		// as the handle to the dialog that closed
		hwndDialog = nullptr;
		return;
	}

	//
	// Dialog key events
	// Can be used to detect keys when the dialog has focus
	// in place of keyPressed/keyReleased
	// "value" contains the key code
	//
	if (title == "WM_KEYUP") {
		// For example 'C' key to close the dialog
		if (value == 67) {
			dialog->Close();
		}
		return;
	}

	if (title == "WM_KEYDOWN") {
		// Will repeat - use as required
		return;
	}

	//
	// Checkbox (show graphics overlay)
	if (title == "Show graphics") {
		bShowGraphics = (bool)(value == 1);
	}

	//
	// Spin control
	// (increments or decrements the numeral in the centre)
	if (title == "Spin 1") {
		shapeNumber = value;
	}

	//
	// Radio buttons (graphics shape)
	if (title == "Circle") {
		bCircle = (value == 1);
	}
	if (title == "Square") {
		bSquare = (value == 1);
	}
	if (title == "Rectangle") {
		bRectangle = (value == 1);
	}

	// Slider mode
	// true - Inform ofApp when trackbar mouse button is released
	// false - continuously when position changes
	if (title == "One click") {
		bOnce = (value == 1);
		dialog->SliderMode(bOnce);
	}

	// Slider mode help button
	if (title == "One click help") {
		std::string str = "Inform ofApp only when the mouse button is released.\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}

	// Slider
	// Value is returned as an integer *100 to allow for slider range 0-1
	// Adjust graphics red, green, blue and alpha values
	if (title == "Slider 1") {
		red = value/100;
	}
	if (title == "Slider 2") {
		green = value/100;
	}
	if (title == "Slider 3") {
		blue = value/100;
	}
	if (title == "Slider 4") {
		// Alpha is 0.0 - 1.0 as an example
		// and is returned in the range 0-100
		// Openframeworks requires 0-255
		alpha = (value*255)/100;
	}

	//
	// Combo font item selection
	//
	if (title == "Combo 1") {
		fontNumber = value;
		comboFont = comboFonts[value]; // the selected font
	}

	// Edit control
	// Returned when the edit button, OK or Cancel pressed
	if (title == "Edit 1") {
		fontText = text.c_str();
	}

	//
	// Push buttons
	//


	// A button to the right of the edit control
	// to get the current edit control text
	if (title == "Edit 1 button") {
		std::string text = dialog->GetEdit("Edit 1");
		if (!text.empty())
			fontText = text;
	}

	// Save current control values
	if (title == "Save button") {
		char initfile[MAX_PATH]={};
		ofFileDialogResult result = ofSystemSaveDialog(initfile, "Select Output File");
		if (result.bSuccess) {
			// ofSystemSaveDialog has already warned for overwrite.
			// Set the overwrite flag true to avoid another warning.
			dialog->Save(result.getPath().c_str(), true);
		}
	}

	// Load control values
	if (title == "Load button") {
		ofFileDialogResult result = ofSystemLoadDialog("Select an ini file", false, ofToDataPath("", true));
		if (result.bSuccess) {

			// Load all controls, regardless of section
			dialog->Load(result.getPath());

			// Specific controls can be loaded if the section names
			// have been defined for them.
			//
			//     dialog->Load(result.getPath(), "Graphics");
			//

			// Return values to ofApp
			dialog->GetControls();
		}
	}

	// Cancel
	if (title == "Cancel button") {
		// Restore all controls with old values
		dialog->Restore();
		// Return values to ofApp
		dialog->GetControls();
		dialog2->Close();
		dialog->Close();
	}

	// Reset all controls to starting values
	if (title == "Reset button") {
		// Reset controls
		dialog->Reset();
		// Return values to ofApp
		dialog->GetControls();
	}

	// OK
	if (title == "OK button") {
		// Get current values of all controls
		// and return values to ofApp
		dialog->GetControls();
		// Close the help dialog if open
		dialog2->Close();
		dialog->Close();
		// The dialog handles are set to null by WM_DESTROY
		// in the respective ofxWinDialog callback functions
	}

	// Help button
	// Open the help dialog
	if (title == "Help button") {
		// Leave the Settings dialog open
		// and open again only if closed
		if(!hwndDialog2)
			hwndDialog2 = dialog2->Open("Help");
	}

}

//
// ================= DIALOG 2 "Help" ===================
//

//
// Add controls and design the second dialog
// An independent callback function allows control
// titles to be the same as for other dialogs.
//
void ofApp::CreateAppDialog2()
{
	int ypos = 20;
	std::string text = "- - - - -  ofxWinDialog help - - - - -";
	dialog2->AddText(text, 15, ypos, 270, 30, SS_CENTER | WS_BORDER);
	ypos += 50;

	// Make the style BS_DEFPUSHBUTTON for all the buttons to add a blue outline
	dialog2->AddButton("Checkbox help", "Checkbox", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Radio help", "Radio button", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Button help", "Push button", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Slider help", "Slider", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Combo help", "Combo box", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Edit help", "Edit text", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Static help", "Static text", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Hyperlink help", "Hyperlink", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Group help", "Group box", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Save help", "Save and Load", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 30;
	dialog2->AddButton("Messagebox help", "MessageBox", 75, ypos, 160, 25, BS_DEFPUSHBUTTON);
	ypos += 45;
	dialog2->AddButton("OK button", "OK", 100, ypos, 100, 30, BS_DEFPUSHBUTTON);

	// Centre on the main window
	dialog2->SetPosition(0, 0, 320, 510);

}

//
// Dialog2 "Help" callback function
//
void ofApp::ofxWinDialogFunction2(std::string title, std::string text, int value)
{
	if (title == "WM_DESTROY") {
		hwndDialog2 = nullptr;
	}

	//
	// Help buttons use SpoutMessageBox in SpoutUtils
	//
	std::string str;
	if (title == "Checkbox help") {
		str = "Checkbox, radio and push buttons have a Title to\n";
		str += "identify the control and Text to display on the control.\n";
		str += "Title and text can have different names.\n";
		str += "If the text is empty, the title is used for display.\n\n";
		str += "    void AddCheckBox(std::string title, std::string text,\n";
		str += "        int x, int y, int width, int height, bool bChecked,\n";
		str += "        DWORD dwStyle = 0);\n\n";
		str += "    x, y, width, height - position and dimensions\n";
		str += "    \"bChecked\" is the initial state, default true (checked).\n";
		str += "    Text style can be BS_LEFT (default), BS_CENTER or BS_RIGHT.\n";
		str += "    Control style can be BS_RIGHTBUTTON for a button at the right.\n";
		str += "    Add the style as an optional last argument\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	if (title == "Radio help") {
		str = "Radio buttons are arranged in a group\n";
		str += "The \"AddRadioGroup\" function starts a new group\n\n";
		str += "    void AddRadioGroup();\n\n";
		str += "    void AddRadioButton(std::string title, std::string text,\n";
		str += "        int x, int y, int width, int height, bool bChecked,\n";
		str += "        DWORD dwStyle = 0);\n\n";
		str += "    x, y, width, height - position and dimensions\n";
		str += "    \"bChecked\" is the initial state, default true (checked).\n";
		str += "    Only the active button of a group should be initially checked.\n";
		str += "    Text style can be BS_LEFT (default), BS_CENTER or BS_RIGHT.\n";
		str += "    Control style can be BS_RIGHTBUTTON for a button at the right.\n";
		str += "    Add the style as an optional last argument\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	if (title == "Button help") {
		str = "Push buttons have an immediate action\n";
		str += "and can be used for any purpose.\n\n";
		str += "  void AddButton(std::string title, std::string text,\n";
		str += "    int x, int y, int width, int height, DWORD dwStyle = 0);\n\n";
		str += "Title and text can have different names.\n";
		str += "If the text is empty, the title is used for display.\n";
		str += "Style can be BS_DEFPUSHBUTTON for a default button.\n";
		str += "or used to enhance the outline if no default is required.\n";
		str += "Add the style as an optional last argument\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	if (title == "Slider help") {
		str = "A slider can have a range of values, min to max,\n";
		str += "tick marks and tick intervals, and an option to show\n";
		str += "the position value at the right of the trackbar.\n";
		str += "Height should be greater than 25 for tick marks to be visible.\n";
		str += "Slider position can be adjusted with the Left or Right keys\n\n";
		str += "    void AddSlider(std::string title, int x, int y, int width, int height,\n";
		str += "        float min, float max, float value, bool bShow = true,\n";
		str += "        float tickinterval = 0); \n\n";
		str += "    x, y, width, height - position and dimensions\n";
		str += "    min, max, - range\n";
		str += "    value - starting position\n";
		str += "    bShow (default true)\n";
		str += "        True adds a value indicator to the right of the slider\n";
		str += "        False if not required\n";
		str += "    tickinterval (default 0)\n";
		str += "        Add the tick interval argument only if tick marks are required\n\n";
		str += "Slider controls normally inform ofApp continuously when the position changes.\n";
		str += "In some cases it is preferable only when the mouse button is released.\n\n";
		str += "    void SliderMode(bool bOnce)\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	if (title == "Combo help") {
		str = "A combo control allows selection from a list of items.\n";
		str += "Each item is a text string and the combo box is initialized\n";
		str += "with a vector of strings. User combo selection informs ofApp\n";
		str += "of the item number selected and the string to be used.\n\n";
		str += "    void AddCombo(std::string title, int x, int y, int width, int height,\n";
		str += "    std::vector<std::string> items, int index);\n\n";
		str += "        x, y, width, height - position and dimensions\n";
		str += "        std::vector<std::string> items - vector of string items\n";
		str += "        index - initial selected index\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
	if (title == "Edit help") {
		str = "An edit control allows entry of text.\n";
		str += "The control itself does not inform ofApp of change.\n";
		str += "The current contents can be returned by a button.\n\n";
		str += "A button can be dedicated to the edit control\n";
		str += "or an OK button can return all control values.\n";
		str += "The function to do this is \"GetControls()\"\n\n";
		str += "    void AddEdit(std::string title, int x, int y, int width, int height,\n";
		str += "        std::string text, DWORD dwStyle = 0);\n\n";
		str += "    x, y, width, height - position and dimensions\n";
		str += "    std::string text - intial text\n";
		str += "    Text style can be BS_LEFT (default), BS_CENTER or BS_RIGHT.\n";
		str += "    The control outline can be enhanced with :\n";
		str += "    WS_BORDER, WS_DLGFRAME or WS_THICKFRAME.\n";
		str += "    Add the style as an optional last argument\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}

	if (title == "Static help") {
		str = "Static text can be used to add text anywhere in the dialog\n";
		str += "Static text is not a control and has no title. Text can be multi-line.\n";
		str += "Allow sufficient height for the number of lines.\n\n";
		str += "    void AddText(std::string text, int x, int y,\n";
		str += "        int width, int height, DWORD dwStyle = 0);\n\n";
		str += "    x, y, width, height - position and dimensions\n";
		str += "    Default style left aligned (SS_LEFT)\n";
		str += "    Additional styles can be :\n";
		str += "      SS_CENTER   - centered\n";
		str += "      SS_RIGHT      - right aligned\n";
		str += "      WS_BORDER - outlined\n";
		str += "      SS_SUNKEN  - sunken edge\n";
		str += "    Add the style as an optional last argument\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}

	if (title == "Hyperlink help") {
		str = "A hyperlink is a special type of static text.\n\n";
		str += "The first argument, the title, is displayed blue in the dialog.\n";
		str += "The second argument, the text, is the action taken.\n";
		str += "ofApp is notified when the hyperlink is clicked.\n\n";
		str += "A hyperlink is typically a URL with a title the same as the\n";
		str += "text to show the url. But the action text can be anything\n";
		str += "recognized by Windows as a command.\n\n";
		str += "    void AddHyperlink(std::string title, std::string text,\n";
		str += "       int x, int y, int width, int height);\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}

	if (title == "Group help") {
		str = "A Group Box is an outline surrounding a group of controls.\n";
		str += "It is not a control and has no title. The text describes the group.\n\n";
		str += "    void AddGroup(std::string text, int x, int y, int width, int height);\n\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}

	if (title == "Save help") {
		str = "Control values can be saved in an initialization file.\n\n";
		str += "    void Save(std::string filename, bool bOverWrite = false);\n\n";
		str += "A section name can be specified for each control. This allows\n";
		str += "controls of a common group to be identifed in the file and\n";
		str += "reloaded using that section name. The default name is the\n";
		str += "control Type (Edit, Checkbox, Button etc.).\n\n";
		str += "    void SetSection(std::string title, std::string section);\n\n";
		str += "A previously saved file can be loaded to restore the values.\n";
		str += "and the section to load can be specified\n\n";
		str += "    void Load(std::string filename, std::string section=\"\");\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}

	if (title == "Messagebox help") {
		// Open the third dialog - SpoutMessageBox functions
		// The help dialog can remain open but is better closed
		dialog2->Close();
		dialog3->Open("SpoutMessageBox");

	}

	if (title == "OK button") {
		dialog2->Close();
	}
}


//
// ================= DIALOG 3 "SpoutMessageBox" ===================
//

//
// Add controls and design the third dialog
//
void ofApp::CreateAppDialog3()
{
	int xpos = 50;
	int ypos = 25;

	std::string text = "- - - - -  SpoutMessageBox  - - - - -\n";
	text += "An enhanced Windows MessageBox\n";
	dialog3->AddText(text, 25, ypos, 365, 50, SS_CENTER | WS_BORDER);
	ypos += 100;

	//
	// Push buttons for messagebox options
	//
	dialog3->AddButton("About", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Position", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Simple", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Variable", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Timeout", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Options", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Instruction", "", xpos, ypos, 120, 25);

	// Second group to the right
	xpos = 200;
	ypos = 125;
	dialog3->AddButton("Icon", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Buttons", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Edit", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Combobox", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Modeless", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Hyperlink", "", xpos, ypos, 120, 25);
	ypos += 30;
	dialog3->AddButton("Clipboard", "", xpos, ypos, 120, 25);
	ypos += 43;

	// OK
	dialog3->AddButton("OK button", "OK", 150, ypos, 100, 30, BS_DEFPUSHBUTTON);

	// Centre on the desktop
	dialog3->SetPosition(-1, -1, 430, 440);

}

//
// Dialog3 callback function
//
void ofApp::ofxWinDialogFunction3(std::string title, std::string text, int value)
{
	if (title == "WM_DESTROY") {
		hwndDialog3 = nullptr;
	}

	//
	// Help buttons use SpoutMessageBox in SpoutUtils
	// All message dialogs are topmost so they do not get lost
	//
	std::string str;

	//
	// About
	//
	// Practical example of an About box.
	// Typically created using resources for Windows programs
	//
	if (title == "About") {
		str = "SpoutMessageBox\n\n";
		str += "An enhanced ";
		str += "<a href=\"https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox\">MessageBox</a> ";
		str += "using ";
		str += "<a href=\"https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect\">TaskDialogIndirect</a>\n";
		str += "with additional options for position, modeless, timeout,\n";
		str += "hyperlinks, instruction, user icon, user buttons, text entry\n";
		str += "and combobox list selection controls.\n\n";
		str += "This dialog provides information on the options\n";
		str += "and the ofApp source provides example code.\n";
		str += "Refer to \"SpoutUtils\" for more utilities\n";
		str += "including Console, Logging, Registry and Timing.\n\n";
		std::string iconfile = GetExePath() + "data\\Spout.ico";
		SpoutMessageBoxIcon(iconfile);
		SpoutMessageBox(NULL, str.c_str(), "About", MB_TOPMOST | MB_OK);
	}

	//
	// Window position
	//
	// SpoutMessageBox opens centred on the desktop by default.
	// Some functions have a window handle argument which can be
	// used to centre on the application window.
	// SpoutMessageBoxWindow can be used to provide a window handle
	// for those functions that do not include one. This setting also
	// applies for all subsequent SpoutMessageBox functions.
	//     spoutMessageBoxWindow(HWND hwnd)
	// A null handle returns to default centre on the desktop
	//
	if (title == "Position") {
		str = "SpoutMessageBox opens centred on the desktop by default.\n";
		str += "Some functions include a window handle argument which\n";
		str += "can be used to centre on the application window.\n\n";
		str += "SpoutMessageBoxWindow provides a window handle\n";
		str += "for those functions that do not include one, and applies\n";
		str += "for all subsequent SpoutMessageBox functions.\n";
		str += "A null handle returns to default centre on the desktop.\n\n";
		str += "    spoutMessageBoxWindow(HWND hwnd)\n\n";
		str += "Click \"Window\" to center messages on the app window\n";
		str += "Click \"Desktop\" to center on the desktop\n";
		str += "Click \"OK\" for no change\n\n";
		str += "If the application window is centred on the desktop now,\n";
		str += "move it to one side so the effect can be more easily seen.\n";
		SpoutMessageBoxButton(1000, L"Window");
		SpoutMessageBoxButton(2000, L"Desktop");
		int iret = SpoutMessageBox(NULL, str.c_str(), "Position", MB_TOPMOST | MB_OK);
		if (iret == 1000) { // Window centre
			SpoutMessageBoxWindow(g_hWnd);
			SpoutMessageBox(NULL, "Messages will be centred on the application window", " ", MB_TOPMOST);
		}
		else if (iret == 2000) { // Desktop centre
			SpoutMessageBoxWindow(NULL);
			SpoutMessageBox(NULL, "Messages will be centred on the desktop", " ", MB_TOPMOST);
		}
	}

	//
	// Simple messagebox with message and optional timeout
	// The dialog closes itself if a timeout is specified.
	//
	if (title == "Simple") {
		str = "Simple messagebox with message and optional millisecond timeout\n";
		str += "The dialog closes itself if a timeout is specified\n\n";
		str += "  SpoutMessageBox(const char * message, DWORD dwTimeout = 0)\n\n";
		str += " For example  :  SpoutMessageBox(\"Simple messagebox\")\n\n";
		str += "Click OK to show this example\n";
		if (SpoutMessageBox(NULL, str.c_str(), "Simple messagebox", MB_TOPMOST | MB_OKCANCEL) == IDOK)
			SpoutMessageBox("Simple messagebox");
	}

	//
	// MessageBox with variable arguments
	//
	if (title == "Variable") {
		str = "Variable arguments provide a replacement for \"printf\"\n";
		str += "which avoids having to open a console.\n";
		str += "Useful for debugging and tracing errors\n";
		str += "Icon and buttons can also be specified\n\n";
		str += "  int SpoutMessageBox(const char * caption, const char* format, ...);\n";
		str += "  int SpoutMessageBox(const char * caption,  UINT uType, const char* format, ...);\n\n";
		str += "For example :\n";
		str += "    float result = 123.0;\n";
		str += "    int error = 2;\n";
		str += "    char *errorstring = \"wrong\";\n";
		str += "    SpoutMessageBox(\"Error\", \"result %4.3f\\nerror = %d (%s)\", result, error, errorstring);\n\n";
		str += "Click OK to show this example\n";
		if (SpoutMessageBox(NULL, str.c_str(), "Variable arguments", MB_TOPMOST | MB_OKCANCEL) == IDOK) {
			float result = 123.0;
			int error = 2;
			char* errorstring = "wrong";
			SpoutMessageBox("Something went wrong", "result = %4.3f\nerror = %d (%s)", result, error, errorstring);
		}
	}

	//
	// Timeout
	// Messagebox with message, caption, and milliseconds timeout
	//
	if (title == "Timeout") {
		str = "A timeout can be added to any of the spoutMessageBox functions\n";
		str += "except for a MessageBox with variable arguments\n\n";
		str += "Timeout is in millseconds and disabled for any MessageBox requiring user input.\n";
		str += "For this example, wait 10 seconds and the message will close.\n";
		SpoutMessageBox(NULL, str.c_str(), "Timeout after 10 seconds", MB_TOPMOST | MB_OK, "WAIT FOR 10 SECONDS", 10000);
	}

	//
	// MessageBox options
	//
	if (title == "Options") {
		str = "Optional arguments are the same as for Windows ";
		str += "<a href=\"https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox\">MessageBox</a>\n";
		str += "but limited by translation to ";
		str += "<a href=\"https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect/\">TaskDialogIndirect</a>\n\n";
		str += "\"Type\" is buttons and icon combined such as, in this case\n";
		str += "   \"MB_TOPMOST | MB_ICONWARNING | MB_YESNO\"\n\n";
		str += "   o Buttons\n";
		str += "       MB_OK\n";
		str += "       MB_OKCANCEL\n";
		str += "       MB_YESNOCANCEL\n";
		str += "       MB_YESNO\n\n";
		str += "   o Topmost\n";
		str += "       MB_TOPMOST\n\n";
		str += "   o Icon\n";
		str += "       MB_ICONERROR\n";
		str += "       MB_ICONWARNING\n";
		str += "       MB_ICONINFORMATION\n\n";
		str += "   o Return (button pressed)\n";
		str += "       IDOK\n";
		str += "       IDCANCEL\n";
		str += "       IDYES\n";
		str += "       IDNO\n\n";
		str += "In this example return is either IDYES, IDNO or IDCANCEL\n";
		str += "Click Yes or No to see the result.\n\n";
		int iRet = SpoutMessageBox(NULL, str.c_str(), "Caption, message icon and type", MB_TOPMOST | MB_ICONWARNING | MB_YESNOCANCEL);
		if (iRet == IDYES) SpoutMessageBox("IDYES");
		if (iRet == IDNO) SpoutMessageBox("IDNO");
	}

	//
	// Instruction
	// MessageBox with message, caption, type, instruction
	//
	if (title == "Instruction") {
		str = "MessageBox with message, caption, type and instruction\n\n";
		str += "The main instruction is a special heading in large blue font\n";
		str += "above the message and is useful to draw attention to the content\n";
		str += "Use the long form of the MessageBox and include an instruction\n\n";
		str += "spoutMessageBox(NULL, \"Message\", \"Caption\", MB_OK, \"Instruction\")\n\n";
		SpoutMessageBox(NULL, str.c_str(), "Main instruction", MB_TOPMOST | MB_OK, "Instruction");
	}

	//
	// Custom icon
	// A full path to an icon file (.ico) is required
	//
	if (title == "Icon") {
		std::string iconfile = GetExePath() + "data\\Spout.ico";
		SpoutMessageBoxIcon(iconfile);
		str = "A custom icon can be loaded from an icon \".ico\" image file:\n";
		str += "A full path to the icon file is required.\n\n";
		str += "    std::string iconfile = GetExePath() + \"data\\Spout.ico\"\n";
		str += "    SpoutMessageBoxIcon(iconfile)\n\n";
		str += "An icon can also be loaded using an icon handle.\n\n";
		str += "    void SpoutMessageBoxIcon(HICON hIcon);\n\n";
		SpoutMessageBox(NULL, str.c_str(), "Custom Icon", MB_TOPMOST | MB_OK);
	}

	//
	// Custom buttons
	// The button ID is returned by the messagebox
	if (title == "Buttons") {
		str = "As well as OK / CANCEL etc, multiple user buttons can be added.\n";
		str += "Each button has a unique ID and button text. The text is a wide string.\n";
		str += "The ID is returned and can be used to choose the required action.\n\n";
		str += "      SpoutMessageBoxButton(1000, L\"Button 1\");\n";
		str += "      SpoutMessageBoxButton(2000, L\"Button 2\");\n";
		str += "      SpoutMessageBoxButton(3000, L\"Button 3\");\n";
		str += "      int iret = SpoutMessageBox(\"User button message\", \"Choose a button\");\n";
		str += "      if(iret == 1000) SpoutMessageBox(\"Button 1 pressed\");\n";
		str += "      if(iret == 2000) SpoutMessageBox(\"Button 2 pressed\");\n";
		str += "      if(iret == 3000) SpoutMessageBox(\"Button 3 pressed\");\n\n";
		str += "Choose a button\n";
		SpoutMessageBoxButton(1000, L"Button 1");
		SpoutMessageBoxButton(2000, L"Button 2");
		SpoutMessageBoxButton(3000, L"Button 3");
		int iret = SpoutMessageBox(NULL, str.c_str(), "User button message", MB_TOPMOST | MB_OK);
		if (iret == 1000) SpoutMessageBox("Button 1 pressed");
		if (iret == 2000) SpoutMessageBox("Button 2 pressed");
		if (iret == 3000) SpoutMessageBox("Button 3 pressed");
	}

	//
	// Edit control for text entry
	//
	if (title == "Edit") {
		str = "An edit control within the MessageBox can be used to return a string.\n";
		str += "If the string argument is not empty, it is shown in the edit control.\n\n";
		str += "   o Caption only\n";
		str += "      The edit control is in the main content area\n";
		str += "         std::string editstring;\n";
		str += "         SpoutMessageBox(NULL, NULL, \"Text entry with caption\", MB_OK, editstring)\n\n";
		str += "   o Caption with message\n";
		str += "      The edit control is in the footer area\n";
		str += "      This example shows an exiting text entry.\n";
		str += "      The same applies for the caption only option.\n";
		str += "         std::string editstring = \"Existing text\";\n";
		str += "         SpoutMessageBox(NULL, \"Enter new text\", \"Caption\", MB_OK, editstring)\n\n";
		str += "   o Icon\n";
		str += "      Custom icons can be used with both caption and message with caption\n";
		str += "         std::string icofile = GetExePath()+\"data/Spout.ico\", true);\n";
		str += "         SpoutMessageBoxIcon(icofile);\n\n";
		str += "Select an option and repeat for more options\n";
		SpoutMessageBoxButton(1000, L"Caption");
		SpoutMessageBoxButton(2000, L"Message");
		SpoutMessageBoxButton(3000, L"Icon");
		int iret = SpoutMessageBox(NULL, str.c_str(), "Edit control", MB_TOPMOST | MB_OK);
		std::string editstring;
		if (iret == 1000) {
			//                     hwnd  message  caption                    buttons                edit string
			iret = SpoutMessageBox(NULL, NULL, "Text entry with caption", MB_TOPMOST | MB_OKCANCEL, editstring);
			if (iret != 0 && !editstring.empty())
				SpoutMessageBox(NULL, editstring.c_str(), "Text entered", MB_TOPMOST | MB_OK);
		}
		if (iret == 2000) {
			editstring = "existing entry";
			if (SpoutMessageBox(NULL, "This example also shows an existing entry.\nEnter new text in the edit control in the footer area.\n", "Message text entry", MB_TOPMOST | MB_OKCANCEL, editstring) == IDOK) {
				if (!editstring.empty())
					SpoutMessageBox(NULL, editstring.c_str(), "Text entered", MB_TOPMOST | MB_OK);
			}
		}
		if (iret == 3000) {
			std::string icofile = GetExePath()+"data/Spout.ico";
			SpoutMessageBoxIcon(icofile);
			if (SpoutMessageBox(NULL, "Example of using a custom icon with text entry\n",
				"Text entry with icon", MB_OKCANCEL, editstring) == IDOK) {
				if (!editstring.empty())
					SpoutMessageBox(NULL, editstring.c_str(), "Text entered", MB_TOPMOST | MB_OK);
			}
		}
	}

	//
	// MessageBox with a combo box control for item selection,
	// the selected item index is returned
	//
	if (title == "Combobox") {
		str = "A combo box control within the MessageBox can be used\nto select an item from a list and return the selection index.\n\n";
		str += "A vector of strings is passed to fill the item list. The index value\n";
		str += "passed in is used to show an initial item in the combobox and is\n";
		str += "is returned as the selected item index\n\n";
		str += "    std::vector<std::string>items;\n";
		str += "    items.push_back(\"Item 1\");\n";
		str += "    items.push_back(\"Item 2\");\n";
		str += "    items.push_back(\"Item 3\");\n";
		str += "    items.push_back(\"Item 4\");\n";
		str += "    int index = 0;\n";
		str += "    if (SpoutMessageBox(NULL, NULL, \"Select item\", MB_OKCANCEL, items, index) == IDOK) {\n";
		str += "        SpoutMessageBox(\"Item selection\", \"%d (%s)\", index, items[index].c_str());\n";
		str += "    }\n\n";
		str += "Caption, Message or Icon options are the same as the edit control\n";
		str += "Select an option and repeat for more\n";
		SpoutMessageBoxButton(1000, L"Caption");
		SpoutMessageBoxButton(2000, L"Message");
		SpoutMessageBoxButton(3000, L"Icon");
		int buttonid = SpoutMessageBox(NULL, str.c_str(), "Combo box control", MB_TOPMOST | MB_OK);
		std::vector<std::string>items;
		items.push_back("Item 0");
		items.push_back("Item 1");
		items.push_back("Item 2");
		items.push_back("Item 3");
		int index = 1; // starting index (0-3)
		if (buttonid == 1000) {
			// Caption only
			if (SpoutMessageBox(NULL, NULL, "Select item", MB_TOPMOST | MB_OKCANCEL, items, index) == IDOK) {
				SpoutMessageBox("Item selection", "%d (%s)", index, items[index].c_str());
			}
		}
		if (buttonid == 2000) {
			index = 2; // starting index (0-3)
			// Caption with message
			if (SpoutMessageBox(NULL, "This is a list of items\nSelect an item\nThe item index will be returned", "Select item", MB_TOPMOST | MB_OKCANCEL, items, index) == IDOK) {
				SpoutMessageBox("Item selection", "%d (%s)", index, items[index].c_str());
			}
		}
		if (buttonid == 3000) {
			index = 3; // starting index (0-3)
			// Caption with message and icon
			std::string icofile = GetExePath()+"data/Spout.ico";
			SpoutMessageBoxIcon(icofile);
			if (SpoutMessageBox(NULL, "Combo box with user icon\nThis is a list of items\nSelect an item\nThe item index will be returned", "Select item", MB_TOPMOST | MB_OKCANCEL, items, index) == IDOK) {
				SpoutMessageBox("Item selection", "%d (%s)", index, items[index].c_str());
			}
		}
	}

	//
	// Modeless mode
	//
	// This transfers the message to another program
	// "SpoutPanel.exe" so the dialog does not stop the application's
	// operation which continues after the messagebox is displayed and the
	// messagebox remains until closed even if the application is closed first.
	// Spout must have been downloaded and SpoutPanel or SpoutSettings run at least once.
	// Modeless is disabled for any dialog requiring user input.
	if (title == "Modeless") {
		str = "A MessageBox activated directly form an application is normally modal\n";
		str += "and stops the application until closed.\n";
		str += "\"Modeless\" mode transfers the message to another program\n";
		str += "\"SpoutPanel.exe\" so the dialog does not stop the application's operation.\n";
		str += "The MessageBox remains open even if the applicaion is closed first.\n";
		str += "\"Modeless\" is disabled for any dialog requiring user input.\n\n";
		str += "       SpoutMessageBoxModeless(true);\n";
		str += "       SpoutMessageBoxModeless(false);\n\n";
		str += "For this to work, you must have downloaded a <a href=\"https://github.com/leadedge/Spout2/releases/\">Spout release</a>\n";
		str += "and run SpoutPanel or SpoutSettings at least once.\n\n";
		str += "In this example, the messagebox is activated from a modeless dialog\n";
		str += "so the effect cannot be demonstrated.\n\n";
		SpoutMessageBox(NULL, str.c_str(), "Modeless message", MB_TOPMOST | MB_OK);
	}

	//
	// Hyperlink
	//
	// Hyperlinks can be included in the content using HTML format.
	// For example : <a href=\"https://spout.zeal.co/\">Spout home page</a>
	// Only double quotes are supported and must be escaped.
	//
	if (title == "Hyperlink") {
		str = "Hyperlinks can be included in the content using HTML format.\n";
		str += "Only double quotes are supported and must be escaped.\n";
		str += "The MessageBox closes if the link is selected.\n\n";
		str += "For example : \n\n";
		// This section shows up in the MessageBox as code
		str += "<a href=\\\"https://spout.zeal.co/\\\">Spout home page</a>\n\n";
		// This is the required format
		str += "This appears as  <a href=\"https://spout.zeal.co/\">Spout home page</a>.\n";
		str += "Refer to the ofApp source code for details.\n";
		SpoutMessageBox(NULL, str.c_str(), "Hyperlink", MB_TOPMOST | MB_OK);
	}

	// Clipboard - copy text to the clipboard
	if (title == "Clipboard") {
		str = "Text can be copied to the clipboard by using \n\n";
		str += "    bool CopyToClipBoard(HWND hwnd, const char* text);\n\n";
		str += "\"hwnd\" is the window handle used by ";
		str += "<a href=\"https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-openclipboard\">OpenClipboard</a>\n";
		str += "to indicate the window to be associated with the open clipboard.\n";
		str += "It can be NULL and the open clipboard is associated with the current task.\n\n";
		str += "OK to copy the source of this message to the clipboard, CANCEL to skip.\n";
		if (SpoutMessageBox(NULL, str.c_str(), "Clipboard", MB_TOPMOST | MB_OKCANCEL) == IDOK) {
			CopyToClipBoard(NULL, str.c_str());
			SpoutMessageBox("Text copied to the clipboard\nCheck by using a text editor and \"Ctrl-V\" or \"Edit > Paste\"");
		}
	}

	if (title == "OK button") {
		dialog3->Close();
	}
}
