//
// ofxWinDialog.cpp
//
#include "ofxWinDialog.h"
#include <windows.h>
#include <stdio.h>

// Static method to handle messages for dialog
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND m_hDialog = nullptr; // Dialog window handle
HWND hwndOKButton = NULL;  // OK Button
HWND hwndCancelButton = NULL;  // Cancel Button
WNDCLASS wndClass; // Dialog window class structure

//
// Control variables
//
struct ctl {
    std::string Type;  // Control type
    std::string Title; // Control title

    // Control size
    int X = 0;
    int Y = 0;
    int Width = 0;
    int Height = 0;

    std::string Text;  // Edit control text
    std::vector<std::string> Items; // Combo list items
    int Index = 0; // Combo list index
   
    // Slider
    float Min = 0; // Range min
    float Max = 0; // Range max
    float SliderVal = 0; // Slider value
    float Tick = 0; // Tick interval
    
    // Buttons
    int Val = 0; // Value
    int Group = 0; // Radio button group
    bool First = false; // First in group flag (see AddRadioGroup)

    uint64_t ID; // Control ID
    DWORD Style; // Static text and button style
    HWND hwndControl;
    HWND hwndType;
    HWND hwndSlider;
    HWND hwndSliderVal; // Static text control to display the value of the slider
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

/*
#ifdef UNICODE
    wndClass.lpfnWndProc = WindowProc; // Window procedure
    wndClass.lpszClassName = L"ofxWinDialogClass";
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW); // Control background color
    // Register the window class
    if (!RegisterClassW(&wndClass)) {
        MessageBoxA(NULL, "Dialog window class registration failed", "Error", MB_OK | MB_ICONERROR);
        return;
    }
#else
    wndClass.lpfnWndProc = WindowProc; // Window procedure
    wndClass.lpszClassName = "ofxWinDialogClass";
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW); // Control background color
    // Register the window class
    // if (!RegisterClassA(&wndClass)) {
    // LJ DEBUG
    // if (!RegisterClassA(&wndClass)) {
    if (!RegisterClass(&wndClass)) {
        MessageBoxA(NULL, "Dialog window class registration failed", "Error", MB_OK | MB_ICONERROR);
        return;
    }
#endif
*/

    // Initialize common controls for trackbar
    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);

}

ofxWinDialog::~ofxWinDialog() {
    // Close the dialog
    SendMessage(m_hDialog, WM_CLOSE, 0, 0);
    // Unregister the window class
    UnregisterClass(wndClass.lpszClassName, m_hInstance);

/*
#ifdef UNICODE
    UnregisterClassW(wndClass.lpszClassName, m_hInstance);
#else
    // LJ DEBUG
    // UnregisterClassA(wndClass.lpszClassName, m_hInstance);
    UnregisterClass(wndClass.lpszClassName, m_hInstance);
#endif
*/

}

void ofxWinDialog::SetFont(std::string name, LONG height, LONG weight)
{
    fontname = name;
    fontheight = height;
    fontweight = weight;
}


// Checkbox
void ofxWinDialog::AddCheckBox(std::string title, int x, int y, int width, int height, bool bChecked)
{
    ctl control{};
    control.Type = "Checkbox";
    control.Title = title;
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
void ofxWinDialog::AddRadioButton(std::string title, int x, int y, int width, int height, bool bChecked)
{
    ctl control{};
    control.Type = "Radio";
    control.Title = title;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    control.Val=(int)bChecked;
    control.Group=nRadioGroup; // Radio button group
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
void ofxWinDialog::AddEdit(std::string title, int x, int y, int width, int height, std::string text)
{
    ctl control{};
    control.Type = "Edit";
    control.Title = title;
    control.Text = text;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

//
// Combo box list control
//
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

// Group box
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

// Static text
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

// Set radio button
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
                SendMessageA(controls[i].hwndControl, (UINT)CB_SETCURSEL, (WPARAM)item, 0L);
                InvalidateRect(controls[i].hwndControl, NULL, TRUE);
            }
        }
    }
}


// Update all controls with current values
// Used by ofApp for an OK button
void ofxWinDialog::Update()
{
    // Retrieve text from the edit controls
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Edit") {
            char textBuffer[256]{};
            GetWindowTextA(controls[i].hwndControl, textBuffer, sizeof(textBuffer));
            controls[i].Text=textBuffer;
        }
    }
    // Inform ofApp of update
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
            else
                pThis->DialogFunction(controls[i].Title, "", controls[i].Val);
        }
    }
}


// Reset all controls with old values
// Used for a Cancel button
void ofxWinDialog::Reset()
{
    // Restore old controls
    controls = oldcontrols;

    // Refresh display and inform ofApp
    Refresh();

}

// Refresh all dialog controls with new values
// Used for Reset, Load from ini file and ofApp for the Reset button
void ofxWinDialog::Refresh()
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Checkbox" || controls[i].Type == "Radio") {
            if (controls[i].Val == 1)
                SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_CHECKED, 0);
            else
                SendMessage(controls[i].hwndControl, BM_SETCHECK, BST_UNCHECKED, 0);
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
                pThis->DialogFunction(controls[i].Title, "", controls[i].SliderVal*100.0f);
            else
                pThis->DialogFunction(controls[i].Title, "", controls[i].Val);
        }
    }

}


// Save controls to an initialization file
void ofxWinDialog::Save(std::string filename)
{
    // Replace extension if not ".ini"
    size_t pos = filename.rfind(".ini");
    if (pos == std::string::npos) {
        // Strip existing extension if any
        pos = filename.rfind(".");
        if (pos != std::string::npos && pos > filename.length()-5) {
            filename = filename.substr(0, pos);
        }
        // Add ".ini" extension
        filename = filename + ".ini";
    }

    // Initialization file full path - data directory
    std::string inipath = ofToDataPath(filename, true);

    // Save control values
    char tmp[MAX_PATH]={ 0 };
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {
            
            // printf("Controls[%d] type [%s]\n", (int)i, controls[i].Type.c_str());

            if (controls[i].Type == "Combo") {
                printf("Combo [%s] [%d]\n", controls[i].Title.c_str(), controls[i].Index);
                sprintf_s(tmp, MAX_PATH, "%d", controls[i].Index);
                WritePrivateProfileStringA((LPCSTR)"Combo", (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
            else if (controls[i].Type == "Edit") {
                printf("Edit [%s] [%s]\n", controls[i].Title.c_str(), controls[i].Text.c_str());
                WritePrivateProfileStringA((LPCSTR)"Edit", (LPCSTR)controls[i].Title.c_str(), (LPCSTR)controls[i].Text.c_str(), (LPCSTR)inipath.c_str());
            }
            else if (controls[i].Type == "Slider") {
                printf("Slider [%s] [%f]\n", controls[i].Title.c_str(), controls[i].SliderVal);
                sprintf_s(tmp, MAX_PATH, "%.2f", controls[i].SliderVal);
                WritePrivateProfileStringA((LPCSTR)"Slider", (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
            else {
                printf("%s [%s] [%d]\n", controls[i].Type.c_str(), controls[i].Title.c_str(), controls[i].Index);
                sprintf_s(tmp, MAX_PATH, "%d", controls[i].Val);
                WritePrivateProfileStringA((LPCSTR)controls[i].Type.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
        }
    }
}

// Load controls from an initialization file
void ofxWinDialog::Load(std::string filename)
{
    char tmp[MAX_PATH]={ 0 };

    // Replace extension if not ".ini"
    size_t pos = filename.rfind(".ini");
    if (pos == std::string::npos) {
        // Strip existing extension if any
        pos = filename.rfind(".");
        if (pos != std::string::npos && pos > filename.length()-5) {
            filename = filename.substr(0, pos);
        }
    }

    // Add ".ini" extension
    filename = filename + ".ini";

    // Initialization file full path - data directory
    std::string inipath = ofToDataPath(filename, true);

    // Check if file exists
    if (_access(inipath.c_str(), 0) == -1) {
        sprintf_s(tmp, "Initialization file does not exist\n [%s]", inipath.c_str());
        MessageBoxA(NULL, tmp, "Warning", MB_OK | MB_TOPMOST);
        return;
    }

    // Load control values
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type    != "Static"
            && controls[i].Type != "Group"
            && controls[i].Type != "Button"
            && controls[i].Type != "OK"
            && controls[i].Type != "CANCEL") {

            if (controls[i].Type == "Combo") {
                GetPrivateProfileStringA((LPCSTR)"Combo", (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str());
                if (tmp[0]) controls[i].Index = atoi(tmp);
            }
            else if (controls[i].Type == "Edit") {
                GetPrivateProfileStringA((LPCSTR)"Combo", (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str());
                if (tmp[0]) controls[i].Text = tmp;
            }
            else if (controls[i].Type == "Slider") {
                GetPrivateProfileStringA((LPCSTR)"Slider", (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str());
                if (tmp[0]) controls[i].SliderVal = atof(tmp);
            }
            else {
                GetPrivateProfileStringA((LPCSTR)controls[i].Type.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str());
                if (tmp[0]) controls[i].Val = atoi(tmp);
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
        return 0;

    int xpos = x;
    int ypos = y;
    if (y == 0) {
        // Centre on the host window
        RECT rect{};
        GetWindowRect(m_hwnd, &rect);
        int rw = rect.right - rect.left;
        int rh = rect.bottom - rect.top;
        xpos = rect.left + rw/2 - width/2;
        ypos = rect.top  + rh/2 - height/2;
        // Offset left or right from the left side of the host window
        if (x != 0) {
            xpos = rect.left + x;
        }
    }

    // Convert title to wide chars if Unicode
#ifdef UNICODE
    /*
    std::wstring strTitle;
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, windowTitle, (int)strlen(windowTitle), NULL, 0);
    strTitle.resize(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, windowTitle, (int)strlen(windowTitle), &strTitle[0], size_needed);
    */

    wchar_t title[MAX_PATH]{};
    // MultiByteToWideChar(CP_UTF8, 0, windowTitle, (int)strlen(windowTitle), title, size_needed);
    
    // mbstate_t ps{}; // initial state 
    // mbsrtowcs_s(NULL, title, &windowTitle, MAX_PATH, &ps);

    mbstowcs_s(NULL, title, windowTitle, MAX_PATH);

#else
    // std::string strTitle = windowTitle;
    char title[MAX_PATH]{};
    strcpy_s(title, MAX_PATH, windowTitle);
#endif

    // No minimize, maximize or menu
    // Allow for custom font
    DWORD dwStyle = DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU;
    if (!fontname.empty()) dwStyle |= DS_SETFONT;
    else dwStyle |= DS_SHELLFONT;
    HWND hwnd = CreateWindow(wndClass.lpszClassName, title, dwStyle,
        xpos, ypos, width, height,
        m_hwnd,      // Parent window
        NULL,        // No menu
        m_hInstance, // Parent instance
        NULL);

/*
#ifdef UNICODE
    // Convert title to wide chars to avoid incorrect title font
    std::wstring wstrTitle;
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, windowTitle, (int)strlen(windowTitle), NULL, 0);
    wstrTitle.resize(size_needed);
    MultiByteToWideChar(CP_UTF8, 0, windowTitle, (int)strlen(windowTitle), &wstrTitle[0], size_needed);
    // No minimize or maximize menu items
    // Allow for custom font
    DWORD dwStyle = DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU;
    if (!fontname.empty()) dwStyle |= DS_SETFONT;
    else dwStyle |= DS_SHELLFONT;
    HWND hwnd = CreateWindow(wndClass.lpszClassName, strTitle.c_str(), dwStyle,
        xpos, ypos, width, height,
        m_hwnd,      // Parent window
        NULL,        // No menu
        m_hInstance, // Parent instance
        NULL);
#else
    DWORD dwStyle = DS_3DLOOK | DS_CENTER | DS_MODALFRAME | WS_CAPTION | WS_VISIBLE | WS_POPUP | WS_SYSMENU;
    if (!fontname.empty()) dwStyle |= DS_SETFONT;
    else dwStyle |= DS_SHELLFONT;
    // LJ DEBUG
    // HWND hwnd = CreateWindowA(wndClass.lpszClassName, (LPCSTR)windowTitle, dwStyle,
    HWND hwnd = CreateWindow(wndClass.lpszClassName, strTitle.c_str(), dwStyle,
        xpos, ypos, width, height,
        m_hwnd,      // Parent window
        NULL,        // No menu
        m_hInstance, // Parent instance
        NULL);
#endif
*/

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

            hwndc = CreateWindowExA(0, "BUTTON", controls[i].Title.c_str(),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd,        // Parent window
                (HMENU)ID,   // Control ID
                m_hInstance, // Parent instance handle
                NULL);

            if (hwndc) {
                // Initial checkbox state
                SendMessageA(hwndc, BM_SETCHECK, controls[i].Val, 0);
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

            DWORD dwStyle = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON;
            if (controls[i].First == 1)
                dwStyle = WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP;

            hwndc = CreateWindowExA(0, "BUTTON", controls[i].Title.c_str(), dwStyle,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                // Initial state
                SendMessageA(hwndc, BM_SETCHECK, controls[i].Val, 0);
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
                    35, controls[i].Height, hwnd, NULL, m_hInstance, NULL);

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

            hwndc = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", controls[i].Text.c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
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
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                // Add combo box items
                if (controls[i].Items.size() > 0) {
                    for (size_t j = 0; j<controls[i].Items.size(); j++) {
                        SendMessageA(hwndc, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)controls[i].Items[j].c_str());
                    }
                }
                // Display an initial item in the selection field
                SendMessageA(hwndc, CB_SETCURSEL, (WPARAM)controls[i].Index, (LPARAM)0);
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
            DWORD dwStyle = WS_TABSTOP | WS_VISIBLE | WS_CHILD;
            // The button control style can be sepcified
            // OK button is BS_DEFPUSHBUTTON
            if (controls[i].Style > 0)
                dwStyle |= controls[i].Style;
            else
                dwStyle |= BS_PUSHBUTTON;
            hwndc = CreateWindowExA(0, "BUTTON", controls[i].Text.c_str(),
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
        SendMessageA(hwnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));
        for (size_t i=0; i<controls.size(); i++) {
            SendMessageA(controls[i].hwndControl, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));
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
// Local window message callback procedure
//
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    
    switch (msg) {

        
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
                        int index = (int)SendMessageA(controls[i].hwndControl, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                        if (index != CB_ERR) pThis->DialogFunction(controls[i].Title, "", index);
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
                            if (SendMessageA((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED)
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
                            if (j == controls[i].Group) {
                                int selectedGroup   = -1;
                                int selectedControl = -1;
                                // ID of the radio button selected
                                if (LOWORD(wParam) == controls[i].ID) {
                                    selectedGroup = j;
                                    if (SendMessageA((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                                        selectedControl = (int)i;
                                    }
                                }
                                if (selectedControl >= 0) {
                                    // Look though all the radio buttons and set the selected one
                                    // Others in the same group are set to zero
                                    for (size_t k=0; k<controls.size(); k++) {
                                        if (controls[k].Type == "Radio") {
                                            if (selectedGroup == controls[k].Group) {
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
                        if (LOWORD(wParam) == controls[i].ID) { // ID of the checkbox selected
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
                        int pos = (int)SendMessageA(controls[i].hwndControl, TBM_GETPOS, 0, 0);
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
            DestroyWindow(hwnd);
            m_hDialog = NULL;
            // Inform ofApp
            pThis->DialogFunction("WM_DESTROY", "", true);
            break;
        // Handle other messages here if needed
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
