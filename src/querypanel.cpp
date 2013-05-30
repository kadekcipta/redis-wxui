#include "querypanel.h"
#include "wx/wx.h"
#include "hiredis/hiredis.h"

QueryPanel::QueryPanel(wxWindow *parent, RedisConnection *connection):
    wxPanel(parent), m_connection(connection)
{
    m_searchText = new wxTextCtrl(this, ID_TEXT_FIND, "", wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    m_searchTrigger = new wxButton(this, ID_COMMAND_FIND, wxT("Find Keys"), wxPoint(-1,-1), wxSize(-1, 28));
    m_closeTrigger = new wxButton(this, wxID_CLOSE, wxT("Disconnect"), wxPoint(-1,-1), wxSize(-1, 28));
    m_resultText = new wxTextCtrl(this, 101, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE, wxDefaultValidator);

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(m_searchText, 1, wxEXPAND);
    hbox->Add(m_searchTrigger, 0, wxRIGHT, 3);
    hbox->Add(m_closeTrigger, 0);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(hbox, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
    vbox->Add(m_resultText, 1, wxEXPAND | wxALL & ~wxTOP, 3);
    SetSizer(vbox);

    Connect(ID_COMMAND_FIND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QueryPanel::OnFind));
    Connect(ID_TEXT_FIND, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(QueryPanel::OnEnterKey));
}

QueryPanel::~QueryPanel()
{
    if (m_connection != NULL)
    {
        delete m_connection;
        m_connection = NULL;
    }
}

wxString QueryPanel::GetSearchText() const
{
    return m_searchText->GetValue();
}

void QueryPanel::OnFind(wxCommandEvent& evt)
{
    DoFindKeys(GetSearchText());
}

void QueryPanel::OnEnterKey(wxKeyEvent& evt)
{
    DoFindKeys(GetSearchText());
}

void QueryPanel::IterateArrayResponse(redisReply **response, size_t length)
{
    redisReply **rep = response;
    for (int i = 0; i < length; i++)
    {
        if ((*rep)->type == REDIS_REPLY_STRING)
        {
            redisReply *r = *rep;
            wxString response(r->str);
            m_resultText->AppendText(response);
            m_resultText->AppendText("\n");
        }

        ++rep;
    }
}

void QueryPanel::DoFindKeys(const wxString& keyPatterns)
{
    redisContext *ctx = redisConnect("127.0.0.1", 6379);

    wxString realPatterns = keyPatterns;
    realPatterns.Trim();

    if (keyPatterns == "")
    {
        realPatterns = "*";
    }

    redisReply *reply = (redisReply*)redisCommand(ctx, "KEYS %s", realPatterns.GetData().AsChar());

    if (reply != NULL)
    {
        m_resultText->Clear();

        switch (reply->type) {
        case REDIS_REPLY_STRING:
            break;

        case REDIS_REPLY_ARRAY:
            IterateArrayResponse(reply->element, reply->elements);
            break;

        default:
            break;
        }

        freeReplyObject(reply);
    }

    redisFree(ctx);
}
