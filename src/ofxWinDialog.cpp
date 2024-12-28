//
// ofxWinDialog.cpp
//
#include "ofxWinDialog.h"
#include <windows.h>
#include <stdio.h>


// Main Windows message procedure that forwards
// messages to the instance's message handler
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Message hook to enable the tab key
// https://learn.microsoft.com/en-us/windows/win32/winmsg/getmsgproc
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
static HHOOK hMsgHook = NULL;

// Static window handle for GetMsgProc
static HWND hwndDialog = NULL;


ofxWinDialog::ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd, std::wstring className) {

    m_hInstance = hInstance;
    m_hwnd = hWnd;

    pApp = app; // The ofApp class pointer

    // ofApp callback function for return of control values
    // Set by AppDialogFunction
    pAppDialogFunction = nullptr;

    // Main Windows message handling procedure
    // to forward messages to the class message procedure
    wndClass.lpfnWndProc = MainWndProc;
    
    // Window class name for mutltiple dialogs
    if (!className.empty())
        wndClass.lpszClassName = className.c_str();
    else
        wndClass.lpszClassName = L"ofxWinDialogClass";
    m_ClassName = wndClass.lpszClassName;
        
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW); // Control background color

    // Register the window class
    if (!RegisterClassW(&wndClass)) {
        MessageBoxA(NULL, "Dialog window class registration failed", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Create the message hook to endable the tab key
    if (!hMsgHook) {
        hMsgHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId());
    }
}


ofxWinDialog::~ofxWinDialog() {
    // Close the dialog window
    if(m_hDialog) SendMessage(m_hDialog, WM_CLOSE, 0, 0);
    // Unregister the window class
    UnregisterClassW(m_ClassName.c_str(), m_hInstance);
    // Release message hook
    if (hMsgHook) UnhookWindowsHookEx(hMsgHook);
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
    // nRadioGroup is initialized to -1 
    // and the group numbering starts at 0
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

//
// Slider control
//
// x, y, width, height - position and dimensions in the dialog
// min, max, - range
// value - starting position
// bShow (default true)
//    True adds a value indicator to the right of the slider
//    False if not required
// tickinterval (default 0)
//    If tick marks are required, add both the bShow and tick interval arguments.
//        In this example if the slider range is 0-255 (256) and an interval
//        of 16 gives 16 ticks. Height should be greater than 25
//        for tick marks to be visible.
//
void ofxWinDialog::AddSlider(std::string title,
    int x, int y, int width, int height, 
    float min, float max, float value, bool bShow, float tickinterval)
{
    ctl control{};
    control.Type = "Slider";
    control.Title = title;
    control.Index = bShow; // flag to show value to the right
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

// Slider mode
// Inform ofApp when the mouse button is released (true)
// or continously as the position changes (false)
void ofxWinDialog::SliderMode(bool bOnce)
{
    bOneClick = bOnce;
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

// Static Group box
// A group box is not a control and has no title
void ofxWinDialog::AddGroup(std::string text, int x, int y, int width, int height)
{
    ctl control{};
    control.Type = "Group";
    control.Title = text;
    control.Text = text;
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
// Static text is not a control and has no title
void ofxWinDialog::AddText(std::string text, int x, int y, int width, int height, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Static";
    control.Title = text;
    control.Text = text;
    control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

// Hyperlink
// Title is the text displayed, control text is the action taken
// If the text is empty, ofApp is notified when the hyperlink is clicked
void ofxWinDialog::AddHyperlink(std::string title, std::string text, int x, int y, int width, int height)
{
    ctl control{};
    control.Type = "Static";
    control.Title = title; // The title that appears in the dialog
    control.Text  = text; // The action to be taken.
    // Enable notifications for action and owner draw for text colour
    control.Style = SS_NOTIFY | SS_OWNERDRAW;
    control.Index = 1; // To identify a hyperlink from static text
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
                // Update the control 'Text' string
                controls[i].Text = str;
            }
        }
    }
    // Return the control text if found
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
                if (controls[i].Index > 0) {
                    char tmp[8]{};
                    if (controls[i].Max >= 100.0f)
                        sprintf_s(tmp, 8, "%.0f", controls[i].SliderVal);
                    else if (controls[i].Max >= 10.0f)
                        sprintf_s(tmp, 8, "%.1f", controls[i].SliderVal);
                    else
                        sprintf_s(tmp, 8, "%.2f", controls[i].SliderVal);
                    SetWindowTextA(controls[i].hwndSliderVal, (LPCSTR)tmp);
                }
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
            }
        }
    }
}

// Get current controls
// This function is called from ofApp to return control values
void ofxWinDialog::GetControls()
{
    char tmp[MAX_PATH]{};
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {
            if (controls[i].Type == "Combo")
                DialogFunction(controls[i].Title, "", controls[i].Index);
            else if (controls[i].Type == "Slider")
                DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
            else if (controls[i].Type == "Edit") {
                if (m_hDialog) {
                    GetWindowTextA(controls[i].hwndControl, (LPSTR)tmp, MAX_PATH);
                    controls[i].Text = tmp;
                }
                DialogFunction(controls[i].Title, controls[i].Text, true);
            }
            else
                DialogFunction(controls[i].Title, "", controls[i].Val);
        }
    }
}

// Restore controls with old values
// ofApp calls GetControls to get the updated values
void ofxWinDialog::Restore()
{
    // Restore old controls
    controls = oldcontrols;
    // Refresh dialog display
    Refresh();
}

// Refresh the dialog controls with new values
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
            if (controls[i].Index > 0) {
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
// ofApp calls GetControls to get the updated values
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
            else if (controls[i].Type == "Checkbox") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
                    if (tmp[0]) controls[i].Val = atoi(tmp);
                }
            }
        }
    }

    // Refresh the dialog with the new controls
    Refresh();

}

// Dialog window icon
void ofxWinDialog::SetIcon(HICON hIcon)
{
    m_hIcon = hIcon;
}

// Dialog position and size are set before opening the window
void ofxWinDialog::SetPosition(int x, int y, int width, int height)
{
    dialogX = x;
    dialogY = y;
    dialogWidth = width;
    dialogHeight = height;
}

// Create dialog window
// Dialog position and size must have been set by SetPosition
HWND ofxWinDialog::Open(std::string title)
{
    // Safety
    if (dialogWidth == 0 || dialogHeight == 0)
        return NULL;

    // Parent window size
    RECT rect{};
    GetWindowRect(m_hwnd, &rect);
    int rwidth  = rect.right - rect.left;
    int rheight = rect.bottom - rect.top;

    int xpos = dialogX;
    int ypos = dialogY;
    //  o If x and y are both positive, that position is used
    //	o If x and y are both zero, centre on the host window
    //  o if y is zero, offset from the centre by the x amount
    //  o If x and y are both negative, centre on the desktop
    //  o if x is CW_USEDEFAULT the system selects the default x and ignores y 
    if (dialogX == 0 && dialogY == 0) {
        // Centre on the host window
        xpos = rect.left + rwidth/2  - dialogWidth/2;
        ypos = rect.top  + rheight/2 - dialogHeight/2;
    }
    else if (dialogX < 0 && dialogY < 0) {
        xpos = (GetSystemMetrics(SM_CXSCREEN)-dialogWidth)/2;
        ypos = (GetSystemMetrics(SM_CYSCREEN)-dialogHeight)/2;
    }
    else if (dialogY == 0 && dialogX != CW_USEDEFAULT) {
        // If y is zero, offset left or right 
        // from the left side of the host window
        ypos = rect.top  + rheight/2 - dialogHeight/2;
        xpos = rect.left + rwidth/2  - dialogWidth/2;
        ypos = rect.top  + rheight/2 - dialogHeight/2;
        xpos = rect.left + dialogX;
        if (xpos < 0) xpos = 0;
    }

    // Title is wide chars
    wchar_t wtitle[MAX_PATH]{};
    mbstowcs_s(NULL, wtitle, title.c_str(), MAX_PATH);

    // No minimize, maximize or menu
    // WS_CAPTION | WS_SYSMENU gives a close button and icon
    HWND hwnd = CreateWindowW(m_ClassName.c_str(), wtitle,
        WS_CAPTION | WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU,
        xpos, ypos, dialogWidth, dialogHeight,
        m_hwnd,      // Parent window
        NULL,        // No menu
        m_hInstance, // Parent instance
        NULL);

    if (!hwnd) {
        DWORD dwError = GetLastError();
        std::string str = "Window creation failed : " + std::to_string(dwError);
        MessageBoxA(NULL, str.c_str(), "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

     // Store the instance pointer in the window's user data
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

    // Save static window handle for GetMsgProc
    hwndDialog = hwnd;

    // Class window handle
    m_hDialog = hwnd;

    // Set focus back to the parent window
    SetFocus(m_hwnd);

    // Dialog window icon if specified
    if (m_hIcon) {
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
    }

    // Clear all window handles for repeat open of the same dialog
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
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON;
            // The first radio button in the group has WS_GROUP style
            if (controls[i].First == 1)
                dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP;
            // Style can also be BS_RIGHTBUTTON - default is a button to the left of the text
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;
            else
                dwStyle |= BS_LEFT;

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
            if (controls[i].Tick > 0.0f)
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
                if (controls[i].Tick > 0.0f)
                    SendMessage(hwndc, TBM_SETTICFREQ, (int)(controls[i].Tick*100.0f), 0);

                // Disable visual themes for trackbar
                DisableTheme(hwnd, hwndc);

                // Slider value text display
                if (controls[i].Index > 0) {
                    // Create a static text control to display the value of the slider
                    HWND hwndval = CreateWindowExA(
                        0, "STATIC", "0", WS_VISIBLE | WS_CHILD | SS_RIGHT, // right aligned
                        controls[i].X + controls[i].Width, controls[i].Y,
                        40, controls[i].Height, hwnd, NULL, m_hInstance, NULL);
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
        }

        //
        // Edit control
        //
        if (controls[i].Type == "Edit") {
            // Text alignment can be ES_LEFT, ES_RIGHT or ES_CENTER. Default is ES_LEFT.
            // Outline can be WS_BORDER, WS_DLGFRAME
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | controls[i].Style;
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
                        // Item string is wide chars for unicode and multi-byte
                        wchar_t itemstr[MAX_PATH]{};
                        mbstowcs_s(NULL, itemstr, controls[i].Items[j].c_str(), MAX_PATH);
                        SendMessageW(hwndc, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)itemstr);
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
            // The button control style can be specified as default
            // BS_DEFPUSHBUTTON (1) default style is BS_PUSHBUTTON (0)
            // For example the OK button is usually the default.
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;

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
            hwndc = CreateWindowExA(0, "BUTTON", controls[i].Text.c_str(),
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

            hwndc = CreateWindowExA(0, "STATIC", controls[i].Text.c_str(),
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
             if (controls[i].hwndSliderVal)
                 SendMessage(controls[i].hwndSliderVal, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));
         }
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    // Set focus back to the parent window
    SetFocus(m_hwnd);

    return hwnd;

}

// Close the dialog window
void ofxWinDialog::Close()
{
    if (m_hDialog && IsWindow(m_hDialog)) {
        // Bring the app window to the top
        SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        PostMessage(m_hDialog, WM_CLOSE, 0, 0);
    }
}

// ofApp callback function for return of control values
void ofxWinDialog::AppDialogFunction(void(ofApp::* function)(std::string title, std::string text, int value))
{
    pAppDialogFunction = function;
}

// Pass back the dialog item title and state to ofApp
void ofxWinDialog::DialogFunction(std::string title, std::string text, int value)
{
    if(pApp && pAppDialogFunction)
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

// ---------------------------------------------
// Disable Visual Style themes for dialog controls
// if using common controls version 6.0.0.0
// All controls if hwndControl is not specified
void ofxWinDialog::DisableTheme(HWND hwndDialog, HWND hwndControl)
{
    if (hwndControl) {
        SetWindowTheme(hwndControl, L"", L"");
    }
    else {
        HWND hwndChild = GetWindow(hwndDialog, GW_CHILD);
        while (hwndChild != NULL)
        {
            SetWindowTheme(hwndChild, L"", L"");
            hwndChild = GetWindow(hwndChild, GW_HWNDNEXT);
        }
    }
}


//
// Windows message callback function
//
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Retrieve the instance pointer (ofxWinDialog object)
    ofxWinDialog* pDlg = reinterpret_cast<ofxWinDialog*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
 
    // Pass messages on to the class message handling function
    if (pDlg) pDlg->WindowProc(hwnd, uMsg, wParam, lParam);

    // Default message handling
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//
// Class window message handling procedure for multiple dialogs
//
void ofxWinDialog::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPDRAWITEMSTRUCT lpdis ={};
    HCURSOR cursorHand = NULL;

    switch (msg) {

        case WM_DRAWITEM:
            // Received once only
            cursorHand=LoadCursor(NULL, IDC_HAND);
            // The blue hyperlink
            lpdis = (LPDRAWITEMSTRUCT)lParam;
            if (lpdis->itemID == -1) break;
            for (size_t i=0; i<controls.size(); i++) {
                // Index = 1 identifies the hyperlink
                if (controls[i].Type == "Static" && controls[i].Index == 1) {
                    if (LOWORD(wParam) == controls[i].ID) {
                        // Blue colour
                        // Title is the text displayed, control text is the action taken
                        SetTextColor(lpdis->hDC, RGB(40, 100, 190));
                        DrawTextA(lpdis->hDC, controls[i].Title.c_str(), -1, &lpdis->rcItem, DT_CENTER);
                        // Set a hand cursor
                        SetClassLongPtr(controls[i].hwndControl, GCLP_HCURSOR, (LONG_PTR)cursorHand);
                    }
                }
            }
            break;
        
        // Check For Windows system messages
        case WM_SYSCOMMAND:

            switch (wParam) {         // Check System Calls
                case SC_SCREENSAVE:   // Screensaver Trying To Start?
                case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
                    return;           // Prevent From Happening
            }
            break;

         case WM_COMMAND:

             // Handle control events and inform the app
             if (controls.size() > 0) {

                 // Hyperlink
                 for (size_t i=0; i<controls.size(); i++) {
                     if (controls[i].Type == "Static" && controls[i].Index == 1) {
                         if (LOWORD(wParam) == controls[i].ID) {
                             if(!controls[i].Text.empty()) {
                                // Title is the text displayed, control text is the action taken
                                ShellExecuteA(hwnd, "open", controls[i].Text.c_str(), NULL, NULL, SW_SHOWNORMAL);
                            }
                         }
                     }
                 }

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
                                 DialogFunction(controls[i].Title, "", index);
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
                                 DialogFunction(controls[i].Title, "", controls[i].Val);
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
                                                     DialogFunction(controls[k].Title, "", controls[k].Val);
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
                                 DialogFunction(controls[i].Title, "", 1);
                             }
                         } // end Push button
                     } // end all button controls

                     // IDOK and IDCANCEL are not used
                     // OK and Cancel buttons are handled by ofApp
                 }
             }
             break;

        case WM_HSCROLL:

            //
            // Sliders
            //
            if (controls.size() > 0) {
                for (size_t i=0; i<controls.size(); i++) {
                    if (controls[i].Type == "Slider") {
                        if ((HWND)lParam == controls[i].hwndControl) {

                            // Current position of the slider
                            int pos = (int)SendMessage(controls[i].hwndControl, TBM_GETPOS, 0, 0);

                            // Check for direction keys, left/right, up/down
                            // Default trackbar style is Down=Right and Up=Left (CommCtrl.h)
                            if (wParam == SB_LINELEFT) { // Left key
                                pos = pos - 100;
                                SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos);
                            }
                            else if(wParam == SB_LINERIGHT) { // Right key
                                pos = pos + 100;
                                int max = (int)(controls[i].Max*100.0);
                                if (pos > max) pos = max;
                                SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos);
                            }

                            controls[i].SliderVal = (float)pos/100.0f;

                            // Slider value text display
                            if (controls[i].hwndSliderVal) {
                                char tmp[8]{};
                                if (controls[i].Max >= 100.0f)
                                    sprintf_s(tmp, 8, "%.0f", controls[i].SliderVal);
                                else if (controls[i].Max >= 10.0f)
                                    sprintf_s(tmp, 8, "%.1f", controls[i].SliderVal);
                                else
                                    sprintf_s(tmp, 8, "%.2f", controls[i].SliderVal);
                                SetWindowTextA(controls[i].hwndSliderVal, (LPCSTR)tmp);
                            }

                            // If not one-click mode Inform ofApp of the slider position change
                            if (!bOneClick) {
                                DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                            }
                            else if (wParam == SB_ENDSCROLL) { // Mouse release or key up
                                DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                            }
                        }
                    }
                }
            }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            DestroyWindow(hwnd);
            DialogFunction("WM_DESTROY", "", PtrToUint(m_hDialog));
            m_hDialog = nullptr;
            break;
    }

    // Static callback handles defaults
}


//
// To enable the tab key - IsDialogMessage must be called
//
// This only works correctly when one dialog is open at a time
// which is typically the case.
//
// The tab key is disabled if a second dialog is opened due to
// conflict with static variables. It is restored if the dialog
// is closed and opened again.
//
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    LPMSG lpMsg = (LPMSG)lParam;
    if (nCode >= 0 && PM_REMOVE == wParam) {
        // Only translate key events
        if ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST)) {
            ofxWinDialog* pDlg = reinterpret_cast<ofxWinDialog*>(GetWindowLongPtr(hwndDialog, GWLP_USERDATA));
            if (pDlg && pDlg->m_hDialog) {
                if (IsDialogMessage(pDlg->m_hDialog, lpMsg)) {
                    // The value returned from this hookproc is ignored,
                    // and cannot be used to tell Windows the message has been handled.
                    // To avoid further processing, convert the message to WM_NULL.
                    lpMsg->message = WM_NULL;
                    lpMsg->lParam = 0;
                    lpMsg->wParam = 0;
                }
            }
        }
    }
    return CallNextHookEx(hMsgHook, nCode, wParam, lParam);
}
