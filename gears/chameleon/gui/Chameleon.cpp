

#include "../common/CommonHeaders.h"
#include "ChameleonWindow.h"
#include <wx/statusbr.h>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <wx/msw/tbar95.h>

#include "../editor/editor.h"
//#include "updateuihandler.h"
#include "ChameleonNotebook.h"
#include "dialogs/OptionsDialog.h"
#include "dialogs/RemoteFileDialog.h"
#include "dialogs/wxTermContainer.h"
#include "../perms/p.h"
#include "../common/ProjectInfo.h"
#include "../network/networking.h"
#include "../compiler/compiler.h"
#include "../common/Options.h"
//#include "wxtelnet.h"
#include "wxssh.h"

#include "new.xpm"
#include "open.xpm"
#include "save.xpm"
#include "build.xpm"
#include "button.xpm"

//#include "connect.xpm"
//#include "disconnect.xpm"
#include "connect16.xpm"
#include "disconnect16.xpm"

#include "start.xpm"
#include "stop.xpm"
#include "stepnext.xpm"
#include "stepout.xpm"
#include "stepover.xpm"
#include "pause.xpm"

#include "moz.xpm"

#define ID_RANDOMTEXTLABEL 9991

#include "../common/debug.h"


BEGIN_EVENT_TABLE(ChameleonWindow, wxFrame)
	EVT_MENU						(ID_NEW_SOURCE, ChameleonWindow::OnFileNew)
	EVT_MENU						(ID_OPEN_SOURCE_LOCAL, ChameleonWindow::OnFileOpen)
	EVT_MENU						(ID_OPEN_SOURCE_REMOTE, ChameleonWindow::OnFileOpen)
	EVT_MENU						(ID_QUIT,  ChameleonWindow::OnQuit)
	EVT_MENU						(ID_ABOUT, ChameleonWindow::OnAbout)
	EVT_MENU						(ID_TEST, ChameleonWindow::Test)
	EVT_MENU						(ID_SAVE, ChameleonWindow::OnSave)
	EVT_MENU						(ID_SAVE_AS, ChameleonWindow::OnSaveAs)
	EVT_MENU						(ID_CLOSEPAGE, ChameleonWindow::OnFileClose)
	EVT_MENU						(ID_CLOSETAB, ChameleonWindow::CloseTab)
	EVT_MENU						(ID_CLOSEALL, ChameleonWindow::OnFileCloseAll)
	EVT_UPDATE_UI					(ID_SAVE, ChameleonWindow::OnUpdateSave)
	EVT_MENU						(ID_STARTCONNECT, ChameleonWindow::OnConnect)
	EVT_MENU						(ID_DISCONNECT, ChameleonWindow::OnDisconnect)
	EVT_MENU						(ID_COMPILE, ChameleonWindow::OnCompile)
	EVT_MENU						(ID_UNDO, ChameleonWindow::OnUndo)
	EVT_MENU						(ID_REDO, ChameleonWindow::OnRedo)
	EVT_MENU						(ID_OPTIONS, ChameleonWindow::OnToolsOptions)
	EVT_MENU						(ID_CUT, ChameleonWindow::OnCut)
	EVT_MENU						(ID_COPY, ChameleonWindow::OnCopy)
	EVT_MENU						(ID_PASTE, ChameleonWindow::OnPaste)
	EVT_CLOSE						(ChameleonWindow::OnClose)
	EVT_NOTEBOOK_PAGE_CHANGED		(ID_NOTEBOOK_ED,   ChameleonWindow::OnPageChange)
	//EVT_SPLITTER_SASH_POS_CHANGED	(ID_SPLITTER, ChameleonWindow::OnTermResize)
	EVT_SIZE						(ChameleonWindow::OnSize)
	EVT_SPLITTER_DCLICK				(ID_SPLITPROJECTEDITOR, ChameleonWindow::OnSplitterDoubleClick)
	EVT_SPLITTER_DCLICK				(ID_SPLITEDITOROUTPUT, ChameleonWindow::OnSplitterDoubleClick)
	EVT_TREE_ITEM_RIGHT_CLICK		(ID_PROJECTTREE, ChameleonWindow::OnTreeItemRightClick)
	EVT_MENU						(ID_PROJECT_ADDFILE, ChameleonWindow::OnAddFileToProject)
	EVT_TREE_ITEM_ACTIVATED			(ID_PROJECTTREE, ChameleonWindow::OnTreeItemActivated)
	EVT_MENU						(ID_NEW_PROJECT, ChameleonWindow::OnFileNewProject)
	EVT_MENU						(ID_OPEN_PROJECT_LOCAL, ChameleonWindow::OnOpenProjectFile)
	EVT_MENU						(ID_OPEN_PROJECT_REMOTE, ChameleonWindow::OnOpenProjectFile)
	EVT_MENU						(ID_CLOSE_PROJECT, ChameleonWindow::CloseProjectFile)
	EVT_MENU						(ID_PROJECT_REMOVEFILE, ChameleonWindow::OnRemoveFileFromProject)

	
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
	//m_appStarting = true;
	wxStopWatch stopwatch;
	
	// should be approximately 80x15 for the terminal
	this->SetSize(660, 736);

	// Show a log window for all debug messages
#ifdef _DEBUG
	logWindow = new wxLogWindow(this, "Debug messages");
	wxLog::SetActiveTarget(logWindow);
#endif

	wxIcon icon(moz_xpm);
	SetIcon(icon);
	
	m_remoteMode = true;

	// Open up the configuration file, assumed to be in the user's home directory

	wxString authorizedCode = "0";
	wxString enabledCode = "0";

	// by default, enable nothing
	wxString defaultAuthorizedCode = "0";
	wxString defaultEnableCode = "0";

	m_options = new Options;
	m_perms = new Permission(defaultAuthorizedCode, defaultEnableCode);

	wxFileName configName(wxGetHomeDir(), "chameleon.ini");

	m_config = new wxFileConfig("Chameleon", wxEmptyString, configName.GetFullPath());

	if(configName.FileExists())
	{
		m_options->SetHostname(m_config->Read("Network/hostname"));
		m_options->SetUsername(m_config->Read("Network/username"));
		
		authorizedCode = m_config->Read("Permissions/authorized", defaultAuthorizedCode);
		enabledCode = m_config->Read("Permissions/enabled", defaultEnableCode);
		
		if(!m_perms->setGlobalAuthorized(authorizedCode))
		{
			wxLogDebug("Authcode initialization failed!  Code: %s", authorizedCode.c_str());
			authorizedCode = "0";
		}
		else
		{
			
		}
		m_perms->setGlobalEnabled(enabledCode);		
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

	wxFileName plinkPath(wxGetCwd(), "plink.exe");  // <-- Mark, look at Options.cpp
	m_options->SetPlinkApp(plinkPath.GetFullPath());

	wxFileName pscpPath(wxGetCwd(), "pscp.exe");  
	m_options->SetPscpApp(pscpPath.GetFullPath());


	m_network = new Networking(m_options);
	m_compiler = new Compiler(m_options, m_network);

	m_optionsDialog = new OptionsDialog(this, ID_OPTIONSDIALOG, "Options");
	UpdatePermsList();

	m_optionsDialog->SetServerAddress(m_options->GetHostname());
	m_optionsDialog->SetUsername(m_options->GetUsername());
	m_optionsDialog->SetPassword1(wxEmptyString);
	m_optionsDialog->SetPassword2(wxEmptyString);
	m_optionsDialog->SetAuthCode(authorizedCode);



	m_splitEditorOutput = new wxSplitterWindow(this, ID_SPLITEDITOROUTPUT);

	m_splitProjectEditor = new wxSplitterWindow(m_splitEditorOutput, ID_SPLITPROJECTEDITOR);

	m_book = new ChameleonNotebook(m_splitProjectEditor, ID_NOTEBOOK_ED);


	m_noteTerm = new ChameleonNotebook(m_splitEditorOutput, ID_NOTEBOOK_TERM);

	m_termContainer = new wxTermContainer(m_noteTerm, ID_TERM_CONTAINER);

	m_terminal = new wxSSH(m_termContainer, ID_TERMINAL, m_network, wxPoint(0, 0));

	m_termContainer->SetTerminal(m_terminal);
	
	m_terminal->set_mode_flag(GTerm::CURSORINVISIBLE);

	m_compilerTextbox = new wxTextCtrl(m_noteTerm, ID_COMPILERTEXTBOX, wxEmptyString, wxDefaultPosition,
									wxDefaultSize, wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY);


	// project setup
	m_projectTree = new wxTreeCtrl(m_splitProjectEditor, ID_PROJECTTREE);

	
	UpdateTerminalNotebook();

	m_remoteFileDialog = new RemoteFileDialog(this, ID_REMOTEFILEDIALOG);
	m_remoteFileDialog->SetNetworking(m_network);

	wxTreeItemId rootNode = m_projectTree->AddRoot("No project open", 0, 1);
	m_projectFileFolders[0] = m_projectTree->AppendItem(rootNode, "Source files", 0, 1);
	m_projectFileFolders[1] = m_projectTree->AppendItem(rootNode, "Header files", 0, 1);
	m_projectFileFolders[2] = m_projectTree->AppendItem(rootNode, "Libraries", 0, 1);


	m_currentPage = 0;
	m_fileNum = 0;

	m_appClosing = false;
	m_setSelection = false;

	UpdateMenuBar();

	wxToolBar* toolBar = CreateToolBar(wxTB_FLAT | wxTB_TEXT);

	SetToolBar(toolBar);
	UpdateToolbar();

	m_statusBar = CreateStatusBar(2);

	// create the initial blank open file
	wxCommandEvent ev;
	OnFileNew(ev);
	PageHasChanged(m_currentPage);


	m_filterCPPFiles = "C++ source files (*.cpp, *.c)|*.cpp;*.c";
	m_filterHeaderFiles += "C++ header files (*.h, *.hpp)|*.h;*.hpp";	
	m_filterProjectFiles = "Chameleon project files (*.cpj)|*.cpj";
	m_filterAllFiles = "All files (*.*)|*.*";

	m_currentProjectInfo = NULL;

	m_appStarting = false;
	m_bProjectOpen = false;	
}

ChameleonWindow::~ChameleonWindow()
{
	m_config->Flush();

	delete m_perms;
	delete m_optionsDialog;
	delete m_network;
	delete m_compiler;
	delete m_config;
	delete m_options;
}

// create a new blank file
void ChameleonWindow::OnFileNew (wxCommandEvent &WXUNUSED(event)) 
{
	m_fileNum += 1;

	wxString locationPrefix = "(?) ";

	wxString noname = locationPrefix + "<untitled> " + wxString::Format ("%d", m_fileNum);
	ChameleonEditor* edit = new ChameleonEditor (this, m_book, -1);
	m_currentEd = edit;
	m_currentPage = m_book->GetPageCount();
	m_book->AddPage (edit, noname, true);
}

// called whenever the active tab is changed
void ChameleonWindow::PageHasChanged (int pageNr) 
{
	// no pages - null out the current ed pointer
	// You know, this should really never happen with the current design
	// 3/11/04: Unless, of course, we're closing out the program...
	if (m_book->GetPageCount() == 0) 
	{
		m_currentPage = -1;
		m_currentEd = NULL;
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
		m_currentEd = static_cast< ChameleonEditor * > (m_book->GetPage (m_currentPage));
		m_book->SetSelection(pageNr);
		m_currentEd->SetFocus();
	}
	else
	{
		m_currentPage = -1;
		m_currentEd = NULL;
	}
	m_book->Refresh();
	if(m_currentEd != NULL)
	{
		m_currentEd->Refresh();
		wxString tabText = m_book->GetPageText(m_currentPage);

		wxString statusText;

		wxRegEx reModeTest = "\\((R|L|\\?)\\) ";

		if(reModeTest.Matches(tabText))
		{
			wxString modeString = reModeTest.GetMatch(tabText, 1);
			
			if(modeString == "R")
			{
				m_remoteMode = true;
				statusText = "Remote mode";
			}
			else if(modeString == "L")
			{
				m_remoteMode = false;
				statusText = "Local mode";
			}
			else if(modeString == "?")
			{
				m_remoteMode = true;
				statusText = "Unsaved file";
			}
			m_statusBar->SetStatusText(statusText, 0);
		}
	}
}

// event handler for closing the whole program
void ChameleonWindow::OnClose(wxCloseEvent &event) 
{
	m_appClosing = true;

	CloseAllFiles();

	// double check in case something went wrong
	if (m_currentEd && m_currentEd->Modified()) 
	{
		if (event.CanVeto())
		{
			event.Veto (true);
		}

		return;
	}

	if(m_bProjectOpen)
	{
		CloseProjectFile();
	}

	Destroy();
}

void ChameleonWindow::CloseTab()
{
	int tab = GetIntVar(VN_CLICKEDTAB);
	CloseFile(tab);
	m_book->Refresh();
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
			ChameleonEditor* pEdit = static_cast <ChameleonEditor* >(m_book->GetPage(pageNr));
			m_book->DeletePage (pageNr);
		}
		// closing out the last buffer, reset it to act as a new one
		else
		{
			m_fileNum = 1;
			wxString locationPrefix = "(?) ";
			wxString noname = locationPrefix + "<untitled> " + wxString::Format ("%d", m_fileNum);
			m_book->SetPageText (pageNr, noname);
			m_currentEd->ResetEditor();
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
	ChameleonEditor *edit = static_cast <ChameleonEditor * > (m_book->GetPage (pageNr));

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

		
		// the file hasn't been saved yet, grab the "<untitled> #" bit from the tab
		if(fileName == wxEmptyString)
		{
			int selectedTab = m_book->GetSelection();
			wxString tabText = m_book->GetPageText(selectedTab);

			int idx = tabText.Index('*');

			if(idx != wxNOT_FOUND)
			{
				tabText.Remove(idx, 1);
			}

			// remove the (R), (L), or (?) from the tab
			wxRegEx reLocationPrefix = "\\((R|L|\\?)\\) ";
			reLocationPrefix.Replace(&tabText, wxEmptyString);
			
			fileName = tabText;
		}
		saveMessage += fileName;
		
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
			SaveFile(false, FILE_ALLSOURCES);
			m_currentEd = tmpCurrentEd;
			m_currentEd->SetFocus();

			if (edit->Modified()) 
			{

				wxString errorMessage = fileName + " could not be saved!";
				wxMessageBox (_(errorMessage), _("File not closed"),
					wxOK | wxICON_EXCLAMATION);
				m_currentEd->Refresh();
			}
		}

		return result;
	}
	// if I'm here, doesn't matter if I return wxNO or wxYES, just as long as it's not wxCANCEL
	return wxNO;
}

void ChameleonWindow::OnFileOpen (wxCommandEvent &event)
{
	int id = event.GetId();
	if( id == ID_OPEN_SOURCE_LOCAL)
	{
		m_remoteMode = false;
	}
	else if(id == ID_OPEN_SOURCE_REMOTE)
	{
		m_remoteMode = true;
	}
	else
	{
		wxLogDebug("OnFileOpen: Invalid menu ID.  ID: %d", id);
		return;
	}

	wxArrayString fnames = OpenFile(FILE_ALLSOURCES);

	if(fnames.Count() > 0)
	{
		OpenSourceFile (fnames);
		PageHasChanged (m_currentPage);
	}
	
}

wxArrayString ChameleonWindow::OpenFile(FileFilterType filterType)
{
	wxArrayString fnames;
	if (!m_currentEd)
	{
		return fnames;
	}

	wxString filterString;

	switch(filterType)
	{
		case FILE_ALLSOURCES:
			filterString = m_filterCPPFiles;
			filterString += "|";
			filterString += m_filterHeaderFiles;
			filterString += "|";
			filterString += m_filterAllFiles;
			break;
		case FILE_SOURCEONLY:
			filterString = m_filterCPPFiles;
			break;
		case FILE_HEADERONLY:
			filterString = m_filterHeaderFiles;
			break;
		case FILE_PROJECT:
			filterString = m_filterProjectFiles;
			break;
		case FILE_ALLFILES:
		default:
			filterString = m_filterAllFiles;
			break;
	}

	if(!m_remoteMode)
	{
		wxFileDialog dlg (this, _("Open"), "", "", filterString,
			// | wxMULTIPLE
			wxOPEN | wxFILE_MUST_EXIST  | wxCHANGE_DIR);
		if (dlg.ShowModal() != wxID_OK)
		{
			return fnames;
		}
		m_currentEd->SetFocus();
		dlg.GetPaths (fnames);
	}
	else
	{
		SetStatusText("Showing remote file dialog...");
		wxBeginBusyCursor();

		if(m_remoteFileDialog->Prepare(true, filterString))
		{
			wxEndBusyCursor();
			int result = m_remoteFileDialog->ShowModal();
			m_currentEd->SetFocus();

			if(result != wxOK)
			{
				return fnames;
			}

			wxString remoteFileName = m_remoteFileDialog->GetRemoteFileNameAndPath();
			fnames.Add(remoteFileName);
		}
		else
		{
			wxEndBusyCursor();
			return fnames;
		}
	}

	return fnames;	
}

void ChameleonWindow::OpenSourceFile (wxArrayString fnames) 
{
	int firstPageNr = -1;
	wxString fileContents = wxEmptyString;
	wxString fileNameNoPath;

	wxString locationPrefix;

	if(m_remoteMode)
	{
		locationPrefix = "(R) ";
	}
	else
	{
		locationPrefix = "(L) ";
	}


	for (size_t n = 0; n < fnames.GetCount(); n++) 
	{
		fileNameNoPath = wxEmptyString;
		int pageNr = GetPageNum(fnames[n]);

		if(!GetFileContents(fnames[n], fileContents, fileNameNoPath))
		{
			return;
		}

		if(fileContents == wxEmptyString)
		{
			return;
		}

		wxFileName newFileName(fnames[n]);

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
			
			m_currentEd = static_cast< ChameleonEditor * > (m_book->GetPage (m_currentPage));
			m_currentEd->LoadFileText(fileContents);
		}
		// current buffer is empty and untouched, so load the file into it
		else if (m_currentEd && 
					(!m_currentEd->Modified() && 
					 !m_currentEd->HasBeenSaved() && 
					 m_currentEd->GetText().IsEmpty()) ) 
		{
			m_currentEd->LoadFileText(fileContents);

			m_currentEd->SetFilename(newFileName, m_remoteMode);

			m_book->SetPageText(m_currentPage, locationPrefix + fileNameNoPath);
		}
		// need to create a new buffer for the file
		else
		{
			ChameleonEditor *edit = new ChameleonEditor (this, m_book, -1);

			edit->LoadFileText(fileContents);
			edit->SetFilename(newFileName, m_remoteMode);
			m_currentEd = edit;
			m_currentPage = m_book->GetPageCount();
			m_book->AddPage (m_currentEd, locationPrefix + fileNameNoPath, true);
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
		edit = static_cast <ChameleonEditor *> (m_book->GetPage(pageNum));

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
	//wxIPV4address local;
	//local.LocalHost();

	NetworkStatus isok = m_network->GetStatus();
	if(isok == NET_GOOD) 
	{ 
		m_terminal->Connect(wxEmptyString, wxEmptyString, wxEmptyString);
		wxLogDebug("Connected: %d", m_terminal->IsConnected());
	}
	else 
	{
		wxLogDebug("Tried to start Terminal without Networking == good");
	}

}


// my "I need to try something out, I'll stick it in here" function
void ChameleonWindow::Test(wxCommandEvent& WXUNUSED(event))
{
	TextManager tm = m_terminal->GetTM();

	//tm.PrintViewport();

	//tm.PrintContents();

	tm.PrintToBitmap();
	/*
	wxString homepath = m_network->GetHomeDirPath();
	wxString filename = "~/java/numeric/GCD.java";
	filename.Replace("~", homepath);
	wxFileName newpath(filename);
	newpath.MakeRelativeTo("/some/other/directory/entirely", wxPATH_UNIX);
	wxLogDebug("Home: %s, relative path: %s", homepath.c_str(), newpath.GetFullPath(wxPATH_UNIX).c_str());
*/


	/*
	if(!m_bProjectOpen)
	{
		return;
	}

	for(size_t i = 0; i < m_currentProjectInfo->headerFiles.Count(); i++)
	{
		wxLogDebug("Header %d: %s", i, m_currentProjectInfo->headerFiles[i].c_str());
	}

	for(size_t i = 0; i < m_currentProjectInfo->sourceFiles.Count(); i++)
	{
		wxLogDebug("Source %d: %s", i, m_currentProjectInfo->sourceFiles[i].c_str());
	}

	for(size_t i = 0; i < m_currentProjectInfo->libraryFiles.Count(); i++)
	{
		wxLogDebug("Library %d: %s", i, m_currentProjectInfo->libraryFiles[i].c_str());
	}	
	*/
	
	/*
	wxArrayString choices;
	choices.Add("Remote");
	choices.Add("Local");
	wxString choice = wxGetSingleChoice("Test message", "Test caption",choices);
	wxLogDebug("choice = \"%s\"", choice);
	*/

	
	

	/*
	wxString testText = "(?) <untitled> 1";
	wxRegEx reTesting = "\\((R|L|\\?)\\) ";

	if(reTesting.Matches(testText))
	{
		wxString otherMatch = reTesting.GetMatch(testText, 1);
		//wxLogDebug("match: \"%s\", otherMatch: \"%s\"", match, otherMatch);
	}
	
	*/	


	/*
	wxString fileContents;
	fileContents = "[Test1]\nitem1=blah\nitem2=blah2\nitem3=blah3";
	wxMemoryInputStream projectFileStream(fileContents, fileContents.Len());

	//wxFileConfig config(projectFileStream);
	wxFileConfig config("ChameleonTest", wxEmptyString, "c:\\temp\\ChameleonTest.cpj");
	config.Write("Test1/testing", "atest");
	config.Write("Test2/another", "test");

	config.Flush();
	*/

	/*
	wxMemoryOutputStream outputStream;

	config.FlushToStream(outputStream);

	wxString resultContents;
	size_t streamsize = outputStream.GetSize();
	char* bufptr = resultContents.GetWriteBuf(streamsize);
	outputStream.CopyTo(bufptr,streamsize);
	resultContents.UngetWriteBuf();
	*/

	int q = 42;

	/*
	wxString fingerprint = "blah blah 1024 37:d3:00:80:6d:69:8d:ff:cd:81:20:a2:8a:93:39:ba blah blah";
	wxRegEx reFingerprint = "[[:digit:]]+[[:blank:]]+([[:xdigit:]]{2}:)+[[:xdigit:]]{2}";

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
	*/
	


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
	SaveFile(false, FILE_ALLSOURCES);
}

void ChameleonWindow::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	SaveFile(true, FILE_ALLSOURCES);
}

bool ChameleonWindow::SaveFile(bool saveas, FileFilterType filterType)
{
	wxString filename;
	wxString fileContents;

	bool originalRemoteMode = m_remoteMode;

	bool doSaveAs = saveas || !m_currentEd->HasBeenSaved() || (m_remoteMode != m_currentEd->LastSavedRemotely());

	//bool remoteEnabled = m_perms->isEnabled(PERM_REMOTELOCAL);
	bool isSourceFile = true;

	wxString filterString;
	switch(filterType)
	{
	case FILE_ALLSOURCES:
		filterString = m_filterCPPFiles;
		filterString += "|";
		filterString += m_filterHeaderFiles;
		filterString += "|";
		filterString += m_filterAllFiles;
		break;
	case FILE_SOURCEONLY:
		filterString = m_filterCPPFiles;
		break;
	case FILE_HEADERONLY:
		filterString = m_filterHeaderFiles;
		break;
	case FILE_PROJECT:
		filterString = m_filterProjectFiles;
		isSourceFile = false;
		break;
	case FILE_ALLFILES:
	default:
		filterString = m_filterAllFiles;
		break;
	}

	// default to saving remotely
	m_remoteMode = false;

	//if(doSaveAs && remoteEnabled)
	
	// assume for the moment that we won't have remote mode as a permission
	if(doSaveAs)
	{
		wxArrayString choices;
		choices.Add("Remote");
		choices.Add("Local");
		wxString choice = wxGetSingleChoice("Would you like to save this file: \n\n1) Remotely (on the server) \n2) Locally (on the computer you're sitting at)?", 
											"Save File - location",choices);
		if(choice == "Local")
		{
			m_remoteMode = false;
		}
		else if(choice == wxEmptyString)
		{
			// user hit Cancel - don't save
			m_remoteMode = originalRemoteMode;
			return false;
		}
	}
	// new project files will always have doSaveAs = true, 
	// so this just needs to be for source files
	else
	{
		m_remoteMode = m_currentEd->LastSavedRemotely();
	}

	if(isSourceFile)
	{
		fileContents = m_currentEd->GetText();
	}
	// we must be saving a new project
	else
	{
		fileContents = "[Headers]\n\n[Sources]\n\n[Libraries]\n";
	}

	if(!m_remoteMode)
	{
		if(doSaveAs)
		{		
			// the last item in a filter's list will be the default extension if none is given
			// ie, right now, .cpp is the default extension for C++ files

			wxString title = "Save File As";

			if(filterType == FILE_PROJECT)
			{
				title = "Save New Project";
			}
			wxFileDialog dlg (this, title, "", "", filterString, 
								wxSAVE | wxOVERWRITE_PROMPT | wxCHANGE_DIR);

			// ie, user clicked cancel
			if(dlg.ShowModal() != wxID_OK) 
			{ 
				return false; 
			}

			filename = dlg.GetPath();
		}
		
		else
		{
			filename = m_currentEd->GetFileNameAndPath();
		}
		
		if(isSourceFile)
		{
			m_currentEd->SetFocus();

			wxFileName fn(filename);
			m_currentEd->SetFilename(fn, false);
			m_currentEd->SaveFile(filename);

			wxString simpleFileName = m_currentEd->GetFilename();

			int currentTab = m_book->GetSelection();

			wxString locationPrefix = "(L) ";
			m_book->SetPageText(currentTab, locationPrefix + simpleFileName);
			m_book->Refresh();
		}
		else
		{
			wxFile newProjectFile(filename, wxFile::write);
			newProjectFile.Write(fileContents);
			newProjectFile.Close();
		}
		
	}
	// remote mode
	else
	{
		wxString remotePath, remoteFile;

		if(doSaveAs)
		{
			if(m_remoteFileDialog->Prepare(false, filterString))
			{
				int result = m_remoteFileDialog->ShowModal();
				m_currentEd->SetFocus();

				if(result != wxOK)
				{
					return false;
				}

				remotePath = m_remoteFileDialog->GetRemotePath();
				remoteFile = m_remoteFileDialog->GetRemoteFileName();
			}
			else
			{
				return false;
			}			
		}
		else
		{
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
			return false;
		}

		if(isSourceFile)
		{
			wxString currentFilenameAndPath = m_currentEd->GetFileNameAndPath();
			if((currentFilenameAndPath == wxEmptyString) ||
				(currentFilenameAndPath != wxFileName(remotePath, remoteFile, wxPATH_UNIX).GetFullPath()))
			{
				int currentTab = m_book->GetSelection();
				wxString locationPrefix = "(R) ";
				m_book->SetPageText(currentTab, locationPrefix + remoteFile);
			}

			wxFileName fn(remotePath, remoteFile, wxPATH_UNIX);
			m_currentEd->SetFilename(fn, true);
		}
		else
		{
			// store the name so we can set the name in the project tree
			filename = wxFileName(remotePath, remoteFile, wxPATH_UNIX).GetFullPath();
		}		
	}

	if(isSourceFile)
	{
		m_currentEd->EmptyUndoBuffer();
		m_currentEd->SetSavePoint();
		m_book->Refresh();
	}
	else
	{
		if(m_bProjectOpen)
		{
			CloseProjectFile();
		}
		

		m_currentProjectInfo = new ProjectInfo;
		m_bProjectOpen = true;

		m_currentProjectInfo->isRemote = m_remoteMode;
		wxFileName projectPath(filename);
		m_currentProjectInfo->projectName = projectPath.GetFullName();
		m_currentProjectInfo->projectBasePath = projectPath.GetPath(true, m_remoteMode ? wxPATH_UNIX : wxPATH_DOS);

		wxTreeItemId rootItem = m_projectTree->GetRootItem();
		m_projectTree->SetItemText(rootItem, projectPath.GetFullName());
	}

	return true;
}


void ChameleonWindow::ResizeSplitter()
{

}

void ChameleonWindow::OnToolsOptions(wxCommandEvent &event)
{		
	//UpdatePermsList();

	if(m_terminal->IsConnected())
	{
		m_optionsDialog->DisableServerSettings();
	}
	else
	{
		m_optionsDialog->EnableServerSettings();
	}

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

	// assuming we assigned it properly, Set it 
	if(target != NULL)
	{
		*target = value;
	}

}

// same as SetIntVar, only getting
int ChameleonWindow::GetIntVar(int variableName)
{
	int* target = TargetInt(variableName);

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

void ChameleonWindow::UpdateMenuBar()
{
	wxMenuBar* menuBar = GetMenuBar();

	bool localModeEnabled = m_perms->isEnabled(PERM_LOCALMODE);
	if(menuBar != NULL)
	{
		SetMenuBar(NULL);
		delete menuBar;
	}

	menuBar = new wxMenuBar();
	SetMenuBar(menuBar);

	wxMenu* menuFile = new wxMenu();

	menuFile->Append(ID_NEW_SOURCE, "&New\tCtrl-N", "Create a new C++ file");
	if(localModeEnabled)
	{
		menuFile->Append(ID_OPEN_SOURCE_LOCAL, "&Open C++ File (Local)", "Open an existing C++ file");
	}
	
	menuFile->Append(ID_OPEN_SOURCE_REMOTE, "&Open C++ File (Remote)", "Open an existing C++ file");
	menuFile->Append(ID_CLOSEPAGE, "&Close", "Close the active file");
	menuFile->Append(ID_CLOSEALL, "Close All Files");
	menuFile->AppendSeparator();
	menuFile->Append(ID_SAVE, "&Save C++ File\tCtrl-S", "Save the current file");
	menuFile->Append(ID_SAVE_AS, "Save C++ File &As", "Save the current file as a different name");
	menuFile->AppendSeparator();

	if(m_perms->isEnabled(PERM_PROJECTS))
	{
		menuFile->Append(ID_NEW_PROJECT, "Create New Project");

		if(localModeEnabled)
		{
			menuFile->Append(ID_OPEN_PROJECT_LOCAL, "Open Project File (Local)");
		}
		
		menuFile->Append(ID_OPEN_PROJECT_REMOTE, "Open Project File (Remote)");
		menuFile->Append(ID_CLOSE_PROJECT, "Close Project");
		menuFile->AppendSeparator();
	}
	
	menuFile->Append(ID_QUIT, "E&xit\tAlt-F4", "Quit this program");

	menuBar->Append(menuFile, "&File");


	wxMenu* menuEdit = new wxMenu();

	menuEdit->Append(ID_UNDO, "&Undo\tCtrl-Z");
	menuEdit->Append(ID_REDO, "&Redo\tCtrl-Y");
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_CUT, "Cu&t\tCtrl-X");
	menuEdit->Append(ID_COPY, "&Copy\tCtrl-C");
	menuEdit->Append(ID_PASTE, "&Paste\tCtrl-V");
	
	// TODO Implement Edit menu items

	/*
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_INDENT, "Indent text\tTab");
	menuEdit->Append(ID_UNINDENT, "Unindent text\tShift-Tab");
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_SELECTALL, "Select all\tCtrl-A");
	menuEdit->Append(ID_SELECTLINE, "Select line");
	*/

	menuBar->Append(menuEdit, "&Edit");


	wxMenu* menuTools = new wxMenu();

	menuTools->Append(ID_OPTIONS, "&Options");

	if(m_perms->isEnabled(PERM_TERMINAL))
	{
		menuTools->AppendSeparator();

		menuTools->Append(ID_STARTCONNECT, "&Connect to server");
		menuTools->Append(ID_DISCONNECT, "&Disconnect from server");
	}

	menuBar->Append(menuTools, "&Tools");


	if(m_perms->isEnabled(PERM_COMPILE))
	{
		wxMenu* compileMenu = new wxMenu();
		compileMenu->Append(ID_COMPILE, "Compile File");
		menuBar->Append(compileMenu, "&Compile");
	}


	/*
	if(m_perms->isEnabled(PERM_PROJECTS))
	{
		wxMenu* projectMenu = new wxMenu();
		projectMenu->Append(ID_PROJECT_ADDFILE, "Add file to project");
		projectMenu->Append(ID_PROJECT_REMOVEFILE, "Remove file from project");
		projectMenu->Append(ID_PROJECT_SETTINGS, "Project settings");
		menuBar->Append(projectMenu, "&Project");
	}
	*/


	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(ID_HELP, "Chameleon help contents");
	helpMenu->Append(ID_ABOUT, "&About Chameleon", "Show about dialog");	
	
	menuBar->Append(helpMenu, "&Help");



}

void ChameleonWindow::UpdateToolbar()
{
	wxToolBar* t = GetToolBar();
	t->ClearTools();

	/* Since wxWindows 2.4.2 doesn't let us find tools by ID, the
	 * only way to handle this is to delete the tools to make
	 * sure they don't exist, then conditionally re-add them.
	 * Fortunately, if a tool doesn't exist, DeleteTool() does nothing.
	 * This is made easier in wxWindows 2.5.1, with FindById();
	 */

	/*
	t->DeleteTool(ID_TEST);

	t->DeleteTool(ID_COMPILE);

	for(int i = ID_DEBUG_IDS_FIRST; i < ID_DEBUG_IDS_LAST; i++)
	{
		t->DeleteTool(i);		
	}
	*/
	

	wxBitmap bmNew(new_xpm);
	t->AddTool(ID_NEW_SOURCE, "New", bmNew);	

    
	wxBitmap bmOpen(open_xpm);

	if(m_perms->isEnabled(PERM_LOCALMODE))
	{
		t->AddTool(ID_OPEN_SOURCE_LOCAL, "Open (L)", bmOpen);
	}
	
	t->AddTool(ID_OPEN_SOURCE_REMOTE, "Open (R)", bmOpen);

	wxBitmap bmSave(save_xpm);
	t->AddTool(ID_SAVE, "Save", bmSave);

	if(m_perms->isEnabled(PERM_TERMINAL))
	{
		t->AddSeparator();

		wxBitmap bmConnect(connect16_xpm);
		t->AddTool(ID_STARTCONNECT, "Connect", bmConnect);

		wxBitmap bmDisconnect(disconnect16_xpm);
		t->AddTool(ID_DISCONNECT, "Disconnect", bmDisconnect);
		
	}


	if(m_perms->isEnabled(PERM_TEST))
	{
		t->AddSeparator();	

		wxBitmap bmTest(button_xpm);
		t->AddTool(ID_TEST, "Test", bmTest);
	}

	if(m_perms->isEnabled(PERM_COMPILE))
	{
		t->AddSeparator();
		wxBitmap bmCompile(build_xpm);
		t->AddTool(ID_COMPILE, "Compile", bmCompile);
	}
	
	if(m_perms->isEnabled(PERM_DEBUG))
	{
		t->AddSeparator();
		wxBitmap bmStart(start_xpm);
		t->AddTool(ID_START, "Run", bmStart);

		wxBitmap bmPause(pause_xpm);
		t->AddTool(ID_PAUSE, "Pause", bmPause);

		wxBitmap bmStop(stop_xpm);
		t->AddTool(ID_STOP, "Stop", bmStop);

		wxBitmap bmStepNext(stepnext_xpm);
		t->AddTool(ID_STEPNEXT, "Step next", bmStepNext);

		wxBitmap bmStepOver(stepover_xpm);
		t->AddTool(ID_STEPOVER, "Step over", bmStepOver);

		wxBitmap bmStepOut(stepout_xpm);
		t->AddTool(ID_STEPOUT, "Step out", bmStepOut);
	}

	t->Realize();

}

bool ChameleonWindow::UpdateAuthCode()
{
	wxString newAuthCode = m_optionsDialog->GetAuthCode();

	newAuthCode.MakeUpper();
	if(m_perms->setGlobalAuthorized(newAuthCode))
	{
		m_config->Write("Permissions/authorized", newAuthCode);
		UpdatePermsList();
		m_optionsDialog->SetAuthCode(newAuthCode);
		return true;
	}
	else
	{
		return false;
	}	
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


	UpdateToolbar();
	UpdateMenuBar();
	UpdateTerminalNotebook();

	if(m_perms->isEnabled(PERM_PROJECTS))
	{
		if(!m_projectTree->IsShown())
		{
			m_splitProjectEditor->SplitVertically(m_projectTree, m_book, 200);
			m_splitProjectEditor->SetMinimumPaneSize(20);
			m_projectTree->Show();

		}
	}
	else
	{
		if(m_projectTree->IsShown())
		{
			m_splitProjectEditor->Unsplit(m_projectTree);
			//m_splitProjectEditor->Initialize(m_book);
			m_projectTree->Hide();
			m_book->Refresh();
		}
	}

	for(int i = 0; i < m_book->GetPageCount(); i++)
	{
		ChameleonEditor* edit = static_cast<ChameleonEditor*> (m_book->GetPage(i));
		edit->UpdateSyntaxHighlighting();
	}

	m_options->SetHostname(m_optionsDialog->GetServerAddress());
	m_options->SetUsername(m_optionsDialog->GetUsername());
	if(! m_options->SetPassphrase(m_optionsDialog->GetPassword1()) ) {
		// Password has a " in it
		// Mark, use this as you feel fit.
	}
	

	m_config->Write("Network/hostname", m_options->GetHostname());
	m_config->Write("Network/username", m_options->GetUsername());

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

void ChameleonWindow::OnSplitterDoubleClick(wxSplitterEvent &event)
{
	event.Veto();
}

void ChameleonWindow::OnSize(wxSizeEvent &event)
{
	event.Skip();	
}

void ChameleonWindow::PassImageList(wxImageList* imagelist)
{
	m_projectTree->AssignImageList(imagelist);
}

void ChameleonWindow::OnTreeItemRightClick(wxTreeEvent& event)
{
	if(!m_bProjectOpen)
	{
		wxMessageBox("You do not currently have a project open.", "No project open", 
					wxOK | wxCENTRE | wxICON_EXCLAMATION);
		return;
	}
    
	wxTreeItemId item = event.GetItem();
	m_clickedTreeItem = item;
	wxMenu popupMenu;

	wxString label = "Add file";		

	if(m_clickedTreeItem == m_projectFileFolders[0])
	{		
		m_projectSelectedFolderType = FILE_SOURCEONLY;
	}
	else if(m_clickedTreeItem == m_projectFileFolders[1])
	{		
		m_projectSelectedFolderType = FILE_HEADERONLY;
	}
	else if(m_clickedTreeItem == m_projectFileFolders[2])
	{		
		m_projectSelectedFolderType = FILE_LIBRARY;
	}

	if(m_projectTree->GetItemParent(item) == m_projectTree->GetRootItem())
	{
		switch(m_projectSelectedFolderType)
		{
			case FILE_SOURCEONLY:
				label = "Add source file";
				break;
			case FILE_HEADERONLY:
				label = "Add header file";
				break;
			case FILE_LIBRARY:
				label = "Add library";
				break;
		}

		popupMenu.Append(ID_PROJECT_ADDFILE, label);

	}
	// user right-clicked a file in the project
	else
	{
		popupMenu.Append(ID_PROJECT_REMOVEFILE, "Remove file from project");
	}

	wxPoint p = event.GetPoint();
	m_projectTree->PopupMenu(&popupMenu, p);
}

void ChameleonWindow::OnOpenProjectFile(wxCommandEvent &event)
{	
	int id = event.GetId();
	wxPathFormat currentPathFormat = wxPATH_UNIX;
	if( id == ID_OPEN_PROJECT_LOCAL)
	{
		m_remoteMode = false;
		currentPathFormat = wxPATH_DOS;
	}
	else if(id == ID_OPEN_PROJECT_REMOTE)
	{
		m_remoteMode = true;
	}
	else
	{
		wxLogDebug("OnOpenProjectFile: Invalid menu ID.  ID: %d", id);
		return;
	}

	// TODO Check right here if a project is already open, and if it is,
	//		ask the user if he wants to replace the open one
	
	if(m_bProjectOpen)
	{
		wxString message = "A project is already open.  Do you want to replace the currently open project";
		message += " with a different one?";
		int result = wxMessageBox(message, "Project already open", wxOK | wxCANCEL);

		if(result == wxCANCEL)
		{
			return;
		}
	}	

	wxArrayString fileNames = OpenFile(FILE_PROJECT);

	if(fileNames.Count() < 1)
	{
		wxLogDebug("No files selected when adding to project");
		return;
	}

	m_projectTree->Freeze();
	if(m_bProjectOpen)
	{
		CloseProjectFile();
	}	
	
	m_currentProjectInfo = new ProjectInfo;

	m_projectTree->DeleteChildren(m_projectFileFolders[0]);
	m_projectTree->DeleteChildren(m_projectFileFolders[1]);
	m_projectTree->DeleteChildren(m_projectFileFolders[2]);

	wxString fileContents;
	wxString fileName;

	GetFileContents(fileNames[0], fileContents, fileName);


	// set the current project's base path based on the project file's directory, 
	// choosing the path separators based on where the file is stored

	wxFileName projectFileName(fileNames[0]);

	m_currentProjectInfo->projectBasePath = projectFileName.GetPath(currentPathFormat);
	m_currentProjectInfo->projectName = projectFileName.GetFullName();
	m_currentProjectInfo->isRemote = m_remoteMode;

	wxTreeItemId rootItem = m_projectTree->GetRootItem();
	m_projectTree->SetItemText(rootItem, fileName);
	m_projectTree->SetItemBold(rootItem);

	wxMemoryInputStream projectFileStream(fileContents, fileContents.Len());
	
	wxFileConfig config(projectFileStream);


	LoadFilesIntoProjectTree("/Sources", "source", m_projectFileFolders[0], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Headers", "header", m_projectFileFolders[1], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Libraries", "library", m_projectFileFolders[2], config, currentPathFormat);

	m_projectTree->Thaw();

	m_bProjectOpen = true;
}


void ChameleonWindow::OnAddFileToProject(wxCommandEvent &event)
{
	wxLogDebug("event id: %d", event.GetId());
	m_remoteMode = m_currentProjectInfo->isRemote;
	wxArrayString fileNames = OpenFile(m_projectSelectedFolderType);

	if(fileNames.Count() < 1)
	{
		wxLogDebug("No files selected when adding to project");
		return;
	}

	wxString fileToOpen = fileNames[0];

	wxPathFormat currentPathFormat = wxPATH_DOS;

	wxString basePath = m_currentProjectInfo->projectBasePath;	

	if(m_currentProjectInfo->isRemote)
	{
		currentPathFormat = wxPATH_UNIX;
		wxString homepath = m_network->GetHomeDirPath();
		fileToOpen.Replace("~", homepath);
		basePath.Replace("~", homepath);
	}

	wxFileName relativeFilePath(fileToOpen);
	relativeFilePath.MakeRelativeTo(basePath, currentPathFormat);

	if(m_currentProjectInfo->FileExistsInProject(relativeFilePath.GetFullPath(currentPathFormat)))
	{
		wxString message = "The file " + fileNames[0] + " already exists in this project, so it was not added.";
		wxMessageBox(message, "File already in project", wxOK | wxCENTRE | wxICON_EXCLAMATION);
		return;
	}

	wxString fileContents;
	wxString fileName;
	

	m_remoteMode = m_currentProjectInfo->isRemote;
	GetFileContents(fileNames[0], fileContents, fileName);

	m_currentProjectInfo->AddFileToProject(relativeFilePath.GetFullPath(currentPathFormat), m_projectSelectedFolderType);

	int iconIndex = m_remoteFileDialog->GetIconIndex(relativeFilePath.GetExt());

	FileNameTreeData* data = new FileNameTreeData();
	data->filename = fileNames[0];
	
	m_projectTree->AppendItem(m_clickedTreeItem, fileName, iconIndex, -1, data);
	m_projectTree->SortChildren(m_clickedTreeItem);
	m_projectTree->Refresh();	
}

void ChameleonWindow::OnTreeItemActivated(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();

	wxTreeItemId rootItem = m_projectTree->GetRootItem();

	if( item != rootItem)
	{
		wxTreeItemId parentItem = m_projectTree->GetItemParent(item);

		if( (parentItem == m_projectFileFolders[0]) ||
			(parentItem == m_projectFileFolders[1]) ||
			(parentItem == m_projectFileFolders[2]))
		{
			FileNameTreeData* data = static_cast <FileNameTreeData* > (m_projectTree->GetItemData(item));
			wxLogDebug("Activated filename: %s", data->filename);
			wxArrayString filesToOpen;
			filesToOpen.Add(data->filename);

			OpenSourceFile(filesToOpen);
		}
	}
	m_currentEd->Refresh();
	m_book->Refresh();
}

void ChameleonWindow::CloseProjectFile()
{
	wxPathFormat currentPathFormat = (m_currentProjectInfo->isRemote ? wxPATH_UNIX : wxPATH_DOS);
	wxMemoryInputStream mis("", 0);
	wxFileConfig config(mis);

	config.SetPath("/Headers");
	for(size_t i = 0; i < m_currentProjectInfo->headerFiles.Count(); i++)
	{
		wxString headerName = "header";
		headerName << (i + 1);
		wxFileName relativeName(m_currentProjectInfo->headerFiles[i]);
		config.Write(headerName, relativeName.GetFullPath(currentPathFormat));
	}

	config.SetPath("/Sources");
	for(size_t i = 0; i < m_currentProjectInfo->sourceFiles.Count(); i++)
	{
		wxString sourceName = "source";
		sourceName << (i + 1);
		wxFileName relativeName(m_currentProjectInfo->sourceFiles[i]);
		config.Write(sourceName, relativeName.GetFullPath(currentPathFormat));
	}

	config.SetPath("/Libraries");
	for(size_t i = 0; i < m_currentProjectInfo->libraryFiles.Count(); i++)
	{
		wxString libraryName = "library";
		libraryName << (i + 1);
		wxFileName relativeName(m_currentProjectInfo->libraryFiles[i]);
		config.Write(libraryName, relativeName.GetFullPath(currentPathFormat));
	}

	wxMemoryOutputStream outputStream;

	config.FlushToStream(outputStream);

	wxString resultContents;
	size_t streamsize = outputStream.GetSize();

	if(streamsize == 0)
	{
		resultContents = "[Headers]\n\n[Sources]\n\n[Libraries]\n";
	}
	else
	{
		char* bufptr = resultContents.GetWriteBuf(streamsize);
		outputStream.CopyTo(bufptr,streamsize);
		resultContents.UngetWriteBuf();
	}
	

	if(m_currentProjectInfo->isRemote)
	{
		m_network->SendFileContents(resultContents, m_currentProjectInfo->projectName, 
									m_currentProjectInfo->projectBasePath);
	}
	else
	{
		wxString filename = m_currentProjectInfo->projectBasePath + "\\" +
							m_currentProjectInfo->projectName;
		wxFile projectFile(filename, wxFile::write);
		projectFile.Write(resultContents);
		projectFile.Close();
		
	}
	
	m_projectTree->DeleteChildren(m_projectFileFolders[0]);
	m_projectTree->DeleteChildren(m_projectFileFolders[1]);
	m_projectTree->DeleteChildren(m_projectFileFolders[2]);

	m_projectTree->SetItemText(m_projectTree->GetRootItem(), "No project");

	delete m_currentProjectInfo;
	m_bProjectOpen = false;
}

void ChameleonWindow::LoadFilesIntoProjectTree(wxString configPath, wxString configEntryBaseName, 
										   wxTreeItemId treeid, wxFileConfig& config, wxPathFormat currentPathFormat)
{
	config.SetPath(configPath);

	int numEntries = config.GetNumberOfEntries();

	for(int i = 1; i <= numEntries; i++)
	{
		wxString keyname = configEntryBaseName;
		keyname << i;
		wxString fileName = config.Read(keyname);		

		if(fileName != wxEmptyString)
		{
			wxFileName newFileName(fileName);
			int iconNum = m_remoteFileDialog->GetIconIndex(newFileName.GetExt());

			FileNameTreeData* data = new FileNameTreeData();
			data->filename = newFileName.GetFullPath(currentPathFormat);

			m_projectTree->AppendItem(treeid, newFileName.GetFullName(), iconNum, -1, data);

			if(configEntryBaseName == "source")
			{
				m_currentProjectInfo->sourceFiles.Add(fileName);
			}
			else if(configEntryBaseName == "header")
			{
				m_currentProjectInfo->headerFiles.Add(fileName);
			}
			else if(configEntryBaseName == "library")
			{
				m_currentProjectInfo->libraryFiles.Add(fileName);
			}
			else
			{
				wxLogDebug("Bad entry string in CW::LFIPT.  Value: %s", configEntryBaseName.c_str());
			}
		}
	}

	m_projectTree->SortChildren(treeid);
}

void ChameleonWindow::OnDisconnect(wxCommandEvent &event)
{
	m_terminal->Disconnect();
}

void ChameleonWindow::OnCompile(wxCommandEvent &event)
{
	m_compilerTextbox->Clear();

	bool doCompile = true;

	if(!m_currentEd->HasBeenSaved())
	{
		wxMessageBox("Please save this file before trying to compile it.");

		doCompile = false;
	}
	else if(m_currentEd->Modified())
	{
		// yes, same message... this can later be changed to ask about 
		// saving, then do the appropriate thing.
		wxMessageBox("Please save this file before trying to compile it.");

		doCompile = false;
	}

	if(doCompile)
	{
		bool isAdvanced = m_perms->isEnabled(PERM_ADVANCEDCOMPILE);
		bool isProj = false;
		if(m_currentProjectInfo != NULL && 
			m_currentProjectInfo->FileExistsInProject(m_currentEd->GetFilename()) ) 
		{
			isProj = true;
		}

		if(!isAdvanced) 
		{
			if(isProj) 
			{
				m_compiler->CompileProject(m_currentProjectInfo, m_remoteMode, m_compilerTextbox);
			}
			else 
			{
				m_compiler->CompileFile(m_currentEd->GetFilePath(), m_currentEd->GetFilename(), m_remoteMode, m_compilerTextbox);
			}
		}
		else 
		{
			// Forward Planning
			wxMessageBox("Advanced Compiling is currently not supported.");
		}
	}
}

void ChameleonWindow::OnFileNewProject(wxCommandEvent &event)
{
	SaveFile(true, FILE_PROJECT);
}

void ChameleonWindow::OnRemoveFileFromProject(wxCommandEvent &event)
{
	FileNameTreeData* treeData = static_cast<FileNameTreeData*> (m_projectTree->GetItemData(m_clickedTreeItem));

	if(m_currentProjectInfo->FileExistsInProject(treeData->filename))
	{
		m_currentProjectInfo->RemoveFileFromProject(treeData->filename, m_projectSelectedFolderType);
		m_projectTree->Delete(m_clickedTreeItem);
	}
}

void ChameleonWindow::OnFileCloseAll(wxCommandEvent &event)
{
	CloseAllFiles();
}

void ChameleonWindow::CloseAllFiles()
{
	int cnt = m_book->GetPageCount();
	for (int i = 0; i < cnt; i++) 
	{
		CloseFile();
	}

	PageHasChanged();
}

void ChameleonWindow::UpdateTerminalNotebook()
{
	int splitterLocation = m_splitEditorOutput->GetSashPosition();

	int numPages = m_noteTerm->GetPageCount();
	for(int i = 0; i < numPages; i++)
	{
		wxNotebookPage* page = m_noteTerm->GetPage(0);
		page->Hide();
		m_noteTerm->RemovePage(0);
	}

	if(m_perms->isEnabled(PERM_TERMINAL))
	{
		m_noteTerm->AddPage(m_termContainer, "Terminal");
	}

	bool advCompileEnabled = m_perms->isEnabled(PERM_ADVANCEDCOMPILE);
	bool basicCompileEnabled = m_perms->isEnabled(PERM_COMPILE);

	int compilerTextboxIdx = m_noteTerm->FindPagePosition(m_compilerTextbox);
	bool compilerTextboxShown = (compilerTextboxIdx != -1);
	if(m_perms->isEnabled(PERM_COMPILE))
	{
		if(advCompileEnabled)
		{
			// TODO Advanced compiler listctrl here
		}
		else
		{
			m_noteTerm->AddPage(m_compilerTextbox, "Compiler Output");
		}

	}
	else
	{
		if(advCompileEnabled)
		{

		}
		else
		{
			m_compilerTextbox->Hide();
		}
	}

	if(m_noteTerm->GetPageCount() == 0)
	{
		m_splitEditorOutput->Unsplit();
		m_noteTerm->Hide();
	}
	else
	{
		if(!m_splitEditorOutput->IsSplit())
		{
			m_splitEditorOutput->SplitHorizontally(m_splitProjectEditor, m_noteTerm, -330);//-200);
			m_splitEditorOutput->SetMinimumPaneSize(20);
			m_noteTerm->Show();	
		}		
	}
	m_noteTerm->Refresh();


	if(m_perms->isEnabled(PERM_PROJECTS))
	{
		if(!m_splitProjectEditor->IsSplit())
		{
			m_splitProjectEditor->SplitVertically(m_projectTree, m_book, 200);
			m_splitProjectEditor->SetMinimumPaneSize(20);
			m_projectTree->Show();
		}
		
	}
	else
	{
		m_splitProjectEditor->Unsplit(m_projectTree);
		m_splitProjectEditor->Initialize(m_book);
		m_projectTree->Hide();		
	}
	m_book->Refresh();

}