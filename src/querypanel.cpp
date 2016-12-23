#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wx.h>
#include <wx/aui/auibook.h>
#include <wx/stc/stc.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/splitter.h>
#include <wx/settings.h>
#include <wx/grid.h>
#include <wx/numdlg.h>
#include <wx/checklst.h>
#include <limits.h>

#include "querypanel.h"
#include "kveditordialog.h"
#include "selectdbdialog.h"
#include "simplechart.h"
#include "serverinfopanel.h"

wxDEFINE_EVENT(wxEVT_MONITOR_SYSTEM_STATS, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_MONITOR_SERVER_INFO, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_MONITOR_CONNECTED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_MONITOR_ERROR, wxThreadEvent);

MonitorThread::MonitorThread(ConnectionPanel *handler, const wxString& host, int port, const wxString& passwd):
    wxThread(wxTHREAD_DETACHED),
    m_handler(handler)
{
    m_connection = new RedisConnection(host, port, wxEmptyString, passwd);
}

MonitorThread::~MonitorThread()
{
    wxMessageOutputDebug().Printf("~MonitorThread()");

    wxCriticalSectionLocker enter(m_handler->m_csection);
    m_handler->m_monitorThread = NULL;
    if (m_connection) {
        delete m_connection;
        m_connection = NULL;
    }
}

wxThread::ExitCode MonitorThread::Entry()
{
    wxMessageOutputDebug().Printf("MonitorThread::Entry()");
    wxMessageOutputDebug().Printf("Connecting to %s:%d", m_connection->GetRemoteHost(), m_connection->GetRemotePort());

    if (!m_connection->Connect()) {
        // report error
        auto evtError = new wxThreadEvent(wxEVT_MONITOR_ERROR);
        evtError->SetString(m_connection->GetLastError());
        wxQueueEvent(m_handler, evtError);

        return (wxThread::ExitCode)-1;
    }

    // connected
    auto evtConnected = new wxThreadEvent(wxEVT_MONITOR_CONNECTED);
    evtConnected->SetPayload(m_connection->GetServerInfo());
    wxQueueEvent(m_handler, evtConnected);

    while (!TestDestroy()) {
        if (m_connection != NULL && m_connection->IsConnected())
        {
            // system stats event
            auto eventSystemStats = new wxThreadEvent(wxEVT_MONITOR_SYSTEM_STATS);
            eventSystemStats->SetPayload(m_connection->GetMemoryStatus());
            wxQueueEvent(m_handler, eventSystemStats);

            // server info stats
            auto eventServerInfo = new wxThreadEvent(wxEVT_MONITOR_SERVER_INFO);
            eventServerInfo->SetPayload(m_connection->GetServerInfo());
            wxQueueEvent(m_handler, eventServerInfo);

        } else break;

        wxThread::Sleep(1000);
    }

    return (wxThread::ExitCode)0;
}

ConnectionPanel::ConnectionPanel(wxWindow *parent,
                                 const wxString &host,
                                 int port,
                                 const wxString &passwd):
    wxPanel(parent),
    m_redisHost(host),
    m_redisPort(port),
    m_redisPassword(passwd),
    m_currentDb(0),
    m_monitorThread(0),
    m_connection(0)

{
    m_rawCommandFont = new wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier 10 Pitch"));
    m_notebook = new wxAuiNotebook(
                this,
                ID_TAB_EXPLORE,
                wxDefaultPosition,
                wxDefaultSize,
                wxAUI_NB_LEFT);

    ((wxAuiNotebook*)m_notebook)->SetArtProvider(new wxAuiSimpleTabArt);

    // dashboard
    wxPanel *dashboardPanel = new wxPanel(m_notebook);
    wxBoxSizer *vboxDashboard = new wxBoxSizer(wxVERTICAL);
    TimeLogChart *memChart = new TimeLogChart(dashboardPanel, ID_MEMORY_CHART, wxT("Memory Status"));
    memChart->SetValueAxisFormat("%.2fM");
    memChart->AddChart("Used", wxColour(*wxBLUE));
    memChart->AddChart("Peak", *wxRED);
    memChart->AddChart("RSS", wxColour("orange"));

    vboxDashboard->Add(memChart, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);

    TimeLogChart *cpuChart = new TimeLogChart(dashboardPanel, ID_CPU_CHART, wxT("CPU Status"));
    cpuChart->SetValueAxisFormat("%.1f");
    cpuChart->SetValueAxisScale(1.0);
    cpuChart->AddChart("Sys", wxColour(*wxGREEN));
    cpuChart->AddChart("User", *wxRED);

    vboxDashboard->Add(cpuChart, 1, wxEXPAND | wxTOP | wxBOTTOM, 5);

    wxBoxSizer *hboxDashboard = new wxBoxSizer(wxHORIZONTAL);

    hboxDashboard->Add(vboxDashboard, 1, wxEXPAND);
    // server info group choices
    hboxDashboard->Add(new wxCheckListBox(dashboardPanel, ID_SERVER_INFO_GROUP_CHOICES), 0, wxEXPAND | wxALL, 3);
    // server info
    hboxDashboard->Add(new ServerInfoPanel(dashboardPanel, ID_SERVER_INFO, wxT("Server Information")), 1, wxEXPAND);

    dashboardPanel->SetSizer(hboxDashboard);
    m_notebook->AddPage(dashboardPanel, wxT("Server Status"));

    // keys searching
    wxPanel *keysSearchingPanel = new wxPanel(m_notebook);
    wxTextCtrl *txtKeysSearch = new wxTextCtrl(keysSearchingPanel, ID_TEXT_KEY, wxEmptyString, wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    wxButton *btnKeySearch = new wxButton(keysSearchingPanel, ID_COMMAND_FIND_KEYS, wxT("Find"), wxPoint(-1,-1), wxSize(-1, 28));
    wxListBox *lboxKeys = new wxListBox(keysSearchingPanel, ID_LBOX_KEYS, wxDefaultPosition, wxDefaultSize);

    wxBoxSizer *hboxKeysSearch = new wxBoxSizer(wxHORIZONTAL);
    hboxKeysSearch->Add(txtKeysSearch, 1, wxEXPAND);
    hboxKeysSearch->Add(btnKeySearch, 0);

    wxBoxSizer *vboxKeysSearch = new wxBoxSizer(wxVERTICAL);
    vboxKeysSearch->Add(hboxKeysSearch, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
    vboxKeysSearch->Add(lboxKeys, 1, wxEXPAND | wxALL & ~wxTOP, 3);

    keysSearchingPanel->SetSizer(vboxKeysSearch);
    m_notebook->AddPage(keysSearchingPanel, wxT("Find Keys"));

    // raw commands
    wxPanel *rawCommandPanel = new wxPanel(m_notebook);
    wxTextCtrl *txtCommand = new wxTextCtrl(rawCommandPanel, ID_TEXT_COMMAND, wxEmptyString, wxDefaultPosition, wxSize(-1, 28), wxTE_PROCESS_ENTER, wxDefaultValidator);
    wxButton *btnCommand = new wxButton(rawCommandPanel, ID_COMMAND_RAW, wxT("Execute"), wxPoint(-1,-1), wxSize(-1, 28));
    wxTextCtrl *txtCommandResult = new wxTextCtrl(rawCommandPanel, ID_TEXT_COMMAND_RESULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    txtCommandResult->SetBackgroundColour(*wxBLACK);
    txtCommandResult->SetForegroundColour(*wxLIGHT_GREY);
    txtCommandResult->SetFont(*m_rawCommandFont);
    txtCommandResult->SetEditable(false);

    wxBoxSizer *hboxCommand = new wxBoxSizer(wxHORIZONTAL);
    hboxCommand->Add(txtCommand, 1, wxEXPAND);
    hboxCommand->Add(btnCommand, 0);

    wxBoxSizer *vboxCommand = new wxBoxSizer(wxVERTICAL);
    vboxCommand->Add(hboxCommand, 0, wxALIGN_LEFT | wxEXPAND | wxALL, 3);
    vboxCommand->Add(txtCommandResult, 1, wxEXPAND | wxALL & ~wxTOP, 3);

    rawCommandPanel->SetSizer(vboxCommand);
    m_notebook->AddPage(rawCommandPanel, wxT("Raw Commands"));

    wxBoxSizer *sizerMain = new wxBoxSizer(wxHORIZONTAL);
    sizerMain->Add(m_notebook, 1, wxEXPAND | wxALL, 3);
    SetSizer(sizerMain);
    Layout();

    Connect(ID_COMMAND_FIND_KEYS, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConnectionPanel::OnFind));
    Connect(ID_TEXT_KEY, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(ConnectionPanel::OnEnterKey));
    Connect(ID_LBOX_KEYS, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(ConnectionPanel::OnKeySelected));
    Connect(ID_LBOX_KEYS, wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(ConnectionPanel::OnKeyDoubleClicked));
    Connect(ID_COMMAND_RAW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConnectionPanel::OnRawCommand));
    Connect(ID_TEXT_COMMAND, wxEVT_COMMAND_TEXT_ENTER, wxKeyEventHandler(ConnectionPanel::OnEnterKey));
    Connect(ID_SERVER_INFO_GROUP_CHOICES, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(ConnectionPanel::OnGroupSelected));

    Bind(wxEVT_MONITOR_SYSTEM_STATS, &ConnectionPanel::OnMonitorSystemStat, this);
    Bind(wxEVT_MONITOR_SERVER_INFO, &ConnectionPanel::OnMonitorServerInfo, this);

    Bind(wxEVT_MONITOR_CONNECTED, [=](wxThreadEvent &evt) {
        UpdateServerStatusInfo(wxEmptyString); // clear state
        auto data = evt.GetPayload<wxArrayString>();
        PopulateServerInfoGroups(data);
    });

    Bind(wxEVT_MONITOR_ERROR, [=](wxThreadEvent &evt) {
        UpdateServerStatusInfo(evt.GetString());
    });

    UpdateServerStatusInfo(wxT("Connecting..."));

    StartMonitor();
}

ConnectionPanel::~ConnectionPanel()
{
    Cleanup();

    delete m_rawCommandFont;
}

void ConnectionPanel::UpdateServerStatusInfo(const wxString &status)
{
    ServerInfoPanel *siPanel = (ServerInfoPanel*)FindWindow(ID_SERVER_INFO);
    if (siPanel)
        siPanel->SetStatusInfo(status);
}

void ConnectionPanel::OnMonitorSystemStat(wxThreadEvent &evt)
{
    auto systemStat = evt.GetPayload<RedisSystemStatus>();

    TimeLogChart *memChart = (TimeLogChart *)FindWindow(ID_MEMORY_CHART);
    if (memChart != NULL)
    {
        memChart->AddChartValue(0, (double)systemStat.GetUsed());
        memChart->AddChartValue(1, (double)systemStat.GetPeak());
        memChart->AddChartValue(2, (double)systemStat.GetRss());
    }

    TimeLogChart *cpuChart = (TimeLogChart *)FindWindow(ID_CPU_CHART);
    if (cpuChart != NULL)
    {
        cpuChart->AddChartValue(0, systemStat.GetCPUSys());
        cpuChart->AddChartValue(1, systemStat.GetCPUUser());
    }
}

void ConnectionPanel::OnMonitorServerInfo(wxThreadEvent &evt)
{
    ServerInfoPanel *siPanel = (ServerInfoPanel*)FindWindow(ID_SERVER_INFO);
    if (siPanel != NULL)
       siPanel->UpdateInfo(evt.GetPayload<wxArrayString>());
}

void ConnectionPanel::OnGroupSelected(wxCommandEvent &evt)
{
    wxCheckListBox *groups = (wxCheckListBox*)FindWindow(ID_SERVER_INFO_GROUP_CHOICES);
    wxArrayString selectedStrings;
    for (int i = 0; i < groups->GetCount(); i++)
    {
        if (groups->IsChecked((unsigned int)i))
        {
            selectedStrings.Add(groups->GetString(i));
        }
    }

    ServerInfoPanel *siPanel = (ServerInfoPanel*)FindWindow(ID_SERVER_INFO);
    if (siPanel != NULL)
    {
        siPanel->SetSelectedGroups(selectedStrings);
    }
}

// This should be loaded from configuration, for time being it's just hardcoded to some arbitrary values
wxArrayString ConnectionPanel::GetServerInfoGroupsFromConfig()
{
    wxArrayString selectedInfoGroups;
    selectedInfoGroups.Add(wxT("Server"));
    selectedInfoGroups.Add(wxT("Clients"));

    return selectedInfoGroups;
}

void ConnectionPanel::PopulateServerInfoGroups(wxArrayString &infos)
{
    wxCheckListBox *groups = (wxCheckListBox*)FindWindow(ID_SERVER_INFO_GROUP_CHOICES);
    if (groups != NULL)
    {
        wxArrayString selectedInfoGroups = GetServerInfoGroupsFromConfig();
        for (int i = 0; i < infos.GetCount(); i++)
        {
            if (infos[i].StartsWith(wxT("#")))
            {
                wxString groupName = infos[i].SubString(2, infos[i].Length());
                int n = groups->Append(groupName);
                if (selectedInfoGroups.Index(groupName, false) != -1)
                    groups->Check(n);
            }
        }

        ServerInfoPanel *siPanel = (ServerInfoPanel*)FindWindow(ID_SERVER_INFO);
        if (siPanel != NULL)
            siPanel->SetSelectedGroups(selectedInfoGroups);
    }

    m_notebook->GetPage(0)->InvalidateBestSize();
    m_notebook->GetPage(0)->Layout();
}

void ConnectionPanel::StartMonitor()
{
    if (m_monitorThread)
        Cleanup();

    m_monitorThread = new MonitorThread(this, m_redisHost, m_redisPort, m_redisPassword);
    if (m_monitorThread->Run() != wxTHREAD_NO_ERROR) {
        wxLogError("Error while starting the monitor thread");
        delete m_monitorThread;
        m_monitorThread = NULL;
    }
}

wxString ConnectionPanel::GetSearchText() const
{
    return ((wxTextCtrl*)FindWindow(ID_TEXT_KEY))->GetValue();
}

wxString ConnectionPanel::GetCommandText() const
{
    return ((wxTextCtrl*)FindWindow(ID_TEXT_COMMAND))->GetValue();
}

void ConnectionPanel::OnKeyDoubleClicked(wxCommandEvent &evt)
{
    EditKeyValue();
}

wxListBox *ConnectionPanel::GetKeyListBox() const
{
    wxWindow *ctrl = FindWindow(ID_LBOX_KEYS);
    if (ctrl != NULL)
    {
        return (wxListBox*)ctrl;
    }

    return NULL;
}

void ConnectionPanel::OnKeySelected(wxCommandEvent &evt)
{
    if (m_connection != NULL && m_connection->IsConnected())
    {
    }
}

wxString ConnectionPanel::GetSelectedKey() const
{
    if (GetKeyListBox() != NULL)
    {
        return GetKeyListBox()->GetStringSelection();
    }

    return wxEmptyString;
}

void ConnectionPanel::OnFind(wxCommandEvent& evt)
{
    FindKeys(GetSearchText());
}

void ConnectionPanel::OnRawCommand(wxCommandEvent &evt)
{
    ExecuteCommand(GetCommandText());
}

void ConnectionPanel::OnEnterKey(wxKeyEvent& evt)
{
    if (evt.GetEventObject() == FindWindow(ID_TEXT_KEY))
    {
        FindKeys(GetSearchText());
    }
    else if (evt.GetEventObject() == FindWindow(ID_TEXT_COMMAND))
    {
        ExecuteCommand(GetCommandText());
    }
}

void ConnectionPanel::OnDisconnect(wxCommandEvent& evt)
{
    Cleanup();
}

void ConnectionPanel::ExecuteCommand(const wxString &command)
{
    if (!command.IsEmpty() && m_connection != NULL && m_connection->IsConnected())
    {
        RedisSimpleValue ret = m_connection->ExecuteCommand(command);
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

void ConnectionPanel::FindKeys(const wxString& keyFilter)
{
    GetKeyListBox()->Clear();
    if (m_connection)
    {
        m_connection->FindKeys(keyFilter);
        wxArrayString &result = m_connection->GetKeysResult();
        for (int i = 0; i < result.Count(); i++)
        {
            GetKeyListBox()->Append(result[i]);
        }
    }
}

void ConnectionPanel::Cleanup()
{
    // start of thread termination
    {
        wxCriticalSectionLocker enter(m_csection);
        if (m_monitorThread != NULL) {
            if (m_monitorThread->Delete() != wxTHREAD_NO_ERROR)
                wxLogError("Error while deleting thread !");
        }
    }

    while (true) {
        {
            wxCriticalSectionLocker enter(m_csection);
            if (!m_monitorThread) break;
        }
        wxThread::This()->Sleep(1);
    }
    // end of thread termination

    if (m_connection != NULL) {
        delete m_connection;
        m_connection = NULL;
    }

    wxMessageOutputDebug().Printf("Cleanup");
}

void ConnectionPanel::EditKeyValue()
{
    wxString key = GetSelectedKey();
    if (key != wxEmptyString && m_connection != NULL && m_connection->IsConnected())
    {
        KeyValueEditorDialog dlg(this, wxT("Edit Key-Value Pair"), key, m_connection->GetValue(key));
        if (dlg.ShowModal() == wxID_OK)
        {
            m_connection->SetValue(dlg.GetKey(), dlg.GetValue());
        }
    }
}

void ConnectionPanel::AddKeyValue()
{
    KeyValueEditorDialog dlg(this, wxT("Add Key-Value Pair"), wxEmptyString, RedisSimpleValue(wxEmptyString));
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        m_connection->SetValue(dlg.GetKey(), dlg.GetValue());
    }
}

void ConnectionPanel::DeleteKey()
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

void ConnectionPanel::SelectDb()
{
    SelectDbdialog dlg(this, wxT("Select Database"), m_currentDb);
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        uint selectedDb = dlg.GetDb();
        if (m_connection->SelectDb(selectedDb))
        {
            m_currentDb = selectedDb;
            FindKeys(GetSearchText());
        }
    }
}

void ConnectionPanel::ExpireKey()
{
    wxNumberEntryDialog dlg(this, wxEmptyString, wxT("Please set the expiration time in seconds"), wxT("Set Key Expiration"), 10, 1, 2147483647);
    if (dlg.ShowModal() == wxID_OK && m_connection != NULL && m_connection->IsConnected())
    {
        m_connection->Expire(GetSelectedKey(), (int)dlg.GetValue());
    }
}
