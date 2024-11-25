//Include necessary header files
#include "MainFrame.h"
#include "io.h"
#include "wx/taskbar.h"
#include <wx/timer.h>
#include <wx/msgdlg.h>

// Register event handlers for the MainFrame
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)  // Handle window close event
wxEND_EVENT_TABLE()

// MainFrame constructor - initializes the main window of the application
MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, "LED Display Board", wxDefaultPosition, wxSize(1200, 270),
        (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) | wxSTAY_ON_TOP),  // Create a fixed-size window that stays on top
    m_dataReady(false),        // Initialize data ready flag
    m_threadShutdown(false),   // Initialize thread shutdown flag
    m_mutex(),                 // Initialize mutex for thread synchronization
    m_condition(m_mutex),      // Initialize condition variable for thread synchronization
    ioTimer(nullptr)          // Initialize I/O timer pointer
{
    //set Icon for the program
    wxIcon appIcon; (wxT("IDI_ICON1"), wxBITMAP_TYPE_ICO_RESOURCE);
   
    SetIcon(appIcon);  // Sets the icon for the window
    // Set up the main frame with fixed dimensions
    wxSize frameSize = wxSize(1200, 270);
    SetMinSize(frameSize);
    SetMaxSize(frameSize);
    SetSize(frameSize);
    InitializeUI();    // Set up the user interface
    InitializeIO();    // Initialize I/O communication
}

// Initialize the user interface components
void MainFrame::InitializeUI()
{
    SetBackgroundColour(*wxWHITE);  // Set white background

    // Initialize scrolling banner with default message
    banner = new ScrollingBanner(this);
    banner->UpdateBanner("Waiting for i/o input...", 1);
    banner->SetMinSize(wxSize(1200, 150));
    banner->SetMaxSize(wxSize(1200, 150));

    // Create and format the port information text
    wxString portInfo = wxString::Format("Ports Being Used: Status: %ld, Speed: %ld, Text: %ld to %ld",
        STATUS_PORT, SPEED_PORT, DATA_PORT_START, DATA_PORT_START + 101);

    // Create and style the static text display
    staticText = new wxStaticText(this, wxID_ANY, portInfo,
        wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    staticText->SetFont(wxFont(12, wxFONTFAMILY_MODERN, wxFONTSTYLE_MAX, wxFONTWEIGHT_NORMAL));
    staticText->SetForegroundColour(*wxBLACK);

    // Set up vertical layout
    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(banner, 0, wxEXPAND | wxALL, 5);
    sizer->AddSpacer(5);
    sizer->Add(staticText, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 5);
    sizer->AddSpacer(5);

    SetSizer(sizer);
    CreateStatusBar();  // Create status bar at bottom of window
    SetStatusText("Waiting for i/o input...");
}

// Initialize I/O communication and timer
void MainFrame::InitializeIO()
{
    try {
        WRITE_IO_BYTE(STATUS_PORT, 1);  // Set initial status
        ioTimer = new wxTimer(this);    // Create timer for I/O polling
        Bind(wxEVT_TIMER, &MainFrame::OnIOTimer, this);  // Bind timer to handler
        ioTimer->Start(100);  // Start timer with 100ms interval
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "I/O Error", wxOK | wxICON_ERROR);
        Close(true);
    }
}

// Timer event handler for I/O operations
void MainFrame::OnIOTimer(wxTimerEvent& event)
{
    try {
        static unsigned char statusByte = 1;

        // Check if shutdown is requested
        if (m_threadShutdown)
        {
            ioTimer->Stop();
            return;
        }

        statusByte = READ_IO_BYTE(STATUS_PORT);  // Read current status

        // Handle different status codes
        if (statusByte == EXIT_STATUS)  // Exit command received
        {
            Close(true);
            return;
        }
        else if (statusByte == 0)  // New data available
        {
            ReadData();  // Read the new data
            WRITE_IO_BYTE(STATUS_PORT, 2);  // Set status to processing
        }
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "I/O Error", wxOK | wxICON_ERROR);
        Close(true);
    }
}

// Read data from I/O ports
void MainFrame::ReadData()
{
    try {
        std::string bannerText;
        // Read text data until terminator is found
        for (long currentPort = DATA_PORT_START; ; currentPort++)
        {
            unsigned char byte = READ_IO_BYTE(currentPort);
            if (byte == DATA_TERMINATOR)  // Stop at terminator byte
                break;
            bannerText += static_cast<char>(byte);
        }

        int speed = READ_IO_BYTE(SPEED_PORT);  // Read scroll speed

        // Update member variables with thread safety
        wxMutexLocker lock(m_mutex);
        m_bannerText = bannerText;
        m_speed = speed;
        m_dataReady = true;

        HandleNewData(m_bannerText, m_speed);  // Process the new data
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "I/O Error", wxOK | wxICON_ERROR);
        Close(true);
    }
}

// Process and display new data
void MainFrame::HandleNewData(const std::string& text, int speed)
{
    banner->UpdateBanner(wxString::FromUTF8(text), speed);  // Update banner display

    // Update status bar with new text and speed
    wxString statusText = wxString::Format("Text: %s | Speed: %d",
        wxString::FromUTF8(text), speed);
    SetStatusText(statusText);
    Layout();
}

// Handle critical errors
void MainFrame::OnCriticalError()
{
    // Display error dialog and close application
    wxMessageDialog* errorDialog = new wxMessageDialog(this,
        "An unrecoverable error has occurred. The application will now exit.",
        "Critical Error",
        wxOK | wxICON_ERROR);

    errorDialog->ShowModal();
    errorDialog->Destroy();
    Close(true);
}

// Handle window close event
void MainFrame::OnClose(wxCloseEvent& event)
{
    m_threadShutdown = true;  // Signal thread shutdown

    // Clean up timer
    if (ioTimer)
    {
        ioTimer->Stop();
        Unbind(wxEVT_TIMER, &MainFrame::OnIOTimer, this);
    }

    event.Skip();  // Allow the default close operation to proceed
}

// MainFrame destructor - cleanup
MainFrame::~MainFrame()
{
    // Clean up timer if it still exists
    if (ioTimer)
    {
        delete ioTimer;
        ioTimer = nullptr;
    }
}