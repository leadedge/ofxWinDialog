/*

	ofApp.cpp

	ofxWinDialog example-full

	Extended example of using the ofxWinDialog addon to create
	Windows style dialogs for an Openframeworks application.

	Three dialogs are created :
	  1) ofxWinDialog functions
	  2) Help for ofxWinDialog
	  3) SpoutMessageBox from SpoutUtils
	
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

// 
// Dialog setup and callback functions
// Moved to a separate file to avoid complication of ofApp code
// Refer to this file for details.
//
// CreateAppDialog()
// CreateAppDialog2()
// CreateAppDialog3()
// ofxWinDialogFunction
// ofxWinDialogFunction2
// ofxWinDialogFunction3
//
#include "ofAppDialog.hpp"

//--------------------------------------------------------------
void ofApp::setup() {

	// Set the app name on the title bar
	ofSetWindowTitle("ofxWinDialog example");

	// Font for general use from the windows\fonts folder
	LoadWindowsFont(myFont, "verdana.ttf", 14);

	// Load truetype fonts for the dialog combo box screen text
	LoadWindowsFont(tahoma,    "tahoma.ttf",   30);
	LoadWindowsFont(verdana,   "verdana.ttf",  30);
	LoadWindowsFont(georgia,   "georgia.ttf",  30);
	LoadWindowsFont(trebuchet, "trebuc.ttf",   30);
	LoadWindowsFont(jokerman,  "21205___.TTF", 40);
	LoadWindowsFont(staccato,  "TT0610M_.TTF", 40);

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

	// Register an ofApp function that is called when a dialog control is changed.
	// ofxWinDialogFunction shows how messages are returned and handled by ofApp.
	dialog->AppDialogFunction(&ofApp::ofxWinDialogFunction);

	// Icon for the dialog window (option)
	//
	// Pre-defined Windows icons can be used
	//    IDI_INFORMATION, IDI_WARNING, IDI_ERROR, IDI_SHIELD
	//    HICON hIcon = LoadIcon(NULL, IDI_INFORMATION);
	// Or loaded from file
	std::string icopath = ofToDataPath("Spout.ico", true);
	HICON hIcon = reinterpret_cast<HICON>(LoadImageA(nullptr, icopath.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
	dialog->SetIcon(hIcon);

	// Set the main window icon as well
	SendMessage(ofGetWin32Window(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(ofGetWin32Window(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// Custom font (option)
	// The font can be any defined in Windows\Fonts
	//    name   - "Tahoma", "Ms Shell Dlg", "Trebuchet", "Ms Shell Dlg" etc.
	//    height - height in logical units
	//    weight - FW_NORMAL, FW_MEDIUM, FW_BOLD etc. Default FW_NORMAL.
	// More details : https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
	// The default font is "Ms Shell Dlg" 8.
	// Create a larger font for this example.
	// Font height is in dialog units (72 per inch)
	// and adjusts to the screen dots per inch.
	dialog->SetFont("Trebuchet", 12);

	// CreateAppDialog adds controls to the dialog
	// and sets the dialog size and opening position.
	CreateAppDialog();

	// The dialog is opened/closed by right mouse click.
	hwndDialog = dialog->Open("Settings");

	// For the graphics
	counter = 0;
	ofSetCircleResolution(50);
	ofBackground(255, 255, 255);

	//
	// Multiple dialogs can be added.
	// The callback function and class name 
	// must be different for each dialog object.
	//

	//
	// Second dialog (Help)
	// Shows how to use ofxWinDialog
	// Opened/closed by a "Help" button in the main dialog
	//
	dialog2 = new ofxWinDialog(this, hInstance, g_hWnd, "DialogClass2");
	// A separate callback function is used for each dialog
	dialog2->AppDialogFunction(&ofApp::ofxWinDialogFunction2);
	// Windows information icon, no custom font
	dialog2->SetIcon(LoadIcon(NULL, IDI_INFORMATION));

	// The dialog background can be defined as
	// hex, red,green,blue or a COLORREF value
	//     dialog->BackGroundColor(int hexcode);
	//     dialog->BackGroundColor(int red, int grn, int blu);
	//     dialog->BackGroundColor(COLORREF col);
	// Useful references :
	//     https://www.html-color-codes.info/color-names/
	//     https://www.computerhope.com/htmcolor.htm
	//
	// Light is best for the dialog
	dialog2->BackGroundColor(0xF7E7CE); // Champagne - RGB(247, 231, 206)
	// Windows system colours can be used (see WinUser.h):
	// dialog2->BackGroundColor(GetSysColor(CTLCOLOR_BTN)); // Light blue
	//
	// Text and Button colour can also be specified
	// Darker colours can be used for text and buttons
	// (see ofAppDialog.hpp)
	//     dialog->TextColor
	//     dialog->ButtonColor

	// Add controls to dialog2 and centre on the main window
	// Click the "Help" button to open it
	CreateAppDialog2();

	//
	// Third dialog (SpoutMessageBox)
	// Opened/closed by a "MessageBox" button in the help dialog
	//
	// ofxWinDialog includes the spoututils namespace
	// so that all SpoutUtils functions are generally available.
	//
	// SpoutMessageBox is an enhanced MessageBox using TaskDialogIndirect
	// and includes many of the features of ofxWinDialog itself
	// such as text entry, combo box and multiple buttons.
	// Often this is sufficient instead of a separate dialog.
	//
	// This dialog shows how to use SpoutMessageBox and
	// "CreateAppDialog3" has source code for each example.
	//
	dialog3 = new ofxWinDialog(this, hInstance, g_hWnd, "DialogClass3");
	dialog3->AppDialogFunction(&ofApp::ofxWinDialogFunction3);
	dialog3->SetIcon(hIcon); // Use the Spout icon loaded previously
	dialog3->BackGroundColor(GetSysColor(COLOR_WINDOW)); // White
	CreateAppDialog3();

	// Details of dialog creation and callback 
	// functions can be found in ofAppDialog.hpp


} // end Setup


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
		//   red, green and blue are changed by slider controls.
		ofSetColor(red, green, blue, alpha);
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

		// Draw a number in the middle of the circle
		ofSetColor(0);
		ofRectangle r;
		std::string str;
		if (comboFont.isLoaded()) {
			str = std::to_string(shapeNumber);
			r = comboFont.getStringBoundingBox(str, 0, 0);
			xpos = ofGetWidth() / 2 - (int)(r.getWidth() / 2.0);
			ypos = ofGetHeight() / 2 + (int)(r.getHeight() / 2.0);
			comboFont.drawString(str, xpos, ypos);
		}

		//
		// Show the dialog combo box font selection and edit control text
		//

		// The selected font (fontNumber) is returned by the combo box index
		// The font name is the string of that combo item (comboItems[fontNumber])
		// The selected font (comboFont) is the item in the comboFonts vector (comboFonts[fontNumber])
		// Text (fontText) is returned by an edit control and drawn with the selected font

		// Draw the font name and dialog edit control text strings centered on the window
		str = comboItems[fontNumber];
		r = myFont.getStringBoundingBox(str, 0, 0);
		xpos = ofGetWidth()/2 - (int)(r.getWidth()/2.0);
		myFont.drawString(str.c_str(), xpos, 30);

		// Draw the font selected from the combo box
		if (comboFont.isLoaded()) {
			ofSetColor(0);
			r = comboFont.getStringBoundingBox(fontText, 0, 0);
			xpos = ofGetWidth()/2 - (int)(r.getWidth()/2.0);
			comboFont.drawString(fontText, xpos, 80);
		}

		// Draw the text string selected from the list box
		ofSetColor(255);
		str = listItems[listNumber];
		r = myFont.getStringBoundingBox(str, 0, 0);
		xpos = ofGetWidth()/2 - (int)(r.getWidth()/2.0);
		myFont.drawString(str.c_str(), xpos, ofGetHeight()-50);

		// Information text
		str = "Right mouse click - open/close dialog";
		r = myFont.getStringBoundingBox(str, 0, 0);
		xpos = ofGetWidth()/2 - (int)(r.getWidth()/2.0);
		myFont.drawString("Right mouse click - open/close dialog", xpos, ofGetHeight() - 20);

	}
	else {
		ofSetColor(255);
		myImage.draw(0, 0, ofGetWidth(), ofGetHeight());
	}

} // end Draw

// When the main dialog has focus, WM_KEYUP/WM_KEYDOWN messages
// are passed to the callback procedure "ofxWinDialogFunction"

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
	// dialogs can be open at the same time. However,
	// only open if the second "Help" dialog is not open
	// to align with the behaviour of Windows modal dialogs.
	// On close, the dialog handle is set to null by WM_DESTROY
	// in the ofxWinDialog callback function
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
