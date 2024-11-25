
// MainFrame.h
// Main window class for the LED Display Board application


#pragma once

// Include required wxWidgets components
#include <wx/frame.h>
#include <wx/thread.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include "wx/taskbar.h"
#include <string>
#include "ScrollingBanner.h"

/**
 * @brief Main window class that handles the LED display interface and I/O operations
 * 
 * This class manages the main application window, including:
 * - LED display banner
 * - I/O communication with the LED hardware
 * - Thread synchronization for data handling
 * - User interface elements
 */
class MainFrame : public wxFrame
{
public:
    // Constructor and destructor
    MainFrame();
    virtual ~MainFrame();
    
    // Event handlers
    void OnClose(wxCloseEvent& event);

private:

    // Constants for i/o communication
    static const long STATUS_PORT = 20;        // Port for status communication
    static const long SPEED_PORT = 10;         // Port for speed control
    static const long DATA_PORT_START = 150;   // Starting port for text data
    static const unsigned char EXIT_STATUS = 99;     // Status code for exit command
    static const unsigned char DATA_TERMINATOR = 0xFF; // Marks end of data transmission

    // Forward declaration for I/O thread handling
    class IOThread;


    // GUI Components

    ScrollingBanner* banner;     // Displays scrolling text
    wxStaticText* staticText;    // Shows port information
    wxTimer* ioTimer;            // Timer for I/O polling


    // Thread Synchronization
    wxMutex m_mutex;             // Mutex for thread safety
    wxCondition m_condition;     // Condition variable for thread synchronization
    bool m_threadShutdown;       // Flag to signal thread shutdown
    bool m_dataReady;           // Flag indicating new data is available
    std::string m_bannerText;   // Current banner text
    int m_speed;                // Current scroll speed

    IOThread* ioThread;         // Pointer to I/O thread

    // Private Methods
    void InitializeUI();        // Sets up the user interface
    void InitializeIO();        // Initializes I/O communication
    void OnIOTimer(wxTimerEvent& event);  // Handles I/O timer events
    void ReadData();           // Reads data from I/O ports
    void HandleNewData(const std::string& text, int speed);  // Processes new data
    void OnCriticalError();    // Handles critical errors

    wxDECLARE_EVENT_TABLE();      // Macro for wxWidgets event handling
};
