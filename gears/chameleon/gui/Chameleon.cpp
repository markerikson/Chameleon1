#include "ChameleonWindow.h"
#include <wx/fdrepdlg.h>

#include <wx/config.h>
#include <wx/msw/iniconf.h>
#include <wx/timer.h>
#include <wx/regex.h>
#include <wx/splash.h>
#include <wx/utils.h>
#include "../common/debug.h"
#include "../common/datastructures.h"

#ifdef MSVC6
#include "../common/fixvsbug.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_EVENT_TABLE(ChameleonWindow, wxFrame)
	EVT_MENU						(ID_NEW, ChameleonWindow::OnFileNew)
	EVT_MENU						(ID_OPEN, ChameleonWindow::OnFileOpen)
	EVT_MENU						(ID_QUIT,  ChameleonWindow::OnQuit)
	EVT_MENU						(ID_ABOUT, ChameleonWindow::OnAbout)
	EVT_MENU						(ID_TEST, ChameleonWindow::Test)
	EVT_MENU						(ID_SAVE, ChameleonWindow::OnSave)
	EVT_MENU						(ID_SAVE_AS, ChameleonWindow::OnSaveAs)
	EVT_MENU						(ID_PAGECLOSE, ChameleonWindow::OnFileClose)
	EVT_MENU						(ID_CLOSETAB, ChameleonWindow::CloseTab)
	EVT_UPDATE_UI					(ID_SAVE, ChameleonWindow::OnUpdateSave)
	EVT_MENU						(ID_STARTCONNECT, ChameleonWindow::OnConnect)
	EVT_MENU						(ID_UNDO, ChameleonWindow::OnUndo)
	EVT_MENU						(ID_REDO, ChameleonWindow::OnRedo)
	EVT_MENU						(ID_OPTIONS, ChameleonWindow::OnToolsOptions)
	EVT_MENU						(ID_CUT, ChameleonWindow::OnCut)
	EVT_MENU						(ID_COPY, ChameleonWindow::OnCopy)
	EVT_MENU						(ID_PASTE, ChameleonWindow::OnPaste)
	EVT_CLOSE						(ChameleonWindow::OnClose)
	EVT_NOTEBOOK_PAGE_CHANGED		(ID_NOTEBOOK_ED,   ChameleonWindow::OnPageChange)
	//EVT_SPLITTER_SASH_POS_CHANGED	(ID_SPLITTER, ChameleonWindow::OnTermResize)
	
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)
//----------------------------------------------------------------------
// `Main program' equivalent: the program execution "starts" here

bool MyApp::OnInit()
{
    ChameleonWindow *frame = new ChameleonWindow("Chameleon",
                                 wxPoint(5, 5), wxSize(400, 300));

    frame->Show(TRUE);
    return TRUE;
}

//----------------------------------------------------------------------


#pragma hdrstop


// frame constructor
ChameleonWindow::ChameleonWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	wxStopWatch stopwatch;
	this->SetClientSize(640, 480);

	// Show a log window for all debug messages
#ifdef DEBUG
	logWindow = new wxLogWindow(this, "Debug messages");
	wxLog::SetActiveTarget(logWindow);
#endif

	wxIcon icon(moz_xpm);
	SetIcon(icon);
	

	m_remoteMode = true;


	long time1 = stopwatch.Time();
	stopwatch.Start();


	

	// enable only PERM_AUTOINDENT and PERM_REMOTELOCAL.  Eventually, will rely only on the provided value.

	/*
	int permBitmask = 0;

	permBitmask |= 1 << PERM_AUTOINDENT;
	permBitmask |= 1 << PERM_REMOTELOCAL;
	permBitmask |= 1 << PERM_SYNTAXHIGHLIGHT;
	permBitmask |= 1 << PERM_TERMINAL;

	m_perms->setGlobalAuthorized(permBitmask);

	
	for(int i = PERM_FIRST; i < PERM_LAST; i++)
	{
		m_perms->disable(i);
	}
	
	m_perms->enable(PERM_AUTOINDENT);
	m_perms->enable(PERM_REMOTELOCAL);
	m_perms->enable(PERM_SYNTAXHIGHLIGHT);
	m_perms->enable(PERM_TERMINAL);

	*/
	long time2 = stopwatch.Time();
	stopwatch.Start();


	// Open up the configuration file, assumed to be in the user's home directory

	wxString hostname = wxEmptyString; 
	wxString username = wxEmptyString;
	long authorizedCode;
	long enabledCode;

	// by default, enable nothing
	long defaultEnableCode = 0x0;
	long defaultAuthorizedCode = 0x0;

	wxFileName configName(wxGetHomeDir(), "chameleon.ini");

	m_config = new wxIniConfig("Chameleon", wxEmptyString, configName.GetFullPath());

	if(configName.FileExists())
	{

		hostname = m_config->Read("Network/hostname");
		username = m_config->Read("Network/username");

		authorizedCode = m_config->Read("Permissions/authorized", defaultAuthorizedCode);
		enabledCode = m_config->Read("Permissions/enabled", defaultEnableCode);
		wxLogDebug("Loaded permissions from config: authorizedCode = %u, enabledCode = %u", authorizedCode, enabledCode);

	}
	else
	{
		wxLogDebug("Failed to locate config file, loading default permissions");
		authorizedCode = defaultAuthorizedCode;
		enabledCode = defaultEnableCode;

		m_config->Write("Network/hostname", wxEmptyString);
		m_config->Write("Network/username", wxEmptyString);
		m_config->Write("Permissions/authorized", defaultAuthorizedCode);
		m_config->Write("Permissions/enabled", defaultEnableCode);

	}

	m_perms = new Permission(authorizedCode, enabledCode);
	

	m_network = new Networking();

	wxFileName plinkPath(wxGetCwd(), "plink.exe");
	m_network->SetPlinkProg(plinkPath.GetFullPath());

	wxFileName pscpPath(wxGetCwd(), "pscp.exe");
	m_network->SetPlinkProg(pscpPath.GetFullPath());

	m_network->SetDetailsNoStatus(hostname, username, "");


	long time3 = stopwatch.Time();
	stopwatch.Start();
	
	m_optionsDialog = new OptionsDialog(this, ID_OPTIONSDIALOG, "Options");
	UpdatePermsList();

	m_optionsDialog->SetServerAddress(hostname);
	m_optionsDialog->SetUsername(username);
	m_optionsDialog->SetPassword1(wxEmptyString);
	m_optionsDialog->SetPassword2(wxEmptyString);


	long time4 = stopwatch.Time();
	stopwatch.Start();
	 


    // Menu creation
    wxMenu* menuFile = new wxMenu();

	menuFile->Append(ID_NEW, "&New\tCtrl-N", "Create a new file");
	menuFile->Append(ID_OPEN, "&Open\tCtrl-O", "Open an existing file");
	menuFile->Append(ID_PAGECLOSE, "&Close");
	menuFile->InsertSeparator(3);
	menuFile->Append(ID_SAVE, "&Save\tCtrl-S", "Save the current file");
	menuFile->Append(ID_SAVE_AS, "Save &As", "Save the current file as a different name");
	menuFile->Append(ID_QUIT, "E&xit\tAlt-X", "Quit this program");

	wxMenu* menuTools = new wxMenu();

	menuTools->Append(ID_OPTIONS, "&Options");

	//menuTools->Append(ID_STARTCONNECT, "&Connect");
	//menuTools->InsertSeparator(2);

	wxMenu* menuEdit = new wxMenu();

	menuEdit->Append(ID_UNDO, "&Undo\tCtrl-Z");
	menuEdit->Append(ID_REDO, "&Redo\tCtrl-Y");
	menuEdit->InsertSeparator(2);
	menuEdit->Append(ID_CUT, "Cu&t\tCtrl-X");
	menuEdit->Append(ID_COPY, "&Copy\tCtrl-C");
	menuEdit->Append(ID_PASTE, "&Paste\tCtrl-V");
	

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_ABOUT, "&About...\tCtrl-A", "Show about dialog");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
	menuBar->Append(menuTools, "&Tools");
    menuBar->Append(helpMenu, "&Help");

	
	wxToolBar* toolBar = CreateToolBar(wxTB_FLAT | wxTB_TEXT);

	wxBitmap bmNew(new_xpm);
	toolBar->AddTool(ID_NEW, "New File", bmNew);	
		
	wxBitmap bmOpen(open_xpm);
	toolBar->AddTool(ID_OPEN, "Open File", bmOpen);

	wxBitmap bmSave(save_xpm);
	toolBar->AddTool(ID_SAVE, "Save File", bmSave);

	/*
	toolBar->InsertSeparator(3);
	
	wxBitmap bmBuild(build_xpm);
	toolBar->AddTool(ID_COMPILE, "Compile", bmBuild);

	toolBar->InsertSeparator(5);

	wxBitmap bmTest(button_xpm);
	toolBar->AddTool(ID_TEST, "Test", bmTest);

	toolBar->InsertSeparator(7);
	*/
	

	/*
	toolBar->AddTool(ID_OPEN_REMOTE, "Remote Open", bmOpen);
	toolBar->AddTool(ID_SAVE_REMOTE, "Remote Save", bmSave);
	*/
	

	if(m_perms->isEnabled(PERM_DEBUG))
	{
		AddDebugButtons();
	}

	toolBar->Realize();

	SetToolBar(toolBar);
	
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif


	long time5 = stopwatch.Time();
	stopwatch.Start();

	m_remoteFileDialog = new RemoteFileDialog(this, ID_REMOTEFILEDIALOG);

	long time6 = stopwatch.Time();
	stopwatch.Start();
	m_remoteFileDialog->SetNetworking(m_network);
	long time8 = stopwatch.Time();
	stopwatch.Start();

	m_split = new wxSplitterWindow(this, ID_SPLITTER);

	m_book = new ChameleonNotebook(m_split, ID_NOTEBOOK_ED);

	//uih = new UpdateUIHandler(this);
	//ed->PushEventHandler(uih);

	m_currentPage = 0;
	m_fileNum = 0;

	m_appClosing = false;
	m_setSelection = false;

	// create the initial blank open file
	wxCommandEvent ev;
	OnFileNew(ev);
	PageHasChanged(m_currentPage);

	// set up the terminal portion of the GUI
	m_noteTerm = new ChameleonNotebook(m_split, ID_NOTEBOOK_TERM);
	m_telnet = new wxTelnet( m_noteTerm, ID_TELNET, wxPoint(-1, -1), 80, 24);
	m_noteTerm->AddPage(m_telnet, "Terminal");
	
	if(m_perms->isEnabled(PERM_TERMINAL))
	{
		m_split->SplitHorizontally(m_book, m_noteTerm, -200);		
		//m_split->SetMinimumPaneSize(200);


	}
	else
	{
		m_split->Initialize(m_book);
	}
	
	long time7 = stopwatch.Time();
	stopwatch.Pause();

	wxLogDebug("Startup timing: initial lines = %d", time1);
	wxLogDebug("Startup timing: permissions = %d", time2);
	wxLogDebug("Startup timing: config = %d", time3);
	wxLogDebug("Startup timing: OptionsDialog = %d", time4);
	wxLogDebug("Startup timing: menus = %d", time5);
	wxLogDebug("Startup timing: RemoteFileDialog() = %d", time6);
	wxLogDebug("Startup timing: rfd->SetNetwork = %d", time8);
	wxLogDebug("Startup timing: notebook and editor = %d", time7);

}

ChameleonWindow::~ChameleonWindow()
{
	m_config->Flush();

	delete m_perms;
	//delete m_openFiles;
	delete m_optionsDialog;
	delete m_network;
	delete m_config;


}

// create a new blank file
void ChameleonWindow::OnFileNew (wxCommandEvent &WXUNUSED(event)) 
{
	m_fileNum += 1;
	wxString noname = "<untitled> " + wxString::Format ("%d", m_fileNum);
	ChameleonEditor* edit = new ChameleonEditor (this, m_book, -1);
	m_currentEd = edit;
	m_currentPage = m_book->GetPageCount();
	//m_currentEd->SetFileNameAndPath(wxEmptyString, wxEmptyString, true);
	m_book->AddPage (edit, noname, true);
}

// called whenever the active tab is changed
void ChameleonWindow::PageHasChanged (int pageNr) 
{
	// no pages - null out the current ed pointer
	if (m_book->GetPageCount() == 0) 
	{
		m_currentPage = -1;
		m_currentEd = NULL;
		//m_openFiles->SetSelection (0);
		return;
	}

	// no page passed in
	if (pageNr == -1)
	{
		pageNr = m_book->GetSelection();
		
	}

	if (m_book->GetPageCount() <= pageNr)
	{
		pageNr = pageNr - 1;
	}

	// activate the selected page
	if (pageNr >= 0) 
	{
		m_currentPage = pageNr;
		m_currentEd = (ChameleonEditor *) m_book->GetPage (m_currentPage);
		m_book->SetSelection(pageNr);
		m_currentEd->SetFocus();
		//m_files->SetSelection (m_currentPage);
	}
	else
	{
		m_currentPage = -1;
		m_currentEd = NULL;
		//m_files->SetSelection (0);
	}
	m_book->Refresh();
	if(m_currentEd != NULL)
	{
		m_currentEd->Refresh();
	}
}

// event handler for closing the whole program
void ChameleonWindow::OnClose(wxCloseEvent &event) 
{
	// close all open files
	int cnt = m_book->GetPageCount();

	m_appClosing = true;

	for (int i = 0; i < cnt; i++) 
	{
		CloseFile();
	}
	

	PageHasChanged();

	// double check in case something went wrong
	if (m_currentEd && m_currentEd->Modified()) 
	{
		if (event.CanVeto())
		{
			event.Veto (true);
		}

		return;
	}

	Destroy();
}

void ChameleonWindow::CloseTab()
{
	int tab = GetIntVar(VN_CLICKEDTAB);
	CloseFile(tab);
}

void ChameleonWindow::CloseFile (int pageNr) 
{
	if (pageNr == -1)
	{
		pageNr = m_book->GetSelection();
	}

	// gives the user a chance to save if the file has been modified
	int modifiedFileResult = HandleModifiedFile(pageNr, true);

	// a wxYES result is taken care of inside HandleModifiedFile, and a
	// wxNO is handled implicitly by the fact that the file isn't saved.
	if(modifiedFileResult == wxCANCEL)
	{
		return;
	}

	if (m_book->GetPageCount() > 0) 
	{
		wxString currentFileName = wxEmptyString;
		if ((m_book->GetPageCount() > 1) || m_appClosing) 
		{
			currentFileName = m_currentEd->GetFileNameAndPath();
			ChameleonEditor* pEdit = (ChameleonEditor*)m_book->GetPage(pageNr);
			m_book->DeletePage (pageNr);
		}
		// closing out the last buffer, reset it to act as a new one
		else
		{
			m_fileNum = 1;
			m_currentEd->ClearAll();
			m_currentEd->SetSavePoint();
			m_currentEd->EmptyUndoBuffer();
			wxString noname = "<untitled> " + wxString::Format ("%d", m_fileNum);
			m_book->SetPageText (pageNr, noname);
			m_currentEd->SetFileNameAndPath(wxEmptyString, wxEmptyString, true);
		}
		if(m_book->GetPageCount() > 0)
		{
			//wxString currentEditorFileName = m_currentEd->GetFileNameAndPath();
			if(currentFileName != wxEmptyString)
			{
				int newSelectedPageNum = GetPageNum(currentFileName);
				PageHasChanged(newSelectedPageNum);
			}
		}		
	}
}

// gives the user a chance to save if the file's been modified
int ChameleonWindow::HandleModifiedFile(int pageNr, bool closingFile)
{
	ChameleonEditor *edit = (ChameleonEditor *) m_book->GetPage (pageNr);

	if (!edit)
	{
		return wxCANCEL;
	}

	if (edit->Modified()) 
	{
		//wxString fileName = wxFileName(edit->GetFilename()).GetFullName();
		wxString saveMessage = "The file ";
		wxString fileName = edit->GetFileNameAndPath();
		saveMessage += fileName;

		/*
		// the file hasn't been saved yet, grab the "<untitled> #" bit from the tab
		if(fileName = wxEmptyString)
		{
			int selectedTab = m_book->GetSelection();
			wxString tabText = m_book->GetPageText(selectedTab);

			int idx = tabText.Index('*');

			if(idx != wxNOT_FOUND)
			{
				tabText.Remove(idx, 1);
			}
			fileName = tabText;
		}
		saveMessage += fileName;
		*/
		saveMessage << " has unsaved changes.  Do you want to save them before the file is ";
		
		if(closingFile)
		{
			saveMessage += "closed?";
		}
		else
		{
			saveMessage += "reloaded?";
		}

		int result = wxMessageBox (_(saveMessage), _("Close"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
		if( result == wxYES) 
		{
			//PageHasChanged(pageNr);
			ChameleonEditor* tmpCurrentEd = m_currentEd;
			m_currentEd = edit;
			// only do a Save As if necessary
			SaveFile(false);
			m_currentEd = tmpCurrentEd;
			m_currentEd->SetFocus();

			if (edit->Modified()) 
			{

				wxString errorMessage = fileName + " could not be saved!";
				wxMessageBox (_(errorMessage), _("Close abort"),
					wxOK | wxICON_EXCLAMATION);
			}
		}

		return result;
	}
	// if I'm here, doesn't matter if I return wxNO or wxYES, just as long as it's not wxCANCEL
	return wxNO;
}

void ChameleonWindow::OnFileOpen (wxCommandEvent &WXUNUSED(event)) 
{
	OpenFile();
}

void ChameleonWindow::OpenFile()
{
	if (!m_currentEd)
	{
		return;
	}

	wxArrayString fnames;

	if(! (m_perms->isEnabled(PERM_REMOTELOCAL) && m_remoteMode))
	{
		wxFileDialog dlg (this, _("Open files"), "", "", "C++ files (*.cpp, *.h)|*.cpp;*.h",
			wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE | wxCHANGE_DIR);
		if (dlg.ShowModal() != wxID_OK)
		{
			return;
		}
		m_currentEd->SetFocus();
		dlg.GetPaths (fnames);
	}
	// in remote mode
	else
	{
		SetStatusText("Showing remote file dialog...");
		wxBeginBusyCursor();

		if(m_remoteFileDialog->Prepare(true))
		{
			wxEndBusyCursor();
			int result = m_remoteFileDialog->ShowModal();
			m_currentEd->SetFocus();

			if(result != wxOK)
			{
				return;
			}

			wxString remoteFileName = m_remoteFileDialog->GetRemoteFileNameAndPath();
			fnames.Add(remoteFileName);
		}
		else
		{
			wxEndBusyCursor();
			return;
		}
	}

	OpenFile (fnames);
	PageHasChanged (m_currentPage);
}

void ChameleonWindow::OpenFile (wxArrayString fnames) 
{
	int firstPageNr = -1;
	wxString fileContents = wxEmptyString;
	wxString fileNameNoPath;


	for (size_t n = 0; n < fnames.GetCount(); n++) 
	{
		fileNameNoPath = wxEmptyString;
		int pageNr = GetPageNum(fnames[n]);

		// filename is already open
		if (pageNr >= 0) 
		{
			int modifiedFileResult = HandleModifiedFile(pageNr, false);

			// user canceled the open request, skip the reload
			if(modifiedFileResult == wxCANCEL)
			{
				continue;
			}
			m_setSelection = true;
			m_book->SetSelection (pageNr);
			m_setSelection = false;
			m_currentPage = pageNr;
			
			
			if(GetFileContents(fnames[n], fileContents, fileNameNoPath))
			{
				if(fileContents != wxEmptyString)
				{
					m_currentEd->LoadFileText(fileContents);
				}
			}
		}
		// current buffer is empty and untouched, so load the file into it
		else if (m_currentEd && (!m_currentEd->Modified() && !m_currentEd->HasBeenSaved() && m_currentEd->GetText().IsEmpty())) 
		{
			if(GetFileContents(fnames[n], fileContents, fileNameNoPath))
			{
				if(fileContents != wxEmptyString)
				{
					m_currentEd->LoadFileText(fileContents);
					//m_currentEd->SetFilename(fnames[n]);

					// TODO this may be redundant.  I'll deal with it when we decide on how to handle remote mode
					if(m_remoteMode)
					{
						wxString remotePath = m_remoteFileDialog->GetRemotePath();
						wxString remoteFilename = m_remoteFileDialog->GetRemoteFileName();
						//m_currentEd->SetRemoteFileNameAndPath(remotePath, remoteFilename);
						m_currentEd->SetFileNameAndPath(remotePath, remoteFilename, true);
					}
					else
					{
						wxFileName localFN(fnames[n]);
						wxString localFilename = localFN.GetFullName();
						wxString localPath = localFN.GetPath(false, wxPATH_DOS);
						//m_currentEd->SetLocalFileNameAndPath(localPath, localFileName);
						m_currentEd->SetFileNameAndPath(localPath, localFilename, false);
					}

					m_book->SetPageText(m_currentPage, fileNameNoPath);
				}
			}
		}
		// need to create a new buffer for the file
		else
		{
			ChameleonEditor *edit = new ChameleonEditor (this, m_book, -1);

			if(GetFileContents(fnames[n], fileContents, fileNameNoPath))
			{
				if(fileContents != wxEmptyString)
				{
					edit->LoadFileText(fileContents);
//					edit->SetFilename(fnames[n]);

					// TODO ditto as with the previous else
					if(m_remoteMode)
					{
						wxString remotePath = m_remoteFileDialog->GetRemotePath();
						wxString remoteFilename = m_remoteFileDialog->GetRemoteFileName();
						//edit->SetRemoteFileNameAndPath(remotePath, remoteFilename);
						edit->SetFileNameAndPath(remotePath, remoteFilename, true);
					}
					else
					{
						wxFileName localFN(fnames[n]);
						wxString localFileName = localFN.GetFullName();
						wxString localPath = localFN.GetPath(false, wxPATH_DOS);
						//edit->SetLocalFileNameAndPath(localPath, localFileName);
						edit->SetFileNameAndPath(localPath, localFileName, false);
					}
					m_currentEd = edit;
					m_currentPage = m_book->GetPageCount();
					m_book->AddPage (m_currentEd, fileNameNoPath, true);
				}
			}
			// failed to load the file contents, zap the new editor
			else
			{
				delete edit;
			}
		}

		if (firstPageNr < 0)
		{
			firstPageNr = m_currentPage;
		}
	}

	// show the active tab, new or otherwise
	if (firstPageNr >= 0) 
	{
		m_currentPage = firstPageNr;
		m_setSelection = true;
		m_book->SetSelection (m_currentPage);
		m_setSelection = false;
	}
}

// abstracts out retrieving a file's contents
bool ChameleonWindow::GetFileContents(wxString fileToLoad, wxString &fileContents, wxString &fileName)
{
	wxFileName fn(fileToLoad);
	if(m_remoteMode)
	{
		wxString remotePath = fn.GetPath(false, wxPATH_UNIX);
		wxString remoteFile = fn.GetFullName();

		wxBeginBusyCursor();
		fileContents = m_network->GetFileContents(remoteFile, remotePath);
		wxEndBusyCursor();
		
		NetworkCallResult netStatus = CheckNetworkStatus();
		if(netStatus == NETCALL_REDO)
		{
			wxBeginBusyCursor();
			fileContents = m_network->GetFileContents(remoteFile, remotePath);
			wxEndBusyCursor();
		}
		else if(netStatus == NETCALL_FAILED)
		{
			return false;
		}
	}
	else
	{
		wxFile file (fileToLoad);

		if( !file.IsOpened() )
		{
			return false;

		}

		long lng = file.Length();

		if( lng > 0 )
		{
			char *pFileContents = fileContents.GetWriteBuf(lng);

			file.Read(pFileContents, lng);

			fileContents.UngetWriteBuf();
		}

	}

	fileName = fn.GetFullName();
	return true;   
}


// if the filename is already open, return the tab it's in
int ChameleonWindow::GetPageNum (const wxString &fname) 
{
	ChameleonEditor *edit;
	for (int pageNum = 0; pageNum < m_book->GetPageCount(); pageNum++) 
	{
		edit = (ChameleonEditor *) m_book->GetPage(pageNum);

		if (edit && (edit->GetFileNameAndPath() == fname))
		{			
			return pageNum;
		}
	}

	return -1;
}

void ChameleonWindow::OnPageChange (wxNotebookEvent &WXUNUSED(event)) 
{
	if (!m_setSelection)
	{
		PageHasChanged();
	}
}



void ChameleonWindow::OnPageClose (wxCommandEvent &event) 
{
	if (m_clickedTabNum >= 0) 
	{
		CloseFile(m_clickedTabNum);
		PageHasChanged(m_clickedTabNum);
	}
}

void ChameleonWindow::OnFileClose (wxCommandEvent &WXUNUSED(event)) 
{
	if (!m_currentEd) 
	{
		return;
	}

	CloseFile();
	PageHasChanged();
}

void ChameleonWindow::OnConnect(wxCommandEvent& WXUNUSED(event))
{
	wxIPV4address local;
	local.LocalHost();

	wxString hostName = "163.11.42.198";
	//("Address: " + local.Hostname())
	wxLogDebug("Connecting to address: %s", hostName);
	//m_telnet->Connect(local.Hostname(), 3012);
	m_telnet->Connect(hostName, 23);
	wxLogDebug("Connected: %d", m_telnet->IsConnected());

}


// my "I need to try something out, I'll stick it in here" function
void ChameleonWindow::Test(wxCommandEvent& WXUNUSED(event))
{

	wxString fingerprint = "blah blah 1024 37:d3:00:80:6d:69:8d:ff:cd:81:20:a2:8a:93:39:ba blah blah";
	wxRegEx reFingerprint = "[[:digit:]]+[[:blank:]]+([[:xdigit:]]{2}:){15}[[:xdigit:]]{2}";

	if(reFingerprint.IsValid())
	{
		if(reFingerprint.Matches(fingerprint))
		{
			wxString match = reFingerprint.GetMatch(fingerprint);
			wxLogDebug("Matched: \"%s\"", match);
		}
		else
		{
			wxLogDebug("Failed match");
		}
	}
	else
	{
		wxLogDebug("Invalid regex");
	}
	}


// zap the program frame
void ChameleonWindow::OnCloseWindow(wxCloseEvent& event)
{
	this->Destroy();
}


void ChameleonWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void ChameleonWindow::CloseApp()
{

}

void ChameleonWindow::OnUndo(wxCommandEvent &event)
{
	m_currentEd->Undo();
	m_book->Refresh();
}

void ChameleonWindow::OnRedo(wxCommandEvent &event)
{
	m_currentEd->Redo();
	m_book->Refresh();
}

void ChameleonWindow::OnCopy(wxCommandEvent &event)
{
	m_currentEd->Copy();
}

void ChameleonWindow::OnCut(wxCommandEvent &event)
{
	m_currentEd->Cut();
}

void ChameleonWindow::OnPaste(wxCommandEvent &event)
{
	m_currentEd->Paste();
}



void ChameleonWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
	msg.Printf( _T("Chameleon: The Adaptive IDE"));
	msg += CHAMELEON_VERSION_NUM;

    wxMessageBox(msg, "About Chameleon", wxOK | wxICON_INFORMATION, this);
}

// not currently used.  may come into play later regarding terminal resizing
void ChameleonWindow::CheckSize()
{
	int x, y;
	this->GetClientSize(&x, &y);
	wxString result;
	result << x << ", " << y;
	::wxMessageBox(result );
	m_currentEd->SetScrollWidth(x);
	
}

// wxWindows calls this repeatedly during empty processing time.  This updates
// the status of the save button/menu item, as well as the * on the active tab if modified
void ChameleonWindow::OnUpdateSave(wxUpdateUIEvent &event)
{
	bool enable = m_currentEd->Modified();

	int tabNum = m_book->GetSelection();
	wxString title = m_book->GetPageText(tabNum);

	if(enable)
	{
		int tabNum = m_book->GetSelection();
		wxString title = m_book->GetPageText(tabNum);

		if(!title.Contains("*"))
		{
			title += "*";
			m_book->SetPageText(tabNum, title);
			m_book->Refresh();
		}		
	}
	else
	{
		if(title.Contains("*"))
		{
			title.RemoveLast(1);
			m_book->SetPageText(tabNum, title);
			m_book->Refresh();
		}
	}
	event.Enable(enable);
}


void ChameleonWindow::OnSave(wxCommandEvent& WXUNUSED(event))
{
	SaveFile(false);
}

void ChameleonWindow::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	SaveFile(true);
}

void ChameleonWindow::SaveFile(bool saveas)
{
	wxString filename;

	bool doSaveAs = saveas || !m_currentEd->HasBeenSaved() || (m_remoteMode != m_currentEd->LastSavedRemotely());

	// check if we're in local mode or not
	if(!(m_perms->isEnabled(PERM_REMOTELOCAL) && m_remoteMode))
	{
		if(doSaveAs)
		{		
			// the last item in a filter's list will be the default extension if none is given
			// ie, right now, .cpp is the default extension for C++ files
			wxFileDialog dlg (this, _T("Save file"), _T(""), 
				_T(""), _T("C++ files (*.cpp, *.h, *.c)|*.c;*.h;*.cpp|All files (*.*)|*.*"),
							wxSAVE | wxOVERWRITE_PROMPT | wxCHANGE_DIR);

			// ie, user clicked cancel
			if(dlg.ShowModal() != wxID_OK) 
			{ 
				return; 
			}

			m_currentEd->SetFocus();
			
			filename = dlg.GetPath();
			wxFileName fn(filename);

			
			m_currentEd->SetFileNameAndPath(fn.GetPath(false, wxPATH_DOS), fn.GetFullName(), false);
			
			m_currentEd->SaveFile(filename);
		}
		else
		{
			m_currentEd->SaveFileLocal();
		}
	}
	// remote mode
	else
	{
		wxString remotePath, remoteFile, fileContents;

		fileContents = m_currentEd->GetText();
		if(doSaveAs)
		{
			if(m_remoteFileDialog->Prepare(false))
			{
				int result = m_remoteFileDialog->ShowModal();
				m_currentEd->SetFocus();

				if(result != wxOK)
				{
					return;
				}

				remotePath = m_remoteFileDialog->GetRemotePath();
				remoteFile = m_remoteFileDialog->GetRemoteFileName();
			}
			else
			{
				return;
			}
			
		}
		else
		{
			//remoteFile = m_currentEd->GetRemoteFileName();
			//remotePath = m_currentEd->GetRemotePath();
			remoteFile = m_currentEd->GetFilename();
			remotePath = m_currentEd->GetFilePath();
		}

		wxBeginBusyCursor();
		m_network->SendFileContents(fileContents, remoteFile, remotePath);
		wxEndBusyCursor();
		
		NetworkCallResult netStatus = CheckNetworkStatus();
		if(netStatus == NETCALL_REDO)
		{
			wxBeginBusyCursor();
			m_network->SendFileContents(fileContents, remoteFile, remotePath);
			wxEndBusyCursor();
		}
		else if(netStatus == NETCALL_FAILED)
		{
			return;
		}

		wxString currentFilenameAndPath = m_currentEd->GetFileNameAndPath();
		if((currentFilenameAndPath == wxEmptyString) ||
			(currentFilenameAndPath != wxFileName(remotePath, remoteFile, wxPATH_UNIX).GetFullPath()))
		{
			int currentTab = m_book->GetSelection();
			m_book->SetPageText(currentTab, remoteFile);
		}

		//m_currentEd->SetFilename(remoteFile);
		//m_currentEd->SetRemoteFileNameAndPath(remotePath, remoteFile);
		m_currentEd->SetFileNameAndPath(remotePath, remoteFile, true);
	}

	m_currentEd->EmptyUndoBuffer();
	m_currentEd->SetSavePoint();
	m_book->Refresh();
}


void ChameleonWindow::ResizeSplitter()
{
	/*
	int x, y;
	this->GetClientSize(&x, &y);
	int newsize = (y / 7) * 6;	

	split->SetSashPosition(newsize);	
	*/	
}

void ChameleonWindow::OnToolsOptions(wxCommandEvent &event)
{		
	//UpdatePermsList();
	int result = m_optionsDialog->ShowModal();
	m_currentEd->SetFocus();

	if(result == wxOK)
	{
		EvaluateOptions();		
	}
}


// I've got severable integer variables I need to access at various points,
// so rather than doing a separate Get/Set for each of them, I did this
void ChameleonWindow::SetIntVar(int variableName, int value)
{
	// figure out which integer I'm setting
	int* target = TargetInt(variableName);

	/*
	switch(variableName)
	{
	case VN_NUMPAGES:
		target = &m_numPages;
		break;
	case VN_CURRENTPAGE:
		target = &m_currentPage;
		break;
	case VN_CLICKEDTAB:
		target = &m_clickedTabNum;
		break;
	default:
		DEBUGLOG("Failed to properly set variable")
		return;
	}
	*/
	//*target = value;

	// assuming we assigned it properly, Set it 
	if(target != NULL)
	{
		*target = value;
	}

}

/*
ChameleonEditor* ChameleonWindow::GetCurrentEditor()
{
	return m_currentEd;
}
*/

/*
Networking* ChameleonWindow::GetNetworking()
{
	return NULL;//m_network;
}
*/

// same as SetIntVar, only getting
int ChameleonWindow::GetIntVar(int variableName)
{
	int* target = TargetInt(variableName);

	/*
	switch(variableName)
	{
	case VN_NUMPAGES:
		target = &m_numPages;
		break;
	case VN_CURRENTPAGE:
		target = &m_currentPage;
		break;
	default:
		::wxLogDebug("Failed to properly retrieve variable");
		return 0;
	}
	*/

	if(target != NULL)
	{
		return *target;
	}
	else
	{
		return 0;
	}
}

// figure out which integer value I'm wanting to Get/Set
int* ChameleonWindow::TargetInt(int variableName)
{
	switch(variableName)
	{
	case VN_NUMPAGES:
		return &m_numPages;
		break;
	case VN_CURRENTPAGE:
		return &m_currentPage;
		break;
	case VN_CLICKEDTAB:
		return &m_clickedTabNum;
		break;
	default:
		wxLogDebug("Failed to properly set variable.  variableName = %d", variableName);
			return NULL;
	}
}



bool ChameleonWindow::IsEnabled(int permission)
{
	return m_perms->isEnabled(permission);
}

bool ChameleonWindow::InRemoteMode()
{
	return m_remoteMode;
}

void ChameleonWindow::AddDebugButtons()
{
	wxToolBar* toolBar = GetToolBar();

	wxBitmap bmStart(start_xpm);
	toolBar->AddTool(ID_START, "Run", bmStart);

	wxBitmap bmPause(pause_xpm);
	toolBar->AddTool(ID_PAUSE, "Pause", bmPause);

	wxBitmap bmStop(stop_xpm);
	toolBar->AddTool(ID_STOP, "Stop", bmStop);

	wxBitmap bmStepNext(stepnext_xpm);
	toolBar->AddTool(ID_STEPNEXT, "Step next", bmStepNext);

	wxBitmap bmStepOver(stepover_xpm);
	toolBar->AddTool(ID_STEPOVER, "Step over", bmStepOver);

	wxBitmap bmStepOut(stepout_xpm);
	toolBar->AddTool(ID_STEPOUT, "Step out", bmStepOut);

}

void ChameleonWindow::UpdateAuthCode()
{
	/*
	m_optionsDialog->SetServerAddress("james.cedarville.edu");
	m_optionsDialog->SetUsername("testuser1");
	m_optionsDialog->SetPassword1("password");
	m_optionsDialog->SetPassword2("password");
	*/

	long newAuthCode = m_optionsDialog->GetAuthCode();

	if(newAuthCode != -1)
	{
		m_perms->setGlobal(newAuthCode);

		m_config->Write("Permissions/authorized", newAuthCode);
	}

	UpdatePermsList();
	
}

void ChameleonWindow::UpdatePermsList()
{
	wxCheckListBox* checkList = m_optionsDialog->GetListBox();

	checkList->Clear();
	m_permNumMap.clear();

	wxString optionname;
	wxArrayString optionList;

	for(int i = PERM_FIRST; i < PERM_LAST; i++)
	{		
		if(m_perms->isAuthorized(i))
		{
			optionname = m_perms->getPermName(i);

			int optionMapNum = checkList->GetCount();
			checkList->Append(optionname);

			// need to keep track of which perm is which list item
			// TODO this will probably need to be put somewhere else once the user can
			// enter a different authorization code
			m_permNumMap[optionMapNum] = i;

			if(m_perms->isEnabled(i))
			{
				checkList->Check(optionMapNum, true);
			}
		}		
	}
}

void ChameleonWindow::EvaluateOptions()
{
	// update permissions settings from the options dialog
	

	wxCheckListBox* checkList = m_optionsDialog->GetListBox();

	for(int i = 0; i < checkList->GetCount(); i++)
	{
		int mappedPermNum = m_permNumMap[i];

		bool enableOption = checkList->IsChecked(i);

		wxLogDebug("Setting permissions: %s = %s", 
			m_perms->getPermName(mappedPermNum),
			enableOption ? "true" : "false");

		if(enableOption)
		{
			m_perms->enable(mappedPermNum);
		}
		else
		{
			m_perms->disable(mappedPermNum);
		}
	}

	m_config->Write("Permissions/enabled", m_perms->getGlobalEnabled());


	// update the debug buttons
	wxToolBar* t = GetToolBar();

	m_remoteMode = m_perms->isEnabled(PERM_REMOTELOCAL);

	bool debugEnabled = m_perms->isEnabled(PERM_DEBUG);

	wxControl* pTool = t->FindControl(ID_DEBUG_IDS_FIRST);


	for(int i = ID_DEBUG_IDS_FIRST; i < ID_DEBUG_IDS_LAST; i++)
	{
		t->DeleteTool(i);		
	}
	
	
	if(debugEnabled)
	{
		AddDebugButtons();		
		t->Realize();
	}	

	for(int i = 0; i < m_book->GetPageCount(); i++)
	{
		ChameleonEditor* edit = (ChameleonEditor*)m_book->GetPage(i);
		edit->UpdateSyntaxHighlighting();
	}

	wxString hostname = m_optionsDialog->GetServerAddress();
	wxString username = m_optionsDialog->GetUsername();
	wxString password1 = m_optionsDialog->GetPassword1();
	
	m_network->SetDetailsNoStatus(hostname, username, password1);

	m_config->Write("Network/hostname", hostname);
	m_config->Write("Network/username", username);

}

// called after every major network operation
NetworkCallResult ChameleonWindow::CheckNetworkStatus()
{
	NetworkStatus result = m_network->GetStatus();

	switch((int)result)
	{
		case NET_UNKNOWN_HOST:
		{			
			wxString hostname = m_optionsDialog->GetServerAddress();
			wxString fingerprint = m_network->GetStatusDetails();

			wxString message = "The SSH fingerprint for the server " + hostname + " was not recognized.";
			message += "\nThe fingerprint was " + fingerprint + ".  \nDo you want to cache it?";
			int result = wxMessageBox(message, "Unknown SSH Fingerprint", wxYES_NO | wxICON_QUESTION);

			if(result == wxYES)
			{
				m_network->SSHCacheFingerprint();
			}
			return NETCALL_REDO;
			break;
		}
		case NET_READ_ERROR:
		{
			wxString message = "The remote file could not be read properly.  The error was: ";
			message += m_network->GetStatusDetails();
			wxMessageBox(message, "Remote read error", wxOK  | wxICON_EXCLAMATION);
			return NETCALL_FAILED;
			break;
		}
		case NET_WRITE_ERROR:
		{		
			wxString message = "The remote file could not be written properly.  The error was: ";
			message += m_network->GetStatusDetails();
			wxMessageBox(message, "Remote write error", wxOK  | wxICON_EXCLAMATION);
			return NETCALL_FAILED;
			break;
		}
		case NET_ERROR_MESSAGE:
		{		
			wxString message = "An unknown network error has occurred.";
			message += "\nPlease save the information in the debug window to a file (Log menu -> Save)";
			message += "\n and email it to Ben Carhart at p1164514@cedarville.edu.";
			message += "\nA description of what led to the problem would also help us.";
			wxString statusDetails = m_network->GetStatusDetails();;
			message += "\nError details: " + statusDetails;
			wxLogDebug("NET_ERROR_MESSAGE: %s", statusDetails);
			wxMessageBox(message, "Unknown network error", wxOK | wxICON_EXCLAMATION);
			return NETCALL_FAILED;
			break;
		}
		case NET_AUTH_FAILED:
		{
			wxString message = "Chameleon was unable to log you in using the current username and password.";
			message += "\nPlease check them in the Options menu and try again.";
			wxMessageBox(message, "Login failed", wxOK | wxICON_EXCLAMATION);
			return NETCALL_FAILED;
			break;
		}
		case NET_CONN_REFUSED:
		{
			wxString message = "Connection to the remote server was refused.";
			message += "\nPlease check the remote hostname in the Options menu and try again.";
			wxMessageBox(message, "Connection Refused", wxOK | wxICON_EXCLAMATION);
			return NETCALL_FAILED;
			break;
		}
		default:
			return NETCALL_WORKED;
			break;
	}
}

void ChameleonWindow::OnTermResize(wxCommandEvent &event)
{
	//wxSize telnetSize = m_telnet->GetClientSize();

	//wxString message;
	//message.Printf("Size: %d, %d", telnetSize.GetX(), telnetSize.GetY());
	//wxMessageBox(message);

	//m_telnet->ResizeTerminal(telnetSize.GetX(), telnetSize.GetY());
	//m_telnet->UpdateSize();
}