#include "querypanel.h"
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/notebook.h>
#include <wx/listbook.h>

QueryPanel::QueryPanel(wxWindow *parent, RedisConnection *connection):
    wxPanel(parent), m_connection(connection)
{
    m_searchText = new wxTextCtrl(this, ID_TEXT_FIND, "", wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    m_valueText = new wxTextCtrl(this, ID_TEXT_FIND, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_valueText->SetEditable(false);

    m_searchTrigger = new wxButton(this, ID_COMMAND_FIND, wxT("Find Keys"), wxPoint(-1,-1), wxSize(-1, 28));
    m_closeTrigger = new wxButton(this, ID_COMMAND_DISCONNECT, wxT("Disconnect"), wxPoint(-1,-1), wxSize(-1, 28));
    m_resultList = new wxListBox(this, ID_LIST_RESULT, wxDefaultPosition, wxDefaultSize);

    // searching and disconnect controls
    wxBoxSizer *hboxSearch = new wxBoxSizer(wxHORIZONTAL);
    hboxSearch->Add(m_searchText, 1, wxEXPAND);
    hboxSearch->Add(m_searchTrigger, 0, wxRIGHT, 3);
    hboxSearch->Add(m_closeTrigger, 0);

    // list and value
    wxBoxSizer *hboxList = new wxBoxSizer(wxHORIZONTAL);
    hboxList->Add(m_resultList, 1, wxEXPAND);
    hboxList->Add(m_valueText, 1, wxEXPAND | wxLEFT, 3);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(hboxSearch, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
    vbox->Add(hboxList, 1, wxEXPAND | wxALL & ~wxTOP, 3);

    SetSizer(vbox);

    Connect(ID_COMMAND_FIND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QueryPanel::OnFind));
    Connect(ID_TEXT_FIND, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(QueryPanel::OnEnterKey));
    Connect(ID_COMMAND_DISCONNECT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QueryPanel::OnDisconnect));
    Connect(ID_LIST_RESULT, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(QueryPanel::OnKeySelected));
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

void QueryPanel::OnKeySelected(wxCommandEvent &evt)
{
    if (m_connection != NULL && m_connection->IsConnected())
    {
        int selectedIndex = m_resultList->GetSelection();
        if (selectedIndex != -1)
        {
            m_valueText->Clear();
            wxString key = m_resultList->GetString((unsigned)selectedIndex);
            RedisValue value = m_connection->GetValue(key);
            if (value.GetValueType() != REDIS_NONE_VALUE)
            {
                if (value.GetValueType() == REDIS_STR_VALUE)
                {
                    m_valueText->SetValue(value.GetStrValue());
                }
            }
        }
    }
}

void QueryPanel::OnFind(wxCommandEvent& evt)
{
    DoFindKeys(GetSearchText());
}

void QueryPanel::OnEnterKey(wxKeyEvent& evt)
{
    DoFindKeys(GetSearchText());
}

void QueryPanel::OnDisconnect(wxCommandEvent& evt)
{
    wxNotebook *nb = ((wxNotebook*)GetParent());
    for (int i = 0; i < nb->GetPageCount(); i++)
    {
        if (nb->GetPage(i) == this) {
            nb->DeletePage(i);
            break;
        }
    }
}

void QueryPanel::DoFindKeys(const wxString& keyPatterns)
{
    m_resultList->Clear();
    if (m_connection)
    {
        m_connection->FindKV(keyPatterns);

        wxArrayString &result = m_connection->GetKeysResult();

        for (int i = 0; i < result.Count(); i++)
        {
            m_resultList->Append(result[i]);
        }
    }
}
