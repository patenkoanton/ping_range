#include "main.h"
#include "mainframe.h"
#include "orchestrator.h"
#include "output_stream.h"
#include "factory.cpp"      // template module


enum ElementID {
    TEXT_OUTPUT_ID = 2,
};

Mainframe::Mainframe(const wxString &title, const std::string &logfile) : wxFrame(nullptr, wxID_ANY, title), logfile_name(logfile)
{
    // UI controls
    this->set_font();
    this->create_menu();
    this->create_controls();

    // Infrastructure
    // TODO: why are we using make_unique for output_stream, while the console one is created on the stack? (see main.cpp)
    this->text_output_stream = Factory::make_unique<std::ostream, wxTextCtrl*>(this->text_output);
    this->output_to_gui = Factory::make_unique<OutputStream, std::ostream&>(*this->text_output_stream);
    this->error_to_gui = Factory::make_unique<OutputStream, std::ostream&>(*this->text_output_stream);
    this->orchestrator = Factory::make_unique<Orchestrator, OutputStream&, OutputStream&>(*this->output_to_gui, *this->error_to_gui);
}


void Mainframe::set_font()
{
    // Set font style for the whole window.
    auto font = this->GetFont();
    font.MakeLarger();
    this->SetFont(font);
}


void Mainframe::create_menu()
{
    // Create menubar
    this->menu_bar = new wxMenuBar();
    this->file_menu = new wxMenu();

    // 'Save' option
    this->file_menu_save_item = new wxMenuItem(this->file_menu, wxID_SAVE, "Save");
    this->file_menu->Append(this->file_menu_save_item);
    this->file_menu_save_item->Enable(false);       // disable until the ping is finished
    this->file_menu->Bind(wxEVT_MENU, &Mainframe::save_event_handler, this, wxID_SAVE);

    // Complete menubar
    this->menu_bar->Append(this->file_menu, "File");
    this->SetMenuBar(this->menu_bar);
}


void Mainframe::create_controls()
{
    // Create controls
    this->panel = new wxPanel(this, wxID_ANY);
    this->panel->Bind(wxEVT_CHAR_HOOK, &Mainframe::key_event_handler, this);

    this->run_button = new wxButton(panel, wxID_ANY, "Run", wxPoint(350, 140), wxSize(100, 25));
    this->run_button->Bind(wxEVT_BUTTON, &Mainframe::run_button_handler, this);

    this->subnet_input = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(300, 30), wxSize(200, 50));
    this->subnet_label = new wxStaticText(panel, wxID_ANY, "Subnet", wxPoint(240, 46));

    this->mask_input = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxPoint(300, 80), wxSize(200, 50));
    this->mask_label = new wxStaticText(panel, wxID_ANY, "Mask", wxPoint(240, 96));

    this->progress_bar = new wxGauge(panel, wxID_ANY, this->gauge_range, wxPoint(30, 635), wxSize(400, 20));
    this->text_output = new wxTextCtrl(panel, TEXT_OUTPUT_ID, wxEmptyString, wxPoint(30, 200), wxSize(745, 420), wxTE_MULTILINE | wxTE_READONLY);

    // Handle 'close' event, otherwise we'll crash after closing the app.
    this->Bind(wxEVT_CLOSE_WINDOW, &Mainframe::close_event_handler, this);
}

// Sends a stop signal to main application and waits for thread to finish.
void Mainframe::stop()
{
    this->orchestrator->stop();
    if (this->execution_thread.joinable()) {
        this->execution_thread.join();
    }

    if (this->progress_tracking_thread.joinable()) {
        this->progress_tracking_thread.join();
    }
}

void Mainframe::update_progress_bar()
{
    auto progress = this->orchestrator->get_progress();
    this->progress_bar->SetValue(progress);
}

void Mainframe::run_button_handler(wxCommandEvent &event)
{
    this->run();
}

void Mainframe::key_event_handler(wxKeyEvent &event)
{
    // Capture "Enter" key, unless it was pressed in text output form.
    if (this->FindFocus()->GetId() != TEXT_OUTPUT_ID) {
        auto key = event.GetKeyCode();
        if (key == WXK_RETURN || key == WXK_NUMPAD_ENTER){
            this->run();
        }
    }

    // Skip all other keys.
    event.Skip();
}

void Mainframe::close_event_handler(wxCloseEvent &event)
{
    this->stop();
    this->Destroy();
}

void Mainframe::save_event_handler(wxCommandEvent& event)
{
    this->text_output->SaveFile(this->logfile_name);
    wxLogMessage("Log saved to %s", this->logfile_name);
}

void Mainframe::run()
{
    this->stop();
    this->text_output->Clear();
    this->progress_bar->SetValue(0);
    this->file_menu_save_item->Enable(false);

    // Initiate ping (aka orchestrator).
    this->execution_thread = std::thread([this]() {
        std::string address_and_mask = (std::string)this->subnet_input->GetValue() + "/" + (std::string)this->mask_input->GetValue();
        this->orchestrator->start(address_and_mask);
    });

    // Wait for orchestrator to start (or fail to start) to avoid potential race condition.
    while ((this->orchestrator->is_running() || this->orchestrator->is_cancelled()) == false) {
        // NOP
    }

    // A parallel thread to update progress bar. Update progress until the end of execution.
    this->progress_tracking_thread = std::thread([this]() {
        while (this->orchestrator->is_running()) {
            this->update_progress_bar();
        }

        // When ping is finished, orchestrator is no longer "running".
        // Because of that, progress bar might not get fully updated up to 100% and we might see something like 90% after the execution.
        // Workaround: update progress one more [redundand] time.
        this->update_progress_bar();
        this->file_menu_save_item->Enable(true);
    });
}