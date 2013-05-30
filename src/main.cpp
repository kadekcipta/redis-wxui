#include <wx/wx.h>
#include "mainframe.h"

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MainFrame *f = new MainFrame(wxT("Damn Simple Redis UI"));
    f->Show();

    return true;
}
