#ifndef SIMPLE_H
#define SIMPLE_H

#include "querypanel.h"

#define APP_TITLE       wxT("Redis Wx-UI")
#define APP_NAME        wxT("wxWidgets base simple Redis UI")
#define APP_AUTHOR      wxT("Cipta - kadekcipta@gmail.com")

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
    wxNotebook *m_mainTab;
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
    MainFrame(const wxString& title);
};

#endif // SIMPLE_H
