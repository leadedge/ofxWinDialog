//
// example-windows
//
// This example uses ofxWinDialog independently of Openframeworks.
//
// ----------------------------------------------------------------------
//
// ofxWinDialog is designed to use a callback function within
// an Openframeworks 'ofApp' class which is called when dialog
// controls are selected. The function can have any name.
//      void DialogFunction(std::string title, std::string text, int value)
//
// The Openframeworks application would normally register this
// function directly with ofxWinDialog :
//      dialog->AppDialogFunction(&ofApp::DialogFunction);
//
// In order to use ofxWinDialog independently of Openframeworks,
// an 'ofApp' class is simulated within it :
//      ofApp app;
//
// The simulated ofApp class within ofxWinDialog has a callback function
// with the same signature as the Openframeworks callback function :
//      void ofxDialogFunction(std::string title, std::string text, int value)
//
// The independent application sets this function to point to
// its own callback function, in this example 'MainDialogFunction'.
//      app.AppDialogFunction(MainDialogFunction);
//
// The ofApp class then forwards calls from ofxWinDialog to this function.
//
// ----------------------------------------------------------------------

// Header Files
#include <windows.h>
#include "ofxWinDialog.h"
#include <iostream>

// Window global variables
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
WCHAR szWindowClass[MAX_LOADSTRING] { L"OFXWINDIALOGWIN32" };

// Win32 application
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Dialog
ofxWinDialog * dialog; // Dialog object
HWND hwndDialog = NULL; // Dialog window
void CreateAppDialog(); // Create the dialog controls
// Callback function for ofxWinDialog to return control values
void MainDialogFunction(std::string title, std::string text, int value);

// Item strings for the dialog list box
std::vector<std::string> listItems;
std::string listString;
int listNumber = 0;
bool bSelectDay = true; // Dialog checkbox
std::string g_iniPath; // initialization file path


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Console for debugging
	// OpenSpoutConsole();

	// Perform application initialization:
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	// Create an ofxWinDialog 'ofApp' object,
	// simulating an Openframeworks application.
	ofApp app;

	// Create an ofxWinDialog object and pass the simulated 'app'
	// as the Openframeworks 'ofApp' equivalent
	dialog = new ofxWinDialog(&app, g_hInst, g_hWnd);

	// Use the simulated 'ofApp' class to register a function
	// that is called when a dialog control is changed.
	app.AppDialogFunction(MainDialogFunction);

	// Font for the dialog
	dialog->SetFont("Segoe UI", 10);

	// Load an icon from Windows Shell32.dll
	HICON hIcon = dialog->LoadWindowsIcon(27);
	dialog->SetIcon(hIcon);

	// Add controls and design the main dialog
	CreateAppDialog();

	// Initialization file path
	g_iniPath = GetExePath(true);
	// Strip ".exe" and replace with ".ini"
	g_iniPath = g_iniPath.substr(0, g_iniPath.rfind(".")) + ".ini";

	// After the controls have been created,
	// load saved control values if the file exists
	dialog->Load(g_iniPath);

	// Open the dialog
	// The dialog is a separate window and effectively
	// modeless and does not block at this point
	hwndDialog = dialog->Open("ofxWinDialog");

	// Refresh ofApp with control values that
	// were saved and loaded with dialog->Load
	dialog->GetControls();
	
	// Main message loop
	// Wait for WM_DESTROY from the dialog
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Cleanup
	if (dialog != nullptr)
		delete dialog;

	 return (int)msg.wParam;

}

//
// Register the main window class
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex {};
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr; // No icon
	wcex.hCursor = nullptr; // No cursor
	wcex.hbrBackground = nullptr; // No window
	wcex.lpszMenuName = nullptr; // No menu
	wcex.lpszClassName = szWindowClass;

	return RegisterClassExW(&wcex);
}

//
// Create the main window
// This is unused and created with zero size
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	// WS_POPUP with zero size creates an invisible window
	// allowing the dialog to open independently
	// ShowWindow and UpdateWindow not used for the invisible window
	HWND hWnd = CreateWindowW(szWindowClass, nullptr,
		WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;

	// Save instance and handle for dialog
	g_hInst = hInstance;
	g_hWnd = hWnd;

	return TRUE;
}

// Create the dialog controls
// Refer to example-basic and example-full for details
// This example saves settings to an initialization file
 void CreateAppDialog() {

	int ypos = 20;
	dialog->AddHyperlink("ofxWinDialog", "https://github.com/leadedge/ofxWinDialog/", 25, ypos, 345, 30, WS_BORDER);
	ypos += 70;

	dialog->AddCheckBox("Select day", "", 35, ypos, 160, 25, bSelectDay);
	ypos += 35;

	//
	// Prepare item string vector for the combo box
	//
	listItems.push_back("Monday");
	listItems.push_back("Tuesday");
	listItems.push_back("Wednesday");
	listItems.push_back("Thursday");
	listItems.push_back("Friday");
	listItems.push_back("Saturday");
	listItems.push_back("Sunday");
	listNumber = 0; // starting index
	listString = listItems[0]; // starting string

	// Combo box text
	dialog->AddText("Weekday", 35, ypos+2, 70, 30);
	// Add combo box
	dialog->AddCombo("List 1", 120, ypos, 212, 75, listItems, 0);
	ypos += 75;

	//
	// Group box
	//
	// Enclose all controls in a group box
	// Caption colour also gives a slightly darker border
	dialog->TextColor(0xB22222); // FireBrick - RGB(178, 34, 34)
	dialog->AddGroup("Options", 25, 60, 345, ypos-80);

	//
	// Save, Load, Reset
	//
	// Control status can be saved to and loaded from a configuration file
	// using dialog->Save and dialog->Load
	dialog->AddButton("Save button", "Save",      35, ypos, 100, 30);
	dialog->AddButton("Load button", "Load",     140, ypos, 100, 30);
	dialog->AddButton("Reset button", "Reset",   245, ypos, 100, 30);
	ypos += 40;

	// OK, Cancel, Help
	dialog->AddButton("OK button", "OK",          75, ypos, 75, 30, BS_DEFPUSHBUTTON);
	dialog->AddButton("Cancel button", "Cancel", 152, ypos, 75, 30);
	dialog->AddButton("Help button", "Help",     231, ypos, 75, 30);

	// Initialization file sections
	// The default section name for a control is the Type :
	//   "CheckBox", "Radio", "Edit", "Slider", "Combo"
	// A different name can be used to identify controls
	// with a common grouping independent of control type.
	dialog->SetSection("Select day", "Options");
	dialog->SetSection("List 1", "Options");

	// This is important or the dialog will never open
	dialog->SetPosition(-1, -1, 410, 330);

}

// Callback function for ofxWinDialog to return control values
void MainDialogFunction(std::string title, std::string text, int value) {

	// This is required for all dialogs
	// for close of any other windows on exit
	if (title == "WM_DESTROY") {
		hwndDialog = nullptr;
		// Close the main Win32 window
		SendMessage(g_hWnd, WM_DESTROY, 0, 0L);
		return;
	}
	
	// Enable or disable the combo box
	if (title == "Select day") {
		bSelectDay = (value == 1);
		dialog->EnableControl("List 1", bSelectDay);
	}

	// Save controls to an initialization file
	if (title == "Save button") {
		// Save ini file with overwrite
		dialog->Save(g_iniPath, true); 
	}

	// Load controls frm the initialization file
	if (title == "Load button") {
		// Load controls from the saved ini file
		if (dialog->Load(g_iniPath)) {
			// Return new values to ofApp
			dialog->GetControls();
		}
		else {
			SpoutMessageBox(hwndDialog, "Initialization file not found\nSave before Load", "ofxWinDialog", MB_OK | MB_ICONWARNING);
		}
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
		// Save to the ini file
		dialog->Save(g_iniPath, true); 
		// Close the dialog
		dialog->Close();
	}

	if (title == "Cancel button") {
		// Reset all controls with old values
		dialog->Restore();
		// Save the old values
		dialog->Save(g_iniPath, true); 
		// Close the dialog
		dialog->Close();
	}

	
	if (title == "Help button") {
		std::string str = "This example uses ofxWinDialog independently of Openframeworks.\n\n";
		str += "ofxWinDialog is designed to use a callback function within\n";
		str += "an Openframeworks 'ofApp' class which is called when dialog\n";
		str += "controls are selected. The function can have any name.\n";
		str += "     void DialogFunction(std::string title, std::string text, int value)\n\n";
		str += "The Openframeworks application would normally register this\n";
		str += "function directly with ofxWinDialog :\n";
		str += "     dialog->AppDialogFunction(&ofApp::DialogFunction);\n\n";
		str += "In order to use ofxWinDialog independently of Openframeworks,\n";
		str += "an 'ofApp' class is simulated within it :\n";
		str += "     ofApp app;\n\n";
		str += "The simulated ofApp class within ofxWinDialog has a callback function\n";
		str += "with the same signature as the Openframeworks callback function :\n";
		str += "     void ofxDialogFunction(std::string title, std::string text, int value);\n\n";
		str += "The independent application sets this function to point to\n";
		str += "its own callback function, in this example 'MainDialogFunction'.\n";
		str += "     app.AppDialogFunction(MainDialogFunction);\n\n";
		str += "The ofApp class then forwards calls from ofxWinDialog to this function.\n";
		SpoutMessageBox(NULL, str.c_str(), "ofxWinDialog", MB_OK | MB_USERICON | MB_TOPMOST);
	}

}

//
// Process messages for the main window
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	// No command messages
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
