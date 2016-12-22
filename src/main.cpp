#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "mainframe.h"

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MainFrame *f = new MainFrame(NULL, wxID_ANY, APP_TITLE, wxDefaultPosition, wxSize(1024, 780));
    int width, height;
    f->GetSize(& width, & height);
    f->SetSize(wxDefaultCoord, wxDefaultCoord, width, height);
    f->Show();

    return true;
}
