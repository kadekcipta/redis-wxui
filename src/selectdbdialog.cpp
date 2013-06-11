#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "selectdbdialog.h"

SelectDbdialog::SelectDbdialog(wxWindow *parent, const wxString& title, unsigned int db):
    wxDialog(parent, wxID_ANY, title)
{
    m_currentDb = wxString::Format("%d", db);
    Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(SelectDbdialog::OnInitDialog));
}

void SelectDbdialog::OnInitDialog(wxInitDialogEvent &event)
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hboxRemotePort = new wxBoxSizer(wxHORIZONTAL);

    hboxRemotePort->Add(new wxStaticText(this, wxID_ANY, wxT("Database")), 2, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    wxTextCtrl *remotePort = new wxTextCtrl(this, ID_DB, "", wxDefaultPosition, wxSize(200, -1));
    hboxRemotePort->Add(remotePort, 4, wxALIGN_LEFT );
    vbox->Add(hboxRemotePort, 0, wxEXPAND | wxALL, 8);

    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    hboxButtons->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxRIGHT, 5);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 8);
    FindWindow(ID_DB)->SetValidator(wxTextValidator(wxFILTER_DIGITS | wxFILTER_EMPTY, &m_currentDb));

    SetSizer(vbox);
    GetSizer()->Fit(this);
    Center();
    wxDialog::OnInitDialog(event);
}
