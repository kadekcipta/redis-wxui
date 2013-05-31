#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class ConnectionDialog : public wxDialog
{
private:
    wxTextCtrl  *m_remoteHost;
    wxTextCtrl  *m_remotePort;

public:
    ConnectionDialog(wxWindow *parent, const wxString& title);

    wxString GetRemoteHostName() const;
    int GetRemotePort() const;
};

#endif // CONNECTIONDIALOG_H
