/*

	ofApp.cpp

	ofxWinDialog basic example

	Basic example of using the ofxWinDialog addon to create
	Windows style dialogs for an Openframeworks application.

		o Create a new dialog object with a pointer to this class
		o Register an ofApp function that is called when a dialog control is changed.
		o Custom icon
		o Custom font
		o Static text
		o Hyperlink
		o Checkbox
		o Radio button
		o Slider with 0-1 range and tick marks
		o Slider mode ("one-click")
		o Edit (text entry)
		o Group box
		o Push buttons
		o Restore controls
		o Reset controls
		o Key events
		o Dialog open position and size

	Full example :

		o Multiple slider controls
		o Combobox list text selection
		o Configuration file save and load
		o Multiple dialogs (Help and SpoutMessageBox)


	Copyright (C) 2025 Lynn Jarvis.

	https://github.com/leadedge

	https://spout.zeal.co/

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

*/
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	// Set the app name on the title bar
	ofSetWindowTitle("ofxWinDialog basic example");

	// Font for general use from the windows\fonts folder
	LoadWindowsFont(myFont, "verdana.ttf", 14);

	// An image for the example window
	myImage.load("lighthouse.jpg");

	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);

	// Window handle for the dialog
	g_hWnd = ofGetWin32Window();

	// App instance for the dialog
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Create a new dialog object with a pointer to this class
	dialog = new ofxWinDialog(this, hInstance, g_hWnd);

	// Register an ofApp function that is called when a dialog control
	// is changed and the value returned and handled by ofApp.
	dialog->AppDialogFunction(&ofApp::ofxWinDialogFunction);

	// Icon for the dialog window (option)
	// Pre-defined Windows icons can be used
	//    IDI_INFORMATION, IDI_WARNING, IDI_ERROR, IDI_SHIELD
	//    HICON hIcon = LoadIcon(NULL, IDI_INFORMATION);
	// Or loaded from file
	std::string icopath = ofToDataPath("Spout.ico", true);
	HICON hIcon = reinterpret_cast<HICON>(LoadImageA(nullptr, icopath.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
	dialog->SetIcon(hIcon);

	// Custom font (option)
	// The font can be any defined in Windows\Fonts
	//    name   - "Tahoma", "Ms Shell Dlg", "Trebuchet", "Ms Shell Dlg" etc.
	//    height - height in logical units
	//    weight - FW_NORMAL, FW_MEDIUM, FW_BOLD etc. Default FW_NORMAL.
	// More details : https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
	// The default font is "Ms Shell Dlg".
	// Create a larger font for this example.
	dialog->SetFont("Tahoma", 21);

	// CreateAppDialog adds controls to the dialog
	// and sets the dialog size and opening position.
	CreateAppDialog();

	// The dialog is opened/closed by right mouse click.
	hwndDialog = dialog->Open("Settings");

	// For the graphics
	counter = 0;
	ofSetCircleResolution(50);
	ofBackground(255, 255, 255);


} // end Setup


//
// ================= DIALOG ===================
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
	// This is static text but modified with the SS_NOTIFY style so that
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
	dialog->AddRadioButton("Circle", "",     35, ypos, 100, 25, true); // Active radio buttonp
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
	// minimum, maximum - Range is floating point. For example :
	//    0.0 - 1.0, -1.0 - 1.0, 0.0 - 255.0
	// bShow (default true)
	//    If true adds a value text indicator to the right of the slider
	//    Set false if not required
	// tickinterval (default 0)
	//    If tick marks are required, add both the bShow and tick interval arguments.
	//    The slider depth should be greater than 25 for tick marks to be visible.
	//
	// Alpha is 0.0 - 1.0 as an example
	// Ten ticks = 0.1 interval
	// Openframeworks requires 0-255 
	// convert the starting value to 0.0-1.0 for this example
	float start = (float)alpha/255.0;
	dialog->AddSlider("Alpha", 90, ypos, 210, 30, 0.0, 1.0, start, true, 0.1);

	// Text label to the left of the control
	dialog->AddText("Alpha",   35, ypos,  70, 25);
	ypos += 40;
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
	// Reset, OK, Cancel
	//
	dialog->AddButton("Reset button", "Reset",    50, ypos, 95, 30);
	dialog->AddButton("OK button", "OK",         150, ypos, 95, 30, BS_DEFPUSHBUTTON);
	dialog->AddButton("Cancel button", "Cancel", 250, ypos, 95, 30);

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
	// For this example, position to the left of the main window
	// The dialog is opened/closed by right mouse click.
	//
	dialog->SetPosition(-410, 0, 410, 450);


}

//
// Dialog callback function
//
// This function is called by ofxWinDialog when a control is changed.
// The title is checked to test the value and update the required variable.
//
void ofApp::ofxWinDialogFunction(std::string title, std::string text, int value)
{
	if (title == "WM_DESTROY") {
		// For multiple dialogs "value" is returned
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

	// Slider control
	if (title == "Alpha") {
		// Alpha is 0.0 - 1.0 as an example
		// and is returned in the range 0-100
		// Openframeworks requires 0-255
		alpha = (value*255)/100;
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

	// Cancel
	if (title == "Cancel button") {
		// Reset all controls with old values
		dialog->Restore();
		// Return values to ofApp
		dialog->GetControls();
		// Close the dialog
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
		// Get current values of all controls and return to ofApp
		dialog->GetControls();
		// Close the dialog
		dialog->Close();
	}

}

//--------------------------------------------------------------
void ofApp::update() {
	// slowly increment the counter variable to change the graphics size
	// (from Openframeworks graphics example)
	counter = counter + 0.033f;
}


//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0, 0, 0, 255);

	// "Show graphics" is changed by a Checkbox in the dialog
	if (bShowGraphics) {

		// Window image
		myImage.draw(0, 0, ofGetWidth(), ofGetHeight());
		
		//
		// Draw graphics shape
		// (from Openframeworks graphics example)
		// Circle, square or rectangle
		//   Shape (bCircle and bSquare) is changed by dialog radio buttons
		//   alpha is changed by slider control.
		ofSetColor(255, 128, 0, alpha);
		ofFill();
		float radius = 50 + 10 * sin(counter);
		int xpos = ofGetWidth()/2;
		int ypos = ofGetHeight()/2;
		if (bCircle)
			ofDrawCircle(xpos, ypos, radius);
		else if (bSquare)
			ofDrawRectangle(xpos-radius, ypos-radius, radius*2, radius*2);
		else
			ofDrawRectangle(xpos-radius*2, ypos-radius, radius*4, radius*2);

		// Show the dialog combo box font selection and edit control text
		ofSetColor(0);

		// Draw the text string centered on the window
		ofRectangle r = myFont.getStringBoundingBox(fontText, 0, 0);
		xpos = ofGetWidth()/2 - (int)(r.getWidth()/2.0);
		myFont.drawString(fontText.c_str(), xpos, 30);

		// On-screen help
		ofSetColor(255);
		myFont.drawString("Right mouse click - open/close dialog", 130, ofGetHeight()-20);

	}
	else {
		ofSetColor(255);
		myImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	}


} // end Draw


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// Right button to open/close the main dialog
	// The dialogs are effectively modeless and multiple
	// dialogs can be open at the same time.
	// On close, the dialog handle is set to null by WM_DESTROY
	// in the respective ofxWinDialog callback function
	if (button == 2) {
		if (hwndDialog)
			dialog->Close();
		else
			hwndDialog = dialog->Open("Settings");
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


//--------------------------------------------------------------
// Load a truetype font from Windows/Fonts
bool ofApp::LoadWindowsFont(ofTrueTypeFont& font, std::string fontname, int size)
{
	char fontpath[MAX_PATH]{};
	HRESULT hr = SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, fontpath);
	if (SUCCEEDED(hr)) {
		std::string filepath = std::string(fontpath) + "\\" + fontname;
		if (_access(filepath.c_str(), 0) != -1) {
			return font.load(filepath, size);
		}
	}
	return false;
}
