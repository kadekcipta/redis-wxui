#ifndef SIMPLE_H
#define SIMPLE_H

#include "querypanel.h"

#define APP_TITLE       wxT("Redis Monitor")
#define APP_NAME        wxT("Simple Redis Monitor")

class wxString;
class wxNotebook;
class wxUpdateUIEvent;
class wxCommandEvent;
class wxCloseEvent;
class wxIdleEvent;
class wxFrame;
class wxMenu;
class wxStatusBar;
class wxMenuBar;
class wxSizeEvent;

enum {
    ID_MAIN_TAB = 100,
    ID_MAIN_STATUS_BAR,
    ID_MAIN_TOOL_BAR,
    ID_MENU_DISCONNECT,
    ID_MENU_SELECT_DB,
    ID_MENU_QUERY,
    ID_MENU_SETTINGS,
    ID_MENU_EXPIRE
};

class MainFrame : public wxFrame
{
private:
    wxBookCtrlBase *m_mainTab;
    wxStatusBar *m_statusBar;

    wxMenu  *m_fileMenu;
    wxMenu  *m_editMenu;
    wxMenu  *m_helpMenu;

    wxMenuBar   *m_menubar;

private:
    ConnectionPanel * GetActivePanel();

    void InitializeMenubar();
    void InitializeControls();

    void OnRedisConnectedUpdateUI(wxUpdateUIEvent& evt);
    void OnKeySelectedUpdateUI(wxUpdateUIEvent& evt);

    void OnAbout(wxCommandEvent& evt);
    void OnQuit(wxCommandEvent& evt);
    void OnClose(wxCloseEvent& evt);
    void OnAddConnection(wxCommandEvent& evt);
    void OnCloseConnection(wxCommandEvent& evt);
    void OnAddKeyValuePair(wxCommandEvent& evt);
    void OnSelectDb(wxCommandEvent& evt);
    void OnEditKeyValue(wxCommandEvent& evt);
    void OnDeleteKey(wxCommandEvent& evt);
    void OnExpireKey(wxCommandEvent& evt);

    void AddConnection();

public:
    MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE);
};

#endif // SIMPLE_H
