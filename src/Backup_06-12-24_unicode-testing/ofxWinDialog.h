//
// ofxWinDialog.h
//
#pragma once

// Openframeworks
#include "ofMain.h"

#include <windows.h>
#include <string>
#include <vector>
#include <io.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

class ofApp; // Forward declaration

class ofxWinDialog {

public:

    ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd);
    ~ofxWinDialog();

    // Parent window handle
    HWND m_hwnd;
    // Parent instance handle
    HINSTANCE m_hInstance;
    // Pointer to access the ofApp class
    ofApp* pApp;
    // Dialog icon
    HICON m_hIcon = NULL;
    
    // The ofApp menu function
    void(ofApp::* pAppDialogFunction)(std::string title, std::string text, int value);

    // Function from ofApp for return of memu item selection
    void CreateDialogFunction(void(ofApp::* function)(std::string title, std::string text, int value));

    // Function for ofxWinDialog to return dialog item selection to ofApp
    void DialogFunction(std::string title, std::string text, int value);

    //
    // Add controls
    //

    // Checkbox
    void AddCheckBox(std::string title, int x, int y, int width, int height, bool bChecked);
    // Start radio button group
    void AddRadioGroup();
    // Radio button
    void AddRadioButton(std::string title, int x, int y, int width, int height, bool bChecked);
    // Push button
    void AddButton(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);
    // Slider
    void AddSlider(std::string title, int x, int y, int width, int height, float min, float max, float value, float tickinterval = 0.0);
    // Edit control text entry
    void AddEdit(std::string title, int x, int y, int width, int height, std::string text);
    // Combo box list selection
    void AddCombo(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index);
    // Static group box
    void AddGroup(std::string title, int x, int y, int width, int height);
    // Static text
    void AddText(std::string title, int x, int y, int width, int height, DWORD dwStyle = 0);

    //
    // Get controls
    //
    // Get checkbox state
    int GetCheckBox(std::string title);
    // Get rdaio button state
    int GetRadioButton(std::string title);
    // Get slider value
    float GetSlider(std::string title);
    // Get edit control text
    std::string GetEdit(std::string title);
    // Get current combo box item index
    int GetComboItem(std::string title);
    // Get combo box item text
    std::string GetComboText(std::string title, int item);

    // Set controls
    void SetCheckBox(std::string title, int value);
    void SetRadioButton(std::string title, int value);
    void SetSlider(std::string title, float value);
    void SetEdit(std::string title, std::string text);
    void SetCombo(std::string title, int item);

    // Update controls with current values
    void Update();

    // Reset controls with old values
    void Reset();

    // Refresh the dialog with current controls
    // Used by ofApp for Reset and Load
    void Refresh();

    // Save controls initialization file
    void Save(std::string filename);

    // Load controls from an initialization file
    void Load(std::string filename);


    // Icon for the dialog window
    void SetIcon(HICON hIcon);

    // Set a custom font
    // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
    // name   - "Tahoma", "Ms Shell Dlg", "Trebuchet", "Verdana" etc.
    // height - height in logical units
    // weight - FW_NORMAL, FW_BOLD etc. (default FW_NORMAL)
    void SetFont(std::string name, LONG height, LONG weight = FW_NORMAL);

    // Create dialog window with controls
    HWND Open(const char* windowName, int x, int y, int width, int height);
    void Close();

};
