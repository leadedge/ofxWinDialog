//
// ofxWinDialog.h
//
#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <io.h>
#include <Shlwapi.h> // For path functions
#include <uxtheme.h> // To disable visual style themes
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Shlwapi.Lib")
#pragma comment(lib, "UxTheme.lib")

// To detect mouse button up for trackbar controls
LRESULT CALLBACK GetMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
static HHOOK hMouseHook = NULL;
static int iBar = 0; // Trackbar selected

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
static HHOOK hMsgHook = NULL;

class ofApp; // Forward declaration

// Pointer to access this class from the window procedure
// ofxWinDialog* pThis;


//
 // Control variables
 //
// LJ DEBUG
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


class ofxWinDialog {

public:

    // ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd);
    ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd, std::wstring wClass=L"");

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
    // Add controls to the dialog
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
    
    //
    // Static text
    // Style can be :
    //  SS_LEFT   - left aligned
    //	SS_CENTER - centered
    //	SS_RIGHT  - right aligned
    //	WS_BORDER - outlined
    //	SS_SUNKEN - sunken edge
    // Default is left aligned (SS_LEFT)
    void AddText(std::string title, int x, int y, int width, int height, DWORD dwStyle = 0);

    // Hyperlink
    void AddUrl(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle = 0);
    
    // Static group box
    void AddGroup(std::string title, int x, int y, int width, int height);

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

    // Controls and old controls
    std::vector<struct ctl> controls;
    std::vector<struct ctl> oldcontrols;

    LRESULT WProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        LPDRAWITEMSTRUCT lpdis ={};
        HCURSOR cursorHand = NULL;

        switch (msg) {

        case WM_CREATE:
            // LJ DEBUG
            // hMsgHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId());
            // hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, GetMouseProc, NULL, 0);
            break;

        case WM_DRAWITEM:
            // Received once only
            cursorHand=LoadCursor(NULL, IDC_HAND);
            // The blue hyperlink
            lpdis = (LPDRAWITEMSTRUCT)lParam;
            if (lpdis->itemID == -1) break;
            for (size_t i=0; i<controls.size(); i++) {
                if (controls[i].Type == "Static" && controls[i].Index == 1) {
                    if (LOWORD(wParam) == controls[i].ID) {
                        // Blue colour
                        SetTextColor(lpdis->hDC, RGB(40, 100, 190)); // 50, 102, 204));
                        DrawTextA(lpdis->hDC, controls[i].Text.c_str(), -1, &lpdis->rcItem, DT_CENTER);
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
                return 0;             // Prevent From Happening
            }
            break;

        case WM_COMMAND:

            // Handle control events and inform the app
            if (controls.size() > 0) {

                // Hyperlink
                for (size_t i=0; i<controls.size(); i++) {
                    if (controls[i].Type == "Static" && controls[i].Index == 1) {
                        if (LOWORD(wParam) == controls[i].ID) {
                            // LJ DEBUG ::ShellExecuteA(hwnd, "open", controls[i].Text.c_str(), NULL, NULL, SW_SHOWNORMAL);
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
                            // for (int j=0; j<=pThis->nRadioGroup; j++) {
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

                            // Save trackbar control number for the mouse hook
                            iBar = (int)i;

                            // If not one-click mode Inform ofApp of the slider position change
                            // LJ DEBUG
                            // if (!pThis->bOneClick) {
                                // pThis->DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                            // }
                            if (bOneClick) {
                               DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                            }
                        }
                    }
                }
            }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            // Release hooks
            // LJ DEBUG
            if (hMsgHook) UnhookWindowsHookEx(hMsgHook);
            // if (hMouseHook) UnhookWindowsHookEx(hMouseHook);
            DestroyWindow(hwnd);
            m_hDialog = NULL;
            // Inform ofApp
            DialogFunction("WM_DESTROY", "", true);
            break;
            // Handle other messages here if needed
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT WProc2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        LPDRAWITEMSTRUCT lpdis ={};
        HCURSOR cursorHand = NULL;

        switch (msg) {

        case WM_CREATE:
            // LJ DEBUG
            // hMsgHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId());
            // hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, GetMouseProc, NULL, 0);
            break;

        case WM_DRAWITEM:
            // Received once only
            cursorHand=LoadCursor(NULL, IDC_HAND);
            // The blue hyperlink
            lpdis = (LPDRAWITEMSTRUCT)lParam;
            if (lpdis->itemID == -1) break;
            for (size_t i=0; i<controls.size(); i++) {
                if (controls[i].Type == "Static" && controls[i].Index == 1) {
                    if (LOWORD(wParam) == controls[i].ID) {
                        // Blue colour
                        SetTextColor(lpdis->hDC, RGB(40, 100, 190)); // 50, 102, 204));
                        DrawTextA(lpdis->hDC, controls[i].Text.c_str(), -1, &lpdis->rcItem, DT_CENTER);
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
                return 0;             // Prevent From Happening
            }
            break;

        case WM_COMMAND:

            // Handle control events and inform the app
            if (controls.size() > 0) {

                // Hyperlink
                for (size_t i=0; i<controls.size(); i++) {
                    if (controls[i].Type == "Static" && controls[i].Index == 1) {
                        if (LOWORD(wParam) == controls[i].ID) {
                            // LJ DEBUG ::ShellExecuteA(hwnd, "open", controls[i].Text.c_str(), NULL, NULL, SW_SHOWNORMAL);
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
                            // for (int j=0; j<=pThis->nRadioGroup; j++) {
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

                            // Save trackbar control number for the mouse hook
                            iBar = (int)i;

                            // If not one-click mode Inform ofApp of the slider position change
                            // LJ DEBUG
                            // if (!pThis->bOneClick) {
                                // pThis->DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                            // }
                            if (bOneClick) {
                                DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
                            }
                        }
                    }
                }
            }
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            // Release hooks
            // LJ DEBUG
            if (hMsgHook) UnhookWindowsHookEx(hMsgHook);
            // if (hMouseHook) UnhookWindowsHookEx(hMouseHook);
            DestroyWindow(hwnd);
            m_hDialog = NULL;
            // Inform ofApp
            DialogFunction("WM_DESTROY", "", true);
            break;
            // Handle other messages here if needed
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

// private:

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
