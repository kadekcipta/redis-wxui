#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wx.h>
#include <wx/stc/stc.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/numdlg.h>
#include <limits.h>

#include "querypanel.h"
#include "kveditordialog.h"
#include "selectdbdialog.h"

QueryPanel::QueryPanel(wxWindow *parent, RedisConnection *connection):
    wxPanel(parent), m_connection(connection), m_currentDb(0)
{
    wxNotebook *expTabs = new wxNotebook(this, ID_TAB_EXPLORE);

    // dashboard
    wxPanel *dashboardPanel = new wxPanel(expTabs);
    expTabs->AddPage(dashboardPanel, wxT("Dashboard"));

    // keys searching
    wxPanel *keysSearchingPanel = new wxPanel(expTabs);
    wxTextCtrl *txtKeysSearch = new wxTextCtrl(keysSearchingPanel, ID_TEXT_KEY, "", wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    wxButton *btnKeySearch = new wxButton(keysSearchingPanel, ID_COMMAND_FIND_KEYS, wxT("Find"), wxPoint(-1,-1), wxSize(-1, 28));
    wxListBox *lboxKeys = new wxListBox(keysSearchingPanel, ID_LBOX_KEYS, wxDefaultPosition, wxDefaultSize);

    wxBoxSizer *hboxKeysSearch = new wxBoxSizer(wxHORIZONTAL);
    hboxKeysSearch->Add(txtKeysSearch, 1, wxEXPAND);
    hboxKeysSearch->Add(btnKeySearch, 0);

    wxBoxSizer *vboxKeysSearch = new wxBoxSizer(wxVERTICAL);
    vboxKeysSearch->Add(hboxKeysSearch, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
    vboxKeysSearch->Add(lboxKeys, 1, wxEXPAND | wxALL & ~wxTOP, 3);

    keysSearchingPanel->SetSizer(vboxKeysSearch);
    expTabs->AddPage(keysSearchingPanel, wxT("Find Keys"));

    // raw commands
    wxPanel *rawCommandPanel = new wxPanel(expTabs);
    wxTextCtrl *txtCommand = new wxTextCtrl(rawCommandPanel, ID_TEXT_COMMAND, wxEmptyString, wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    wxButton *btnCommand = new wxButton(rawCommandPanel, ID_COMMAND_RAW, wxT("Execute"), wxPoint(-1,-1), wxSize(-1, 28));
    wxTextCtrl *txtCommandResult = new wxTextCtrl(rawCommandPanel, ID_TEXT_COMMAND_RESULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    txtCommandResult->SetBackgroundColour(*wxBLACK);
    txtCommandResult->SetForegroundColour(*wxGREEN);
    txtCommandResult->SetFont(wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT));
    txtCommandResult->SetEditable(false);

    wxBoxSizer *hboxCommand = new wxBoxSizer(wxHORIZONTAL);
    hboxCommand->Add(txtCommand, 1, wxEXPAND);
    hboxCommand->Add(btnCommand, 0);

    wxBoxSizer *vboxCommand = new wxBoxSizer(wxVERTICAL);
    vboxCommand->Add(hboxCommand, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
    vboxCommand->Add(txtCommandResult, 1, wxEXPAND | wxALL & ~wxTOP, 3);

    rawCommandPanel->SetSizer(vboxCommand);
    expTabs->AddPage(rawCommandPanel, wxT("Raw Commands"));

    wxBoxSizer *hboxMain = new wxBoxSizer(wxHORIZONTAL);
    hboxMain->Add(expTabs, 1, wxEXPAND | wxALL, 3);

    SetSizer(hboxMain);

    Connect(ID_COMMAND_FIND_KEYS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QueryPanel::OnFind));
    Connect(ID_TEXT_KEY, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(QueryPanel::OnEnterKey));
    Connect(ID_LBOX_KEYS, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(QueryPanel::OnKeySelected));
    Connect(ID_LBOX_KEYS, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(QueryPanel::OnKeyDoubleClicked));
    Connect(ID_COMMAND_RAW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(QueryPanel::OnRawCommand));
    Connect(ID_TEXT_COMMAND, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(QueryPanel::OnEnterKey));
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
    return ((wxTextCtrl*)FindWindow(ID_TEXT_KEY))->GetValue();
}

wxString QueryPanel::GetCommandText() const
{
    return ((wxTextCtrl*)FindWindow(ID_TEXT_COMMAND))->GetValue();
}

void QueryPanel::OnKeyDoubleClicked(wxCommandEvent &evt)
{
    EditKeyValue();
}

wxListBox *QueryPanel::GetKeyListBox() const
{
    wxWindow *ctrl = FindWindow(ID_LBOX_KEYS);
    if (ctrl != NULL)
    {
        return (wxListBox*)ctrl;
    }

    return NULL;
}

void QueryPanel::OnKeySelected(wxCommandEvent &evt)
{
    if (m_connection != NULL && m_connection->IsConnected())
    {
    }
}

wxString QueryPanel::GetSelectedKey() const
{
    if (GetKeyListBox() != NULL)
    {
        return GetKeyListBox()->GetStringSelection();
    }

    return "";
}

void QueryPanel::OnFind(wxCommandEvent& evt)
{
    DoFindKeys(GetSearchText());
}

void QueryPanel::OnRawCommand(wxCommandEvent &evt)
{
    DoExecuteCommand(GetCommandText());
}

void QueryPanel::OnEnterKey(wxKeyEvent& evt)
{
    if (evt.GetEventObject() == FindWindow(ID_TEXT_KEY))
    {
        DoFindKeys(GetSearchText());
    }
    else if (evt.GetEventObject() == FindWindow(ID_TEXT_COMMAND))
    {
        DoExecuteCommand(GetCommandText());
    }
}

void QueryPanel::OnDisconnect(wxCommandEvent& evt)
{
    CloseConnection();
}

void QueryPanel::DoExecuteCommand(const wxString &command)
{
    if (!command.IsEmpty() && m_connection != NULL && m_connection->IsConnected())
    {
        RedisValue ret = m_connection->ExecuteCommand(command);
        if (ret.GetValueType() != REDIS_NONE_VALUE)
        {
            wxTextCtrl *result = (wxTextCtrl*)FindWindow(ID_TEXT_COMMAND_RESULT);
            if (ret.GetValueType() == REDIS_INT_VALUE)
                result->SetValue(wxString::Format("%d", ret.GetIntValue()));
            else
                result->SetValue(ret.GetStrValue());
        }
    }
}

void QueryPanel::DoFindKeys(const wxString& keyPatterns)
{
    GetKeyListBox()->Clear();
    if (m_connection)
    {
        m_connection->FindKV(keyPatterns);

        wxArrayString &result = m_connection->GetKeysResult();
        for (int i = 0; i < result.Count(); i++)
        {
            GetKeyListBox()->Append(result[i]);
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
            int selectedIndex = GetKeyListBox()->GetSelection();
            if (selectedIndex != -1)
            {
                GetKeyListBox()->Delete((unsigned int)selectedIndex);
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
    wxNumberEntryDialog dlg(this, wxT(""), wxT("Please set the expiration time in seconds"), wxT("Set Key Expiration"), 10, 1, 2147483647);
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        m_connection->Expire(GetSelectedKey(), (int)dlg.GetValue());
    }
}

wxString QueryPanel::GetServerInfo() const
{
    if (m_connection != NULL && m_connection->IsConnected())
        return m_connection->GetServerInfo();

    return wxEmptyString;
}

