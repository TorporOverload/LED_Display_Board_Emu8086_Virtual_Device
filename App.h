 
// App.h
// Application entry point for the LED Display Board program
 

#pragma once

// Include main wxWidgets header
#include <wx/wx.h>

/**
 * @brief Main application class for the LED Display Board
 * 
 * This class serves as the entry point for the application and:
 * - Initializes the wxWidgets framework
 * - Creates the main application window
 * - Sets up the application environment
 */
class App : public wxApp
{
public:
    /**
     * @brief Application initialization method
     * @return true if initialization succeeds, false otherwise
     */
    virtual bool OnInit();
};
