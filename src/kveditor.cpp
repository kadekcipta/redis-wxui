#include "kveditor.h"

KVEditor::KVEditor(wxWindow *parent, const wxString& title):
    wxDialog(parent, wxID_ANY, title,  wxDefaultPosition, wxSize(-1,-1))
{
    Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(KVEditor::OnInitDialog));
}

void KVEditor::OnInitDialog(wxInitDialogEvent &event)
{
    wxDialog::OnInitDialog(event);
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *hboxKeyName = new wxBoxSizer(wxHORIZONTAL);
    hboxKeyName->Add(new wxStaticText(this, wxID_ANY, wxT("Key Name")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND);
    m_txtKeyName = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_EMPTY));
    hboxKeyName->Add(m_txtKeyName, 4, wxALIGN_LEFT );
    vbox->Add(hboxKeyName, 0, wxEXPAND | wxALL & ~wxBOTTOM, 10);

    wxBoxSizer *hboxValue = new wxBoxSizer(wxHORIZONTAL);
    hboxValue->Add(new wxStaticText(this, wxID_ANY, wxT("Value")), 1, wxALIGN_RIGHT | wxALIGN_TOP | wxEXPAND);

    m_txtValue = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), wxTE_MULTILINE, wxTextValidator(wxFILTER_EMPTY | wxFILTER_DIGITS));
    hboxValue->Add(m_txtValue, 4, wxALIGN_LEFT );
    vbox->Add(hboxValue, 1, wxEXPAND | wxALL, 10);
    vbox->Add(-1,-1, 1, wxEXPAND);

    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    hboxButtons->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxRIGHT, 5);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 1, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

    SetSizer(vbox);
    Center();
}
