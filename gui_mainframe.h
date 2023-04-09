#pragma once

#include <wx/wx.h>
#include <thread>
#include <iostream>
#include <memory>
#include "output_stream_gui.h"

class Mainframe : public wxFrame {
    wxPanel *panel;
    wxButton *run_button;

    wxTextCtrl *subnet_input;
    wxStaticText *subnet_label;

    wxTextCtrl *mask_input;
    wxStaticText *mask_label;

    wxTextCtrl *text_output;
    std::shared_ptr<std::ostream> text_output_stream;
    std::shared_ptr<OutputStreamGUI> text_output_stream_inferface;

    void run_button_handler(wxCommandEvent &event);
    std::thread run_button_handler_thread;
    void create_controls();
    void show_output(std::string &arg);
public:
    Mainframe(const wxString& title);
};
