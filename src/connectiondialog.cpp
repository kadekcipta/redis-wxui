#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "connectiondialog.h"

ConnectionDialog::ConnectionDialog(wxWindow *parent, const wxString& title):
    wxDialog(parent, wxID_ANY, title), m_remotePort(wxT("6379"))
{
    Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(ConnectionDialog::OnInitDialog));
}

void ConnectionDialog::CreateControls()
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *flexGrid = new wxFlexGridSizer(4, 2, 5, 5);

    flexGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Host")), 0, wxALIGN_CENTER_VERTICAL);
    flexGrid->Add(new wxTextCtrl(this, ID_HOSTNAME), 1, wxEXPAND);

    flexGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Port")), 0, wxALIGN_CENTER_VERTICAL);
    flexGrid->Add(new wxTextCtrl(this, ID_PORT), 1, wxEXPAND);

    flexGrid->AddSpacer(0);
    flexGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Left it blank if the server is not password-protected.")), 0,
                  wxALIGN_BOTTOM | wxEXPAND | wxTOP, 30);

    flexGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Password")), 0, wxALIGN_CENTER_VERTICAL);
    flexGrid->Add(new wxTextCtrl(this, ID_AUTH_PASSWORD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD), 1, wxEXPAND);

    vbox->Add(flexGrid, 1, wxEXPAND | wxALL, 8);
    vbox->AddSpacer(20);

    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btnOK = new wxButton(this, wxID_OK, wxT("OK"));
    btnOK->SetDefault();
    hboxButtons->Add(btnOK, 0, wxRIGHT, 5);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 8);

    FindWindow(ID_HOSTNAME)->SetValidator(wxTextValidator(wxFILTER_EMPTY, &m_remoteHost));
    FindWindow(ID_PORT)->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_remotePort));
    FindWindow(ID_AUTH_PASSWORD)->SetValidator(wxTextValidator(wxFILTER_NONE, &m_password));

    flexGrid->AddGrowableCol(1, 1);

    SetSizer(vbox);
    SetMinSize(wxSize(400, 150));
    GetSizer()->Fit(this);
    Center();
}

void ConnectionDialog::OnInitDialog(wxInitDialogEvent &event)
{
    CreateControls();
    wxDialog::OnInitDialog(event);
}
