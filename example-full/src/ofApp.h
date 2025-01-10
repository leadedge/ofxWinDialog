/*

	ofApp.h

	ofxWinDialog example

	Copyright (C) 2025 Lynn Jarvis.

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

#pragma once

#include "ofMain.h"
#include "ofxWinDialog.h" // Dialog addon

class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// Used by the app
		HWND g_hWnd = NULL;
		ofTrueTypeFont myFont;
		ofImage myImage;
		float counter = 0.0f; // Graphics size

		//
		// Main Dialog (Settings)
		//
		ofxWinDialog* dialog; // Dialog object
		HWND hwndDialog = NULL; // Dialog window
		void CreateAppDialog(); // Create the dialog controls
		// Callback function for ofxWinDialog to return control values
		void ofxWinDialogFunction(std::string title, std::string text, int value);

		//
		// Second dialog (Help)
		//
		ofxWinDialog* dialog2; // Second dialog object
		HWND hwndDialog2 = NULL; // Second dialog window
		void CreateAppDialog2(); // Create second dialog controls
		void ofxWinDialogFunction2(std::string title, std::string text, int value);

		//
		// Third dialog (SpoutMessageBox)
		//
		ofxWinDialog* dialog3;
		HWND hwndDialog3 = NULL;
		void CreateAppDialog3();
		void ofxWinDialogFunction3(std::string title, std::string text, int value);

		//
		// Variables for the example
		//

		// Show graphics checkbox
		bool bShowGraphics = true;

		// Graphics shape radio buttons
		bool bCircle = true; 
		bool bSquare = false;
		bool bRectangle = false;

		// Slider signal on position change or mouse up
		bool bOnce = false;

		// Graphics colour sliders
		int red = 255;
		int green = 128;
		int blue = 0;
		int alpha = 255;

		// Items for the combo list box
		std::vector<std::string> comboItems;

		// Fonts selected by the combo box
		ofTrueTypeFont verdana;
		ofTrueTypeFont tahoma;
		ofTrueTypeFont georgia;
		ofTrueTypeFont trebuchet;
		ofTrueTypeFont jokerman;
		ofTrueTypeFont staccato;
		int fontNumber = 0; // font number selected

		// Edit text control
		std::string fontText; // Text for example font

};
