#ifndef QUERYPANEL_H
#define QUERYPANEL_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "redisconnection.h"


enum {
    ID_COMMAND_FIND = 200,
    ID_TEXT_FIND,
    ID_LIST_RESULT,
    ID_COMMAND_DISCONNECT
};

class QueryPanel : public wxPanel
{
private:
    RedisConnection *m_connection;
    wxTextCtrl  *m_searchText;
    wxTextCtrl  *m_valueText;
    wxListBox   *m_resultList;
    wxButton    *m_searchTrigger;
    wxButton    *m_closeTrigger;

    virtual void OnFind(wxCommandEvent& evt);
    virtual void OnEnterKey(wxKeyEvent& evt);
    virtual void OnDisconnect(wxCommandEvent& evt);
    virtual void OnKeySelected(wxCommandEvent& evt);

    void IterateArrayResponse(redisReply **response, size_t length);

protected:
    virtual void DoFindKeys(const wxString& keyPatterns);

public:
    QueryPanel(wxWindow *parent, RedisConnection *connection);
    virtual ~QueryPanel();

    wxString GetSearchText() const;
};

#endif // QUERYPANEL_H
