#include "connectiondialog.h"

ConnectionDialog::ConnectionDialog(wxWindow *parent, const wxString& title):
    wxDialog(parent, wxID_ANY, title,  wxDefaultPosition, wxDefaultSize)
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hboxRemoteHost = new wxBoxSizer(wxHORIZONTAL);
    hboxRemoteHost->Add(new wxStaticText(this, wxID_ANY, wxT("Host Name")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    hboxRemoteHost->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY) ), 4, wxALIGN_LEFT );
    vbox->Add(hboxRemoteHost, 0, wxEXPAND | wxALL & ~wxBOTTOM, 10);

    wxBoxSizer *hboxRemotePort = new wxBoxSizer(wxHORIZONTAL);
    hboxRemotePort->Add(new wxStaticText(this, wxID_ANY, wxT("Port")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    hboxRemotePort->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY | wxFILTER_DIGITS)), 4, wxALIGN_LEFT );
    vbox->Add(hboxRemotePort, 0, wxEXPAND | wxALL, 10);

    vbox->Add(-1, -1, wxEXPAND);
    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    hboxButtons->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxRIGHT, 5);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 1, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

    SetSizer(vbox);
    Center();
}

wxString ConnectionDialog::GetRemoteHostName() const
{
    return "";
}

int ConnectionDialog::GetRemotePort() const
{
    return 6379;
}
