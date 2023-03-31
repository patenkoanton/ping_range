#pragma once

#include <wx/wx.h>

class Mainframe : public wxFrame {
    wxPanel *panel;
    wxButton *run_button;

    wxTextCtrl *subnet_input;
    wxStaticText *subnet_label;

    wxTextCtrl *mask_input;
    wxStaticText *mask_label;

    wxTextCtrl *output;

    void run_button_handler(wxCommandEvent &event);
    void create_controls();
public:
    Mainframe(const wxString& title);
};
