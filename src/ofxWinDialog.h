//
// ofxWinDialog.h
//
#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <io.h>
#include <commctrl.h> // For trackbar
#include <Shlwapi.h> // For path functions
#include <uxtheme.h> // For visual style themes
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.Lib")
#pragma comment(lib, "UxTheme.lib")

// This adds the spoututils namespace so that SpoutUtils functions
// are generally available and SpoutMessageBox can be used.
// SpoutUtils includes a manifest for Version 6 common controls.
// See also "DisableTheme" below.
#include "../libs/SpoutUtils.h"
using namespace spoututils;

class ofApp; // Forward declaration

#define MAX_LOADSTRING 100

class ofxWinDialog {

public:

    // Class message handling procedure to allow multiple dialogs
    LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    // Class dialog window handle
    HWND m_hDialog = nullptr;

	ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd, std::string className = "");

    ~ofxWinDialog();

    // Radio button group
    // Nnumbering is incremented by the
    // AddRadioGroup function starting at 0
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
    // and text to display on the control, and these can have different names. 
    // If the text is empty, the title is used for display.
    // Control style can be BS_RIGHTBUTTON - default is a button on the left of the text.
    void AddCheckBox(std::string title, std::string text, int x, int y, int width, int height, bool bChecked, DWORD dwStyle = 0);
    
    // Start radio button group
    void AddRadioGroup();
    
    // Radio button
    // Control style can be BS_RIGHTBUTTON - default is a button on the left of the text.
    void AddRadioButton(std::string title, std::string text, int x, int y, int width, int height, bool bChecked, DWORD dwStyle = 0);
    
    //
    // Push button
    //
    // Style can be BS_DEFPUSHBUTTON for the default button. Default is BS_PUSHBUTTON.
    void AddButton(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);

	// Change button text
	void ButtonText(std::string title, std::string text);

	// Change button background color
	// Set before AddButton
	void ButtonColor(int hexcode);
	void ButtonColor(int red, int grn, int blu);
	void ButtonColor(COLORREF rgb);

	// Image file for picture button (bmp, jpg, png, tga)
	// TextColor can also be used to change the button text
	// Call before AddButton
	void ButtonPicture(std::string path);
	
    //
    // Slider
    //
    // x, y, width, height - position and dimensions
    // min, max, - range
    // value - starting position
    // bShow (default true)
    // bShow (default true)
    //    True adds a value indicator to the right of the slider
    //    False if not required
    // tickinterval (default 0)
    //    If tick marks are required, add the tick interval argument.
    //    Height should be greater than 25 for tick marks to be visible.
    // The slider position can be adjusted with the Left or Right keys
    void AddSlider(std::string title, int x, int y, int width, int height,
        float min, float max, float value, bool bShow = true, float tickinterval = 0);

    // Slider mode
    // Inform ofApp when the mouse button is released (true)
    // or continously as the position changes (false)
    void SliderMode(bool bOnce);
        
    //
    // Edit control text entry
    //
    // Style can be :
    // Text alignment ES_LEFT, ES_RIGHT or ES_CENTER. Default is ES_LEFT.
    // Outline WS_BORDER, WS_DLGFRAME
    void AddEdit(std::string title, int x, int y, int width, int height, std::string text, DWORD dwStyle = 0);
    
    // Combo box
	// Style CBS_DROPDOWN allows user entry
	// Default is CBS_DROPDOWNLIST which prevents user entry
	void AddCombo(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index, DWORD dwStyle = 0);
	// For testing
	void AddCombo(std::string title, int x, int y, int width, int height);

	// List box
	void AddList(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index);

	// Spin control
	// Style can be UDS_ALIGNLEFT or UDS_ALIGNRIGHT (default)
	void AddSpin(std::string title, int x, int y, int width, int height,
		int min, int max, int value, DWORD dwStyle = 0);

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

	// Static text that is not a updated
	void AddText(std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);

	// Static text with an idependent title that can be updated with SetText
	void AddText(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);

	// Static text color
	// Set before AddText
	void TextColor(int hexcode);
	void TextColor(int red, int grn, int blu);
	void TextColor(COLORREF rgb);

    // Hyperlink
    // Title is displayed, text is the action taken
    // If the text is empty, ofApp is notified when the hyperlink is clicked
    // Style can be :
    //  SS_LEFT   - left aligned
    //	SS_CENTER - centered
    //	SS_RIGHT  - right aligned
    void AddHyperlink(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);

	// Set a section name for the control in an initialization file
	// Default is the control Type (Edit, Checkbox, Button etc)
	void SetSection(std::string title, std::string section);

    //
    // Get dialog control values
	//

    // Get checkbox state
    int GetCheckBox(std::string title);
    // Get radio button state
    int GetRadioButton(std::string title);
    // Get slider value
    float GetSlider(std::string title);
    // Get edit control text
    std::string GetEdit(std::string title);
    // Get current combo box item and textx
	int GetComboItem(std::string title, std::string* text = nullptr);
	// Get combo box edit text
    std::string GetComboEdit(std::string title);
	// Get list box item index and text
	int GetListItem(std::string title, std::string* text = nullptr);
    // Get all current control values
    void GetControls();
    // Get the number of controls
    int GetControlNumber();
	// Get the dialog window handle
	HWND GetDialogWindow();


    //
	// Set dialog control values
	//

	void SetCheckBox(std::string title, int value);
    void SetRadioButton(std::string title, int value);
    void SetSlider(std::string title, float value);
    void SetEdit(std::string title, std::string text);
	void SetText(std::string title, std::string text);
    void SetComboItem(std::string title, int item);
	void SetList(std::string title, std::vector<std::string> items, int index);
	void SetListItem(std::string title, int item);
	void SetSpin(std::string title, int value);

	// Enable/Disable a control
	// (Except Hyperlink, Static and Group)
	void EnableControl(std::string title, bool bEnabled);

	// Reset controls with original values
    void Reset();

    // Restore controls with old values
    void Restore();

    // Refresh the dialog with current controls
    void Refresh();

    // Save controls initialization file with optional overwrite
    void Save(std::string filename, bool bOverWrite = false);

    // Load controls from an initialization file
    // The section name can be used to retrieve specific controls
    // ofApp calls GetControls to get the updated values
    bool Load(std::string filename, std::string section="");

    // Set icon for the dialog window
    void SetIcon(HICON hIcon);

    // Get dialog window icon handle if set
    HICON GetIcon();

    // Set a custom font
    // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
    // name    - e.g. "Tahoma", "Ms Shell Dlg" etc.
    // height  - height in logical units
    // weight  - FW_NORMAL, FW_BOLD etc. (default FW_NORMAL)
	// height in dialog units
	void SetFont(std::string name, LONG height, LONG weight = FW_NORMAL);

	// Return the font height set
	LONG GetFontHeight();

	// Return the logical font handle after window creation
	HFONT GetFont();

    // Set dialog position and size
    //  o If x and y are both positive, that position is used
    //  o If x and y are both zero, centre on the host window
    //  o if y is zero, offset from the centre by the x amount
    //  o If x and y are both negative, centre on the desktop
    //  o if x is CW_USEDEFAULT the system selects x and ignores y 
    void SetPosition(int x, int y, int width, int height);

    // Open dialog window with controls
    HWND Open(std::string title);

    // Close dialog window and retain controls
    void Close();

    // Utility
    std::string float2string(float number, int places);

    // Disable Visual Style themes for dialog controls
    // if using common controls version 6.0.0.0
    // All controls if hwndControl is not specified
    void DisableTheme(HWND hwndDialog, HWND hwndControl = NULL);

    // Disable Visual Style themes for a control type
    // and optional specific control
    void DisableTheme(std::string type="", std::string title="");

	//
	// Utility
	//
	int Rgb2Hex(COLORREF col);
	int Rgb2Hex(int r, int g, int b);
	COLORREF Hex2Rgb(int hex, int* red=nullptr, int* grn=nullptr, int* blu=nullptr);


    //
    // Control variables
    //
    struct ctl {

        std::string Type="";  // Control type
        std::string Title=""; // Control title
        std::string Section=""; // Control section for initialization file

        // Control size
        int X = 0;
        int Y = 0;
        int Width = 0;
        int Height = 0;

        std::string Text="";  // Edit control text
        std::vector<std::string> Items; // Combo list items
        int Index = 0; // Combo list index

        // Slider
        float Min = 0; // Range min
        float Max = 0; // Range max
        float SliderVal = 0; // Slider value
        float Tick = 0; // Tick interval

        // Buttons
        int Val = 0; // Value
        int RadioGroup = 0; // Radio button group
        bool First = false; // First in group flag (see AddRadioGroup)

        uint64_t ID = 0LL; // Control ID
        DWORD Style = 0; // Static text and button style
        bool VisualStyle = true; // Enable or diasble Visual Styles for a control
        HWND hwndControl = NULL;
        HWND hwndType = NULL;
        HWND hwndSlider = NULL;
        HWND hwndSliderVal = NULL; // Static text control to display the value of the slider
    };

    // Controls
    std::vector<struct ctl> newcontrols; // Controls for reset
    std::vector<struct ctl> controls; // Working controls
    std::vector<struct ctl> oldcontrols; // Controls for restore

	// Class name
	#ifdef UNICODE
	WCHAR m_ClassName[MAX_LOADSTRING]{};
	#else
	CHAR m_ClassName[MAX_LOADSTRING]{};
	#endif

 private:

    HWND m_hwnd = NULL; // Parent window handle
	HINSTANCE m_hInstance = NULL; // Parent instance handle
	ofApp * pApp = nullptr; // Pointer to access the ofApp class
	HICON m_hIcon = nullptr; // Dialog icon
    HWND hwndOKButton = NULL;  // OK Button
    HWND hwndCancelButton = NULL;  // Cancel Button

	COLORREF g_TextColor = RGB(0, 0, 0); // Static text colour
	COLORREF g_ButtonColor = RGB(0, 0, 0); // Button background (default COLOR_BTNFACE);
	HBITMAP g_hBitmap = nullptr; // Bitmap handle for owner draw button

	// Dialog position and size
    int dialogX = 0;
    int dialogY = 0;
    int dialogWidth = 0;
    int dialogHeight = 0;

    // Variables for optional font
    std::string fontname;
	LONG fontheight = 0;
    LONG fontweight = FW_NORMAL;
	HFONT g_hFont = NULL;


 };
