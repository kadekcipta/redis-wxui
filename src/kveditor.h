#ifndef KVEDITOR_H
#define KVEDITOR_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "redisconnection.h"

class RedisKV
{
private:
    RedisValue  m_value;
    wxString    m_key;

public:
    RedisKV(const wxString& key, const RedisValue& value):m_key(key), m_value(value)
    {
    }

    const wxString& GetKey() const { return m_key; }
    void SetKey(const wxString& key) { m_key = key; }

    const RedisValue& GetValue() const { return m_value; }
    void SetValue(const RedisValue& value) { m_value = value; }
};

class KVEditor : public wxDialog
{
private:
    wxTextCtrl  *m_txtKeyName;
    wxTextCtrl  *m_txtValue;

    virtual void OnInitDialog(wxInitDialogEvent &event);
public:
    KVEditor(wxWindow *parent, const wxString& title);
};

#endif // KVEDITOR_H
