#ifndef QUERYPANEL_H
#define QUERYPANEL_H

#include <wx/notebook.h>
#include "redisconnection.h"

enum {
    ID_COMMAND_FIND_KEYS = 200,
    ID_COMMAND_RAW,
    ID_COMMAND_UPDATE,
    ID_COMMAND_DISCONNECT,
    ID_TAB_EXPLORE,
    ID_TEXT_KEY,
    ID_TEXT_COMMAND,
    ID_LBOX_KEYS,
    ID_TEXT_COMMAND_RESULT
};

class QueryPanel : public wxPanel
{
private:
    RedisConnection *m_connection;
    uint         m_currentDb;

    wxListBox *GetKeyListBox() const;

    void OnFind(wxCommandEvent& evt);
    void OnRawCommand(wxCommandEvent& evt);
    void OnEnterKey(wxKeyEvent& evt);
    void OnDisconnect(wxCommandEvent& evt);
    void OnKeySelected(wxCommandEvent& evt);
    void OnKeyDoubleClicked(wxCommandEvent& evt);

    void IterateArrayResponse(redisReply **response, size_t length);

protected:
    void DoFindKeys(const wxString& keyPatterns);
    void DoExecuteCommand(const wxString& command);

public:
    QueryPanel(wxWindow *parent, RedisConnection *connection);
    virtual ~QueryPanel();

    wxString GetSearchText() const;
    wxString GetSelectedKey() const;

    wxString GetCommandText() const;

    void CloseConnection();
    void AddKeyValue();
    void EditKeyValue();
    void DeleteKey();
    void SelectDb();
    void ExpireKey();

    wxString GetServerInfo() const;
};

#endif // QUERYPANEL_H
