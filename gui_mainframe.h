#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

class Mainframe : public wxFrame {
    wxPanel *panel;
    wxButton *run_button;

    wxTextCtrl *subnet_input;
    wxStaticText *subnet_label;

    wxTextCtrl *mask_input;
    wxStaticText *mask_label;

    wxListCtrl *output;

    void run_button_handler(wxCommandEvent &event);
    void create_controls();
    void show_output(std::string &arg);
public:
    Mainframe(const wxString& title);
};
