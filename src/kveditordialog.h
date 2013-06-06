#ifndef KVEDITORDIALOG_H
#define KVEDITORDIALOG_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "redisconnection.h"

enum {
    ID_KEY_NAME,
    ID_VALUE,
    ID_TYPE
};

class KeyValueEditorDialog : public wxDialog
{
private:
    wxString    m_key;
    wxString    m_value;
    RedisValue  m_redisValue;

    void OnInitDialog(wxInitDialogEvent &event);
    void OnTypeSelected(wxCommandEvent &event);
    void CreateControls();
public:
    KeyValueEditorDialog(wxWindow *parent, const wxString& title, const wxString& key, const RedisValue& value);

    wxString GetKey() { return m_key; }
    RedisValue GetValue() const;
};

#endif // KVEDITORDIALOG_H
