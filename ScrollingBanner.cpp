// ScrollingBanner.cpp
// Implementation of the LED display simulation panel

#include "ScrollingBanner.h"
#include <wx/dcbuffer.h>
#include "resource2.h"
#include <windows.h>
#include <wx/stdpaths.h>
#include <wx/file.h>

// Register event handlers
wxBEGIN_EVENT_TABLE(ScrollingBanner, wxPanel)
    EVT_PAINT(ScrollingBanner::OnPaint)                // Handle paint events
    EVT_TIMER(wxID_ANY, ScrollingBanner::OnTimer)      // Handle timer events
    EVT_ERASE_BACKGROUND(ScrollingBanner::OnEraseBackground)  // Handle background erasing
wxEND_EVENT_TABLE()

// Constructor
// Initializes the banner panel and loads custom font
ScrollingBanner::ScrollingBanner(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
    displayTextPosX(0),
    speedFactor(2),
    timer(this)
{
    // Enable double buffering to prevent flicker
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Load custom LED font from resources
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HRSRC fontRes = FindResource(hInstance, MAKEINTRESOURCE(IDR_FONT2), RT_FONT);
    if (fontRes)
    {
        HGLOBAL fontHandle = LoadResource(hInstance, fontRes);
        if (fontHandle)
        {
            void* fontData = LockResource(fontHandle);
            DWORD fontSize = SizeofResource(hInstance, fontRes);

            // Create temporary file for font installation
            wxString tempDir = wxStandardPaths::Get().GetTempDir();
            wxString tempFile = tempDir + wxT("\\temp_font.ttf");

            wxFile file(tempFile, wxFile::write);
            if (file.IsOpened())
            {
                // Write font data and install
                file.Write(fontData, fontSize);
                file.Close();

                AddFontResourceEx(tempFile.wc_str(), FR_PRIVATE, NULL);

                // Notify system of font change
                PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

                // Clean up temporary file
                wxRemoveFile(tempFile);
            }
        }
    }
}


// Destructor
// Ensures timer is stopped when panel is destroyed
ScrollingBanner::~ScrollingBanner()
{
    if (timer.IsRunning())
    {
        timer.Stop();
    }
}

// UpdateBanner
// Updates the banner text and scroll speed
void ScrollingBanner::UpdateBanner(const wxString& text, int speed)
{
    displayText = text;
    
    // Ensure that the speed is between 0 and 20
    if (speed < 0) {
        speedFactor = 0;
    }
    else if (speed > 20) {
        speedFactor = 20;
    }
    else {
        speedFactor = speed;
    }

    // Set up custom LED font
    textFont = wxFont(55, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                     wxFONTWEIGHT_NORMAL, false, "Bold LED Board-7");

    // Set dark red background color
    SetBackgroundColour(wxColour(61, 14, 14));

    // Handle scrolling or static display
    if (speedFactor > 0)
    {
        // Start scrolling from right edge
        displayTextPosX = GetClientSize().GetWidth();
        if (!timer.IsRunning())
        {
            timer.Start(10);  // Update every 10ms
        }
    }
    else
    {
        // Center text for static display
        if (timer.IsRunning())
        {
            timer.Stop();
        }
        displayTextPosX = (GetClientSize().GetWidth() - GetTextWidth()) / 2;
        Refresh();
    }
}


// OnPaint
// Handles panel redrawing
void ScrollingBanner::OnPaint(wxPaintEvent& event)
{
    // Set up double-buffered drawing
    wxAutoBufferedPaintDC dc(this);
    
    // Clear background
    dc.SetBackground(GetBackgroundColour());
    dc.Clear();

    // Set up text drawing
    dc.SetFont(textFont);
    dc.SetTextForeground(wxColour(244, 14, 14));  // Bright red text

    // Draw the text at current position
    dc.DrawText(displayText, displayTextPosX, 5);
}


// OnTimer
// Updates text position for scrolling effect
void ScrollingBanner::OnTimer(wxTimerEvent& event)
{
    // Move text left by speed factor
    displayTextPosX -= speedFactor;

    // Reset position when text moves completely off screen
    if (displayTextPosX + GetTextWidth() < 0)
    {
        displayTextPosX = GetClientSize().GetWidth();
    }

    // Request redraw
    Refresh();
}


// OnEraseBackground
// Prevents flickering during animation

void ScrollingBanner::OnEraseBackground(wxEraseEvent& event)
{
    // Do nothing to prevent flicker
}


// GetTextWidth
// Calculates the pixel width of the current display text
int ScrollingBanner::GetTextWidth()
{
    // Create DC for text measurement
    wxClientDC dc(this);
    dc.SetFont(textFont);
    wxSize textSize = dc.GetTextExtent(displayText);
    return textSize.GetWidth();
}


// Accessor Methods
int ScrollingBanner::GetSpeedFactor() const
{
    return speedFactor;
}

wxString ScrollingBanner::GetBannerText() const
{
    return displayText;
}