// App.cpp
// Implementation of the LED Display Board application class

#include "App.h"
#include "MainFrame.h"
#include "wx/msw/winundef.h" // Ensure Windows macros don't interfere

// Register the application class with wxWidgets framework
wxIMPLEMENT_APP(App);

// Initialize the application
bool App::OnInit() {
    // Create the main application window
    MainFrame* frame = new MainFrame();
    
    // Display the window and make it visible
    frame->Show(true);
    
    // Set as the main application window
    SetTopWindow(frame);
    
    // Return true to indicate successful initialization
    return true;
}
