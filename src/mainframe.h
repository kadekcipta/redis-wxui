#ifndef SIMPLE_H
#define SIMPLE_H

#include <wx/wx.h>

enum {
    ID_MENU_CONNECT = 100,
    ID_MENU_QUERY,
    ID_MENU_SETTINGS,
    ID_MENU_ABOUT
};

class MainFrame : public wxFrame
{
private:
    wxPanel *m_parent;
    wxPanel *m_serverPanel;
    wxPanel *m_queryPanel;

    wxMenu  *m_fileMenu;
    wxMenu  *m_helpMenu;

    wxMenuBar   *m_menubar;

private:
    void InitializeMenubar();
    void InitializeControls();

    virtual void OnQuit(wxCommandEvent& evt);
    virtual void OnClose(wxCloseEvent& evt);

public:
    MainFrame(const wxString& title);
};

#endif // SIMPLE_H
