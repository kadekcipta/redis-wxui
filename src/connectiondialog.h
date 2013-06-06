#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

enum {
    ID_HOSTNAME,
    ID_PORT
};

class ConnectionDialog : public wxDialog
{
private:
    wxString  m_remoteHost;
    wxString  m_remotePort;

    virtual void OnInitDialog(wxInitDialogEvent &event);
    void CreateControls();

public:
    ConnectionDialog(wxWindow *parent, const wxString& title);

    wxString GetRemoteHostName() { return m_remoteHost; }
    int GetRemotePort()
    {
        unsigned long v = 0;
        if (m_remotePort.ToULong(&v))
        {
            return v;
        }

        return 6379;
    }
};

#endif // CONNECTIONDIALOG_H
