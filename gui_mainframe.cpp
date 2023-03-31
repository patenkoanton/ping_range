#include "gui_mainframe.h"

enum ElementID {
    RUN_BUTTON_ID = 2,
    SUBNET_INPUT_ID,
    MASK_INPUT_ID,
};

Mainframe::Mainframe(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
    this->create_controls();
    CreateStatusBar();
    CreateToolBar();
}


void Mainframe::create_controls()
{
    this->panel = new wxPanel(this);
    this->run_button = new wxButton(panel, RUN_BUTTON_ID, "Run", wxPoint(350, 150), wxSize(100, 25));
    this->run_button->Bind(wxEVT_BUTTON, &Mainframe::run_button_handler, this);

    this->subnet_input = new wxTextCtrl(panel, SUBNET_INPUT_ID, wxEmptyString, wxPoint(300, 30), wxSize(200, 50));
    this->subnet_label = new wxStaticText(panel, wxID_ANY, "Subnet", wxPoint(240, 46));

    this->mask_input = new wxTextCtrl(panel, MASK_INPUT_ID, wxEmptyString, wxPoint(300, 80), wxSize(200, 50));
    this->mask_label = new wxStaticText(panel, wxID_ANY, "Mask", wxPoint(240, 96));

    this->output = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(30, 200), wxSize(745, 420), wxTE_MULTILINE | wxTE_READONLY);
}


void Mainframe::run_button_handler(wxCommandEvent &event)
{
    wxLogStatus("Button clicked: %s", this->subnet_input->GetValue());
}
