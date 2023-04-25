#pragma once

#include <wx/wx.h>
#include <thread>
#include <iostream>
#include <memory>
#include "orchestrator.h"
#include "output_stream_gui.h"

class Mainframe : public wxFrame {
    wxMenuBar *menu_bar;
    wxMenu *file_menu;
    wxMenuItem *file_menu_save_item;

    wxPanel *panel;

    wxButton *run_button;
    std::thread execution_thread;

    wxTextCtrl *subnet_input;
    wxStaticText *subnet_label;

    wxTextCtrl *mask_input;
    wxStaticText *mask_label;

    wxGauge *progress_bar;
    std::thread progress_tracking_thread;

    wxTextCtrl *text_output;
    std::shared_ptr<std::ostream> text_output_stream;
    std::shared_ptr<OutputStreamGUI> output_to_gui;
    std::shared_ptr<Orchestrator> orchestrator;

    // Useful methods
    void set_font();
    void create_menu();
    void create_controls();
    void run();
    void stop();

    // Event handlers
    void run_button_handler(wxCommandEvent &event);
    void close_event_handler(wxCloseEvent &event);
    void key_event_handler(wxKeyEvent &event);
    void save_event_handler(wxCommandEvent& event);

    // Private vars
    const std::string logfile_name;
    const int gauge_range = 100;
public:
    Mainframe(const wxString &title, const std::string &logfile);
};
