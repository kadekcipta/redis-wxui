#ifndef KVEDITORDIALOG_H
#define KVEDITORDIALOG_H

#include "redisconnection.h"

class wxDialog;
class wxWindow;
class wxString;
class wxCommandEvent;
class wxInitDialogEvent;

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
    RedisSimpleValue  m_redisValue;

    void OnInitDialog(wxInitDialogEvent &event);
    void OnTypeSelected(wxCommandEvent &event);
    void CreateControls();
public:
    KeyValueEditorDialog(wxWindow *parent, const wxString& title, const wxString& key, const RedisSimpleValue& value);

    wxString GetKey() { return m_key; }
    RedisSimpleValue GetValue() const;
};

#endif // KVEDITORDIALOG_H
