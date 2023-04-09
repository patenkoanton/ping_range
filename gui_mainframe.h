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
    void run_button_handler(wxCommandEvent &event);
    std::shared_ptr<Orchestrator> orchestrator;

    wxTextCtrl *subnet_input;
    wxStaticText *subnet_label;

    wxTextCtrl *mask_input;
    wxStaticText *mask_label;

    wxTextCtrl *text_output;
    std::shared_ptr<std::ostream> text_output_stream;
    std::shared_ptr<OutputStreamGUI> text_output_stream_inferface;

    void create_controls();
public:
    Mainframe(const wxString& title);
};
