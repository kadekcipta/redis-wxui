#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/settings.h>
#include <wx/aui/auibook.h>
#include <wx/notebook.h>
#include "mainframe.h"
#include "querypanel.h"
#include "connectiondialog.h"
#include "kveditordialog.h"
#include "simplechart.h"
#include "res/network.xpm"

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
      wxFrame(parent, id, title, pos, size, style),
      m_mainTab(nullptr)
{
    InitializeMenubar();
    InitializeControls();
    Center();
    SetMinClientSize(wxSize(300, 350));
}

void MainFrame::InitializeControls()
{

    m_mainTab = new wxAuiNotebook(
                this,
                wxID_ANY,
                wxDefaultPosition,
                wxDefaultSize,
                wxAUI_NB_BOTTOM | wxAUI_NB_TAB_MOVE);


    auto tabArtProvider = new wxAuiSimpleTabArt;
    tabArtProvider->SetColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUHILIGHT));
    ((wxAuiNotebook*)m_mainTab)->SetArtProvider(tabArtProvider);

    wxBoxSizer *sizerMain = new wxBoxSizer(wxVERTICAL);
    sizerMain->Add(m_mainTab, 1, wxBOTTOM | wxEXPAND, 0);
    SetSizer(sizerMain);
    Layout();
}

void MainFrame::InitializeMenubar()
{
    // creates menubar
    m_fileMenu = new wxMenu();
    m_fileMenu->Append(wxID_ADD, wxT("Add Connection..."), wxT("Add new Redis connection"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(wxID_NEW, wxT("New Key Value Pair..."), wxT("Add new key value pair"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_MENU_SELECT_DB, wxT("Select Database..."), wxT("Select the database index"));
    m_fileMenu->Append(ID_MENU_DISCONNECT, wxT("Disconnect"), wxT("Close the current connection"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(wxID_EXIT, wxT("E&xit"));

    m_editMenu = new wxMenu();
    m_editMenu->Append(wxID_EDIT, wxT("Modify Value..."), wxT("Modify selected key's value"));
    m_editMenu->Append(wxID_DELETE, wxT("Delete Key"), wxT("Delete the selected key"));
    m_editMenu->AppendSeparator();
    m_editMenu->Append(ID_MENU_EXPIRE, wxT("Set Expiration"), wxT("Set expiration for selected key"));

    m_helpMenu = new wxMenu();
    m_helpMenu->Append(wxID_ABOUT, wxT("About..."));

    m_menubar = new wxMenuBar();
    m_menubar->Append(m_fileMenu, wxT("&File"));
    m_menubar->Append(m_editMenu, wxT("&Edit"));
    m_menubar->Append(m_helpMenu, wxT("&Help"));

    SetMenuBar(m_menubar);

    m_statusBar = new wxStatusBar(this, ID_MAIN_STATUS_BAR);
    SetStatusBar(m_statusBar);

    wxToolBar *toolbar = new wxToolBar(this, ID_MAIN_TOOL_BAR);
    long style = toolbar->GetWindowStyle() | wxTB_TEXT;
    toolbar->SetWindowStyle(style);

    wxBitmap bm(network_xpm);
    toolbar->AddTool(wxID_ADD, wxT("Add Connection"), bm);
    toolbar->AddSeparator();
    SetToolBar(toolbar);

    // wire events

    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAbout));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQuit));
    Connect(wxID_ADD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAddConnection));
    Connect(ID_MENU_DISCONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnCloseConnection));
    Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAddKeyValuePair));
    Connect(wxID_EDIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnEditKeyValue));
    Connect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnDeleteKey));
    Connect(ID_MENU_EXPIRE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnExpireKey));
    Connect(ID_MENU_SELECT_DB, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSelectDb));

    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
    Connect(wxID_NEW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnRedisConnectedUpdateUI));
    Connect(ID_MENU_SELECT_DB, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnRedisConnectedUpdateUI));
    Connect(ID_MENU_DISCONNECT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnRedisConnectedUpdateUI));

    Connect(wxID_DELETE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnKeySelectedUpdateUI));
    Connect(wxID_EDIT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnKeySelectedUpdateUI));
    Connect(ID_MENU_EXPIRE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnKeySelectedUpdateUI));
}

void MainFrame::OnKeySelectedUpdateUI(wxUpdateUIEvent &evt)
{
    ConnectionPanel *panel = GetActivePanel();
    evt.Enable(panel != NULL && panel->GetSelectedKey() != wxEmptyString);
}

void MainFrame::OnRedisConnectedUpdateUI(wxUpdateUIEvent &evt)
{
    evt.Enable(GetActivePanel() != NULL);
}

ConnectionPanel * MainFrame::GetActivePanel()
{
    if (m_mainTab != NULL && m_mainTab->GetPageCount() > 0)
    {
        return (ConnectionPanel*)m_mainTab->GetCurrentPage();
    }

    return NULL;
}

void MainFrame::OnEditKeyValue(wxCommandEvent &evt)
{
    if (GetActivePanel() != NULL)
    {
        GetActivePanel()->EditKeyValue();
    }
}

void MainFrame::OnDeleteKey(wxCommandEvent &evt)
{
    if (GetActivePanel() != NULL)
    {
        GetActivePanel()->DeleteKey();
    }
}

void MainFrame::OnExpireKey(wxCommandEvent &evt)
{
    if (GetActivePanel() != NULL)
    {
        GetActivePanel()->ExpireKey();
    }
}

void MainFrame::OnSelectDb(wxCommandEvent &evt)
{
    if (GetActivePanel() != NULL)
    {
        GetActivePanel()->SelectDb();
    }
}

void MainFrame::OnAddKeyValuePair(wxCommandEvent &evt)
{
    if (GetActivePanel() != NULL)
    {
        GetActivePanel()->AddKeyValue();
    }
}

void MainFrame::AddConnection()
{
    ConnectionDialog dlg(this, wxT("Add Connection"));
    if (dlg.ShowModal() == wxID_OK)
    {

        RedisConnection *connection = new RedisConnection(dlg.GetRemoteHostName(), dlg.GetRemotePort(), wxEmptyString, dlg.GetPassword());
        if (connection->Connect())
        {
            // query panel will take connection ownership
            ConnectionPanel *qpanel = new ConnectionPanel(m_mainTab, connection);
            m_mainTab->AddPage(qpanel, connection->GetTitle());
        }
        else
        {
            wxString message = connection->GetLastError();
            delete connection;

            wxMessageBox(message);
        }
    }
}

void MainFrame::OnAddConnection(wxCommandEvent &evt)
{
    AddConnection();
}

void MainFrame::OnCloseConnection(wxCommandEvent &evt)
{
    if (GetActivePanel() != NULL)
    {
        GetActivePanel()->CloseConnection();
    }
}

void MainFrame::OnAbout(wxCommandEvent &evt)
{
    wxMessageBox(wxString::Format(wxT("%s\n"), APP_NAME), wxT("About"));
}

void MainFrame::OnQuit(wxCommandEvent& evt)
{
    Close();
}

void MainFrame::OnClose(wxCloseEvent &evt)
{
    if (m_mainTab != NULL && m_mainTab->GetPageCount() > 0)
    {
        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Close all the active connections and quit?"), wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

        int ret = dial->ShowModal();
        dial->Destroy();

        if (ret == wxID_YES)
        {
              Destroy();
        } else
        {
              evt.Veto();
        }
    }
    else
    {
        Destroy();
    }
}
