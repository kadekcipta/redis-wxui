#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "connectiondialog.h"

ConnectionDialog::ConnectionDialog(wxWindow *parent, const wxString& title):
    wxDialog(parent, wxID_ANY, title)
{
    Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(ConnectionDialog::OnInitDialog));
}

void ConnectionDialog::CreateControls()
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hboxRemoteHost = new wxBoxSizer(wxHORIZONTAL);
    hboxRemoteHost->Add(new wxStaticText(this, wxID_ANY, wxT("Host Name")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    wxTextCtrl *remoteHost = new wxTextCtrl(this, ID_HOSTNAME, "", wxDefaultPosition, wxSize(200, -1));
    hboxRemoteHost->Add(remoteHost, 4, wxALIGN_LEFT );
    vbox->Add(hboxRemoteHost, 0, wxEXPAND | wxALL & ~wxBOTTOM, 8);

    wxBoxSizer *hboxRemotePort = new wxBoxSizer(wxHORIZONTAL);
    hboxRemotePort->Add(new wxStaticText(this, wxID_ANY, wxT("Port")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

    wxTextCtrl *remotePort = new wxTextCtrl(this, ID_PORT, "", wxDefaultPosition, wxSize(200, -1));
    hboxRemotePort->Add(remotePort, 4, wxALIGN_LEFT );
    vbox->Add(hboxRemotePort, 0, wxEXPAND | wxALL, 8);

    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    hboxButtons->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxRIGHT, 5);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 8);

    FindWindow(ID_HOSTNAME)->SetValidator(wxTextValidator(wxFILTER_EMPTY, &m_remoteHost));
    FindWindow(ID_PORT)->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_remotePort));

    SetSizer(vbox);
    GetSizer()->Fit(this);
    Center();
}

void ConnectionDialog::OnInitDialog(wxInitDialogEvent &event)
{
    CreateControls();
    wxDialog::OnInitDialog(event);
}
