#ifndef QUERYPANEL_H
#define QUERYPANEL_H

#include <wx/bookctrl.h>
#include <wx/thread.h>

#include "redisconnection.h"

class wxDialog;
class wxWindow;
class wxString;
class wxCommandEvent;
class wxInitDialogEvent;
class wxPanel;
class wxNotebook;
class wxTimerEvent;
class wxTimer;
class wxFont;
class wxOnSizeEvent;
class wxEventHandler;

enum {
    ID_COMMAND_FIND_KEYS = 200,
    ID_COMMAND_RAW,
    ID_COMMAND_UPDATE,
    ID_COMMAND_DISCONNECT,
    ID_TAB_EXPLORE,
    ID_TEXT_KEY,
    ID_TEXT_COMMAND,
    ID_LBOX_KEYS,
    ID_TEXT_COMMAND_RESULT,
    ID_TIMER,
    ID_MEMORY_CHART,
    ID_CPU_CHART,
    ID_SERVER_INFO_GROUP_CHOICES,
    ID_SERVER_INFO
};


wxDECLARE_EVENT(wxEVT_MONITOR_SYSTEM_STATS, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_MONITOR_SERVER_INFO, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_MONITOR_CONNECTED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_MONITOR_ERROR, wxThreadEvent);

class ConnectionPanel;
class MonitorThread: public wxThread
{
public:
    MonitorThread(ConnectionPanel *handler, const wxString&, int, const wxString&);
    ~MonitorThread();

protected:
    virtual wxThread::ExitCode Entry();

    ConnectionPanel  *m_handler;

private:
    RedisConnection *m_connection;
};

class ConnectionPanel : public wxPanel
{
private:
    RedisConnection *m_connection;
    uint         m_currentDb;
    wxTimer     *m_timer;
    wxFont      *m_rawCommandFont;
    wxBookCtrlBase *m_notebook;
    wxThread    *m_monitorThread;
    wxCriticalSection m_csection;

    wxString    m_redisHost;
    wxString    m_redisPassword;
    int m_redisPort;

    friend class MonitorThread;


    bool tryConnect();

    wxListBox *GetKeyListBox() const;
    wxArrayString GetServerInfoGroupsFromConfig();
    void PopulateServerInfoGroups(wxArrayString&);

    void OnFind(wxCommandEvent& evt);
    void OnRawCommand(wxCommandEvent& evt);
    void OnEnterKey(wxKeyEvent& evt);
    void OnDisconnect(wxCommandEvent& evt);
    void OnKeySelected(wxCommandEvent& evt);
    void OnKeyDoubleClicked(wxCommandEvent& evt);
    void OnGroupSelected(wxCommandEvent& evt);

    void OnMonitorSystemStat(wxThreadEvent &evt);
    void OnMonitorServerInfo(wxThreadEvent &evt);

    void UpdateServerStatusInfo(const wxString&);
    void IterateArrayResponse(redisReply **response, size_t length);
    void FindKeys(const wxString& keyFilter);
    void ExecuteCommand(const wxString& command);
    void StartMonitor();

public:
    ConnectionPanel(wxWindow *parent, const wxString &host, int port, const wxString &passwd);
    virtual ~ConnectionPanel();

    wxString GetSearchText() const;
    wxString GetSelectedKey() const;

    wxString GetCommandText() const;

    void Cleanup();
    void AddKeyValue();
    void EditKeyValue();
    void DeleteKey();
    void SelectDb();
    void ExpireKey();
};

#endif // QUERYPANEL_H
