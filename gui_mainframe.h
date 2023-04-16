#pragma once

#include <wx/wx.h>
#include <thread>
#include <iostream>
#include <memory>
#include "orchestrator.h"
#include "output_stream_gui.h"

class Mainframe : public wxFrame {
    wxPanel *panel;

    wxButton *run_button;
    std::thread run_button_handler_thread;

    wxTextCtrl *subnet_input;
    wxStaticText *subnet_label;

    wxTextCtrl *mask_input;
    wxStaticText *mask_label;

    wxGauge *gauge;
    std::thread gauge_update_thread;

    wxTextCtrl *text_output;
    std::shared_ptr<std::ostream> text_output_stream;
    std::shared_ptr<OutputStreamGUI> output_to_gui;
    std::shared_ptr<Orchestrator> orchestrator;

    // Initiate stop for all threads/components.
    bool stop_now = false;

    // Useful methods
    void create_controls();
    void run();
    void stop();

    // Event handlers
    void run_button_handler(wxCommandEvent &event);
    void close_event_handler(wxCloseEvent &event);
    void key_event_handler(wxKeyEvent &event);
public:
    Mainframe(const wxString& title);
};
