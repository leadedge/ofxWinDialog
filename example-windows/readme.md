- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

## example-windows

### How to use ofxWinDialog independently of Openframeworks.

ofxWinDialog reqires an application with an instance and window handle.
This example creates an invisible window so that just the dialog is shown.

example-windows.sln is a Visual Studio 2022 solution file that is set up to build the example as follows :

1) Download the ofxWinDialog addon and copy into the openframeworks 'addons' folder.
2) Copy the complete 'example-windows' folder to the 'apps\myApps' folder.
3) Open example-windows.sln with Visual Studio.
4) Change to 'Release' configuration and build the project.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ofxWinDialog is designed to use a callback function within
an Openframeworks 'ofApp' class which is called when dialog
controls are selected. The function can have any name.\
     <pre>void DialogFunction(std::string title, std::string text, int value);</pre>

The Openframeworks application would normally register this
function directly with ofxWinDialog :\
     <pre>dialog->AppDialogFunction(&ofApp::DialogFunction);</pre>

In order to use ofxWinDialog independently of Openframeworks,
an 'ofApp' class is simulated within it :\
     <pre>ofApp app;</pre>

The simulated ofApp class within ofxWinDialog has a callback function
with the same signature as the Openframeworks callback function :\
     <pre>void ofxDialogFunction(std::string title, std::string text, int value);</pre>

The independent application sets this function to point to
its own callback function, in this example 'MainDialogFunction'.\
     <pre>app.AppDialogFunction(MainDialogFunction);<\pre>

The ofApp class then forwards calls from ofxWinDialog to this function.

