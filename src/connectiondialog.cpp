#include "connectiondialog.h"

ConnectionDialog::ConnectionDialog(wxWindow *parent, const wxString& title):
    wxDialog(parent, wxID_ANY, title,  wxDefaultPosition, wxSize(400,150))
{
    Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(ConnectionDialog::OnInitDialog));
}

wxString ConnectionDialog::GetRemoteHostName() const
{
    return m_remoteHost->GetValue();
}

int ConnectionDialog::GetRemotePort() const
{
    wxString portValue = m_remotePort->GetValue();
    if (portValue == "")
    {
        portValue = "6379";
    }

    long port = 6379;
    if (!portValue.ToLong(&port))
        return 6389;

    return (int)port;
}


void ConnectionDialog::OnInitDialog(wxInitDialogEvent &event)
{
    wxDialog::OnInitDialog(event);
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hboxRemoteHost = new wxBoxSizer(wxHORIZONTAL);
    hboxRemoteHost->Add(new wxStaticText(this, wxID_ANY, wxT("Host Name")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_remoteHost = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY));
    hboxRemoteHost->Add(m_remoteHost, 4, wxALIGN_LEFT );
    vbox->Add(hboxRemoteHost, 0, wxEXPAND | wxALL & ~wxBOTTOM, 10);

    wxBoxSizer *hboxRemotePort = new wxBoxSizer(wxHORIZONTAL);
    hboxRemotePort->Add(new wxStaticText(this, wxID_ANY, wxT("Port")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    m_remotePort = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY | wxFILTER_DIGITS));
    hboxRemotePort->Add(m_remotePort, 4, wxALIGN_LEFT );
    vbox->Add(hboxRemotePort, 0, wxEXPAND | wxALL, 10);

    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    hboxButtons->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxRIGHT, 5);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 1, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

    SetSizer(vbox);
    Center();
}
