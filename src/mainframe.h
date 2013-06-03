#ifndef SIMPLE_H
#define SIMPLE_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/notebook.h>
#endif

enum {
    ID_MAIN_TAB = 100,
    ID_MAIN_STATUS_BAR,
    ID_MAIN_TOOL_BAR,
    ID_MENU_CONNECT,
    ID_MENU_QUERY,
    ID_MENU_SETTINGS,
    ID_MENU_MODIFY_VALUE,
    ID_MENU_DELETE,
    ID_MENU_ABOUT
};

class MainFrame : public wxFrame
{
private:
    wxPanel *m_parent;
    wxPanel *m_serverPanel;
    wxPanel *m_queryPanel;
    wxNotebook *m_mainTab;
    wxStatusBar *m_statusBar;

    wxMenu  *m_fileMenu;
    wxMenu  *m_editMenu;
    wxMenu  *m_helpMenu;

    wxMenuBar   *m_menubar;

private:
    void InitializeMenubar();
    void InitializeControls();

    virtual void OnQuit(wxCommandEvent& evt);
    virtual void OnClose(wxCloseEvent& evt);
    virtual void OnAddConnection(wxCommandEvent& evt);

    void AddConnection();

public:
    MainFrame(const wxString& title);
};

#endif // SIMPLE_H
