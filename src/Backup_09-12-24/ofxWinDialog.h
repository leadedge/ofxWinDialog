//
// ofxWinDialog.h
//
#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <io.h>
#include <commctrl.h>
#include <Shlwapi.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.Lib")

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

    // Checkbox, radio and push buttons have a title to identify the control
    // and text to display on the control, and can have different names. 
    // If the text is empty, the title is used for display.
    // Style can be BS_RIGHTBUTTON - default is a button on the left of the text.
    void AddCheckBox(std::string title, std::string text, int x, int y, int width, int height, bool bChecked, DWORD dwStyle = 0);
    // Start radio button group
    void AddRadioGroup();
    // Radio button
    void AddRadioButton(std::string title, std::string text, int x, int y, int width, int height, bool bChecked, DWORD dwStyle = 0);
    // Push button
    // Style can be BS_DEFPUSHBUTTON. Default is BS_PUSHBUTTON.
    void AddButton(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);
    // Slider
    void AddSlider(std::string title, int x, int y, int width, int height, float min, float max, float value, float tickinterval = 0.0);
    // Edit control text entry
    // Style can be ES_LEFT, ES_RIGHT or ES_CENTER. Default is ES_LEFT.
    void AddEdit(std::string title, int x, int y, int width, int height, std::string text, DWORD dwStyle = 0);
    // Combo box list selection
    void AddCombo(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index);
    // Static text
    // Style can be :
    //  SS_LEFT   - left aligned
    //	SS_CENTER - centered
    //	SS_RIGHT  - right aligned
    //	WS_BORDER - outlined
    //	SS_SUNKEN - sunken edge
    // Default is left aligned (SS_LEFT)
    void AddText(std::string title, int x, int y, int width, int height, DWORD dwStyle = 0);
    // Static group box
    void AddGroup(std::string title, int x, int y, int width, int height);

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

    // Get all current control values
    void GetControls();

    //
    // Set controls
    //

    // Set a section name for the control in an initialization file
    // Default is the control Type (Edit, Checkbox, Button etc)
    void SetSection(std::string title, std::string section);

    void SetCheckBox(std::string title, int value);
    void SetRadioButton(std::string title, int value);

    void SetSlider(std::string title, float value);
    void SetEdit(std::string title, std::string text);
    void SetCombo(std::string title, int item);

    // Restore controls with old values
    void Restore();

    // Refresh the dialog with current controls
    void Refresh();

    // Save controls initialization file
    // Optional overwrite
    void Save(std::string filename, bool bOverWrite = false);

    // Load controls from an initialization file
    // The section name can be used to retrieve specific controls
    void Load(std::string filename, std::string section="");

    // Icon for the dialog window
    void SetIcon(HICON hIcon);

    // Set a custom font
    // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
    // name   - e.g. "Tahoma", "Ms Shell Dlg" etc.
    // height - height in logical units
    // weight - FW_NORMAL, FW_BOLD etc. (default FW_NORMAL)
    void SetFont(std::string name, LONG height, LONG weight = FW_NORMAL);

    // Create dialog window with controls
    //  o If x and y are both positive, that position is used
    //	o If x is zero, centre on the host window
    //  o If y is zero, offset from the left of the host window
    //    by the x amount, left negative or right positive
    //	o If x and y are both are zero, centre on the desktop
    HWND Open(const char* windowName, int x, int y, int width, int height);

    // Close dialog window and retain controls
    void Close();

    // Utility
    std::string float2string(float number, int places);


};
