#include <wx/wx.h>

#include "main.h"
#include "mainframe.h"

wxIMPLEMENT_APP(Main);

bool Main::OnInit()
{
    auto frame = new Mainframe("PING SUBNET", "./ping_subnet.log");
    frame->SetClientSize(800, 670);
    frame->Center();
    frame->Show();
    return true;
}
