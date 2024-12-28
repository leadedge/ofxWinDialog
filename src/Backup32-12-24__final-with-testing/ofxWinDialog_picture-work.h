//
// ofxWinDialog.h
//
#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <io.h>
#include <commctrl.h> // For trackbars
#include <Shlwapi.h> // For path functions
#include <uxtheme.h> // To disable visual style themes
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.Lib")
#pragma comment(lib, "UxTheme.lib")

class ofApp; // Forward declaration

class ofxWinDialog {

public:

    ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd);

    ~ofxWinDialog();

    // Dialog window handle
    HWND m_hDialog = nullptr;

    // Radio button group (numbering is incremented startin at 0)
    int nRadioGroup = -1;

    // First in the group flag
    bool bRadioFirst = false;

    // Trackbar notify mode
    bool bOneClick = false;

    // The ofApp menu function
    void(ofApp::* pAppDialogFunction)(std::string title, std::string text, int value);

    // Function from ofApp for return of memu item selection
    void AppDialogFunction(void(ofApp::* function)(std::string title, std::string text, int value));

    // Function for ofxWinDialog to return dialog item selection to ofApp
    void DialogFunction(std::string title, std::string text, int value);

    //
    // Dialog controls
    //

    //
    // Checkbox
    //
    // Checkbox, radio and push buttons have a title to identify the control
    // and text to display on the control, and can have different names. 
    // If the text is empty, the title is used for display.
    // Style can be BS_RIGHTBUTTON - default is a button on the left of the text.
    void AddCheckBox(std::string title, std::string text, int x, int y, int width, int height, bool bChecked, DWORD dwStyle = 0);
    
    // Start radio button group
    void AddRadioGroup();
    
    // Radio button
    void AddRadioButton(std::string title, std::string text, int x, int y, int width, int height, bool bChecked, DWORD dwStyle = 0);
    
    //
    // Push button
    //
    // Style can be BS_DEFPUSHBUTTON. Default is BS_PUSHBUTTON.
    void AddButton(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);
    
    // Slider mode
    // Inform ofApp continuously when position changes
    // or when trackbar mouse button is released
    void SliderMode(bool bOnce);

    //
    // Slider
    //
    // x, y, width, depth - position and dimensions in the dialog
    // bShow (default true)
    //    If true adds a value indicator to the right of the slider
    //    Set false if not required
    // tickinterval (default 0)
    //    If tick marks are required, add the tick interval argument.
    //    The slider depth should be greater than 25 for tick marks to be visible.
    void AddSlider(std::string title, int x, int y, int width, int height,
        float min, float max, float value, bool bShow = true, float tickinterval = 0);
    
    //
    // Edit control text entry
    //
    // Style can be :
    // Text alignment ES_LEFT, ES_RIGHT or ES_CENTER. Default is ES_LEFT.
    // Outline WS_BORDER, WS_DLGFRAME
    void AddEdit(std::string title, int x, int y, int width, int height, std::string text, DWORD dwStyle = 0);
    
    // Combo box list selection
    void AddCombo(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index);
    
    // Static group box
    // A group box is not a control and has no title
    void AddGroup(std::string text, int x, int y, int width, int height);

    //
    // Static text
    // Style can be :
    //  SS_LEFT   - left aligned
    //	SS_CENTER - centered
    //	SS_RIGHT  - right aligned
    //	WS_BORDER - outlined
    //	SS_SUNKEN - sunken edge
    // Default is left aligned (SS_LEFT)
    // Static text is not a control and has no title
    void AddText(std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);

    // Hyperlink
    // Title is displayed, text is the action taken
    void AddHyperlink(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);
    
    // Picture
    void AddPicture(std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);

    //
    // Get dialog control values
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
    // Set dialog control values
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
    // ofApp calls GetControls to get the updated values
    void Load(std::string filename, std::string section="");

    // Icon for the dialog window
    void SetIcon(HICON hIcon);

    // Set a custom font
    // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
    // name   - e.g. "Tahoma", "Ms Shell Dlg" etc.
    // height - height in logical units
    // weight - FW_NORMAL, FW_BOLD etc. (default FW_NORMAL)
    void SetFont(std::string name, LONG height, LONG weight = FW_NORMAL);

    // Open dialog window with controls
    //  o If x and y are both positive, that position is used
    //	o If x and y are both zero, centre on the host window
    //  o if y is zero, offset from the centre by the x amount
    //  o If x and y are both negative, centre on the desktop
    //  o if x is CW_USEDEFAULT the system selects x and ignores y 
    HWND Open(const char* windowName, int x, int y, int width, int height);

    // Close dialog window and retain controls
    void Close();

    // Utility
    std::string float2string(float number, int places);

private:

    HWND m_hwnd = NULL; // Parent window handle
    HINSTANCE m_hInstance = NULL; // Parent instance handle
    ofApp* pApp = nullptr; // Pointer to access the ofApp class
    HICON m_hIcon = nullptr; // Dialog icon
    HWND hwndOKButton = NULL;  // OK Button
    HWND hwndCancelButton = NULL;  // Cancel Button
    WNDCLASSW wndClass{}; // Dialog window class structure

    // Variables for optional font
    std::string fontname;
    LONG fontheight = 0;
    LONG fontweight = FW_NORMAL;

 };
