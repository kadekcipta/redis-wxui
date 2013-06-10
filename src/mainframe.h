#ifndef SIMPLE_H
#define SIMPLE_H

#include <wx/notebook.h>
#include "querypanel.h"

enum {
    ID_MAIN_TAB = 100,
    ID_MAIN_STATUS_BAR,
    ID_MAIN_TOOL_BAR,
    ID_MENU_CONNECT,
    ID_MENU_DISCONNECT,
    ID_MENU_ADD_KV,
    ID_MENU_SELECT_DB,
    ID_MENU_QUERY,
    ID_MENU_SETTINGS,
    ID_MENU_EDIT_KEY_VALUE,
    ID_MENU_EXPIRE,
    ID_MENU_DELETE,
    ID_MENU_ABOUT
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
    void InitializeMenubar();
    void InitializeControls();

    void OnRedisConnectedUpdateUI(wxUpdateUIEvent& evt);
    void OnKeySelectedUpdateUI(wxUpdateUIEvent& evt);

    QueryPanel * GetActivePanel();

    void OnIdle(wxIdleEvent &evt);

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
