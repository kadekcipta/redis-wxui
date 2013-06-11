#ifndef SELECTDBDIALOG_H
#define SELECTDBDIALOG_H

enum {
    ID_DB
};

class wxDialog;
class wxWindow;
class wxString;
class wxInitDialogEvent;

class SelectDbdialog : public wxDialog
{
private:
    wxString    m_currentDb;

    virtual void OnInitDialog(wxInitDialogEvent &event);
    void CreateControls();
public:
    SelectDbdialog(wxWindow *parent, const wxString& title, unsigned int db);

    int GetDb() const
    {
        unsigned long v = 0;
        if (m_currentDb.ToULong(&v))
        {
            return v;
        }

        return 0;
    }
};

#endif // SELECTDBDIALOG_H
