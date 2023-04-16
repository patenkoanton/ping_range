#include <wx/wx.h>

#include "gui_app.h"
#include "gui_mainframe.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    auto frame = new Mainframe("C++ GUI");
    frame->SetClientSize(800, 655);
    frame->Center();
    frame->Show();
    return true;
}
