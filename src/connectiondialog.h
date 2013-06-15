#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

class wxDialog;
class wxWindow;
class wxString;
class wxInitDialogEvent;

enum {
    ID_HOSTNAME,
    ID_PORT,
    ID_AUTH_PASSWORD
};

class ConnectionDialog : public wxDialog
{
private:
    wxString  m_remoteHost;
    wxString  m_remotePort;
    wxString  m_password;

    virtual void OnInitDialog(wxInitDialogEvent &event);
    void CreateControls();

public:
    ConnectionDialog(wxWindow *parent, const wxString& title);

    const wxString& GetRemoteHostName() const { return m_remoteHost; }
    int GetRemotePort() const
    {
        unsigned long v = 0;
        if (m_remotePort.ToULong(&v))
        {
            return v;
        }

        return 6379;
    }

    const wxString& GetPassword() const { return m_password; }
};

#endif // CONNECTIONDIALOG_H
