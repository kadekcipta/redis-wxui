#include "mainframe.h"
#include "serverpanel.h"
#include "querypanel.h"
#include "connectiondialog.h"
#include "res/network.xpm"

MainFrame::MainFrame(const wxString& title): wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(950, 500))
{
    InitializeMenubar();
    InitializeControls();
    Center();
}

void MainFrame::InitializeControls()
{
    m_mainTab = new wxNotebook(this, ID_MAIN_TAB);
}

void MainFrame::InitializeMenubar()
{
    // creates menubar
    m_fileMenu = new wxMenu();
    m_fileMenu->Append(ID_MENU_CONNECT, wxT("Connect..."));
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(ID_MENU_SETTINGS, wxT("Settings..."))->Enable(false);
    m_fileMenu->AppendSeparator();
    m_fileMenu->Append(wxID_EXIT, wxT("E&xit"));

    m_editMenu = new wxMenu();
    m_editMenu->Append(ID_MENU_MODIFY_VALUE, wxT("Modify Value..."))->Enable(false);
    m_editMenu->Append(ID_MENU_DELETE, wxT("Delete Key"))->Enable(false);

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
    toolbar->AddTool(ID_MENU_CONNECT, wxT("Connect"), bm);
    SetToolBar(toolbar);

    // wire events
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnQuit));
    Connect(ID_MENU_CONNECT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAddConnection));
    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
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

void MainFrame::OnQuit(wxCommandEvent& evt)
{
    Close();
}

void MainFrame::OnClose(wxCloseEvent &evt)
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
