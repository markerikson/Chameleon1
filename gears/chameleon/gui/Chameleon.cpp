#include "ChameleonWindow.h"
#include <wx/fdrepdlg.h>


#include "../common/debug.h"


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
	EVT_MENU						(ID_PAGECLOSE, ChameleonWindow::OnFileClose)
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

	 m_perms = new Permission();

	 m_perms->setGlobal(0xFF);

	 for(int i = PERM_FIRST; i < PERM_LAST; i++)
	 {
		 m_perms->enable(i);
	 }
	 

	 m_openFiles = new wxArrayString();


    // create a menu bar
    wxMenu* menuFile = new wxMenu();

	menuFile->Append(ID_NEW, "&New\tCtrl-N", "Create a new file");
	menuFile->Append(ID_OPEN, "&Open\tCtrl-O", "Open an existing file");
	menuFile->Append(ID_PAGECLOSE, "&Close");
	menuFile->InsertSeparator(3);
	menuFile->Append(ID_SAVE, "&Save\tCtrl-S", "Save the current file");
	menuFile->Append(ID_QUIT, "E&xit\tAlt-X", "Quit this program");

	wxMenu* menuTools = new wxMenu();

	menuTools->Append(ID_OPTIONS, "&Options");
	menuTools->Append(ID_STARTCONNECT, "&Connect");
	menuTools->InsertSeparator(2);

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


	if(m_perms->isEnabled(PERM_DEBUG))
	{
		AddDebugButtons();
	}




	toolBar->Realize();

	SetToolBar(toolBar);
	
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Statusbar text");
#endif // wxUSE_STATUSBAR


	 m_optionsDialog = new OptionsDialog(this, -1, "Options");

	

	
	
	


	//m_permNumMap = new IntIntHashmap();
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
			
			m_permNumMap[optionMapNum] = i;

			if(m_perms->isEnabled(i))
			{
				checkList->Check(optionMapNum, true);
			}
		}		
	}
	

	


	
	 m_split = new wxSplitterWindow(this, 5205);
	


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
	
	m_split->SplitHorizontally(m_book, m_noteTerm, -200);	
	m_split->SetMinimumPaneSize(200);

}

ChameleonWindow::~ChameleonWindow()
{
	delete m_perms;
	delete m_openFiles;
	delete m_optionsDialog;


}

void ChameleonWindow::OnFileNew (wxCommandEvent &WXUNUSED(event)) 
{
	m_fileNum += 1;
	wxString noname = "<untitled> " + wxString::Format ("%d", m_fileNum);
	ChameleonEditor* edit = new ChameleonEditor (this, m_book, -1);
	m_currentEd = edit;
	//m_currentEd->SetDropTarget (new DropFiles (this));
	m_currentPage = m_book->GetPageCount();
	m_book->AddPage (edit, noname, true);
	m_openFiles->Add (noname);

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
		if (wxMessageBox (_("Text is not saved, save before closing?"), _("Close"),
			wxYES_NO | wxICON_QUESTION) == wxYES) 
		{
			edit->SaveFile();
			//SaveFileLocal(true);

			if (edit->Modified()) 
			{
				wxMessageBox (_("Text could not be saved!"), _("Close abort"),
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
			wxString noname = "<untitled> " + wxString::Format (".%d", m_fileNum);
			m_book->SetPageText (pageNr, noname);
			//m_currentEd->SetFilename (wxEmptyString);
			m_currentEd->ClearAll();
			m_currentEd->SetSavePoint();
			//m_files->Add (noname);
		}
	}
}

void ChameleonWindow::OnFileOpen (wxCommandEvent &WXUNUSED(event)) 
{
	if (!m_currentEd)
	{
		return;
	}

	wxArrayString fnames;
	wxFileDialog dlg (this, _("Open files"), "", "", "C++ files (*.cpp, *.h)|*.cpp;*.h",
		wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE | wxCHANGE_DIR);
	if (dlg.ShowModal() != wxID_OK)
	{
		return;
	}
	dlg.GetPaths (fnames);
	OpenFile (fnames);
	PageHasChanged (m_currentPage);
}

void ChameleonWindow::OpenFile (wxArrayString fnames) 
{
	int firstPageNr = -1;
	for (size_t n = 0; n < fnames.GetCount(); n++) 
	{
		wxFileName w(fnames[n]); 
		w.Normalize(); 
		fnames[n] = w.GetFullPath();
		wxString filename = wxFileName (fnames[n]).GetFullName();
		int pageNr = GetPageNum(fnames[n]);
		if (pageNr >= 0) 
		{
			m_setSelection = true;
			m_book->SetSelection (pageNr);
			m_setSelection = false;
			m_currentPage = pageNr;
			m_currentEd->LoadFile (fnames[n]);
		}
		else if (m_currentEd && (!m_currentEd->Modified() && m_currentEd->GetFilename().IsEmpty())) 
		{
			if (m_currentEd->LoadFile (fnames[n])) 
			{
				m_book->SetPageText (m_currentPage, filename);
				//m_files->Replace (fnames[n], m_currentPage);
			}
		}
		else
		{
			ChameleonEditor *edit = new ChameleonEditor (this, m_book, -1);
			if (edit->LoadFile (fnames[n])) 
			{
				m_currentEd = edit;
				//m_currentEd->SetDropTarget (new DropFiles (this));
				m_currentPage = m_book->GetPageCount();
				m_book->AddPage (m_currentEd, filename, true);
				//m_files->Add (fnames[n]);
			}
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

void ChameleonWindow::OpenFile(wxCommandEvent& WXUNUSED(event))
{
	/*
	wxString filetext;
	wxString filename = ::wxFileSelector("Pick a file", "",
		"", "", "", wxOPEN, NULL);
	if(!filename.empty())
	{

		wxFile file(filename);
		wxString st;

		char* buff = st.GetWriteBuf(file.Length());
		file.Read(buff, file.Length());
		st.UngetWriteBuf();

		// TODO multiple buffer
		ed->InsertText(0, st);

		saveFileName = filename;

	}
	*/
	::wxMessageBox("Open File: obsolete function");
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

	

	//wxFindReplaceDialog frdialog(this, new wxFindReplaceData, "Find");
	/*
	DEBUGLOG("CW::Test")
	wxString value = "PERM_DEBUG: ";
	value << m_perms->isEnabled(PERM_DEBUG);
	DEBUGLOG(value)
	//ed->Undo();
//	wxEventType et = event.GetEventType();

	if(m_perms->isEnabled(PERM_DEBUG))
	{
		m_perms->disable(PERM_DEBUG);
	}
	else
	{
		m_perms->enable(PERM_DEBUG);
	}

	EvaluateOptions();
	*/
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
	// TODO multiple buffer
	m_currentEd->Undo();
}

void ChameleonWindow::OnRedo(wxCommandEvent &event)
{
	// TODO multiple buffer
	m_currentEd->Redo();
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
	event.Enable(true);
	//TODO multiple buffer
	//ed->GetModify());
}


void ChameleonWindow::OnSave(wxCommandEvent& WXUNUSED(event))
{
	::wxMessageBox("OnSave");
	SaveFileLocal(false);
}

void ChameleonWindow::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	::wxMessageBox("OnSaveAs");
	SaveFileLocal(true);
}


void ChameleonWindow::SaveFileLocal(bool saveas)
{

	::wxMessageBox("SaveFileLocal: obsolete function!");
/*
	wxString name = wxEmptyString;

	
	if( (saveFileName == wxEmptyString) ||
		(saveas) )
	{

		SaveFileDialog sfd = new SaveFileDialog();
		sfd.Filter = "HTML files (*.html)|*.html|All files (*.*)|*.*";
		DialogResult dr = sfd.ShowDialog();

		wxString filetext;
		wxString filetypes = "C++ files (*.cpp)|*.cpp|Header files (*.h)|*.h|All files (*.*)|*.*";
		//wxString filename = ::wxFileSelector("Pick a file", "",
		//	"", "", "", wxSAVE, NULL);
		wxFileDialog fd(NULL, "Save As", wxEmptyString, 
						wxEmptyString, filetypes, wxSAVE);

		int dialogResult = fd.ShowModal();
		//ed->On

		if(dialogResult == wxID_OK)
		{			
			wxString filename = fd.GetFilename();
			int filteridx = fd.GetFilterIndex();

			wxString message = "Filter index: ";
			message << filteridx;
			
			//::wxMessageBox(message);

			
			switch(filteridx)
			{
				case 0:
					::wxMessageBox("Filter index: 0");
					break;
				case 1:
					::wxMessageBox()
			}
			
		


			wxFile file(filename, wxFile::write);

			// TODO multiple buffer
			file.Write(ed->GetText());
			file.Close();
	

			
			wxTextFile file(filename);
			file.Read
			if(!file.Exists())
			{
			file.Create();
			}
			wxString buffertext = ed->GetText();

			file.AddLine(buffertext);
			file.Write();
			file.Close();
			
		}


		if(dr == DialogResult.OK)
		{	
			name  = sfd.FileName;

			if(sfd.FilterIndex == 1)
			{
				int idx = name.LastIndexOf(".");
				string justname = name.Substring(0, idx);
				name = justname + ".html";
			}	
		}
		else
		{
			return;
		}
	}
	else
	{
		name = saveFileName;
	}			

	StreamWriter sw = new StreamWriter(name);
	sw.WriteLine(ed.TextBox.Text);			
	sw.Close();
	ed.TextBox.Modified = false;



	}

*/
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
	int result = m_optionsDialog->ShowModal();

	if(result == wxOK)
	{
		//wxGrid* m_optGrid = m_optionsDialog->GetGrid();
		//:wxMessageBox("wxOK");
		//for(int i = PERM_FIRST; i < PERM_LAST; i++)
		//{
			//wxString value = m_optGrid->GetCellValue(i, 0);
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

/*			if(value == "No")
			{
				m_perms->disable(i);
			}
			else
			{
				m_perms->enable(i);
			}
			*/
		//}
	}
	EvaluateOptions();

}


void ChameleonWindow::SetIntVar(int variableName, int value)
{
	int* target;

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
	*target = value;

}

int ChameleonWindow::GetIntVar(int variableName)
{
	int* target;

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

	return *target;
	
}

bool ChameleonWindow::IsEnabled(int permission)
{
	return m_perms->isEnabled(permission);
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

void ChameleonWindow::EvaluateOptions()
{
	wxToolBar* t = GetToolBar();

	bool debugEnabled = m_perms->isEnabled(PERM_DEBUG);

	wxControl* pTool = t->FindControl(ID_DEBUG_IDS_FIRST);


	for(int i = ID_DEBUG_IDS_FIRST; i < ID_DEBUG_IDS_LAST; i++)
	{
		//t->RemoveTool(i);
		t->DeleteTool(i);		
	}
	
	
	if(debugEnabled)
	{
		AddDebugButtons();		
		t->Realize();
	}	
}

