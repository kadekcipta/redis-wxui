#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/notebook.h>
#include "mainframe.h"
#include "querypanel.h"
#include "connectiondialog.h"
#include "kveditordialog.h"
#include "simplechart.h"
#include "res/network.xpm"

MainFrame::MainFrame(const wxString& title): wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(750, 700))
{
    InitializeMenubar();
    InitializeControls();
    Center();
}

void MainFrame::InitializeControls()
{
//    m_mainTab = new wxNotebook(this, ID_MAIN_TAB, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    // test chart
    TimeLogChart *chart = new TimeLogChart(this, wxID_ANY, wxT("Memory Status"));
}

void MainFrame::InitializeMenubar()
{
    // creates menubar
    m_fileMenu = new wxMenu();
    m_fileMenu->Append(ID_MENU_CONNECT, wxT("Add Connection..."));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_MENU_ADD_KV, wxT("New Key Value Pair..."));
    m_fileMenu->Append(ID_MENU_SELECT_DB, wxT("Select Database..."));
    m_fileMenu->Append(ID_MENU_DISCONNECT, wxT("Disconnect"));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_MENU_SETTINGS, wxT("Settings..."))->Enable(false);
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(wxID_EXIT, wxT("E&xit"));

    m_editMenu = new wxMenu();
    m_editMenu->Append(ID_MENU_EDIT_KEY_VALUE, wxT("Modify Value..."));
    m_editMenu->Append(ID_MENU_DELETE, wxT("Delete Key"));
    m_editMenu->AppendSeparator();
    m_editMenu->Append(ID_MENU_EXPIRE, wxT("Set Expiration"));

    m_helpMenu = new wxMenu();
    m_helpMenu->Append(ID_MENU_ABOUT, wxT("About..."));

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
    toolbar->AddTool(ID_MENU_CONNECT, wxT("Add Connection"), bm);
    toolbar->AddSeparator();
    SetToolBar(toolbar);

    // wire events

    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQuit));
    Connect(ID_MENU_CONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAddConnection));
    Connect(ID_MENU_DISCONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnCloseConnection));
    Connect(ID_MENU_ADD_KV, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAddKeyValuePair));
    Connect(ID_MENU_EDIT_KEY_VALUE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnEditKeyValue));
    Connect(ID_MENU_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnDeleteKey));
    Connect(ID_MENU_EXPIRE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnExpireKey));
    Connect(ID_MENU_SELECT_DB, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnSelectDb));

    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));

    Connect(ID_MENU_ADD_KV, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnRedisConnectedUpdateUI));
    Connect(ID_MENU_SELECT_DB, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnRedisConnectedUpdateUI));
    Connect(ID_MENU_DISCONNECT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnRedisConnectedUpdateUI));

    Connect(ID_MENU_DELETE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnKeySelectedUpdateUI));
    Connect(ID_MENU_EDIT_KEY_VALUE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnKeySelectedUpdateUI));
    Connect(ID_MENU_EXPIRE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::OnKeySelectedUpdateUI));

    Connect(wxEVT_IDLE, wxIdleEventHandler(MainFrame::OnIdle));
}

void MainFrame::OnKeySelectedUpdateUI(wxUpdateUIEvent &evt)
{
    QueryPanel *panel = GetActivePanel();
    if (panel != NULL)
    {
        evt.Enable(panel->GetSelectedKey() != "");
    }
    else
    {
        evt.Enable(false);
    }
}

void MainFrame::OnRedisConnectedUpdateUI(wxUpdateUIEvent &evt)
{
    evt.Enable(GetActivePanel() != NULL);
}

QueryPanel * MainFrame::GetActivePanel()
{
    if (m_mainTab != NULL && m_mainTab->GetPageCount() > 0)
    {
        return (QueryPanel*)m_mainTab->GetCurrentPage();
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
        RedisConnection *connection = new RedisConnection(dlg.GetRemoteHostName(), dlg.GetRemotePort());
        if (connection->Connect())
        {
            // query panel will take connection ownership
            QueryPanel *qpanel = new QueryPanel(m_mainTab, connection);
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

void MainFrame::OnQuit(wxCommandEvent& evt)
{
    Close();
}

void MainFrame::OnClose(wxCloseEvent &evt)
{
    if (m_mainTab != NULL && m_mainTab->GetPageCount() > 0)
    {

        wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Are you sure to quit?"), wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

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

void MainFrame::OnIdle(wxIdleEvent &evt)
{
//    if (GetActivePanel() != NULL)
//    {
//        SetStatusText(GetActivePanel()->GetServerInfo());
//        evt.RequestMore();
//    }
//    else
//    {
//        SetStatusText("");
//        evt.RequestMore();
//    }

    evt.Skip();
}
