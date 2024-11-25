// ScrollingBanner.h
// Implements a scrolling text display panel for LED simulation

#pragma once

// Include required wxWidgets components
#include <wx/wx.h>
#include <wx/timer.h>

/**
 * @brief Panel class that simulates an LED display with scrolling text
 * 
 * This class provides a visual representation of an LED display panel with:
 * - Smooth scrolling text animation
 * - Adjustable scroll speed
 * - Custom LED-style font rendering
 * - Static text display option (speed = 0)
 */
class ScrollingBanner : public wxPanel
{
public:
    // Constructor and destructor
    ScrollingBanner(wxWindow* parent);
    ~ScrollingBanner();

    //-------------------------------------------------------------------------
    // Public Methods
    //-------------------------------------------------------------------------
    /**
     * @brief Updates the banner text and scroll speed
     * @param text The text to display
     * @param speed Scroll speed (0 = static, 1-20 = scrolling speed)
     */
    void UpdateBanner(const wxString& text, int speed);

    /**
     * @brief Gets the current scroll speed
     * @return Current speed factor (0-20)
     */
    int GetSpeedFactor() const;

    /**
     * @brief Gets the current banner text
     * @return Current display text
     */
    wxString GetBannerText() const;

private:

    // Member Variables
    
    wxTimer timer;           // Timer for animation control
    wxString displayText;    // Text currently being displayed
    wxFont textFont;         // Custom LED-style font
    int displayTextPosX;     // Current X position of text
    int speedFactor;         // Current scroll speed

    
    // Private Methods - Event Handlers
     
    void OnPaint(wxPaintEvent& event);           // Handles paint events
    void OnTimer(wxTimerEvent& event);           // Updates text position
    void OnEraseBackground(wxEraseEvent& event); // Prevents flicker
    int GetTextWidth();                          // Calculates text width

    wxDECLARE_EVENT_TABLE();    // Macro for wxWidgets event handling
};