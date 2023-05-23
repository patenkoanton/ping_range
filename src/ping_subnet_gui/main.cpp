#include <wx/wx.h>

#include <iostream>
#include "main.h"
#include "mainframe.h"
#include "custom_exception.h"

wxIMPLEMENT_APP(Main);

bool Main::OnInit()
{
     // Gracefully exit when a KNOWN exception is thrown.
    try {
        auto frame = new Mainframe("PING SUBNET", "./ping_subnet.log");
        frame->SetClientSize(800, 670);
        frame->Center();
        frame->Show();
    } catch (const CustomException &exc) {
        std::cerr << exc.what() <<std::endl;
        return false;
    }

    return true;
}
