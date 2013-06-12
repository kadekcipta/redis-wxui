#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "kveditordialog.h"

KeyValueEditorDialog::KeyValueEditorDialog(wxWindow *parent, const wxString& title, const wxString& key, const RedisSimpleValue& value):
    wxDialog(parent, wxID_ANY, title,  wxDefaultPosition, wxSize(-1,-1)), m_key(key), m_redisValue(value)
{
    // currently doesn't use since redis doesn's have dedicated integer type
    if (value.GetValueType() == REDIS_STR_VALUE)
        m_value = value.GetStrValue();
    else
        m_value = wxString::Format("%d", value.GetIntValue());

    Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(KeyValueEditorDialog::OnInitDialog));
}

void KeyValueEditorDialog::CreateControls()
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    vbox->Add(new wxStaticText(this, wxID_ANY, wxT("It will overwrite the value of the existing key with the same name.")), 1, wxEXPAND | wxALL, 8);
    vbox->AddSpacer(10);

    wxBoxSizer *hboxKeyName = new wxBoxSizer(wxHORIZONTAL);
    hboxKeyName->Add(new wxStaticText(this, wxID_ANY, wxT("Key Name")), 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxRIGHT, 8);
    wxTextCtrl *txtKeyName = new wxTextCtrl(this, ID_KEY_NAME, wxEmptyString, wxDefaultPosition, wxSize(500, -1));
    hboxKeyName->Add(txtKeyName, 4, wxALIGN_LEFT );
    vbox->Add(hboxKeyName, 0, wxEXPAND | wxALL & ~wxBOTTOM, 8);

    wxBoxSizer *hboxValue = new wxBoxSizer(wxHORIZONTAL);
    hboxValue->Add(new wxStaticText(this, wxID_ANY, wxT("Value")), 1, wxALIGN_RIGHT | wxALIGN_TOP | wxEXPAND | wxRIGHT, 8);
    wxTextCtrl *txtValue = new wxTextCtrl(this, ID_VALUE, "", wxDefaultPosition, wxSize(-1, 250), wxTE_MULTILINE);
    hboxValue->Add(txtValue, 4, wxALIGN_LEFT );
    vbox->Add(hboxValue, 0, wxEXPAND | wxALL, 8);

    vbox->AddSpacer(10);

    wxBoxSizer *hboxButtons = new wxBoxSizer(wxHORIZONTAL);
    hboxButtons->Add(new wxButton(this, wxID_OK, wxT("OK")), 0, wxRIGHT, 8);
    hboxButtons->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), 0, wxBOTTOM);
    vbox->Add(hboxButtons, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 8);

    SetSizer(vbox);
    GetSizer()->Fit(this);
    Center();
}

void KeyValueEditorDialog::OnInitDialog(wxInitDialogEvent &event)
{
    CreateControls();
    FindWindow(ID_KEY_NAME)->SetValidator(wxTextValidator(wxFILTER_EMPTY, &m_key));
    FindWindow(ID_VALUE)->SetValidator(wxTextValidator(wxFILTER_EMPTY, &m_value));

    wxDialog::OnInitDialog(event);
}

// Currently this just works for the string value
RedisSimpleValue KeyValueEditorDialog::GetValue() const
{
    // type of integer
    long v = 0;
    if (m_value.ToLong(&v))
    {
        return RedisSimpleValue((int)v);
    }

    // type of string
    return RedisSimpleValue(m_value);
}
