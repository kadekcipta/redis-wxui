#include "querypanel.h"
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/numdlg.h>
#include <limits.h>

#include "kveditordialog.h"
#include "selectdbdialog.h"

QueryPanel::QueryPanel(wxWindow *parent, RedisConnection *connection):
    wxPanel(parent), m_connection(connection), m_currentDb(0)
{
    wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER);
    wxPanel *leftPanel = new wxPanel(splitter);
    wxPanel *rightPanel = new wxPanel(splitter);

    m_searchText = new wxTextCtrl(leftPanel, ID_TEXT_FIND, "", wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    m_valueText = new wxTextCtrl(rightPanel, ID_TEXT_FIND, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_valueText->SetEditable(false);
    m_valueText->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

    m_searchTrigger = new wxButton(leftPanel, ID_COMMAND_FIND, wxT("Find Keys"), wxPoint(-1,-1), wxSize(-1, 28));
    m_resultList = new wxListBox(leftPanel, ID_LIST_RESULT, wxDefaultPosition, wxDefaultSize);

    // searching and disconnect controls
    wxBoxSizer *hboxSearch = new wxBoxSizer(wxHORIZONTAL);
    hboxSearch->Add(m_searchText, 1, wxEXPAND);
    hboxSearch->Add(m_searchTrigger, 0);

    wxBoxSizer *vboxLeft = new wxBoxSizer(wxVERTICAL);
    vboxLeft->Add(hboxSearch, 0, wxALIGN_LEFT | wxEXPAND);
    vboxLeft->Add(m_resultList, 1, wxEXPAND | wxTOP, 3);

    wxBoxSizer *vboxRight = new wxBoxSizer(wxVERTICAL);
    vboxRight->Add(m_valueText, 1, wxEXPAND);

    wxBoxSizer *hboxMain = new wxBoxSizer(wxHORIZONTAL);

    leftPanel->SetSizer(vboxLeft);
    rightPanel->SetSizer(vboxRight);

    splitter->SplitVertically(leftPanel, rightPanel, parent->GetSize().GetWidth()/2);
    hboxMain->Add(splitter, 1, wxEXPAND | wxALL, 3);

    SetSizer(hboxMain);

    Connect(ID_COMMAND_FIND, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QueryPanel::OnFind));
    Connect(ID_TEXT_FIND, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(QueryPanel::OnEnterKey));
    Connect(ID_LIST_RESULT, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(QueryPanel::OnKeySelected));
    Connect(ID_LIST_RESULT, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(QueryPanel::OnKeyDoubleClicked));
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

void QueryPanel::OnKeyDoubleClicked(wxCommandEvent &evt)
{
    EditKeyValue();
}

void QueryPanel::RefreshCurrentKeyValue()
{
    int selectedIndex = m_resultList->GetSelection();
    if (selectedIndex != -1)
    {
        wxString key = m_resultList->GetString((unsigned)selectedIndex);
        RedisValue value = m_connection->GetValue(key);
        if (value.GetValueType() != REDIS_NONE_VALUE)
        {
            if (value.GetValueType() == REDIS_STR_VALUE)
            {
                m_valueText->Clear();
                m_valueText->SetValue(value.GetStrValue());
            }
        }
    }
}

void QueryPanel::OnKeySelected(wxCommandEvent &evt)
{
    if (m_connection != NULL && m_connection->IsConnected())
    {
        RefreshCurrentKeyValue();
    }
}

wxString QueryPanel::GetSelectedKey() const
{
    if (m_resultList != NULL)
    {
        return m_resultList->GetStringSelection();
    }

    return "";
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
    CloseConnection();
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

void QueryPanel::CloseConnection()
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

void QueryPanel::EditKeyValue()
{
    wxString key = GetSelectedKey();
    if (key != "" && m_connection != NULL && m_connection->IsConnected())
    {
        KeyValueEditorDialog dlg(this, wxT("Edit Key-Value Pair"), key, m_connection->GetValue(key));
        if (dlg.ShowModal() == wxID_OK)
        {
            m_connection->SetValue(dlg.GetKey(), dlg.GetValue());
            RefreshCurrentKeyValue();
        }
    }
}

void QueryPanel::AddKeyValue()
{
    KeyValueEditorDialog dlg(this, wxT("Add Key-Value Pair"), "", RedisValue(""));
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        m_connection->SetValue(dlg.GetKey(), dlg.GetValue());
    }
}

void QueryPanel::DeleteKey()
{
    wxMessageDialog dlg(this, wxT("Are you sure to delete key: '" + GetSelectedKey() + "'' ?"), wxT("Delete Key"), wxNO_DEFAULT | wxYES_NO | wxCANCEL | wxICON_ASTERISK);
    if (dlg.ShowModal() == wxID_YES && m_connection != NULL && m_connection->IsConnected())
    {
        if (m_connection->DeleteKey(GetSelectedKey()))
        {
            int selectedIndex = m_resultList->GetSelection();
            if (selectedIndex != -1)
            {
                m_resultList->Delete((unsigned int)selectedIndex);
                m_valueText->Clear();
            }
        }
    }
}

void QueryPanel::SelectDb()
{
    SelectDbdialog dlg(this, wxT("Select Database"), m_currentDb);
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        uint selectedDb = dlg.GetDb();
        if (m_connection->SelectDb(selectedDb))
        {
            m_currentDb = selectedDb;
            DoFindKeys(GetSearchText());
        }
    }
}

void QueryPanel::ExpireKey()
{
    wxNumberEntryDialog dlg(this, wxT(""), wxT(""), wxT("Set Key Expiration"), 10, 1, 2147483647);
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        m_connection->Expire(GetSelectedKey(), (int)dlg.GetValue());
    }
}
