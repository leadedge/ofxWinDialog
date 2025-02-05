//
// ofxWinDialog.cpp
//
// Revisions :
//      01.01.25 - Correct missing Radio button in Load
//               - Skip *100 for control values with range > 1000
//               - SB_LINELEFT / SB_LINERIGHT
//                 Move 100 units for trackbars with range > 1000 
//      02.01.25 - Remove disable themes for trackbar
//               - Add function to disable themes for a control type or a specific control
//      03.01.25 - Save original controls
//               - Add "Reset()" function
//      04.01.25 - newcontrols - refresh slider text entry flags and handles
//               - DisableTheme - disable Visual Style themes at control creation
//                 all controls, control type, specific control
//               - Create full and basic examples
//      07.01.25 - Main window create without WS_VISIBLE
//                 ShowWindow when all controls have been created
//                 Add GetIcon
//                 Add WM_KEYDOWN and WM_KEYUP to inform ofApp of 
//                 key press and release when the dialog has focus
//      08.01.25 - Move Add WM_KEYDOWN and WM_KEYUP to GetMsgProc
//                 Correct trackbar SB_LINELEFT / SB_LINERIGHT for range
//      09.01.25 - Add style to AddHyperlink
//                 Close dialog for hyperlink
//      10.01.25 - Load - set newcontrols first if Load is before Open
//                 Close dialog for Reset
//                 Change Load from void to bool
//      11.01.25 - Reset newcontrols ID in Open for new dialog
//                 Remove GetControls() from load. Called by ofApp after Load.
//		18.01.25 - Resolve title errors for Unicode/Multibyte build
//				   Constructor : use std::string instead of std::wstring
//				     Class name conditional on #ifdef UNICODE
//				     WNDCLASS structure local
//				   Open : Replace CreateWindowW with CreateWindow
//				   Rename GetMsgProc to GetKeyMsgProc to avoid naming conflicts
//				   Save - full path on overwrite warn.
//		22.01.25 - SetFont - Specify font height in dialog units.
//				   Convert dialog units to logical units based on screen dpi.
//				   Add GetFontHeight() function.
//		23.01.25 - Add Spin control. Update full example.
//				   Change class callback function from void to LRESULT
//		24.01.25 - Add List box. Add GetListItem, GetListText.
//				   Enable combo box edit entry. Add GetComboEdit.
//				   Remove unused GetSpin from header.
//				   Update help dialog for combo edit, spin control and list box.
//				   Update full example.
//		26.01.25 - Add static text colour function - TextColor
//				   Add global variable for static text - g_TextColor
//				   Change SetCombo to SetComboItem
//				   Change GetComboText to GetComboEdit
//				   Change GetComboItem to get item and text.
//				   Change Setlist from item to all items.
//				   Add SetlistItem 
//				   Change GetListItem to get item and text.
//				   List selection, return both index and text to ofApp
//				   Move Windows system messages from WindowProc to MainWndProc
//		28.01.25 - Change TextColor to SetStaticColor
//				   Update spin control position in WM_NOTIFY
//				   Change TextColor to SetStaticColor
//		29.01.25 - Add static text control with a title that can be updated
//				   Remove SetFocus to parent window after Open
//				   AddText - Items string vector to RGB value of text colour
//				   WM_CTCOLORSTATIC - set static text colour from items vector
//				   Update spin control position in WM_NOTIFY
//				   Add SetButton function
//		30.01.25 - Add EnableControl function
//		31.01.25 - Add GetDialogWindow function
//		01.02.25 - Correct SetSpin. Was "List" instead of "Spin"
//		02.02.25 = Add style option for AddCombo (CBS_DROPDOWN allows user entry)
//		03.02.25 - Add picture buttons.
//				   Add - ButtonPicture(std::string path)
//				   Add button text colour to items vector
//				   Add stb_image.h to libs folder
//		04.02.25 - Change SetStaticColor back to TextColor. Add overloads.
//				      void TextColor(int hexcode);
//				      void TextColor(int red, int grn, int blu);
//				      void TextColor(COLORREF rgb);
//				   Set control text colour from control.item vector to control.Index
//				   Add utilities : Rgb2Hex and Hex2Rgb
//				   Add button colour function and overloads.
//				      void ButtonColor(int hexcode);
//				      void ButtonColor(int red, int grn, int blu);
//				      void ButtonColor(COLORREF rgb);
//				   Set control background colour to control.Val
//		04.02.25 - Change SetButton to ButtonText
//				   ButtonText - set control text for owner draw
//				   Add Rgb2Hex(COLORREF col) overload
//				   Owner draw button - blue border and dark grey text when pressed
//		05.02.25 - Darker blue hyperlink colour
//				   Group caption text colour (use SetColor)
//				   Correct font in DrawText in WM_DRAW
//
#include "ofxWinDialog.h"
#include <windows.h>
#include <stdio.h>

// To load bmp, jpg, png, tga
// Must be in the cpp file, not the header
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"

// Main Windows message procedure that forwards
// messages to the instance's message handler
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Message hook to enable the tab key
// https://learn.microsoft.com/en-us/windows/win32/winmsg/getmsgproc
LRESULT CALLBACK GetKeyMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
static HHOOK hMsgHook = NULL;

// Static window handle for GetKeyMsgProc
static HWND hwndDialog = NULL;

ofxWinDialog::ofxWinDialog(ofApp* app, HINSTANCE hInstance, HWND hWnd, std::string className)
{
	m_hInstance = hInstance;
	m_hwnd = hWnd;

	pApp = app; // The ofApp class pointer

	// ofApp callback function for return of control values
	// Set by AppDialogFunction
	pAppDialogFunction = nullptr;

	WNDCLASS wndClass{0};

	// Main Windows message handling procedure
	// to forward messages to the class message procedure
	wndClass.lpfnWndProc = MainWndProc;

	// Window class name for mutltiple dialogs
	#ifdef UNICODE
	if (!className.empty())
		mbstowcs_s(NULL, m_ClassName, className.c_str(), MAX_PATH);
	else
		wcscpy_s(m_ClassName, MAX_LOADSTRING, L"ofxWinDialogClass");
	#else
		if (!className.empty())
			// WideCharToMultiByte(CP_ACP, 0, className.c_str(), -1, m_ClassName, MAX_LOADSTRING, NULL, NULL);
			strcpy_s(m_ClassName, MAX_LOADSTRING, className.c_str());
		else
			strcpy_s(m_ClassName, MAX_LOADSTRING, "ofxWinDialogClass");
	#endif

	wndClass.lpszClassName = m_ClassName;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW); // Control background color

    // Register the window class
    if (!RegisterClass(&wndClass)) {
        MessageBoxA(NULL, "Dialog window class registration failed", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Create the message hook to enable the tab key
    if (!hMsgHook) {
        hMsgHook = SetWindowsHookEx(WH_GETMESSAGE, GetKeyMsgProc, NULL, GetCurrentThreadId());
    }
}


ofxWinDialog::~ofxWinDialog() {
    // Close the dialog window
    if(m_hDialog) SendMessage(m_hDialog, WM_CLOSE, 0, 0);
    // Unregister the window class
    UnregisterClass(m_ClassName, m_hInstance);
    // Release message hook
    if (hMsgHook) UnhookWindowsHookEx(hMsgHook);
}

// Set a custom font
// height in dialog units
void ofxWinDialog::SetFont(std::string name, LONG height, LONG weight)
{
    fontname = name;
    fontheight = height;
    fontweight = weight;
}

// Return the font height set
LONG ofxWinDialog::GetFontHeight() {
	return fontheight;
}

// Return the logical font handle
HFONT ofxWinDialog::GetFont() {
	return g_hFont;
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
    control.Index = bShow; // flag to show value text to the right
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

//
// Combo box list control
// Style CBS_DROPDOWN allows user entry
// Default is CBS_DROPDOWNLIST which prevents user entry
void ofxWinDialog::AddCombo(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Combo";
    control.Title = title;
    control.Items = items;
    control.Index = index;
	control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;
    controls.push_back(control);
}

// For testing
void ofxWinDialog::AddCombo(std::string title, int x, int y, int width, int height) {

	std::vector<std::string> items;
	items.push_back("Item 1");
	items.push_back("Item 2");
	items.push_back("Item 3");
	AddCombo(title, x, y, width, height, items, 0);
}

//
// List box control
//
void ofxWinDialog::AddList(std::string title, int x, int y, int width, int height, std::vector<std::string> items, int index) {
	ctl control {};
	control.Type = "List";
	control.Title = title;
	control.Items = items;
	control.Index = index;
	control.X = x;
	control.Y = y;
	control.Width = width;
	control.Height = height;
	controls.push_back(control);
}

//
// Spin control
//
// x, y, width, height - position and dimensions
// min, max, - range. value - starting value
// Style can be UDS_ALIGNLEFT or UDS_ALIGNRIGHT (default)
//
void ofxWinDialog::AddSpin(std::string title, int x, int y, int width, int height,
	int min, int max, int value, DWORD dwStyle)
{

	ctl control {};
	control.Type = "Spin";
	control.Title = title;
	control.Min = (float)min; // min value
	control.Max = (float)max; // max value
	control.Val = value; // initial value
	control.Style = dwStyle;
	control.X = x;
	control.Y = y;
	control.Width = width;
	control.Height = height;
	controls.push_back(control);
}

// Push button
// Text in the button is independent of the title
void ofxWinDialog::AddButton(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle) {
	ctl control {};
	control.Type = "Button";
	control.Title = title;
	control.Text = text;
	control.Style = dwStyle;
    control.X=x;
    control.Y=y;
    control.Width=width;
    control.Height=height;

	//
	// Custom values used in WM_DRAWITEM
	//

	// Picture button bitmap (ButtonPicture)
	if (g_hBitmap) {
		control.hwndType = (HWND)g_hBitmap;
		control.Style |= BS_OWNERDRAW;
		g_hBitmap = nullptr;
	}

	// Button background colour (ButtonColor)
	// Use control Val (default 0);
	if (g_ButtonColor != 0) {
		control.Val = Rgb2Hex(GetRValue(g_ButtonColor),
			GetGValue(g_ButtonColor), GetBValue(g_ButtonColor));
		control.Style |= BS_OWNERDRAW;
		g_ButtonColor = 0;
	}

	// Button text colour (TextColor)
	// Use control index (default 0)
	if (g_TextColor != 0) {
		control.Index = Rgb2Hex(GetRValue(g_TextColor),
			GetGValue(g_TextColor), GetBValue(g_TextColor));
		control.Style |= BS_OWNERDRAW;
		g_TextColor = 0;
	}

    controls.push_back(control);
}

// Change button text
void ofxWinDialog::ButtonText(std::string title, std::string text) {
	for (size_t i = 0; i < controls.size(); i++) {
		// Update the button text
		if (controls[i].Type == "Button") {
			if (controls[i].Title == title) {
				SetWindowTextA(controls[i].hwndControl, text.c_str());
				// Set control text for owner draw
				controls[i].Text = text;
			}
		}
	}
}

// Change button background color
// Set before AddButton
void ofxWinDialog::ButtonColor(int hexcode) {
	ButtonColor(Hex2Rgb(hexcode));
}
void ofxWinDialog::ButtonColor(int red, int grn, int blu) {
	ButtonColor(RGB(red, grn, blu));
}
void ofxWinDialog::ButtonColor(COLORREF rgb) {
	g_ButtonColor = rgb;
}

// Image file for picture button (bmp, jpg, png, tga)
// Call before AddButton
// TextColor can also be used to change the button text
void ofxWinDialog::ButtonPicture(std::string path)
{
	if (_access(path.c_str(), 0) != -1) {

		int width, height, nchannels;
		unsigned char* imageData = stbi_load(path.c_str(), &width, &height, &nchannels, 0);

		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height; // Negative to specify top-down bitmap
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24; // 24 bits for RGB (no alpha)
		bmi.bmiHeader.biCompression = BI_RGB;

		// DIB section
		void* bits = nullptr;
		g_hBitmap = CreateDIBSection(
			NULL, // default device context
			&bmi, // BITMAPINFO structure
			DIB_RGB_COLORS, // Colors are in RGB
			&bits, // Pointer to store the bits
			NULL, // No bitmap handle (using the direct memory buffer)
			0 // Unused
		);

		if (!g_hBitmap)
			return;

		// Copy the raw image data to the bitmap's bits buffer
		// Convert from RGB to the correct memory format for the DIB
		unsigned char * dst = (unsigned char *)bits;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				// Copy the RGB data (stb loads as RGB or RGBA)
				int srcIndex = (y * width + x) * nchannels;
				int dstIndex = (y * width + x)*3; // 3 bytes per pixel (RGB)
				dst[dstIndex] = imageData[srcIndex + 2]; // Red
				dst[dstIndex + 1] = imageData[srcIndex + 1]; // Green
				dst[dstIndex + 2] = imageData[srcIndex]; // Blue
			}
		}
		stbi_image_free(imageData);
	}
	else {
		g_hBitmap = nullptr;
	}

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

	// Group box caption text colour (TextColor)
	// Use control index (default 0);
	if (g_TextColor != 0) {
		control.Index = Rgb2Hex(GetRValue(g_TextColor),
			GetGValue(g_TextColor), GetBValue(g_TextColor));
		g_TextColor = 0;
	}

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


// Static text that is not a updated
void ofxWinDialog::AddText(std::string text, int x, int y, int width, int height, DWORD dwStyle) {
	AddText(text, text, x, y, width, height, dwStyle);
}

// Static text with an idependent title that can be updated with SetText
// If the text is empty, the title is used for text display
void ofxWinDialog::AddText(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle) {
	ctl control {};
	control.Type = "Static";
	control.Title = title;
	control.Text = text;
	control.Style = dwStyle;
	control.X = x;
	control.Y = y;
	control.Width = width;
	control.Height = height;

	// Static text colour
	// Use control index (default 0);
	if (g_TextColor != 0) {
		control.Index = Rgb2Hex(GetRValue(g_TextColor),
			GetGValue(g_TextColor), GetBValue(g_TextColor));
		g_TextColor = 0;
	}

	controls.push_back(control);
}

// Static text color
// Set before AddText
// Reference :
// https://www.html-color-codes.info/color-names/
// https://www.computerhope.com/htmcolor.htm
void ofxWinDialog::TextColor(int hexcode) {
	TextColor(Hex2Rgb(hexcode));
}
void ofxWinDialog::TextColor(int red, int grn, int blu) {
	TextColor(RGB(red, grn, blu));
}
void ofxWinDialog::TextColor(COLORREF rgb) {
	g_TextColor = rgb;
}


// Hyperlink
// Title is the text displayed, control text is the action taken
// If the text is empty, ofApp is notified when the hyperlink is clicked
void ofxWinDialog::AddHyperlink(std::string title, std::string text, int x, int y, int width, int height, DWORD dwStyle)
{
    ctl control{};
    control.Type = "Static";
    control.Title = title; // The title that appears in the dialog
    control.Text  = text; // The action to be taken.
    // Enable notifications for action and owner draw for text colour
    control.Style = SS_NOTIFY | SS_OWNERDRAW | dwStyle;
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

// Get current combo box item index and text
int ofxWinDialog::GetComboItem(std::string title, std::string * text) {
	int index = 0;
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "Combo") {
			if (controls[i].Title == title) {
				index = controls[i].Index;
				if (text) *text = controls[i].Items[index];
			}
		}
	}
	return index;
}

// Get combo box edit text
std::string ofxWinDialog::GetComboEdit(std::string title) {
	std::string str;
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "Combo") {
			if (controls[i].Title == title) {
				char tmp[256] {};
				int len = GetWindowTextA(controls[i].hwndControl, tmp, 256);
				if (len > 0) str = tmp;
			}
		}
	}
	// Return the current edit text
	return str;
}

// Get current list box item index and text
int ofxWinDialog::GetListItem(std::string title, std::string * text) {
	int index = 0;
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "List") {
			if (controls[i].Title == title) {
				index = controls[i].Index;
				if (text) *text = controls[i].Items[index];
			}
		}
	}
	return index;
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

// Enable or disable a control
// Except Hyperlink and Group
void ofxWinDialog::EnableControl(std::string title, bool bEnabled) {
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type    != "Hyperlink"
			&& controls[i].Type != "Group") {
			if (controls[i].Title == title) {
				HWND hwnd = controls[i].hwndControl;
				EnableWindow(hwnd, (BOOL)bEnabled);
				// Redraw immediately
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_INTERNALPAINT);
			}
		}
	}
}

// Set slider value
void ofxWinDialog::SetSlider(std::string title, float value)
{
    for (size_t i=0; i<controls.size(); i++) {
        if (controls[i].Type == "Slider") {
            if (controls[i].Title == title) {
                // Update the Slider value
                controls[i].SliderVal = value;
                if ((controls[i].Max - controls[i].Min) > 1000.0)
                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, (int)controls[i].SliderVal);
                else
                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, (int)(value*100.0f));
                InvalidateRect(controls[i].hwndControl, NULL, TRUE);

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

void ofxWinDialog::SetText(std::string title, std::string text) {
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "Static") {
			if (controls[i].Title == title) {
				SetWindowTextA(controls[i].hwndControl, (LPCSTR)text.c_str());
				controls[i].Text = text;
			}
		}
	}
}

// Set the current combo item
void ofxWinDialog::SetComboItem(std::string title, int item) {
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "Combo") {
			if (controls[i].Title == title && item < (int)controls[i].Items.size()) {
				// Make the item current
				controls[i].Index = item;
				SendMessage(controls[i].hwndControl, (UINT)CB_SETCURSEL, (WPARAM)item, 0L);
			}
		}
	}
}

// Reset the list items
void ofxWinDialog::SetList(std::string title, std::vector<std::string> items, int index) {
	// Addlist
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "List") {
			HWND hwndList = controls[i].hwndControl;
			SendMessageA(hwndList, LB_RESETCONTENT, 0, 0L);
			if (items.size() > 0) {
				for (size_t i = 0; i < items.size(); i++) {
					int pos = (int)SendMessageA(hwndList, LB_ADDSTRING, 0, (LPARAM)items[i].c_str());
					SendMessageA(hwndList, LB_SETITEMDATA, pos, (LPARAM)i);
				}
				// Highlight the current item
				SendMessageA(hwndList, LB_SETCURSEL, (WPARAM)index, 0L);
			}
		}
	}
}

// Set the current list item
void ofxWinDialog::SetListItem(std::string title, int item)
{
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "List") {
			if (controls[i].Title == title) {

				int listsize = (int)SendMessage(controls[i].hwndControl, (UINT)LB_GETCOUNT, (WPARAM)0, 0L);
				int current = (int)SendMessage(controls[i].hwndControl, (UINT)LB_GETCURSEL, (WPARAM)0, 0L);
				if (item < listsize) {
					// Make the item current
					controls[i].Index = item;
					SendMessage(controls[i].hwndControl, (UINT)LB_SETCURSEL, (WPARAM)item, 0L);
				}
			}
		}
	}
}

// Set spin control value
void ofxWinDialog::SetSpin(std::string title, int value) {
	for (size_t i = 0; i < controls.size(); i++) {
		if (controls[i].Type == "Spin") {
			if (controls[i].Title == title) {
				controls[i].Val = value;
				SendMessageA(controls[i].hwndControl, (UINT)UDM_SETPOS, 0, (LPARAM)controls[i].Val);
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
			if (controls[i].Type == "Combo" || controls[i].Type == "List") {
				DialogFunction(controls[i].Title, controls[i].Text, controls[i].Index);
            }
            else if (controls[i].Type == "Slider") {
                DialogFunction(controls[i].Title, "", (int)(controls[i].SliderVal*100.0f));
            }
            else if (controls[i].Type == "Edit") {
                if (m_hDialog) {
                    GetWindowTextA(controls[i].hwndControl, (LPSTR)tmp, MAX_PATH);
                    controls[i].Text = tmp;
                }
                DialogFunction(controls[i].Title, controls[i].Text, true);
            }
			else if (controls[i].Type == "Spin") {
				if (m_hDialog) {
					controls[i].Val = (int)SendMessage(controls[i].hwndControl, UDM_GETPOS, 0, 0);
					DialogFunction(controls[i].Title, "", controls[i].Val);
				}
			}
            else
                DialogFunction(controls[i].Title, "", controls[i].Val);
        }
    }
}

// Get the number of controls
// Can be used to find whether controls have been created
int ofxWinDialog::GetControlNumber()
{
    return static_cast<int>(controls.size());
}

// Get the dialog window handle
HWND ofxWinDialog::GetDialogWindow() {
	return m_hDialog;
}

// Reset controls with orignal values
// ofApp calls GetControls to get the updated values
// and closes the dialog.
void ofxWinDialog::Reset()
{
    // Reset controls
    if(!newcontrols.empty())
        controls = newcontrols;
    Refresh();
}

// Restore controls with old values
void ofxWinDialog::Restore()
{
    controls = oldcontrols;
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

            if ((controls[i].Max - controls[i].Min) > 1000.0)
                SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, (int)controls[i].SliderVal);
            else
                SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, (int)(controls[i].SliderVal*100.0f));
            
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
		if (controls[i].Type == "List") {
			if (!controls[i].Text.empty()) {
				// Delete the existing item text
				SendMessage(controls[i].hwndControl, LB_DELETESTRING, controls[i].Index, 0);
				// Add the new item text
				SendMessage(controls[i].hwndControl, LB_INSERTSTRING, controls[i].Index, (LPARAM)controls[i].Text.c_str());
			}
			// Make the new item current
			SendMessage(controls[i].hwndControl, (UINT)LB_SETCURSEL, (WPARAM)controls[i].Index, 0L);
		}
        if (controls[i].Type == "Edit") {
            SetWindowTextA(controls[i].hwndControl, (LPCSTR)controls[i].Text.c_str());
        }
		if (controls[i].Type == "Spin") {
			SendMessageA(controls[i].hwndControl, (UINT)UDM_SETPOS, 0, (LPARAM)controls[i].Val);
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
        sprintf_s(tmp, MAX_PATH, "%s\nexists - overwrite?", inipath.c_str());
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

            if (controls[i].Type == "Combo" || controls[i].Type == "List") {
                sprintf_s(tmp, MAX_PATH, "%d", controls[i].Index);
                WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
            }
            else if (controls[i].Type == "Edit") {
                WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)controls[i].Text.c_str(), (LPCSTR)inipath.c_str());
            }
			else if (controls[i].Type == "Spin") {
				sprintf_s(tmp, MAX_PATH, "%d", controls[i].Val);
				WritePrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), (LPCSTR)tmp, (LPCSTR)inipath.c_str());
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
bool ofxWinDialog::Load(std::string filename, std::string section)
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
            printf("ofxWinDialog::Load\n%s is not an initialization file\n", filename.c_str());
            return false;
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
        printf("ofxWinDialog::Load\nInitialization file \"%s\" not found.", inipath.c_str());
        return false;
    }

    // Set newcontrols first if Load is before Open
    // hwnd and ID are not set
    if (newcontrols.empty() && !controls.empty())
        newcontrols = controls;

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

            if (controls[i].Type == "Combo" || controls[i].Type == "List") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
                    if (tmp[0]) controls[i].Index = atoi(tmp);
                }
            }
            else if (controls[i].Type == "Edit") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str())> 0) {
                    if (tmp[0]) controls[i].Text = tmp;
                }
            }
			else if (controls[i].Type == "Spin") {
				if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
					if (tmp[0]) controls[i].Val = atoi(tmp);
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
            else if (controls[i].Type == "Radio") {
                if (GetPrivateProfileStringA((LPCSTR)ControlSection.c_str(), (LPCSTR)controls[i].Title.c_str(), NULL, (LPSTR)tmp, MAX_PATH, (LPCSTR)inipath.c_str()) > 0) {
                    if (tmp[0]) controls[i].Val = atoi(tmp);
                }
            }
        }
    }

    // Refresh the dialog with the new controls
    Refresh();

    return true;

}

// Set dialog window icon
void ofxWinDialog::SetIcon(HICON hIcon)
{
    m_hIcon = hIcon;
}

// Get dialog window icon handle if set
HICON ofxWinDialog::GetIcon()
{
    return m_hIcon;
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

	#ifdef UNICODE
	// Title is wide chars
	wchar_t titlechars[MAX_PATH]{};
	mbstowcs_s(NULL, titlechars, title.c_str(), MAX_PATH);
	#else
	char titlechars[MAX_PATH]{};
	strcpy_s(titlechars, MAX_PATH, title.c_str());
	#endif
   
    // No minimize, maximize or menu
    // WS_CAPTION | WS_SYSMENU gives a close button and icon
    // No WS_VISIBLE - ShowWindow when all controls have been created
	HWND hwnd = CreateWindow(m_ClassName, titlechars,
        WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU,
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
                if ((controls[i].Max - controls[i].Min) > 1000.0) {
                    SendMessage(hwndc, TBM_SETRANGE, TRUE, MAKELONG((int)(controls[i].Min), (int)(controls[i].Max)));
                    SendMessage(hwndc, TBM_SETPOS, TRUE, (int)(controls[i].SliderVal));
                }
                else {
                    SendMessage(hwndc, TBM_SETRANGE, TRUE, MAKELONG((int)(controls[i].Min*100.0f), (int)(controls[i].Max*100.0f)));
                    SendMessage(hwndc, TBM_SETPOS, TRUE, (int)(controls[i].SliderVal*100.0f));
                }

                // Set tick interval
                if (controls[i].Tick > 0.0f) {
                    if ((controls[i].Max - controls[i].Min) > 1000.0)
                        SendMessage(hwndc, TBM_SETTICFREQ, (int)(controls[i].Tick), 0);
                    else
                        SendMessage(hwndc, TBM_SETTICFREQ, (int)(controls[i].Tick*100.0f), 0);
                }

                // Slider value text display
                // Index is a flag to show value text to the right
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
                        // hwndSliderVal is only set if Index > 0
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
		// Spin control
		//
		// A spin control increments or decrements a value in
		// a buddy text window and immediately returns it to ofApp.
		if (controls[i].Type == "Spin") {

			// Create the static text buddy window
			// Text alignment can be SS_LEFT (default), SS_RIGHT or SS_CENTER.
			// Outline can be WS_BORDER, SS_SUNKEN
			DWORD dwStyle = 0;
			// Remove the spin control alignment styles
			if (controls[i].Style > 0) {
				dwStyle = controls[i].Style;
				dwStyle &= ~UDS_ALIGNLEFT;
				dwStyle &= ~UDS_ALIGNRIGHT;
				// Add the basic styles
				dwStyle |= (WS_CHILD | WS_VISIBLE | WS_CHILD);
			} else {
				dwStyle = WS_CHILD | WS_VISIBLE | WS_CHILD;
			}

			// Create the static text buddy control
			hwndc = CreateWindowExA(0, "STATIC", "0",
				dwStyle,
				controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
				hwnd, (HMENU)ID, m_hInstance, NULL);

			if (hwndc) {

				// Create the spin control (UPDOWN_CLASS)
				// Style can include UDS_ALIGNLEFT or UDS_ALIGNRIGHT (default)
				if (controls[i].Style > 0) {
					dwStyle = controls[i].Style;
					// Remove UDS_WRAP in case SS_CENTER has been specified
					// for the static text (the values are the same).
					dwStyle &= ~UDS_WRAP;
					// Isolate the UDS style
					if ((dwStyle & UDS_ALIGNLEFT) == UDS_ALIGNLEFT) {
						dwStyle |= UDS_ALIGNLEFT;
					} else {
						dwStyle |= UDS_ALIGNRIGHT;
					}
					dwStyle |= (WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_AUTOBUDDY);
				} else {
					dwStyle = WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_AUTOBUDDY | UDS_ALIGNRIGHT;
				}

				// Position left or right depending on the style
				// X position is connected to the buddy window
				// and depends on UDS_ALIGNLEFT or UDS_ALIGNLEFT
				hwndc = CreateWindowExA(0, UPDOWN_CLASSA, controls[i].Title.c_str(), dwStyle,
					// Set to zero to automatically size to fit the buddy window.
					// Position and size is determined by UDS_ALIGNLEFT or UDS_ALIGNRIGHT.
					0, 0, 0, 0,
					hwnd, (HMENU)ID, m_hInstance, NULL);

				if (hwndc) {
					// Set the range for the up-down control - min, max (integer)
					// The LOWORD of lParam is a short that specifies the maximum position
					// and the HIWORD is a short that specifies the minimum position.
					// MAKELPARAM(low, high)
					SendMessageA(hwndc, (UINT)UDM_SETRANGE, 0, MAKELPARAM(controls[i].Max, controls[i].Min));
					// Set a starting value
					SendMessageA(hwndc, (UINT)UDM_SETPOS, 0, (LPARAM)controls[i].Val);
					// The control hahdle
					controls[i].hwndControl = hwndc;
					controls[i].ID = ID;
					ID++;
				}
			}
		}


        //
        // Combo box list selection control
        //
        if (controls[i].Type == "Combo") {
			// Style CBS_DROPDOWN allows user entry
			// Default is CBS_DROPDOWNLIST which prevents user entry
			DWORD dwStyle = WS_TABSTOP | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
			if (controls[i].Style > 0)
				dwStyle |= controls[i].Style;
			else
				dwStyle |= CBS_DROPDOWNLIST;
            hwndc = CreateWindowExA(WS_EX_CLIENTEDGE, "COMBOBOX", controls[i].Title.c_str(),
                dwStyle, controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
                hwnd, (HMENU)ID, m_hInstance, NULL);

            if (hwndc) {
                // Add combo box items
				if (!controls[i].Items.empty() && controls[i].Items.size() > 0) {
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
		// List box control
		//
		if (controls[i].Type == "List") {
			
			hwndc = CreateWindowExA(WS_EX_CLIENTEDGE, "LISTBOX", controls[i].Title.c_str(),
				WS_TABSTOP | WS_HSCROLL | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
				controls[i].X, controls[i].Y, controls[i].Width, controls[i].Height,
				hwnd, (HMENU)ID, m_hInstance, NULL);

			if (hwndc) {
				// Add list box items
				if (!controls[i].Items.empty() && controls[i].Items.size() > 0) {
					for (size_t j = 0; j < controls[i].Items.size(); j++) {
						// Item string is wide chars for unicode and multi-byte
						wchar_t itemstr[MAX_PATH] {};
						mbstowcs_s(NULL, itemstr, controls[i].Items[j].c_str(), MAX_PATH);
						int pos = (int)SendMessageW(hwndc, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)itemstr);
						// Set the index associated with the item
						SendMessage(hwndc, LB_SETITEMDATA, pos, (LPARAM)j);
						// Retrieve the index with LB_GETITEMDATA
					}
				}

				// Highlight the current selection (controls[i].Index when added)
				SendMessage(hwndc, LB_SETCURSEL, (WPARAM)controls[i].Index, (LPARAM)0);

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


    
    // Disable Visual Styles if flag is set
    // DisableTheme(std::string type, std::string title)
    for (size_t i=0; i<controls.size(); i++) {
        if (!controls[i].VisualStyle) {
            SetWindowTheme(controls[i].hwndControl, L"", L"");
        }
    }
    

    //
    // Save all controls
    //

    // Original controls for reset
    if (newcontrols.empty()) {
        newcontrols = controls;
    }
    else {
        // Update window handles for a new dialog
        for (size_t i=0; i<controls.size(); i++) {
            // Control handles
            newcontrols[i].hwndControl = controls[i].hwndControl;
            // Control ID
            newcontrols[i].ID = controls[i].ID;
            // Slider value text display
            newcontrols[i].hwndSliderVal = controls[i].hwndSliderVal;
        }
    }

    // Old controls for restore (Cancel)
    oldcontrols = controls;

    // Custom dialog font
    if (!fontname.empty() && fontheight > 0) {

         LOGFONTA logFont{};
         logFont.lfWeight = fontweight;
		 logFont.lfHeight = fontheight;
         logFont.lfCharSet = ANSI_CHARSET;
         logFont.lfQuality = DEFAULT_QUALITY;
         strcpy_s(logFont.lfFaceName, 32, fontname.c_str());

		 //
		 // Font height is in dialog units
		 // Convert from dialog units to logical units and adjust for screen dpi.
		 // https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-logfonta
		 //
		 //
		 // Get the screen DPI
		 HDC hdc = GetDC(NULL);
		 int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		 ReleaseDC(NULL, hdc);

		 // Calculate the logical font height based on
		 // the screen dpi and number of dialog points per inch (72)
		 // A negative value ensures that the font scaling is
		 // dynamic and adjusts for the DPI.
		 logFont.lfHeight = -MulDiv(fontheight, dpi, 72);

         // Create the font
         HFONT hFont = CreateFontIndirectA(&logFont);

		 // Save the font handle to retrieve with GetFont
		 g_hFont = hFont;

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

	SetFocus(hwnd);

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
    if(places > 0)
        str = str.substr(0, str.find(".")+places+1);
    else
        str = str.substr(0, str.find("."));
    return str;
}

// ---------------------------------------------
// Disable Visual Style themes for dialog controls
// if using common controls version 6.0.0.0
// after dialog and control creation.
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

// -----------------------------------------------
// Disable Visual Style themes at control creation
//    o all controls
//    o a control type
//    o a specific control
void ofxWinDialog::DisableTheme(std::string type, std::string title)
{
    for (size_t i=0; i<controls.size(); i++) {
        if (!type.empty()) {
            if (controls[i].Type == type) {
                if (!title.empty()) {
                    if (controls[i].Title == title) {
                        controls[i].VisualStyle = false;
                    }
                }
                else {
                    controls[i].VisualStyle = false;
                }
            }
        }
        else {
            controls[i].VisualStyle = false;
        }
    }
}

//
// Utility
//
int ofxWinDialog::Rgb2Hex(COLORREF col) {
	return (GetRValue(col) << 16) | (GetGValue(col) << 8) | GetBValue(col);
}

int ofxWinDialog::Rgb2Hex(int r, int g, int b)
{
	return (r << 16) | (g << 8) | b;
}

COLORREF ofxWinDialog::Hex2Rgb(int hex, int* red, int* grn, int* blu)
{
	int r = (hex >> 16) & 0xFF;
	int g = (hex >> 8) & 0xFF;
	int b = hex & 0xFF;
	if (red != nullptr) {
		*red = (hex >> 16) & 0xFF;
		*grn = (hex >> 8) & 0xFF;
		*blu = hex & 0xFF;
	}
	return RGB(r, g, b);
}

//
// Windows message callback function
//
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Check For Windows system messages
	if(uMsg == WM_SYSCOMMAND) {
		switch (wParam) { // Check System Calls
			case SC_SCREENSAVE: // Screensaver Trying To Start?
			case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
				return TRUE; // Prevent From Happening
		}
	}

	if (uMsg == WM_ERASEBKGND) {
		if (lParam > 0) {
			SpoutMessageBox("WM_ERASEBKGND 1"); // - wParam = 0x%X, lParam = 0x%X\n", wParam, lParam);
		}
	}
    // Retrieve the instance pointer (ofxWinDialog object)
    ofxWinDialog* pDlg = reinterpret_cast<ofxWinDialog*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
 
    // Pass messages on to the class message handling function
    if (pDlg)
		return pDlg->WindowProc(hwnd, uMsg, wParam, lParam);

    // Default message handling
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//
// Class window message handling procedure for multiple dialogs
//
LRESULT ofxWinDialog::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LPDRAWITEMSTRUCT lpdis ={};
    HCURSOR cursorHand = NULL;

    switch (msg) {

		case WM_PAINT:
		{
			// Set the Group box caption colour here
			// because it is not static text
			for (size_t i = 0; i < controls.size(); i++) {
				if (controls[i].Type == "Group") {

					// Text colour for the group box caption
					// RGB is the hex value of the Index number (default 0)
					if (controls[i].Index > 0) {

						HWND hwndCtrl = controls[i].hwndControl;
						PAINTSTRUCT ps;
						HDC hdc = BeginPaint(hwndCtrl, &ps);
						HFONT hOldFont = nullptr;

						// Select the dialog font if created
						if(g_hFont)
							hOldFont = (HFONT)SelectObject(hdc, g_hFont);

						RECT rect;
						GetClientRect(hwndCtrl, &rect);

						// Text color for the caption
						COLORREF col = Hex2Rgb(controls[i].Index);
						SetTextColor(hdc, col);

						// Set the text background colour to
						// a background pixel inside the rectangle
						int x = rect.left   + 4;
						int y = rect.bottom - 4;
						col = GetPixel(hdc, x, y);
						SetBkColor(hdc, col);

						// Warm grey R=212, G=208, B=200.
						// COLOR_INACTIVECAPTION		: 191, 205, 219 ***
						// COLOR_INACTIVECAPTIONTEXT	:   0,   0,   0
						// COLOR_BACKGROUND				:   3, 131, 135
						// COLOR_MENU					: 240, 240, 240
						// COLOR_ACTIVEBORDER			: 180, 180, 180
						// COLOR_INACTIVEBORDER			: 244, 247, 252
						// CTLCOLOR_LISTBOX				: 153, 180, 209
						// CTLCOLOR_EDIT				:   3, 131, 135
						// CTLCOLOR_BTN					: 191, 205, 219 **
						// COLOR_BTNFACE				: 240, 240, 240
						// COLOR_BTNHIGHLIGHT			: 255, 255, 255
						// CTLCOLOR_DLG					: 240, 240, 240
						// CTLCOLOR_STATIC				: 100, 100, 100
						// CTLCOLOR_MSGBOX				: 200, 200, 200 **
						// COLOR_WINDOWFRAME			: 100, 100, 100
						// COLOR_GRAYTEXT				: 109, 109, 109
						// COLOR_HOTLIGHT				:   0, 102, 204 (hyperlink)

						// Grey border slightly darker than normal for better visibility
						// COLOR_INACTIVECAPTION	: 191, 205, 219
						// COLOR_ACTIVEBORDER		: 180, 180, 180
						// CTLCOLOR_MSGBOX			: 200, 200, 200

						HBRUSH hBrush = CreateSolidBrush(GetSysColor(CTLCOLOR_MSGBOX));
						
						// Move the top of the rect down to center the top border with the caption
						rect.top += 10;
						FrameRect(hdc, &rect, hBrush);
						DeleteObject(hBrush);

						// Text color for the caption
						col = Hex2Rgb(controls[i].Index);
						SetTextColor(hdc, col);

						// Set the text background colour to
						// the pixel in centre of the rectangle
						x = (rect.right - rect.left) / 2;
						y = (rect.bottom - rect.top) / 2;
						col = GetPixel(hdc, x, y);
						SetBkColor(hdc, col);

						// Draw the caption text
						rect.top -= 10;
						DrawTextA(hdc, controls[i].Title.c_str(), -1, &rect, DT_SINGLELINE | DT_LEFT | DT_TOP);

						if (hOldFont)
							SelectObject(hdc, hOldFont);

						EndPaint(hwndCtrl, &ps);

					}
				}
			}
		}
		break;

		// Static text colour
		case WM_CTLCOLORSTATIC:
		{
			for (size_t i = 0; i < controls.size(); i++) {
				if (controls[i].Type == "Static") {
					// lParam has the control window handle
					if (controls[i].hwndControl == (HWND)lParam) {
						// RGB text colour is the Index number (default 0)
						if (controls[i].Index > 0) {
							COLORREF col = Hex2Rgb(controls[i].Index);
							SetTextColor((HDC)wParam, col);
							SetBkMode((HDC)wParam, TRANSPARENT);
							// Return a background colour
							// only if the text colour has been set
							return (LRESULT)(HBRUSH)(COLOR_WINDOW);
						}
					}
				}
			}
		}
		break;

        case WM_DRAWITEM:

            lpdis = (LPDRAWITEMSTRUCT)lParam;
            if (lpdis->itemID == -1) break;

			// The blue hyperlink
            for (size_t i=0; i<controls.size(); i++) {
				if (controls[i].Type == "Static"
					&& controls[i].Index == 1 // Index = 1 identifies a hyperlink
					&& !controls[i].hwndType // Not a button
					&& LOWORD(wParam) == controls[i].ID) { // Can also be lpdis->CtlID
					// Title is the text displayed, control text is the action taken
					// COLOR_HOTLIGHT : 0, 102, 204
					// Other blues :
					// RGB(51, 102, 204)
					// RGB(40, 100, 190)
					// RGB(23,  27, 168)
					// RGB( 6,  69, 173)
					SetTextColor(lpdis->hDC, RGB(6, 69, 173));
					DrawTextA(lpdis->hDC, controls[i].Title.c_str(), -1, &lpdis->rcItem, DT_CENTER);
					// Set a hand cursor
					cursorHand = LoadCursor(NULL, IDC_HAND);
					SetClassLongPtr(controls[i].hwndControl, GCLP_HCURSOR, (LONG_PTR)cursorHand);
				} // endif hyperlink

				// Owner draw button
				else if (controls[i].Type == "Button"
					&& controls[i].Index != 1 // not a hyperlink
					&& LOWORD(wParam) == controls[i].ID) {

					HDC hdc = lpdis->hDC;
					RECT rect = lpdis->rcItem; // Button bounding box

					// Backgound colour is control.Val (default 0)
					if (controls[i].Val > 0) {
						HBRUSH hBrush = CreateSolidBrush(Hex2Rgb(controls[i].Val));
						SetBkMode(hdc, TRANSPARENT);
						FillRect(hdc, &rect, hBrush);
						DeleteObject(hBrush);
					}
					else {

						// Picture button
						// Bitmap handle set by image load
						if (controls[i].hwndType) {

							// Transparent white background
							SetBkMode(hdc, TRANSPARENT);
							FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

							// Draw the image
							HDC hdcMem = CreateCompatibleDC(hdc);
							HBITMAP hBitmap = (HBITMAP)controls[i].hwndType;
							HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
							BITMAP bitmap;
							GetObject(hBitmap, sizeof(bitmap), &bitmap);

							// Draw the bitmap to fit the button
							SetStretchBltMode(hdc, COLORONCOLOR); // Fastest method
							StretchBlt(
								hdc, // Destination DC (button)
								0, 0, // Destination position
								(rect.right - rect.left), // Destination width
								(rect.bottom - rect.top), // Destination height
								hdcMem, // Source DC (bitmap)
								0, 0, // Source position
								bitmap.bmWidth, // Source width
								bitmap.bmHeight, // Source height
								SRCCOPY // Copy operation
							);

							// Cleanup
							SelectObject(hdcMem, hOldBitmap);
							DeleteDC(hdcMem);
							// Do not delete the bitmap
							// Keep for repeated button press
						}
					}

					// Draw the button text and frame
					if (lpdis->itemState & ODS_SELECTED) {
						// Button pressed
						if (!controls[i].Text.empty()) {
							if (controls[i].Index > 0) {
								// Invert colour set by TextColor
								int inv = controls[i].Index ^ 0xFFFFFF;
								SetTextColor(hdc, Hex2Rgb(inv));
							} else {
								// Set to dark grey when pressed
								SetTextColor(hdc, RGB(64, 64, 64));
							}
						}
						// Blue border when pressed
						HBRUSH hBrush = CreateSolidBrush(RGB(0, 120, 215));
						FrameRect(hdc, &rect, hBrush);
						DeleteObject(hBrush);
					} // endif pressed
					else {

						// Button not pressed
						if (!controls[i].Text.empty()) {
							if (controls[i].Index > 0) {
								// Colour set by TextColor in control.Index (default 0)
								SetTextColor(hdc, Hex2Rgb(controls[i].Index));
							} else {
								// Black when not pressed
								SetTextColor(hdc, RGB(0, 0, 0));
							}
						}
						// Grey border when not pressed
						HBRUSH hBrush = CreateSolidBrush(RGB(169, 169, 169));
						FrameRect(hdc, &rect, hBrush);
						DeleteObject(hBrush);
					} // endif not pressed
					if (!controls[i].Text.empty()) {
						DrawTextA(hdc, controls[i].Text.c_str(), -1, &lpdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
				} // endif button
            }
            break;
        
		case WM_SETCURSOR:
			for (size_t i = 0; i < controls.size(); i++) {
				if (controls[i].Type == "Button"
					&& controls[i].Index != 1 // not a hyperlink
					&& (controls[i].hwndType || controls[i].Val > 1) ) { // Owner draw button
					// Cursor position
					POINT pt;
					GetCursorPos(&pt);
					// If the mouse is over the button, set a hand cursor
					if (WindowFromPoint(pt) == controls[i].hwndControl) {
						cursorHand = LoadCursor(NULL, IDC_HAND);
						SetCursor(cursorHand);
						return TRUE;
					}
				}
			}
			break;

		case WM_NOTIFY:
			{
				// Spin controls
				if (controls.size() > 0) {
					for (size_t i = 0; i < controls.size(); i++) {
						if (controls[i].Type == "Spin") {
							if (LOWORD(wParam) == controls[i].ID) {
								switch (((LPNMHDR)lParam)->code) {
									case UDN_DELTAPOS:
										LPNMUPDOWN lpnmud = (LPNMUPDOWN)lParam;
										int num = lpnmud->iDelta + lpnmud->iPos;
										if(num < (int)controls[i].Min)
											num = (int)controls[i].Min;
										if (num > (int)controls[i].Max)
											num = (int)controls[i].Max;
										controls[i].Val = num;
										// Update the spin control position
										SendMessage(controls[i].hwndControl, UDM_SETPOS, 0, (LPARAM)num);
										// Inform ofApp
										DialogFunction(controls[i].Title, "", controls[i].Val);
								}
							}
						}
					}
				}
			}
			return TRUE;


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
                                // Close the dialog
                                SendMessage(hwnd, WM_CLOSE, 0, 0);
                            }
                         }
                     }
                 }

                 // Combo box
                 if (HIWORD(wParam) == CBN_SELCHANGE) {
                     // Check all combo and list controls
                     for (size_t i=0; i<controls.size(); i++) {
                         if (controls[i].Type == "Combo") {
                             // Get currently selected combo index
                             // Allow for error if the user edits the list item
                             int index = (int)SendMessage(controls[i].hwndControl, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                             if (index != CB_ERR) {
                                 // Inform ofApp if no error
								 DialogFunction(controls[i].Title, controls[i].Text, index);
                             }
                             // Reset the control index
                             controls[i].Index = index;
                         }

						 if (controls[i].Type == "List") {
							 int index = (int)SendMessage(controls[i].hwndControl, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
							 if (index != LB_ERR) {
								 char tmp[256] {};
								 SendMessageA(controls[i].hwndControl, LB_GETTEXT, index, (LPARAM)tmp);
								 controls[i].Items[index] = tmp;
								 DialogFunction(controls[i].Title, tmp, index);
							 }
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
                            float range = (controls[i].Max - controls[i].Min);
                            if (wParam == SB_LINELEFT) { // Left key
                                // Moves left one unit
                                // Move 100 units for trackbars with range > 1000, 
                                if (range > 1000.0) {
                                    pos = pos - (int)(range/100.0);
                                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos);
                                }
                                else if(range >= 100.0) {
                                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos-100);
                                }
                                else {
                                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos);
                                }
                            }
                            else if(wParam == SB_LINERIGHT) { // Right key
                                if (range > 1000.0) {
                                    pos = pos + (int)(range/100.0);
                                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos);
                                }
                                else if (range >= 100.0) {
                                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos+100);
                                }
                                else {
                                    SendMessage(controls[i].hwndControl, TBM_SETPOS, TRUE, pos);
                                }
                            }

                            if (range > 1000.0)
                                controls[i].SliderVal = (float)pos;
                            else
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

	// Default message handling
	return DefWindowProc(hwnd, msg, wParam, lParam);

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
LRESULT CALLBACK GetKeyMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    LPMSG lpMsg = (LPMSG)lParam;
    if (nCode >= 0 && PM_REMOVE == wParam) {
        if ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST)) {
            ofxWinDialog* pDlg = reinterpret_cast<ofxWinDialog*>(GetWindowLongPtr(hwndDialog, GWLP_USERDATA));
            if (pDlg && pDlg->m_hDialog) {

                // Pass key up and down on to ofApp
                if (lpMsg->message == WM_KEYUP) {
                    pDlg->DialogFunction("WM_KEYUP", "", (int)lpMsg->wParam);
                }
                if (lpMsg->message == WM_KEYDOWN) {
                    pDlg->DialogFunction("WM_KEYDOWN", "", (int)lpMsg->wParam);
                }

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
