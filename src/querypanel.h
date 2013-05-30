#ifndef QUERYPANEL_H
#define QUERYPANEL_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "redisconnection.h"

#include "hiredis/hiredis.h"

enum {
    ID_COMMAND_FIND = 200,
    ID_TEXT_FIND
};

class QueryPanel : public wxPanel
{
private:
    RedisConnection *m_connection;

    wxTextCtrl  *m_searchText;
    wxTextCtrl  *m_resultText;
    wxButton    *m_searchTrigger;
    wxButton    *m_closeTrigger;

    virtual void OnFind(wxCommandEvent& evt);
    virtual void OnEnterKey(wxKeyEvent& evt);

    void IterateArrayResponse(redisReply **response, size_t length);

protected:
    virtual void DoFindKeys(const wxString& keyPatterns);

public:
    QueryPanel(wxWindow *parent, RedisConnection *connection);
    virtual ~QueryPanel();

    wxString GetSearchText() const;
};

#endif // QUERYPANEL_H
