//
// ofxWinDialog.cpp
//
#include "ofxWinDialog.h"
#include <windows.h>
#include <stdio.h>

// Method to handle messages for dialog
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND m_hDialog = nullptr; // Dialog window handle
HWND hwndOKButton = NULL;  // OK Button
HWND hwndCancelButton = NULL;  // Cancel Button
WNDCLASS wndClass; // Dialog window class structure

// To get the tab key working for the modeless dialog
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
static HHOOK hHook = NULL;

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
    HWND hwndControl = NULL;
    HWND hwndType = NULL;
    HWND hwndSlider = NULL;
    HWND hwndSliderVal = NULL; // Static text control to display the value of the slider
};

// Controls and old controls
std::vector<struct ctl> controls;
std::vector<struct ctl> oldcontrols;

// Radio button group
// Numbering is incremented and starts at 0
int nRadioGroup = -1;

// First in the group flag
bool bRadioFirst = false;

// Variables for optional font
std::string fontname;
LONG fontheight = 0;
LONG fontweight = FW_NORMAL;

// Pointer to access the this class from the window procedure
ofxWinDialog* pThis;

ofxWinDialog::ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd) {

    m_hInstance = hInstance;
    m_hwnd = hWnd;

    pThis = this; // Pointer for access the ofxWinMenu class
    pApp = app; // The ofApp class pointer

    // Function to return dialog variables to ofApp
    // Set by CreateDialogFunction in ofApp
    pAppDialogFunction = NULL; 

    // Initialize the dialog window class
#ifdef UNICODE
    wndClass.lpszClassName = L"ofxWinDialogClass";
#else
    wndClass.lpszClassName = "ofxWinDialogClass";
#endif
    wndClass.lpfnWndProc = WindowProc; // Window procedure
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW); // Control background color

    // Register the window class
    if (!RegisterClass(&wndClass)) {
        MessageBoxA(NULL, "Dialog window class registration failed", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Initialize common controls for trackbar
    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);

}

ofxWinDialog::~ofxWinDialog() {
    // Close the dialog window
    SendMessage(m_hDialog, WM_CLOSE, 0, 0);
    // Unregister the window class
    UnregisterClass(wndClass.lpszClassName, m_hInstance);
}

// Set a custom font
void ofxWinDialog::SetFont(std::string name, LONG height, LONG weight)
{
    fontname = name;
    fontheight = height;
    fontweight = weight;
}

// Checkbox
// Text in the checkbox is independent of the title
// Style can be : BS_LEFT, BS_CENTER, BS_RIGHT - default BS_LEFT.
void ofxWinDialog::AddCheckBox(std::string title, std::string text, 
    int x, int y, int width, int height, bool bChecked, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Checkbox";
    control.Title = title;
    control.Text = text;
    control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    control.Val=(int)bChecked;
    controls.push_back(control);
}

// Radio button group
void ofxWinDialog::AddRadioGroup()
{
    // Increment radio button group number
    nRadioGroup++;
    // Reset first of group flag
    bRadioFirst = true;
}

// Radio button
// Text in the radio button is independent of the title
// Style can be : BS_LEFTTEXT, BS_CENTERTEXT, BS_RIGHTTEXT - default BS_LEFTTEXT.
void ofxWinDialog::AddRadioButton(std::string title, std::string text,
    int x, int y, int width, int height, bool bChecked, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Radio";
    control.Title = title;
    control.Text = text;
    control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    control.Val=(int)bChecked;
    control.RadioGroup=nRadioGroup; // Radio button group
    control.First=bRadioFirst; // First radio button flag
    bRadioFirst = false; // Reset true only by AddRadioGroup
    controls.push_back(control);
}

// Slider control
void ofxWinDialog::AddSlider(std::string title,
    int x, int y, int width, int height, float min, float max, float value, float tickinterval)
{
    ctl control{};
    control.Type = "Slider";
    control.Title = title;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    control.Min=min;
    control.Max=max;
    control.SliderVal=value;
    control.Tick=tickinterval;
    controls.push_back(control);
}

// Edit control
void ofxWinDialog::AddEdit(std::string title, int x, int y, int width, int height, std::string text, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Edit";
    control.Title = title;
    control.Text = text;
    control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

// Combo box list control
void ofxWinDialog::AddCombo(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index)
{
    ctl control{};
    control.Type = "Combo";
    control.Title = title;
    control.Items = items;
    control.Index = index;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

// Push button
// Text in the button is independent of the title
void ofxWinDialog::AddButton(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Button";
    control.Title = title;
    control.Text = text;
    control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

//
// Static text
//
// Default style left aligned (SS_LEFT)
// Additional styles can be specified
//	  SS_CENTER - centered
//	  SS_RIGHT  - right aligned
//	  WS_BORDER - outlined
//	  SS_SUNKEN - sunken edge
// Static text is not a control and the title will not conflict with controls
void ofxWinDialog::AddText(std::string title, int x, int y, int width, int height, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Static";
    control.Title = title;
    control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

// Static Group box
void ofxWinDialog::AddGroup(std::string title, int x, int y, int width, int height)
{
    ctl control{};
    control.Type = "Group";
    control.Title = title;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

// Get checkbox state
int ofxWinDialog::GetCheckBox(std::string title)
{
    int state = 0;
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Checkbox") {
            if (controls[i].Title == title) {
                state = controls[i].Val;
            }
        }
    }
    return state;
}

// Get radio button state
int ofxWinDialog::GetRadioButton(std::string title)
{
    int state = 0;
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Radio") {
            if (controls[i].Title == title) {
                state = controls[i].Val;
            }
        }
    }
    return state;
}

// Get slider value
float ofxWinDialog::GetSlider(std::string title)
{
    float value = 0.0f;
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Slider") {
            if (controls[i].Title == title) {
                value = controls[i].SliderVal;
            }
        }
    }
    return value;
}

// Get edit control text
std::string ofxWinDialog::GetEdit(std::string title)
{
    std::string str;
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Edit") {
            if (controls[i].Title == title) {
                // Get the current text from the edit control
                char buffer[256]{};
                GetWindowTextA(controls[i].hwndControl, buffer, sizeof(buffer));
                str = buffer;
                // Update the control text
                controls[i].Text = buffer;
            }
        }
    }
    // Return the control text
    return str;
}

// Get current combo box item index
int ofxWinDialog::GetComboItem(std::string title)
{
    int index = 0;
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Combo") {
            if (controls[i].Title == title) {
                index = controls[i].Index;
            }
        }
    }
    return index;
}

// Get combo box item text
std::string ofxWinDialog::GetComboText(std::string title, int item)
{
    std::string str;
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Combo") {
            if (controls[i].Title == title) {
                str = controls[i].Items[item];
            }
        }
    }
    // Return the item text
    return str;
}

// Set a section name for the control in an initialization file
void ofxWinDialog::SetSection(std::string title, std::string section)
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Title == title) {
            controls[i].Section = section;
        }
    }
}

// Set checkbox state
void ofxWinDialog::SetCheckBox(std::string title, int value)
{
    for (size_t i=0; i<controls.size(); i++) {
        // Update the checkbox state
        if (controls[i].Type == "Checkbox") {
            if (controls[i].Title == title) {
                controls[i].Val = value;
                if (value == 1)
                    SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_CHECKED, 0);
                else
                    SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_UNCHECKED, 0);
                InvalidateRect(controls[i].hwndControl, NULL, TRUE);
            }
        }
    }
}

// Set radio button state
// The application must set all buttons in the group
void ofxWinDialog::SetRadioButton(std::string title, int value)
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Radio") {
            if (controls[i].Title == title) {
               // Update the Radio button state
               controls[i].Val = value;
               if(value == 1)
                    SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_CHECKED, 0);
               else
                   SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_UNCHECKED, 0);
               InvalidateRect(controls[i].hwndControl, NULL, TRUE);
            } // End matching title
        } // End radio button controls
    } // End all controls
}

// Set slider value
void ofxWinDialog::SetSlider(std::string title, float value)
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Slider") {
            if (controls[i].Title == title) {
                // Update the Slider value
                controls[i].SliderVal = value;
                SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, (int)(value*100.0f));
                InvalidateRect(controls[i].hwndControl, NULL, TRUE);
                // Slider value text display
                char tmp[8]{};
                if (controls[i].Max >= 100.0f)
                    sprintf_s(tmp, 8, "%.0f", controls[i].SliderVal);
                else if (controls[i].Max >= 10.0f)
                    sprintf_s(tmp, 8, "%.1f", controls[i].SliderVal);
                else
                    sprintf_s(tmp, 8, "%.2f", controls[i].SliderVal);
                SetWindowTextA(controls[i].hwndSliderVal, (LPCSTR)tmp);
                InvalidateRect(controls[i].hwndSliderVal, NULL, TRUE);
            }
        }
    }
}

void ofxWinDialog::SetEdit(std::string title, std::string text)
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Edit") {
            if (controls[i].Title == title) {
                // Update the edit control
                SetWindowTextA(controls[i].hwndControl, (LPCSTR)text.c_str());
                controls[i].Text=text;
                InvalidateRect(controls[i].hwndControl, NULL, TRUE);
            }
        }
    }
}

// Reset the selected combo item index
void ofxWinDialog::SetCombo(std::string title, int item)
{
     for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Combo") {
            if (controls[i].Title == title && item < (int)controls[i].Items.size()) {
                // Make the item current
                controls[i].Index = item;
                SendMessage(controls[i].hwndControl, (UINT)CB_SETCURSEL, (WPARAM)item, 0L);
                InvalidateRect(controls[i].hwndControl, NULL, TRUE);
            }
        }
    }
}

// Get current control values
void ofxWinDialog::GetControls()
{
    // Get current controls only if the dialog is open
    if (!m_hDialog)
        return;

    // Retrieve text from the edit controls
    // Combo, Slider, Checkbox, Radio buttons send information when changed.
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Edit") {
            char textBuffer[256]{};
            GetWindowTextA(controls[i].hwndControl, textBuffer, sizeof(textBuffer));
            controls[i].Text=textBuffer;
        }
    }

    // Inform ofApp
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {
            if (controls[i].Type == "Combo") {
                pThis->DialogFunction(controls[i].Title, "", controls[i].Index);
            }
            else if (controls[i].Type == "Slider") {
            }
            else if (controls[i].Type == "Edit") {
                pThis->DialogFunction(controls[i].Title, controls[i].Text, true);
            }
            else {
                pThis->DialogFunction(controls[i].Title, "", controls[i].Val);
            }
        }
    }
}

// Restore controls with old values
void ofxWinDialog::Restore()
{
    // Restore old controls
    controls = oldcontrols;
    // Refresh display and inform ofApp
    Refresh();
}

// Refresh all dialog controls with new values
void ofxWinDialog::Refresh()
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Checkbox" || controls[i].Type == "Radio") {
            if (controls[i].Val == 1) {
                SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_CHECKED, 0);
            }
            else {
                SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_UNCHECKED, 0);
            }
        }
        if (controls[i].Type == "Slider") {
            SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, (int)(controls[i].SliderVal*100.0f));
            // Slider value text display
            char tmp[8]{};
            if (controls[i].Max >= 100.0f)
                sprintf_s(tmp, 8, "%.0f", controls[i].SliderVal);
            else if (controls[i].Max >= 10.0f)
                sprintf_s(tmp, 8, "%.1f", controls[i].SliderVal);
            else
                sprintf_s(tmp, 8, "%.2f", controls[i].SliderVal);
            SetWindowTextA(controls[i].hwndSliderVal, (LPCSTR)tmp);
        }
        if (controls[i].Type == "Combo") {
            if (!controls[i].Text.empty()) {
                // Delete the existing item text
                SendMessage(controls[i].hwndControl, CB_DELETESTRING, controls[i].Index, 0);
                // Add the new item text
                SendMessage(controls[i].hwndControl, CB_INSERTSTRING, controls[i].Index, (LPARAM)controls[i].Text.c_str());
            }
            // Make the new item current
            SendMessage(controls[i].hwndControl, (UINT)CB_SETCURSEL, (WPARAM)controls[i].Index, 0L);
            // Select all text in the edit field
            SendMessage(controls[i].hwndControl, CB_SETEDITSEL, 0, MAKELONG(0, -1));
        }
        if (controls[i].Type == "Edit") {
            SetWindowTextA(controls[i].hwndControl, (LPCSTR)controls[i].Text.c_str());
        }
    }

    // Inform ofApp of refresh
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {
            if (controls[i].Type == "Combo")
                pThis->DialogFunction(controls[i].Title, "", controls[i].Index);
            else if (controls[i].Type == "Edit")
                pThis->DialogFunction(controls[i].Title, controls[i].Text, true);
            else if (controls[i].Type == "Slider")
                pThis->DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
            else
                pThis->DialogFunction(controls[i].Title, "", controls[i].Val);
        }
    }

}

// Save controls to an initialization file
void ofxWinDialog::Save(std::string filename, bool bOverWrite)
{
    char tmp[MAX_PATH]{};
    std::string inipath;

    // Section for the control in the initialization file
    std::string ControlSection;

    // Check extension
    size_t pos = filename.rfind(".ini");
    if (pos == std::string::npos) {
        // No extension or not ".ini"
        pos = filename.rfind(".");
        if (pos == std::string::npos) {
            // No extension - add ".ini"
            filename = filename + ".ini";
        }
        else {
            // Extension not "ini" 
            // Strip extension
            filename = filename.substr(0, pos);
            // Add ".ini"
            filename = filename + ".ini";
        }
    }

    // Check for full path
    if (filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
        inipath = filename;
    }
    else {
        // filename only - add full path - (bin\data directory)
        char path[MAX_PATH]{};
        GetModuleFileNameA(NULL, path, MAX_PATH);
        PathRemoveFileSpecA(path);
        inipath = path;
        inipath = inipath + "\\data\\" + filename;
    }

    // Check if file exists if bOverWrite is false
    if (!bOverWrite && _access(inipath.c_str(), 0) != -1) {
        sprintf_s(tmp, MAX_PATH, "%s exists - overwrite?", filename.c_str());
        if(MessageBoxA(NULL, tmp, "Warning", MB_YESNO | MB_TOPMOST) == IDNO)
            return;
    }

    // Get current controls if the dialog is open
    GetControls();

    // Save control values
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {
            
            // A control section name is used if assigned using SetSection
            // Default section name is the control type
            ControlSection = controls[i].Type;
            if (!controls[i].Section.empty()) ControlSection = controls[i].Section;

            if (controls[i].Type == "Combo") {
                sprintf_s(tmp, MAX_PATH, "%d", controls[i].Index);
                WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
            else if (controls[i].Type == "Edit") {
                WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)controls[i].Text.c_str(), (LPCSTR)inipath.c_str());
            }
            else if (controls[i].Type == "Slider") {
                sprintf_s(tmp, MAX_PATH, "%.2f", controls[i].SliderVal);
                WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
            else {
                sprintf_s(tmp, MAX_PATH, "%d", controls[i].Val);
                WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
        }
    }

}

// Load controls from an initialization file
void ofxWinDialog::Load(std::string filename, std::string section)
{
    char tmp[MAX_PATH]{};
    std::string inipath="";

    // Section for the control in the initialization file
    std::string ControlSection="";

    // Check extension
    size_t pos = filename.rfind(".ini");
    if (pos == std::string::npos) {
        // No extension or not ".ini"
        pos = filename.rfind(".");
        if (pos == std::string::npos) {
            // No extension - add ".ini"
            filename = filename + ".ini";
        }
        else {
            // Extension not "ini"
            sprintf_s(tmp, "%s is not an initialization file\n", filename.c_str());
            MessageBoxA(NULL, tmp, "Warning", MB_OK | MB_TOPMOST);
            return;
        }
    }

    // Check for full path
    if(filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
        inipath = filename;
    }
    else {
        // filename only - add full path - (bin\data directory)
        char path[MAX_PATH]{};
        GetModuleFileNameA(NULL, path, MAX_PATH);
        PathRemoveFileSpecA(path);
        inipath = path;
        inipath = inipath + "\\data\\" + filename;
    }

    // Check that the file exists in case an extension was added
    if (_access(inipath.c_str(), 0) == -1) {
        sprintf_s(tmp, "Initialization file \"%s\" not found.", inipath.c_str());
        MessageBoxA(NULL, tmp, "Warning", MB_OK | MB_TOPMOST);
        return;
    }

    // Load control values
    // Only those saved in the ini file are changed
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {

            // Use the section name argument if specified
            // Use the control section name if assigned by SetSection
            // Default section name is the control type
            ControlSection = controls[i].Type;
            if (!section.empty()) 
                ControlSection = section;
            else if (!controls[i].Section.empty()) 
                ControlSection = controls[i].Section;

            if (controls[i].Type == "Combo") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
                    if (tmp[0]) controls[i].Index = atoi(tmp);
                }
            }
            else if (controls[i].Type == "Edit") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str())> 0) {
                    if (tmp[0]) controls[i].Text = tmp;
                }
            }
            else if (controls[i].Type == "Slider") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
                    if (tmp[0]) controls[i].SliderVal = (float)atof(tmp);
                }
            }
            else if (controls[i].Type == "Checkbox") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
                    if (tmp[0]) controls[i].Val = atoi(tmp);
                }
            }
        }
    }

    // Refresh the dialog with the new controls and inform ofApp
    Refresh();
}

// Dialog window icon
void ofxWinDialog::SetIcon(HICON hIcon)
{
    m_hIcon = hIcon;
}

// Create dialog window
HWND ofxWinDialog::Open(const char* windowTitle, int x, int y, int width, int height)
{
    // Prevent repeats
    if (m_hDialog)
        return m_hDialog;

    RECT rect{};
    GetWindowRect(m_hwnd, &rect);
    int rwidth  = rect.right - rect.left;
    int rheight = rect.bottom - rect.top;

    int xpos = x;
    int ypos = y;
    //  o If x and y are both positive, that position is used
    //	o If x and y are both zero, centre on the host window
    //  if y is zero, offset from the centre by the x amount
    if (x == 0 && y == 0) {
        // centre on the desktop work area
        RECT WorkArea{};
        SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID)&WorkArea, 0);
        xpos = ((WorkArea.right  - WorkArea.left) - width) / 2;
        ypos = ((WorkArea.bottom - WorkArea.top)  - height) / 2;
    }
    else if (x == 0) {
        // If x is zero, centre on the host window
        xpos = rect.left + rwidth/2 - width/2;
        ypos = rect.top  + rheight/2 - height/2;
    }
    else if (y == 0) {
        // If y is zero, offset left or right 
        // from the left side of the host window
        xpos = rect.left + rwidth/2 - width/2;
        ypos = rect.top  + rheight/2 - height/2;
        xpos = rect.left + x;
        if (xpos < 0) xpos = 0;
    }

    // Title is wide chars if Unicode
#ifdef UNICODE
    wchar_t title[MAX_PATH]{};
    mbstowcs_s(NULL, title, windowTitle, MAX_PATH);
#else
    char title[MAX_PATH]{};
    strcpy_s(title, MAX_PATH, windowTitle);
#endif

    // No minimize, maximize or menu
    // WS_CAPTION | WS_SYSMENU gives a close button and icon
    HWND hwnd = CreateWindow(wndClass.lpszClassName, title,
        DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU,
        xpos, ypos, width, height,
        m_hwnd,      // Parent window
        NULL,        // No menu
        m_hInstance, // Parent instance
        NULL);

    if (!hwnd) {
        MessageBoxA(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Dialog window icon if specified
    if (m_hIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
    }

    // Clear all window handles for repeats
    for (size_t i=0; i<controls.size(); i++) {
        controls[i].hwndControl = NULL;
    }

    //
    // Draw all controls
    //
    HWND hwndc = NULL;
    uint64_t ID = 1000; // Start control ID
    for (size_t i=0; i<controls.size(); i++) {

        //
        // Checkbox
        //
        if (controls[i].Type == "Checkbox") {
            // Text in the checkbox is independent of the title
            // If the text string is empty, the title is used.
            std::string str = controls[i].Text.c_str();
            if (str.empty()) str = controls[i].Title;
            // Style can be BS_RIGHTBUTTON - default is a button to the left of the text
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX;
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;
            else
                dwStyle |= BS_LEFT;;
            hwndc = CreateWindowExA(0, "BUTTON", str.c_str(), dwStyle,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd,        // Parent window
                (HMENU)ID,   // Control ID
                m_hInstance, // Parent instance handle
                NULL);

            if (hwndc) {
                // Initial checkbox state
                SendMessage(hwndc, BM_SETCHECK, controls[i].Val, 0);
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

        //
        // Radio button
        //
        // The first radio button in the group has the WS_GROUP style
        // to define the beginning of a radio button group. 
        // A new group is started by AddRadioGroup.
        //
        if (controls[i].Type == "Radio") {
            // Text in the radio button is independent of the title
            // If the text string is empty, the title is used.
            std::string str = controls[i].Text.c_str();
            if (str.empty()) str = controls[i].Title;
            DWORD dwStyle = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON;
            // The first radio button in the group has WS_GROUP style
            if (controls[i].First == 1)
                dwStyle = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP;
            // Style can also be BS_RIGHTBUTTON - default is a button to the left of the text
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;
            else
                dwStyle |= BS_LEFT;;
            hwndc = CreateWindowExA(0, "BUTTON", str.c_str(), dwStyle,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                // Initial state
                SendMessage(hwndc, BM_SETCHECK, controls[i].Val, 0);
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

        //
        // Slider
        //
        if (controls[i].Type == "Slider") {
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | TBS_HORZ;
            if (controls[i].Tick > 0.0)
                dwStyle |= TBS_AUTOTICKS;
            else
                dwStyle |= TBS_NOTICKS;

            hwndc = CreateWindowExA(0, TRACKBAR_CLASSA, controls[i].Title.c_str(),
                dwStyle,
                controls[i].X, controls[i].Y,
                controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;

                // Set slider range and initial position
                SendMessage(hwndc, TBM_SETRANGE, TRUE, MAKELONG((int)(controls[i].Min*100.0f), (int)(controls[i].Max*100.0f)));
                SendMessage(hwndc, TBM_SETPOS, TRUE, (int)(controls[i].SliderVal*100.0f));

                // Set tick interval
                if(controls[i].Tick > 0)
                    SendMessage(hwndc, TBM_SETTICFREQ, (int)(controls[i].Tick*100.0f), 0);

                // Create a static text control to display the value of the slider
                HWND hwndval = CreateWindowExA(
                    0, "STATIC", "0", WS_VISIBLE | WS_CHILD | SS_RIGHT, // right aligned
                    controls[i].X + controls[i].Width, controls[i].Y,
                    45, controls[i].Height, hwnd, NULL, m_hInstance, NULL);

                if (hwndval) {
                    // Initial slider value text
                    char tmp[8]{};
                    if (controls[i].Max >= 100.0f)
                        sprintf_s(tmp, 8, "%.0f", controls[i].SliderVal);
                    else if (controls[i].Max >= 10.0f)
                        sprintf_s(tmp, 8, "%.1f", controls[i].SliderVal);
                    else 
                        sprintf_s(tmp, 8, "%.2f", controls[i].SliderVal);
                    SetWindowTextA(hwndval, (LPCSTR)tmp);
                    controls[i].hwndSliderVal = hwndval;
                }
            }
        }

        //
        // Edit control
        //
        if (controls[i].Type == "Edit") {
            // Style can be ES_LEFT, ES_RIGHT or ES_CENTER. Default is ES_LEFT.
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | controls[i].Style;
            hwndc = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", controls[i].Text.c_str(),
                dwStyle,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

        //
        // Combo box list selection control
        //
        if (controls[i].Type == "Combo") {
            // CBS_DROPDOWNLIST prevents user entry
            // CBS_DROPDOWN allows it
            hwndc = CreateWindowExA(WS_EX_CLIENTEDGE, "COMBOBOX", controls[i].Title.c_str(),
                WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                // Add combo box items
                if (controls[i].Items.size() > 0) {
                    for (size_t j = 0; j<controls[i].Items.size(); j++) {
                        SendMessage(hwndc, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)controls[i].Items[j].c_str());
                    }
                }
                // Display an initial item in the selection field
                SendMessage(hwndc, CB_SETCURSEL, (WPARAM)controls[i].Index, (LPARAM)0);
                // Select all text in the edit field
                SendMessage(hwndc, CB_SETEDITSEL, 0, MAKELONG(0, -1));
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

        //
        // Push button
        //
        if (controls[i].Type == "Button") {
            // Text in the button is independent of the title
            // If the text string is empty, the title is used.
            std::string str = controls[i].Text.c_str();
            if (str.empty()) str = controls[i].Title;
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
            // The button control style can be specified
            // OK button is BS_DEFPUSHBUTTON
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;
            else
                dwStyle |= BS_PUSHBUTTON;
            hwndc = CreateWindowExA(0, "BUTTON", str.c_str(),
                dwStyle,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

        //
        // Group box
        //
        if (controls[i].Type == "Group") {

            hwndc = CreateWindowExA(0, "BUTTON", controls[i].Title.c_str(),
                WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

        //
        // Static text
        //
        if (controls[i].Type == "Static") {

            // Default style is left aligned (SS_LEFT)
            // Additional styles can be specified
            //	SS_CENTER - centered
            //	SS_RIGHT  - right aligned
            //	WS_BORDER - outlined
            //	SS_SUNKEN - sunken edge
            DWORD dwStyle = WS_VISIBLE | WS_CHILD;
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;
            else
                dwStyle |= SS_LEFT;

            hwndc = CreateWindowExA(0, "STATIC", controls[i].Title.c_str(),
                dwStyle,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                controls[i].hwndControl = hwndc;
                controls[i].ID = ID;
                ID++;
            }
        }

    } // end all controls

    // Save all controls
    oldcontrols = controls;

    // Custom dialog font
    if (!fontname.empty() && fontheight > 0) {
        LOGFONTA logFont{};
        logFont.lfHeight = fontheight;
        logFont.lfWeight = fontweight;
        logFont.lfCharSet = ANSI_CHARSET;
        logFont.lfQuality = DEFAULT_QUALITY;
        strcpy_s(logFont.lfFaceName, 32, fontname.c_str());
        // Create the font
        HFONT hFont = CreateFontIndirectA(&logFont);
        // Set the font for the dialog and all controls
        SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));
        for (size_t i=0; i<controls.size(); i++) {
            SendMessage(controls[i].hwndControl, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));
            if(controls[i].hwndSliderVal)
                SendMessage(controls[i].hwndSliderVal, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));
        }
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    m_hDialog = hwnd;

    // Set focus back to the parent window
    SetFocus(m_hwnd);

    return hwnd;

}

// Close dialog window
void ofxWinDialog::Close()
{
    if (m_hDialog) {
        PostMessage(m_hDialog, WM_CLOSE, 0, 0);
    }
}

// ofApp Function for return of control values
void ofxWinDialog::CreateDialogFunction(void(ofApp::* function)(std::string title, std::string text, int value))
{
    pAppDialogFunction = function;
}


// Pass back the dialog item title and state to ofApp
void ofxWinDialog::DialogFunction(std::string title, std::string text, int value)
{
    (pApp->*pAppDialogFunction)(title, text, value);
}

//
// Utility
//
std::string ofxWinDialog::float2string(float number, int places)
{
    std::string str = std::to_string(number);

    str = str.substr(0, str.find(".")+places+1);
    return str;
}

/*
// To get the tab key working for the modeless dialog
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
static HHOOK hHook = NULL;

// in WM_INITDIALOG
// To process tab keys
// https://support.microsoft.com/en-us/help/233263/prb-modeless-dialog-box-in-a-dll-does-not-process-tab-key
hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId());

case WM_DESTROY:
        if(hHook) UnhookWindowsHookEx(hHook);
        return (INT_PTR)false;


//
// https://support.microsoft.com/en-us/help/233263/prb-modeless-dialog-box-in-a-dll-does-not-process-tab-key
//
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    LPMSG lpMsg = (LPMSG)lParam;

    if (nCode >= 0 && PM_REMOVE == wParam)
    {
        // Don't translate non-input events.
        if ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST))
        {
            if (IsDialogMessage(hWndSettings, lpMsg))
            {
                // The value returned from this hookproc is ignored,
                // and it cannot be used to tell Windows the message has been handled.
                // To avoid further processing, convert the message to WM_NULL
                // before returning.
                lpMsg->message = WM_NULL;
                lpMsg->lParam = 0;
                lpMsg->wParam = 0;
            }
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}


*/

//
// Local window message callback procedure
//
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

        case WM_CREATE:
            hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId());
            break;
        
        // Check For Windows system messages
        case WM_SYSCOMMAND:
            switch (wParam) {         // Check System Calls
                case SC_SCREENSAVE:   // Screensaver Trying To Start?
                case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
                return 0;             // Prevent From Happening
            }
            break;

        case WM_COMMAND:

            // Handle control events and inform the app

            // Combo box
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                // Check all combo controls
                for (size_t i=0; i<controls.size(); i++) {
                    if (controls[i].Type == "Combo") {
                        // Get currently selected combo index
                        // Allow for error if the user edits the list item
                        int index = (int)SendMessage(controls[i].hwndControl, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                        if (index != CB_ERR) {
                            // Inform ofApp if no error
                            pThis->DialogFunction(controls[i].Title, "", index);
                        }
                        // Reset the control index
                        controls[i].Index = index;
                    }
                }
            }

            // Button control
            if (HIWORD(wParam) == BN_CLICKED) {

                // Check all button controls
                for (size_t i=0; i<controls.size(); i++) {
                    
                    //
                    // Checkbox
                    //
                    if (controls[i].Type == "Checkbox") {
                        if (LOWORD(wParam) == controls[i].ID) { // ID of the checkbox selected
                            // Test if the checkbox is checked or unchecked
                            if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED)
                                controls[i].Val = 1;
                            else
                                controls[i].Val = 0;
                            pThis->DialogFunction(controls[i].Title, "", controls[i].Val);
                        }
                    } // End Checkbox

                    //
                    // Radio button
                    //
                    if (controls[i].Type == "Radio") {
                        // Loop though each radio button group
                        for (int j=0; j<=nRadioGroup; j++) {
                            // Group numbering starts at 0
                            if (j == controls[i].RadioGroup) {
                                int selectedGroup   = -1;
                                int selectedControl = -1;
                                // ID of the radio button selected
                                if (LOWORD(wParam) == controls[i].ID) {
                                    selectedGroup = j;
                                    if (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                                        selectedControl = (int)i;
                                    }
                                }
                                if (selectedControl >= 0) {
                                    // Look though all the radio buttons and set the selected one
                                    // Others in the same group are set to zero
                                    for (size_t k=0; k<controls.size(); k++) {
                                        if (controls[k].Type == "Radio") {
                                            if (selectedGroup == controls[k].RadioGroup) {
                                                if (selectedControl == (int)k) {
                                                    controls[k].Val = 1;
                                                }
                                                else {
                                                    controls[k].Val = 0;
                                                }
                                                // Inform ofApp
                                                pThis->DialogFunction(controls[k].Title, "", controls[k].Val);
                                            }
                                        }
                                    }
                                } // endif selectedControl >= 0
                            } // endif if j == controls[i].Group
                        } // end loop though each radio button group
                    } // end Radio button
                    
                    // Push button
                    if (controls[i].Type == "Button") {
                        if (LOWORD(wParam) == controls[i].ID) { // ID of the button selected
                            // Inform ofApp
                            pThis->DialogFunction(controls[i].Title, "", 1);
                        }
                    } // end Push button

                } // end all button controls

                // IDOK and IDCANCEL are not used
                // OK and Cancel buttons are handled by ofApp

            }
            break;

        case WM_HSCROLL:
            
            //
            // Sliders
            //
            for (size_t i=0; i<controls.size(); i++) {
                if (controls[i].Type == "Slider") {
                    if ((HWND)lParam == controls[i].hwndControl) {
                        // Current position of the slider
                        int pos = (int)SendMessage(controls[i].hwndControl, TBM_GETPOS, 0, 0);
                        controls[i].SliderVal = (float)pos/100.0f;
                        // Slider value text display
                        char tmp[8]{};
                        if (controls[i].Max >= 100.0f)
                            sprintf_s(tmp, 8, "%.0f", controls[i].SliderVal);
                        else if (controls[i].Max >= 10.0f)
                            sprintf_s(tmp, 8, "%.1f", controls[i].SliderVal);
                        else
                            sprintf_s(tmp, 8, "%.2f", controls[i].SliderVal);
                        SetWindowTextA(controls[i].hwndSliderVal, (LPCSTR)tmp);
                        pThis->DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                    }
                }
            }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            if (hHook) UnhookWindowsHookEx(hHook);
            DestroyWindow(hwnd);
            m_hDialog = NULL;
            // Inform ofApp
            pThis->DialogFunction("WM_DESTROY", "", true);
            break;
        // Handle other messages here if needed
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// To enable the tab key
//
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    LPMSG lpMsg = (LPMSG)lParam;

    if (nCode >= 0 && PM_REMOVE == wParam)
    {
        // Don't translate non-input events.
        if ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST))
        {
            if (IsDialogMessage(m_hDialog, lpMsg))
            {
                // The value returned from this hookproc is ignored,
                // and it cannot be used to tell Windows the message has been handled.
                // To avoid further processing, convert the message to WM_NULL
                // before returning.
                lpMsg->message = WM_NULL;
                lpMsg->lParam = 0;
                lpMsg->wParam = 0;
            }
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}
