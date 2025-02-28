# ofxWinDialog

Create Microsoft Windows dialogs for an Openframeworks application.

Can be used together with [ofxWinMenu](https://github.com/leadedge/ofxWinMenu) to create a Windows menu as well as dialogs.

ofxWinDialog has been developed using Visual Studio 2022.

<img  hspace="100" src="https://github.com/user-attachments/assets/c0e04bef-928f-498c-a167-6cf4384d4a8b" width="240"/>

### Controls :
- Checkbox
- Radio button group
- Radio button
- Push button
- Button color
- Picture button
- Spin control
- Slider
- Edit control
- Combo box
- List box
- Group box
- Static text
- Text color
- Hyperlink
- Dialog background color

### Setup

1. Add to your Visual Studio project :
	<pre>
		ofxWinDialog
		src
			ofxWinDialog.cpp
			ofxWinDialog.h
		libs
			SpoutPanel.cpp
			SpoutPanel.h
	</pre>

2. In the Visual Studio project properties :

- Add "ofxWinDialog/src" and "ofxWinDialog/libs" to additional "C/C++/General/Additional Include Directories"

- Add #include "ofxWinDialog.h" to the ofApp header file

### Openframeworks examples

Using the Openframeworks project generator, create a new project, include ofxWinDialog as an addon and generate the project.

- Copy and replace the source files from example-basic or example-full "src" to the project "src" folder.
- Copy all files from the example "bin/data" to the project "bin/data" folder.
- Open the project, re-scan and and re-build.

### Using ofxWinDialog

In the ofApp.h header file declare a menu object

    ofxWinDialog* dialog;
	
In ofApp.cpp create a new dialog object. Pass the application class pointer and the instance and window handles of the application. A class name is required if multiple dialogs are created.

	HWND hwnd = ofGetWin32Window();
	HINSTANCE hInstance = GetModuleHandle(NULL);
    dialog = new ofxWinDialog(this, hinstance, hwnd, L"dialog1");
  
Create a function in ofApp that will receive messages back from ofxWinDialog.

    void ofApp::ofxWinDialogFunction(std::string title, int value);
  
The "title" string and "value" integer are returned whenever a dialog control is changed. Compare the string returned with the dialog control title and use the value to take action as required. If multiple dialogs are created, each requires a separate callback function.

Register the function for the dialog to use :

	dialog->AppDialogFunction(&ofApp::ofxWinDialogFunction);
	
Add controls to the dialog.

For example :

	dialog->AddCheckBox("Show graphics", "", 20, 100, 160, 25, bShowGraphics);

Where "Show graphics" is the string returned to "appDialogFunction" from ofxWinDialog when the user changes the checkbox and "bShowGraphics" is a flag that is changed within ofApp when that string is received. For details of other controls refer to the ofxWinDialog and example source files.

Set the dialog opening position and size. Refer to the example code for opening position options. Position 0, 0 will centre the dialog on the app window.

	dialog->SetPosition(0, 0, 400, 600);
	
Open the dialog window for user input with a unique name and save the handle to test for closure:

	HWND hwndDialog = dialog->Open("Settings");
	
Close and re-open as required, for example with Mouse or Key press :

	if (hwndDialog)
		dialog->Close();
	else
		hwndDialog = dialog->Open("Settings");

The Close() function sends a "WM_DESTROY" message to the callback function, where the dialog handle can be set to NULL.

### Windows example

The 'example-windows' folder contains a complete Visual Stuidio 2022 solution with a Win32 application that shows how to use ofxWinDialog independently of Openframeworks. The principle is the same for any windowed application. Refer to the readme file in the example folder for details.

### SpoutMessageBox

The full example includes a dialog accessed from a "MessageBox" button in the "Help" dialog. This shows the use of "SpoutMessageBox", an enhanced Windows MessageBox using [TaskDialogIndirect](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-taskdialogindirect) that includes many of the features of ofxWinDialog such as text entry, combo box and multiple buttons. Often this is sufficient instead of creating a dialog for the same purpose.

### Binaries

An executable binary is included in the 'example-full\bin' folder to illustrate the function of ofxWinDialog before building the project.

### Copyright

ofxWinDialog - Copyright (C) 2025 Lynn Jarvis [http://spout.zeal.co/](http://spout.zeal.co/)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser  General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses).
