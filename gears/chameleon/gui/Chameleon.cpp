#include "ChameleonWindow.h"
#include <wx/fdrepdlg.h>

#include <wx/config.h>
#include <wx/msw/iniconf.h>
#include "../common/debug.h"

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
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_About;
#endif

	this->SetClientSize(640, 480);

#ifdef DEBUG
	logWindow = new wxLogWindow(this, "Debug messages");
	wxLog::SetActiveTarget(logWindow);
#endif

	wxIcon icon(moz_xpm);
	SetIcon(icon);

	m_config = new wxIniConfig("Chameleon", wxEmptyString, wxGetCwd() + "\\chameleon.ini");

	// Configuration code... I'll get back to it

	/*
	int value = 0;
	wxString tempString;
	m_config->SetPath("Testing");
	//m_config->Write("test3", "testing blah blah");
	//m_config->Flush();
	//tempString = m_config->Read("test2");

	m_config->SetPath("Testing");
	tempString = m_config->Read("test1");
	wxString testMessage = "Test1: ";
	testMessage << value;
	wxMessageBox(testMessage, "Testing");
	*/
	


	m_remoteMode = true;

	m_network = new Networking();
	//m_network->SetPlinkProg(wxGetCwd());
	wxFileName plinkPath(wxGetCwd(), "plink.exe");

	m_network->SetPlinkProg(plinkPath.GetFullPath());
	CheckNetworkStatus();

	wxString password = wxGetPasswordFromUser("Password for the current server:", "Password");
	m_network->SetDetails("james.cedarville.edu", "s1278644", password);
	CheckNetworkStatus();

	m_perms = new Permission();

	// enable only PERM_AUTOINDENT.  Eventually, will rely only on the provided code.
	m_perms->setGlobal(0xff);

	
	for(int i = PERM_FIRST; i < PERM_LAST; i++)
	{
		m_perms->disable(i);
	}
	
	m_perms->enable(PERM_AUTOINDENT);
	m_perms->enable(PERM_REMOTELOCAL);
	 

	 //m_openFiles = new wxArrayString();


    // create a menu bar
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
	


    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_ABOUT, "&About...\tCtrl-A", "Show about dialog");


    // now append the freshly created menu to the menu bar...
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

	
	toolBar->InsertSeparator(3);

	wxBitmap bmBuild(build_xpm);
	toolBar->AddTool(ID_COMPILE, "Compile", bmBuild);

	toolBar->InsertSeparator(5);

	wxBitmap bmTest(button_xpm);
	toolBar->AddTool(ID_TEST, "Test", bmTest);

	toolBar->InsertSeparator(7);

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


	m_optionsDialog = new OptionsDialog(this, ID_OPTIONSDIALOG, "Options");

	InitializeOptionsDialog();

	m_remoteFileDialog = new RemoteFileDialog(this, ID_REMOTEFILEDIALOG);
	m_remoteFileDialog->SetNetworking(m_network);



	 m_split = new wxSplitterWindow(this, ID_SPLITTER);


    //----------------------------------------
    // Set up the editor

	m_book = new ChameleonNotebook(m_split, ID_NOTEBOOK_ED);

	//uih = new UpdateUIHandler(this);
	//ed->PushEventHandler(uih);

	m_currentPage = 0;
	m_fileNum = 0;

	m_appClosing = false;
	m_setSelection = false;

	wxCommandEvent ev;
	OnFileNew(ev);

	PageHasChanged(m_currentPage);

	m_noteTerm = new ChameleonNotebook(m_split, ID_NOTEBOOK_TERM);

	m_telnet = new wxTelnet( m_noteTerm, ID_TELNET, wxPoint(-1, -1), 80, 24);


	m_noteTerm->AddPage(m_telnet, "Terminal");
	
	if(m_perms->isEnabled(PERM_TERMINAL))
	{
		m_split->SplitHorizontally(m_book, m_noteTerm, -200);		
		m_split->SetMinimumPaneSize(200);
	}
	else
	{
		m_split->Initialize(m_book);
	}
	

}

ChameleonWindow::~ChameleonWindow()
{
	delete m_perms;
	//delete m_openFiles;
	delete m_optionsDialog;
	delete m_network;
	delete m_config;


}

void ChameleonWindow::OnFileNew (wxCommandEvent &WXUNUSED(event)) 
{
	m_fileNum += 1;
	wxString noname = "<untitled> " + wxString::Format ("%d", m_fileNum);
	ChameleonEditor* edit = new ChameleonEditor (this, m_book, -1);
	m_currentEd = edit;
	m_currentPage = m_book->GetPageCount();
	m_book->AddPage (edit, noname, true);
	//m_openFiles->Add (noname);

}

void ChameleonWindow::PageHasChanged (int pageNr) 
{
	if (m_book->GetPageCount() == 0) 
	{
		m_currentPage = -1;
		m_currentEd = NULL;
		//m_openFiles->SetSelection (0);
		return;
	}

	if (pageNr == -1)
	{
		pageNr = m_book->GetSelection();
		
	}

	if (m_book->GetPageCount() <= pageNr)
	{
		pageNr = pageNr - 1;
	}

	if (pageNr >= 0) 
	{
		m_currentPage = pageNr;
		m_currentEd = (ChameleonEditor *) m_book->GetPage (m_currentPage);
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
}

void ChameleonWindow::OnClose(wxCloseEvent &event) 
{
	int i;
	int cnt = m_book->GetPageCount();

	m_appClosing = true;

	for (i = 0; i < cnt; i++) 
	{
		CloseFile();
	}

	

	PageHasChanged();

	if (m_currentEd && m_currentEd->Modified()) 
	{
		if (event.CanVeto())
		{
			event.Veto (true);
		}

		return;
	}

	/*
	if (myIsKeyDown (WXK_CONTROL) && !myIsKeyDown ('Q')) 
	{
		StoreFrameSize (GetRect ());
	}
	*/

	/*
	if (m_pageTool)
	{
		m_files->RemoveCbox (m_pageTool);
	}
	*/

	//m_recents->RemoveMenu (m_recentsMenu);

	//((MyApp*)wxTheApp)->RemoveFrame (this);
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

	ChameleonEditor *edit = (ChameleonEditor *) m_book->GetPage (pageNr);

	if (!edit)
	{
		return;
	}

	if (edit->Modified()) 
	{
		wxString fileName = wxFileName(edit->GetFilename()).GetFullName();
		wxString saveMessage = fileName;
		saveMessage << " has unsaved changes.  Do you want to save them before the file is closed?";
		if (wxMessageBox (_(saveMessage), _("Close"),
			wxYES_NO | wxICON_QUESTION) == wxYES) 
		{
			edit->SaveFileAs();
			//SaveFileLocal(true);

			if (edit->Modified()) 
			{

				wxString errorMessage = fileName + " could not be saved!";
				wxMessageBox (_(errorMessage), _("Close abort"),
					wxOK | wxICON_EXCLAMATION);
				return;
			}
		}
	}

	//m_files->Remove (edit->GetFilename());
	if (m_book->GetPageCount() > 0) 
	{
		if ((m_book->GetPageCount() > 1) || m_appClosing) 
		{
			m_book->DeletePage (pageNr);
		}
		else
		{
			m_fileNum = 1;
			m_currentEd->ClearAll();
			m_currentEd->SetSavePoint();
			m_currentEd->EmptyUndoBuffer();
			wxString noname = "<untitled> " + wxString::Format (".%d", m_fileNum);
			m_book->SetPageText (pageNr, noname);
			//m_currentEd->SetFilename (wxEmptyString);

			
			//m_files->Add (noname);
		}
	}
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
		dlg.GetPaths (fnames);
	}
	else
	{
		m_remoteFileDialog->Prepare(true);
		int result = m_remoteFileDialog->ShowModal();

		if(result != wxOK)
		{
			return;
		}

		//wxString remotePath = m_remoteFileDialog->GetRemotePath();
		///wxString remoteFile = m_remoteFileDialog->GetRemoteFileName();

		//wxString fileContents = m_network->GetFileContents(remoteFile, remotePath);

		// Need to revise this - assumes old networking API
		// Actually... I think I can keep this as is for now
		wxString remoteFileName = m_remoteFileDialog->GetRemoteFileNameAndPath();
		fnames.Add(remoteFileName);
	}



	OpenFile (fnames);
	PageHasChanged (m_currentPage);
}

void ChameleonWindow::OpenFile (wxArrayString fnames) 
{
	int firstPageNr = -1;
	wxString fileContents = wxEmptyString;


	for (size_t n = 0; n < fnames.GetCount(); n++) 
	{
		//wxFileName w(fnames[n]); 
		//w.Normalize(); 
		//fnames[n] = w.GetFullPath();
		wxString filename = wxFileName (fnames[n]).GetFullName();
		int pageNr = GetPageNum(fnames[n]);

		// filename is already open
		if (pageNr >= 0) 
		{
			m_setSelection = true;
			m_book->SetSelection (pageNr);
			m_setSelection = false;
			m_currentPage = pageNr;
			
			
			if(GetFileContents(fnames[n], fileContents))
			{
				if(fileContents != wxEmptyString)
				{
					m_currentEd->LoadFileText(fileContents);
				}
			}
		}
		else if (m_currentEd && (!m_currentEd->Modified() && !m_currentEd->HasBeenSaved() && m_currentEd->GetText().IsEmpty())) 
		{
			//m_currentEd->LoadFile(fnames[n]);

			if(GetFileContents(fnames[n], fileContents))
			{
				if(fileContents != wxEmptyString)
				{
					m_currentEd->LoadFileText(fileContents);
					m_book->SetPageText(m_currentPage, filename);
				}
			}

			/*
			if (m_currentEd->LoadFile (fnames[n])) 
			{
				m_book->SetPageText (m_currentPage, filename);
				//m_files->Replace (fnames[n], m_currentPage);
			}
			*/
			
		}
		else
		{
			ChameleonEditor *edit = new ChameleonEditor (this, m_book, -1);

			if(GetFileContents(fnames[n], fileContents))
			{
				if(fileContents != wxEmptyString)
				{
					edit->LoadFileText(fileContents);
					m_currentEd = edit;
					//m_currentEd->SetDropTarget (new DropFiles (this));
					m_currentPage = m_book->GetPageCount();
					m_book->AddPage (m_currentEd, filename, true);
				}
			}
			/*
			if (edit->LoadFile (fnames[n])) 
			{
				m_currentEd = edit;
				//m_currentEd->SetDropTarget (new DropFiles (this));
				m_currentPage = m_book->GetPageCount();
				m_book->AddPage (m_currentEd, filename, true);
				//m_files->Add (fnames[n]);
			}
			*/
			else
			{
				delete edit;
			}
		}

		if (firstPageNr < 0) firstPageNr = m_currentPage;
		//m_files->SetSelection (m_pageNr);
		//m_recents->Add (fnames[n]);
	}
	if (firstPageNr >= 0) {
		m_currentPage = firstPageNr;
		m_setSelection = true;
		m_book->SetSelection (m_currentPage);
		m_setSelection = false;
		//g_statustext->Clear ();
		//g_statustext->Append (_("Opened file: ") + fnames[0]);
	}
}

bool ChameleonWindow::GetFileContents(wxString filename, wxString &fileContents)
{
	//wxString fileContents;
	if(m_remoteMode)
	{
		wxFileName fn(filename);

		wxString remotePath = fn.GetPath(wxPATH_UNIX);
		wxString remoteFile = fn.GetFullName();

		fileContents = m_network->GetFileContents(remoteFile, remotePath);
		CheckNetworkStatus();

		return true;
	}
	else
	{
		//wxString buf;

		wxFile file (filename);

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

			//InsertText(0, buf);
		}

		return true;
	}
}

int ChameleonWindow::GetPageNum (const wxString &fname) 
{
	ChameleonEditor *edit;
	for (int pageNum = 0; pageNum < m_book->GetPageCount(); pageNum++) 
	{
		edit = (ChameleonEditor *) m_book->GetPage(pageNum);

		if (edit && (edit->GetFilename() == fname))
		{			
			return pageNum;
		}
	}

	return -1;
}

void ChameleonWindow::OnPageChange (wxNotebookEvent &WXUNUSED(event)) 
{
	//::wxMessageBox("OnPageChange");
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
	if (!m_currentEd) return;
	CloseFile();
	PageHasChanged();
}

void ChameleonWindow::OnConnect(wxCommandEvent& WXUNUSED(event))
{
	//::wxMessageBox("OnConnect");
	wxIPV4address local;
	local.LocalHost();

	DEBUGLOG("Address: " + local.Hostname())
	m_telnet->Connect(local.Hostname(), 3012);

}


// event handlers
void ChameleonWindow::Test(wxCommandEvent& WXUNUSED(event))
{
	//m_remoteFileDialog->LoadTestData();
	//OpenFile();


	//wxMessageBox(wxGetCwd());
}


void ChameleonWindow::OnCloseWindow(wxCloseEvent& event)
{
	/*
	if(ed->GetModify())
	{
		int answer = ::wxMessageBox("Your file has unsaved changes.  Do you want to save?", "Confirm exit",
			wxYES_NO | wxCANCEL);
		if (answer == wxCANCEL)
		{
			return;
		}
		else
		{
			if(answer == wxYES)
			{
				SaveFileLocal(false);
			}
		}
	}
	*/
	this->Destroy();
}


void ChameleonWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void ChameleonWindow::CloseApp()
{

	// TRUE is to force the frame to close
	
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

    wxMessageBox(msg, "About Chameleon", wxOK | wxICON_INFORMATION, this);
}

void ChameleonWindow::CheckSize()
{
	int x, y;
	this->GetClientSize(&x, &y);
	wxString result;
	result << x << ", " << y;
	::wxMessageBox(result );
	// TODO multiple buffer
	m_currentEd->SetScrollWidth(x);
	
}

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
			//this->Refresh();
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
	//TODO multiple buffer
	//ed->GetModify());
}


void ChameleonWindow::OnSave(wxCommandEvent& WXUNUSED(event))
{
	//::wxMessageBox("OnSave");
	//SaveFileLocal(false);
	//m_currentEd->SaveFileAs();

	SaveFile(false);
}

void ChameleonWindow::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	//::wxMessageBox("OnSaveAs");
	//SaveFileLocal(true);
	//m_currentEd->SaveFileAs();

	SaveFile(true);
}

void ChameleonWindow::SaveFile(bool saveas)
{
	wxString filename;

	bool doSaveAs = saveas || !m_currentEd->HasBeenSaved();

	if(!(m_perms->isEnabled(PERM_REMOTELOCAL) && m_remoteMode))
	{
		if(doSaveAs)
		{			
			wxFileDialog dlg (this, _T("Save file"), _T(""), _T(""), _T("Any file (*)|*"),
							wxSAVE | wxOVERWRITE_PROMPT);

			if(dlg.ShowModal() != wxID_OK) 
			{ 
				return; 
			}

			filename = dlg.GetPath();
			m_currentEd->SaveFile(filename);
		}
		else
		{
			m_currentEd->SaveFile();
		}
	}
	else
	{
		wxString remotePath, remoteFile, fileContents;

		fileContents = m_currentEd->GetText();
		if(doSaveAs)
		{
			m_remoteFileDialog->Prepare(false);
			int result = m_remoteFileDialog->ShowModal();

			if(result != wxOK)
			{
				return;
			}

			remotePath = m_remoteFileDialog->GetRemotePath();
			remoteFile = m_remoteFileDialog->GetRemoteFileName();

			//m_network->SendFile("est.txt", "c:\\temp", remoteFile, remotePath);
			
		}
		else
		{
			remoteFile = m_currentEd->GetRemoteFileName();
			remotePath = m_currentEd->GetRemotePath();
		}

		m_network->SendFileContents(fileContents, remoteFile, remotePath);

		m_currentEd->SetRemoteFileNameAndPath(remotePath, remoteFile);

		m_currentEd->EmptyUndoBuffer();
		m_currentEd->SetSavePoint();
		m_currentEd->SetTabUnmodified();
		m_book->Refresh();
	}
}

/*
void ChameleonWindow::SaveFileLocal(bool saveas)
{
	::wxMessageBox("SaveFileLocal: obsolete function!");
}
*/


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
	int result = m_optionsDialog->ShowModal();

	if(result == wxOK)
	{
		EvaluateOptions();

		
	}
}


void ChameleonWindow::SetIntVar(int variableName, int value)
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
	case VN_CLICKEDTAB:
		target = &m_clickedTabNum;
		break;
	default:
		DEBUGLOG("Failed to properly set variable")
		return;
	}
	*/
	//*target = value;

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
	//return (target != null ? *target : 0);
	
}

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
		DEBUGLOG("Failed to properly set variable")
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

void ChameleonWindow::InitializeOptionsDialog()
{
	m_optionsDialog->SetServerAddress("james.cedarville.edu");
	m_optionsDialog->SetUsername("testuser1");
	m_optionsDialog->SetPassword1("password");
	m_optionsDialog->SetPassword2("password");

	wxCheckListBox* checkList = m_optionsDialog->GetListBox();
	wxString optionname;
	wxArrayString optionList;

	for(int i = PERM_FIRST; i < PERM_LAST; i++)
	{		
		if(m_perms->isAuthorized(i))
		{
			optionname = m_perms->getPermName(i);

			int optionMapNum = checkList->GetCount();
			checkList->Append(optionname);//optionList.GetCount();

			// need to keep track of which perm is which list item
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
}

void ChameleonWindow::CheckNetworkStatus()
{
	NetworkStatus result = m_network->GetStatus();

	switch((int)result)
	{
		case NET_UNKNOWN_HOST:
		{
			
			wxString hostname = m_optionsDialog->GetServerAddress();
			wxString fingerprint = m_network->GetStatusDetails();

			wxString message = "The SSH fingerprint for the server " + hostname + " was not recognized.";
			message += "The fingerprint was " + fingerprint + ".  Do you want to cache it?";
			int result = wxMessageBox(message, "Unknown SSH Fingerprint", wxYES_NO | wxICON_QUESTION);

			if(result == wxYES)
			{
				m_network->SSHCacheFingerprint();
			}
			break;
		}
		case NET_READ_ERROR:
		{
			wxString message = "The remote file could not be read properly.  The error was: ";
			message += m_network->GetStatusDetails();
			wxMessageBox(message, "Remote read error", wxOK);
			break;
		}
		case NET_WRITE_ERROR:
		{		
			wxString message = "The remote file could not be written properly.  The error was: ";
			message += m_network->GetStatusDetails();
			wxMessageBox(message, "Remote write error", wxOK);
			break;
		}
		case NET_ERROR_MESSAGE:
		{		
			wxString message = "An unknown network error has occurred.  \nPlease contact mark.erikson@cedarville.edu";
			message += "\nError details: " + m_network->GetStatusDetails();
			wxMessageBox(message, "Unknown network error", wxOK);
			break;
		}
		case NET_AUTH_FAILED:
		{
			wxString message = "Chameleon was unable to log you in using the current username and password.";
			message += "\nPlease check them in the Options menu and try again.";
			wxMessageBox(message, "Login failed", wxOK | wxICON_EXCLAMATION);
		}
		default:
			break;
	}
}