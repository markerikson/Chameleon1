#define CHAMELEON__CPP

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "ChameleonWindow.h"
#include <wx/statusbr.h>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <wx/msw/tbar95.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/mimetype.h>

#include <math.h>

#include "../common/fixvsbug.h"
#include "../editor/editor.h"
#include "ChameleonNotebook.h"
#include "dialogs/OptionsDialog.h"
#include "dialogs/RemoteFileDialog.h"
#include "dialogs/wxTermContainer.h"
#include "dialogs/VariableWatchPanel.h"
#include "dialogs/CompilerOutputPanel.h"
#include "dialogs/AboutChameleonDialog.h"
#include "../common/ChameleonPrintout.h"
#include "../perms/p.h"
#include "../common/ProjectInfo.h"
#include "../network/networking.h"
#include "../compiler/compiler.h"
#include "../common/Options.h"
#include "../debugger/cham_db.h"
#include "../common/DebugEvent.h"
#include "wxssh.h"
#include "../compiler/compilerevent.h"
#include "IconManager.h"
#include "wxProportionalSplitterWindow.h"
//#include "mmDropMenu.h"

#include "newfile.xpm"
#include "openremote.xpm"
#include "openlocal.xpm"
#include "savefile.xpm"

#include "build.xpm"
#include "compilestop.xpm"
#include "button.xpm"

#include "connect16.xpm"
#include "disconnect16.xpm"

#include "newstart1.xpm"
#include "newcontinue1.xpm"
#include "newstop1.xpm"
#include "stepnext.xpm"
#include "stepout.xpm"
#include "stepover.xpm"
#include "breakpoint_octagon.xpm"
#include "breakpoint_octagon_crossed.xpm"
#include "breakpoint_octagon_disable.xpm"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//! global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL;
wxPageSetupData *g_pageSetupData = (wxPageSetupData*) NULL;

BEGIN_EVENT_TABLE(ChameleonWindow, wxFrame)
	EVT_MENU						(ID_NEW_SOURCE, ChameleonWindow::OnMenuEvent)
	EVT_MENU_RANGE					(ID_OPEN_SOURCE_LOCAL, ID_OPEN_SOURCE_REMOTE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_QUIT,  ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_ABOUT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_TEST, ChameleonWindow::Test)
	EVT_MENU						(ID_SAVE, ChameleonWindow::OnMenuEvent)
	EVT_MENU_RANGE					(ID_SAVE_SOURCE_LOCAL, ID_SAVE_SOURCE_REMOTE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_CLOSEPAGE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_CLOSETAB, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_CLOSEALL, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_STARTCONNECT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_DISCONNECT, ChameleonWindow::OnMenuEvent)
	EVT_MENU_RANGE					(ID_COMPILE, ID_COMPILE_PROJECT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_UNDO, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_REDO, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_OPTIONS, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_CUT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_COPY, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PASTE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_FIND, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_REPLACE, ChameleonWindow::OnMenuEvent)
	EVT_FIND						(-1, ChameleonWindow::OnFindEvent)
	EVT_FIND_NEXT					(-1, ChameleonWindow::OnFindEvent)
	EVT_FIND_REPLACE				(-1, ChameleonWindow::OnFindEvent)
	EVT_FIND_REPLACE_ALL			(-1, ChameleonWindow::OnFindEvent)
	EVT_FIND_CLOSE					(-1, ChameleonWindow::OnFindEvent)
	EVT_CLOSE						(ChameleonWindow::OnClose)
	EVT_NOTEBOOK_PAGE_CHANGED		(ID_NOTEBOOK_ED,   ChameleonWindow::OnPageChange)
	EVT_SPLITTER_SASH_POS_CHANGED	(ID_SPLITEDITOROUTPUT, ChameleonWindow::OnTermResize)
	EVT_SIZE						(ChameleonWindow::OnSize)
	EVT_SPLITTER_DCLICK				(ID_SPLITPROJECTEDITOR, ChameleonWindow::OnSplitterDoubleClick)
	EVT_SPLITTER_DCLICK				(ID_SPLITEDITOROUTPUT, ChameleonWindow::OnSplitterDoubleClick)
	EVT_TREE_ITEM_RIGHT_CLICK		(ID_PROJECTTREE, ChameleonWindow::OnTreeItemRightClick)
	EVT_MENU						(ID_PROJECT_ADDFILE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PROJECT_REMOVEFILE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PROJECT_EXCLUDE_FILE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PROJECT_INCLUDE_FILE, ChameleonWindow::OnMenuEvent)
	EVT_TREE_ITEM_ACTIVATED			(ID_PROJECTTREE, ChameleonWindow::OnTreeItemActivated)
	EVT_MENU						(ID_NEW_PROJECT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_OPEN_PROJECT_LOCAL, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_OPEN_PROJECT_REMOTE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PRINT_PAGE, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PRINT_PREVIEW, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_PRINT_SETUP, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_CLOSE_PROJECT, ChameleonWindow::OnMenuEvent)	
	EVT_MENU						(ID_DEBUG_START, ChameleonWindow::OnDebugCommand)
	EVT_MENU						(ID_DEBUG_CONTINUE, ChameleonWindow::OnDebugCommand)
	EVT_MENU						(ID_DEBUG_STOP, ChameleonWindow::OnDebugCommand)
	EVT_MENU						(ID_DEBUG_STEPNEXT, ChameleonWindow::OnDebugCommand)
	EVT_MENU						(ID_DEBUG_STEPOVER, ChameleonWindow::OnDebugCommand)
	EVT_MENU						(ID_DEBUG_STEPOUT, ChameleonWindow::OnDebugCommand)
	EVT_MENU						(ID_DEBUG_ADDEDITORBREAKPOINT, ChameleonWindow::OnDebugBreakpointCommand)
	EVT_MENU						(ID_DEBUG_REMOVEEDITORBREAKPOINT, ChameleonWindow::OnDebugBreakpointCommand)
	EVT_MENU						(ID_DEBUG_CLEAREDITORBREAKPOINTS, ChameleonWindow::OnDebugBreakpointCommand)
	EVT_DEBUG						(ChameleonWindow::OnDebugEvent)
	EVT_IDLE						(ChameleonWindow::OnIdle)
	EVT_TIMER						(ID_STATUSTIMER, ChameleonWindow::OnStatusTimer)
	EVT_MENU						(ID_HELP, ChameleonWindow::OnMenuEvent)
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

MyApp::~MyApp()
{
}

//----------------------------------------------------------------------


#pragma hdrstop

//////////////////////////////////////////////////////////////////////////////
///  public constructor ChameleonWindow
///  Responsible for instantiating pretty much everything.  It's all initialized here.
///
///  @param  title const wxString & The main window title
///  @param  pos   const wxPoint &  Where on the screen to create the window
///  @param  size  const wxSize &   How big the window should be
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
ChameleonWindow::ChameleonWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{	
	// should be approximately 80x15 for the terminal
	this->SetSize(800, 600);

	// Show a log window for all debug messages
#ifdef _DEBUG
	logWindow = new wxLogWindow(this, "Debug messages");
	wxLog::SetActiveTarget(logWindow);
#endif

	m_updateTimer = NULL;

	wxIcon icon(wxICON(IDI_APPICON));
	SetIcon(icon);
	
	m_remoteMode = true;

	// Open up the configuration file, assumed to be in the user's home directory

	wxString authorizedCode = "0";
	wxString enabledCode = "0";

	// by default, enable nothing
	wxString defaultAuthorizedCode = "10A80000000";
	wxString defaultEnableCode = "0";

	m_options = new Options();

	Permission* perms = m_options->GetPerms();

	wxFileName configName(wxGetHomeDir(), "chameleon.ini");

	m_config = new wxFileConfig("Chameleon", wxEmptyString, configName.GetFullPath());

	if(configName.FileExists())
	{
		m_options->SetHostname(m_config->Read("Network/hostname"));
		m_options->SetUsername(m_config->Read("Network/username"));
		//m_options->SetMingwPath(m_config->Read("Compiler/mingwpath"));

		bool printInColor = (m_config->Read("Miscellaneous/PrintInColor", "true") == "true");
		int printStyle = printInColor ? wxSTC_PRINT_COLOURONWHITE : wxSTC_PRINT_BLACKONWHITE;
		m_options->SetPrintStyle(printStyle);

		bool showToolbarText = (m_config->Read("Interface/ShowToolbarText", "true") == "true");
		m_options->SetShowToolbarText(showToolbarText);

		bool printLineNumbers = (m_config->Read("Miscellaneous/PrintLineNumbers", "false") == "true");
		m_options->SetLineNumberPrinting(printLineNumbers);


		int terminalHistory = m_config->Read("Miscellaneous/TerminalHistory", 100);
		
		authorizedCode = m_config->Read("Permissions/authorized", defaultAuthorizedCode);
		enabledCode = m_config->Read("Permissions/enabled", defaultEnableCode);
		
				
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

	if(authorizedCode == wxEmptyString)
	{
		authorizedCode = defaultAuthorizedCode;
	}
	if(enabledCode == wxEmptyString)
	{
		enabledCode = defaultEnableCode;
	}

	if(!perms->setGlobalAuthorized(authorizedCode))
	{
		wxLogDebug("Authcode initialization failed!  Code: %s", authorizedCode.c_str());
	}
	else
	{
		perms->setGlobalEnabled(enabledCode);
	}
	
	wxFileName plinkPath(wxGetCwd(), "plink.exe");
	m_options->SetPlinkApp(plinkPath.GetFullPath());

	wxFileName pscpPath(wxGetCwd(), "pscp.exe");  
	m_options->SetPscpApp(pscpPath.GetFullPath());

	m_network = new Networking(m_options);
	m_compiler = new Compiler(m_options, m_network);
	m_debugger = new Debugger(m_network, this);

	m_optionsDialog = new OptionsDialog(this, m_options, ID_OPTIONSDIALOG, "Options");

	m_splitEditorOutput = new wxProportionalSplitterWindow(this, ID_SPLITEDITOROUTPUT);
	m_splitProjectEditor = new wxSplitterWindow(m_splitEditorOutput, ID_SPLITPROJECTEDITOR);
	m_book = new ChameleonNotebook(m_splitProjectEditor, ID_NOTEBOOK_ED);

	m_noteTerm = new ChameleonNotebook(m_splitEditorOutput, ID_NOTEBOOK_TERM);
	m_termContainer = new wxTermContainer(m_noteTerm, ID_CONTAINER_TERM);
	m_debugTermContainer = new wxTermContainer(m_noteTerm, ID_CONTAINER_DEBUGTERM);

	m_terminal = new wxSSH(m_termContainer, ID_TERMINAL, m_network, wxPoint(0, 0));
	m_terminal->set_mode_flag(GTerm::CURSORINVISIBLE);
	m_termContainer->SetTerminal(m_terminal);	

	m_debugTerminal = new wxSSH(m_debugTermContainer, ID_DEBUGTERMINAL, m_network, wxPoint(0, 0));
	m_debugTerminal->set_mode_flag(GTerm::CURSORINVISIBLE);
	m_debugTermContainer->SetTerminal(m_debugTerminal);

	m_outputPanel = new CompilerOutputPanel(m_noteTerm, this, ID_COMPILEROUTPUT);
	m_outputPanel->SetAdvanced(perms->isEnabled(PERM_ADVANCEDCOMPILE));

	m_watchPanel = new VariableWatchPanel(m_noteTerm, this, ID_VARWATCHPANEL);

	// project setup
	m_projMultiFiles = NULL;
	m_projectTree = new wxTreeCtrl(m_splitProjectEditor, ID_PROJECTTREE);
	m_projectTree->Hide();

	m_iconManager = new IconManager();
	
	wxImageList* fileIcons = m_iconManager->GetImageList();
	// Sets the list, but doesn't take control of it away
	m_projectTree->SetImageList(fileIcons);

	int idxFolderClosed = m_iconManager->GetIconIndex("FOLDERCLOSED");
	int idxFolderOpen = m_iconManager->GetIconIndex("FOLDEROPEN");

	m_remoteFileDialog = new RemoteFileDialog(this, ID_REMOTEFILEDIALOG);
	m_remoteFileDialog->SetNetworking(m_network);

	// This also sets the imagelist for the listview
	m_remoteFileDialog->SetIconManager(m_iconManager);


	wxTreeItemId rootNode = m_projectTree->AddRoot("No project open", idxFolderClosed, idxFolderOpen);
	m_projectFileFolders[0] = m_projectTree->AppendItem(rootNode, "Source files", idxFolderClosed, idxFolderOpen);
	m_projectFileFolders[1] = m_projectTree->AppendItem(rootNode, "Header files", idxFolderClosed, idxFolderOpen);
	m_projectFileFolders[2] = m_projectTree->AppendItem(rootNode, "Libraries", idxFolderClosed, idxFolderOpen);
	m_projectFileFolders[3] = m_projectTree->AppendItem(rootNode, "Other files", idxFolderClosed, idxFolderOpen);

	m_currentPage = 0;
	m_fileNum = 0;

	m_appClosing = false;
	m_setSelection = false;

	int charWidth = GetCharWidth();

	int fileNameChars = 55;
	int locationChars = 6;	
	int readEditChars = 9;
	int networkStatusChars = 25;
	int lineColChars = 15;

	const int widths[] = {fileNameChars * charWidth, locationChars * charWidth, readEditChars * charWidth, 
							networkStatusChars * charWidth, lineColChars * charWidth};
	m_statusBar = CreateStatusBar (WXSIZEOF(widths), wxST_SIZEGRIP);
	m_statusBar->SetStatusWidths (WXSIZEOF(widths), widths);
	SendSizeEvent();

	m_updateTimer = new wxTimer (this, ID_STATUSTIMER);

	m_splitProjectEditor->Initialize(m_book);
	m_splitEditorOutput->Initialize(m_splitProjectEditor);
	m_splitProjectEditor->Show();
	m_splitEditorOutput->Show();
	m_book->Show();
	EvaluateOptions();
	
	// create the initial blank open file
	NewFile();
	PageHasChanged(m_currentPage);

	m_filterCPPFiles = "C++ source files (*.cpp, *.c)|*.cpp;*.c";
	m_filterHeaderFiles += "C++ header files (*.h, *.hpp)|*.h;*.hpp";	
	m_filterProjectFiles = "Chameleon project files (*.cpj)|*.cpj";

	// If this ever gets ported to Linux, we'd probably want to add
	// Linux library extensions here (.a, .so).  The other issue is that
	// the remote file dialog only looks in ~, which might need to be changed.
	m_filterLibraryFiles = "C++ libraries (*.lib)|*.lib"; 
	m_filterAllFiles = "All files (*.*)|*.*";

	m_extensionMappings["cpj"] = ICON_PROJECT;
	m_extensionMappings["c"] = ICON_SOURCE_C;
	m_extensionMappings["cpp"] = ICON_SOURCE_CPP;
	m_extensionMappings["h"] = ICON_SOURCE_H;
	m_extensionMappings["hpp"] = ICON_SOURCE_H;
	m_extensionMappings["lib"] = ICON_LIBRARY;
	m_extensionMappings["c_disabled"] = ICON_DISABLED_SOURCE_C;
	m_extensionMappings["cpp_disabled"] = ICON_DISABLED_SOURCE_CPP;
	m_extensionMappings["h_disabled"] = ICON_DISABLED_SOURCE_H;
	m_extensionMappings["hpp_disabled"] = ICON_DISABLED_SOURCE_H;

	m_projMultiFiles = NULL;

	m_findReplace = NULL;

	g_printData = new wxPrintData;
	g_pageSetupData = new wxPageSetupDialogData;

	g_printData->SetPaperId(wxPAPER_LETTER);

	m_appStarting = false;
	m_compileProject = false;
}

//////////////////////////////////////////////////////////////////////////////
///  public destructor ~ChameleonWindow
///  Responsible for cleaning up almost everything.
///
///  @return void
///
///  @remarks Networking needs to be deleted last, since several other classes refer to it in their destructors.
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
ChameleonWindow::~ChameleonWindow()
{
	// make sure the config file got written to disk
	m_config->Flush();

	delete g_printData;
	delete g_pageSetupData;

	if(m_findReplace != NULL)
	{
		delete m_findReplace;
	}

	if (m_updateTimer) 
	{
		delete m_updateTimer;
		m_updateTimer = NULL;
	}

	// (hopefully) fixes some issues with widgets not getting deleted for some reason
	m_noteTerm->DestroyChildren();	

	//CleanupDropMenu();


	delete m_optionsDialog;
	delete m_compiler;
	delete m_config;
	delete m_options;
	delete m_debugger;
	delete m_network;	
	delete m_iconManager;
}

//////////////////////////////////////////////////////////////////////////////
///  private OnClose
///  Called whenever the main frame is about to close.  Handles initial resource cleanup.
///
///  @param  event wxCloseEvent & The close event generated by wxWindows
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

	if(m_projMultiFiles != NULL)
	{
		CloseProjectFile();
	}

	if(m_updateTimer)
	{
		m_updateTimer->Stop();
	}

	if(m_terminal->IsConnected())
	{
		m_terminal->Disconnect();
	}

	if(m_debugTerminal->IsConnected())
	{
		m_debugTerminal->Disconnect();
	}

	Destroy();
}

// Main menu and debug stuff begins here

//////////////////////////////////////////////////////////////////////////////
///  private OnMenuEvent
///  Responsible for handling 95% of the menu and button-related events.
///
///  @param  event wxCommandEvent & The event generated by the button/menu click
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnMenuEvent(wxCommandEvent &event)
{
	int id = event.GetId();

	switch(id)
	{
		case ID_NEW_SOURCE:
		{			
			NewFile();
			break;
		}

		case ID_OPEN_SOURCE_LOCAL:
		case ID_OPEN_SOURCE_REMOTE:
		{
			OnOpenSourceFile(id);
			break;
		}

		case ID_SAVE:
		{
			SaveFile(false, true, FILE_ALLSOURCETYPES);
			break;
		}

		case ID_SAVE_SOURCE_LOCAL:
		case ID_SAVE_SOURCE_REMOTE:
		{
			OnSaveSourceFile(id);
			break;
		}

		case ID_NEW_PROJECT:
		{
			SaveFile(true, true, FILE_PROJECT);
			break;
		}

		case ID_OPEN_PROJECT_LOCAL:
		case ID_OPEN_PROJECT_REMOTE:
		{
			OpenProjectFile(id == ID_OPEN_PROJECT_REMOTE);
			break;
		}

		case ID_CLOSE_PROJECT:
		{
			CloseProjectFile();
			break;
		}

		case ID_CLOSETAB:
		{
			CloseTab();
			break;
		}

		case ID_CLOSEPAGE:
		{
			if (!m_currentEd) 
			{
				return;
			}

			CloseFile();
			PageHasChanged();
			break;
		}

		case ID_CLOSEALL:
		{
			CloseAllFiles();
			break;
		}


		case ID_STARTCONNECT:
		{
			OnStartConnect();
			break;
		}

		case ID_DISCONNECT:
		{
			m_terminal->Disconnect();
			break;
		}

		case ID_QUIT:
		{
			Close(TRUE);
			break;
		}

		case ID_REDO:
		case ID_UNDO:
		{
			if(id == ID_REDO)
			{
				m_currentEd->Redo();
			}
			else
			{
				m_currentEd->Undo();
			}
			
			m_book->Refresh();
			break;
		}

		case ID_COPY:
		{
			OnCopy();			
			break;
		}

		case ID_CUT:
		{
			m_currentEd->Cut();
			break;
		}

		case ID_PASTE:
		{
			m_currentEd->Paste();
			break;
		}

		case ID_FIND:
		case ID_REPLACE:
		{			
			OnFindReplace(id);
			break;
		}

		case ID_ABOUT:
		{
			AboutChameleonDialog acd(this);
			acd.ShowModal();
			break;
		}
		
		case ID_OPTIONS:
		{
			OnOptions();
			break;
		}

		case ID_COMPILE_PROJECT:
		case ID_COMPILE:
		{
			// We didn't have time to really test compile cancellation, so this 
			// code is commented out.
			/*
			if(m_compiler->IsCompiling())
			{
				m_compiler->HaltCompiling();
			}
			else
			{
				Compile();
			}
			*/

			if(id == ID_COMPILE_PROJECT)
			{
				m_compileProject = true;
			}
			Compile();
			break;
		}			

		case ID_PROJECT_ADDFILE:
		{
			AddFileToProject();
			break;
		}

		case ID_PROJECT_REMOVEFILE:
		{
			RemoveFileFromProject();
			break;
		}

		case ID_PROJECT_EXCLUDE_FILE:
		case ID_PROJECT_INCLUDE_FILE:
		{
			OnProjectIncludeExcludeFile(id);
			break;
		}

		case ID_PRINT_PAGE:
		{
			OnPrintPage();
			break;
		}

		case ID_PRINT_PREVIEW:
		{
			OnPrintPreview();
			break;
		}

		case ID_PRINT_SETUP:
		{
			OnPrintSetup();
			break;
		}

		case ID_HELP:
		{
			OnHelp();
			break;
		}
	}
}

void ChameleonWindow::OnDebugBreakpointCommand(wxCommandEvent &event)
{
	switch(event.GetId())
	{
		case ID_DEBUG_ADDEDITORBREAKPOINT:
		{
			m_currentEd->OnAddBreakpoint(event);
			break;
		}
		case ID_DEBUG_REMOVEEDITORBREAKPOINT:
		{
			m_currentEd->OnRemoveBreakpoint(event);
			break;
		}
		case ID_DEBUG_CLEAREDITORBREAKPOINTS:
		{
			m_currentEd->OnClearBreakpoints(event);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private OnDebugCommand
///  Responsible for handling all debug-related menu / toolbar commands
///
///  @param  event wxCommandEvent & The menu event generated
///
///  @return void
///
///  @remarks Basically just creates a debug event with the same ID and passes it to
///  @remarks the debugger.  Sets up the start debug information if necesssary.
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnDebugCommand(wxCommandEvent &event)
{
	int eventID = event.GetId();

	wxDebugEvent debugEvent;

	debugEvent.SetId(eventID);

	if(eventID == ID_DEBUG_START)
	{
		ProjectInfo* debugProject;
		FileBreakpointHash bphash;
		if(m_projMultiFiles != NULL)
		{
			debugProject = m_projMultiFiles;
			EditorPointerArray edList = m_projMultiFiles->GetEditors();
			int numOpenProjEds = edList.GetCount();
			for(int i = 0; i < numOpenProjEds; i++)
			{
				ChameleonEditor* ed = edList[i];
				wxArrayInt breaks = ed->GetBreakpoints();
				if(breaks.GetCount() > 0)
				{
					wxFileName fn = ed->GetFileName();
					wxString filename = fn.GetFullPath(wxPATH_UNIX);

					bphash[filename] = breaks;
				}
			}
		}
		else
		{				
			debugProject = m_currentEd->GetProject();
			wxFileName sourceFile = m_currentEd->GetFileName();
			wxString sourceFileName = sourceFile.GetFullPath(wxPATH_UNIX);
			wxArrayInt linenumbers = m_currentEd->GetBreakpoints();
			bphash[sourceFileName] = linenumbers;
		}
		debugEvent.SetProject(debugProject);

		// set up the debug I/O panel connection
		if(debugProject->IsRemote())
		{
			wxString tty = m_debugTerminal->ConnectForDebug();
			debugEvent.SetTTYString("/dev/" + tty);
		}
		debugEvent.SetFileBreakpoints(bphash);
		debugEvent.SetId(ID_DEBUG_START);
	}

	m_debugger->AddPendingEvent(debugEvent);
}

//////////////////////////////////////////////////////////////////////////////
///  private OnDebugEvent
///  Handles debug-related events created by the debugger or other widgets
///
///  @param  event wxDebugEvent & The debug event to handle
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnDebugEvent(wxDebugEvent &event)
{
	int eventID = event.GetId();

	switch(eventID)
	{
	case ID_DEBUG_EXIT_ERROR:
	case ID_DEBUG_EXIT_NORMAL:
		{
			// This is an absurdly naive way to handle removing the focused line
			// marker, but it's easy and shouldn't take up much time
			int numPages = m_book->GetPageCount();
			for(int i = 0; i < numPages; i++)
			{
				ChameleonEditor* pEdit = static_cast <ChameleonEditor* >(m_book->GetPage(i));
				pEdit->MarkerDeleteAll(MARKER_FOCUSEDLINE);
			}
			wxLogDebug("Debugger exit");
			m_debugTerminal->Disconnect(false);

			break;
		}
	case ID_DEBUG_BREAKPOINT:
		{	
			wxString file = event.GetSourceFilename();
			int linenumber = event.GetLineNumber();

			FocusOnLine(file, linenumber);

			if(event.GetVariableNames().GetCount() > 0)
			{
				m_watchPanel->UpdateVariableInfo(event);
			}
			break;
		}
	case ID_DEBUG_ADD_BREAKPOINT:
	case ID_DEBUG_REMOVE_BREAKPOINT:
		{
			if(m_debugger->isDebugging() && m_debugger->isPaused())
			{
				m_debugger->AddPendingEvent(event);
			}			
			break;
		}
	case ID_DEBUG_ADD_WATCH:
	case ID_DEBUG_REMOVE_WATCH:
		{
			m_debugger->AddPendingEvent(event);
			break;
		}
	case ID_DEBUG_VARINFO:
		m_watchPanel->UpdateVariableInfo(event);
		break;
	case ID_DEBUG_RUNTOCURSOR:
		m_debugger->AddPendingEvent(event);
		break;
	default:
		wxLogDebug("Default DebugEvent.  Value: %d", eventID);
		break;
	}
}

// File and editor manipulation code begins here

//////////////////////////////////////////////////////////////////////////////
///  private NewFile
///  Creates a new empty editor and adds it to the editor notebook 
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::NewFile() 
{
	m_fileNum += 1;

	wxString locationPrefix = "(?) ";

	wxString noname = locationPrefix + "<untitled> " + wxString::Format ("%d", m_fileNum);
	ProjectInfo* singleFileProject = new ProjectInfo();
	ChameleonEditor* edit = new ChameleonEditor (this, m_options, singleFileProject, m_book, -1);
	m_currentEd = edit;
	m_currentPage = m_book->GetPageCount();
	m_book->AddPage (edit, noname, true);
}

//////////////////////////////////////////////////////////////////////////////
///  private PageHasChanged
///  Called whenever the active tab has changed, updating the active editor pointer
///
///  @param  pageNr int  [=-1] The index of the newly selected page
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

	if ((int)m_book->GetPageCount() <= pageNr)
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

		if(tabText.StartsWith("(R)"))
		{			
			m_remoteMode = true;
		}
		else if(tabText.StartsWith("(L)"))
		{
			m_remoteMode = false;
		}
		// else assume unsaved file and don't change anything
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private CloseTab
///  Closes a tab after the user right-clicks it and selects "Close"
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::CloseTab()
{
	int tab = GetIntVar(VN_CLICKEDTAB);
	CloseFile(tab);
	m_book->Refresh();
}

//////////////////////////////////////////////////////////////////////////////
///  private CloseFile
///  Closes a given editor, based on its index in the editor notebook
///
///  @param  pageNr int  [=-1] The index of the editor to close
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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
		wxFileName currentFileName;
		if ((m_book->GetPageCount() > 1) || m_appClosing) 
		{
			currentFileName = m_currentEd->GetFileName();
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
			if(currentFileName.IsOk())
			{
				int newSelectedPageNum = GetPageNum(currentFileName);
				PageHasChanged(newSelectedPageNum);
			}
		}		
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private CloseAllFiles
///  Closes all open files
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::CloseAllFiles()
{
	int cnt = m_book->GetPageCount();
	for (int i = 0; i < cnt; i++) 
	{
		CloseFile();
	}

	PageHasChanged();
}

//////////////////////////////////////////////////////////////////////////////
///  private GetPageNum
///  Searches through the open editors to find an editor with the given name
///
///  @param  fn               wxFileName  The editor's filename to find
///  @param  compareWholePath bool        [=true] Compare the entire filename + path, or just the name?
///  @param  startingTab      int         [=0] The index of the first tab to search
///
///  @return int              The index of the located editor, or -1 if not found
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
int ChameleonWindow::GetPageNum(wxFileName fn,  bool compareWholePath, int startingTab)
{
	ChameleonEditor *edit;
	int numPages = m_book->GetPageCount();
	wxString filename = fn.GetFullName();

	for (int pageNum = startingTab; pageNum < numPages; pageNum++) 
	{
		edit = static_cast <ChameleonEditor *> (m_book->GetPage(pageNum));

		bool fileMatches = false;

		if(compareWholePath)
		{
			fileMatches = (edit->GetFileName() == fn);
		}
		else
		{
			fileMatches = (edit->GetFilenameString() == filename);
		}
		if (fileMatches)
		{			
			return pageNum;
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////////
///  private OnPageChange
///  Event handler called when the user clicks a different tab
///
///  @param  event wxNotebookEvent & The notebook event (not used)
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnPageChange (wxNotebookEvent &WXUNUSED(event)) 
{
	if (!m_setSelection)
	{
		PageHasChanged();
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private HandleModifiedFile
///  Handle user interaction when closing or reloading an editor
///
///  @param  pageNr      int   The index of the editor being closed or reloaded
///  @param  closingFile bool  True if the file is being closed, false if it's being reloaded
///
///  @return int         What the user chose to do.  The only real meaningful return value is wxCANCEL.
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
int ChameleonWindow::HandleModifiedFile(int pageNr, bool closingFile)
{
	ChameleonEditor *edit = static_cast <ChameleonEditor * > (m_book->GetPage (pageNr));

	if (!edit)
	{
		return wxCANCEL;
	}

	if (edit->Modified()) 
	{
		wxString saveMessage = "The file ";
		wxString fileName = edit->GetFileNameAndPath();
		
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

		int result = wxMessageBox (saveMessage, "Close", wxYES_NO | wxCANCEL | wxICON_QUESTION);
		if( result == wxYES) 
		{
			ChameleonEditor* tmpCurrentEd = m_currentEd;
			m_currentEd = edit;
			// only do a Save As if necessary
			SaveFile(false, true, FILE_ALLSOURCETYPES);
			m_currentEd = tmpCurrentEd;
			m_currentEd->SetFocus();

			if (edit->Modified()) 
			{
				wxString errorMessage = fileName + " could not be saved!";
				wxMessageBox (errorMessage, "File not closed",
					wxOK | wxICON_EXCLAMATION);
				m_currentEd->Refresh();
			}
		}

		return result;
	}
	// if I'm here, doesn't matter if I return wxNO or wxYES, just as long as it's not wxCANCEL
	return wxNO;
}

//////////////////////////////////////////////////////////////////////////////
///  private OpenFile
///  Shows a file dialog and returns a list of files to open.  Abstracts out local/remote file dialogs.
///
///  @param  filterType    FileFilterType  The type of files to show in the file dialog
///
///  @return wxArrayString The filenames to open
///
///  @remarks Currently, only one filename can be opened at a time (mostly due to the 
///  @remarks fact that the RemoteFileDialog has that limitation).  OpenSourceFile
///  @remarks has the logic to handle multiple filenames, so this could be added 
///  @remarks without too much difficulty.
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxArrayString ChameleonWindow::OpenFile(FileFilterType filterType)
{
	wxArrayString fnames;
	if (!m_currentEd)
	{
		return fnames;
	}

	wxString filterString = ConstructFilterString(filterType);

	if(!m_remoteMode)
	{
		wxFileDialog dlg (this, _("Open"), "", "", filterString,
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
		if(!CheckForBlankPassword())
		{
			return false;
		}

		SetStatusText("Doing network operation...", 3);

		wxBusyCursor busyCursor;
		if(m_remoteFileDialog->Prepare(true, filterString))
		{
			int result = m_remoteFileDialog->ShowModal();
			SetStatusText(wxEmptyString, 3);
			m_currentEd->SetFocus();

			if(result != wxOK)
			{
				return fnames;
			}

			wxString remoteFileName = m_remoteFileDialog->GetRemoteFileNameAndPath();

			wxString userHome;
			if(!m_network->GetHomeDirPath(userHome))
			{
				CheckNetworkStatus();
			}
			else
			{
				remoteFileName.Replace("~", userHome);
				fnames.Add(remoteFileName);
			}			
		}
		else
		{
			CheckNetworkStatus();
			SetStatusText(wxEmptyString, 3);
			return fnames;
		}
	}
	
	return fnames;	
}

//////////////////////////////////////////////////////////////////////////////
///  public OpenSourceFile
///  Opens the given list of source files in the editor
///
///  @param  fnames wxArrayString  The source files to open
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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
		wxFileName newFileName(fnames[n]);
		int pageNr = GetPageNum(newFileName);

		if(!GetFileContents(fnames[n], fileContents, fileNameNoPath))
		{
			return;
		}

		// I'd previously canceled the open here if the file was empty,
		// but we opted to skip that behavior
		/*
		if(fileContents == wxEmptyString)
		{
			continue;
		}
		*/

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
			
			m_currentEd = static_cast< ChameleonEditor* > (m_book->GetPage (m_currentPage));
			m_currentEd->LoadFileText(fileContents);
		}
		
		else
		{
			ProjectInfo* proj;

			if( (m_projMultiFiles != NULL) && (m_projMultiFiles->FileExistsInProject(fnames[n])))
			{
				proj = m_projMultiFiles;
			}
			else
			{
				proj = new ProjectInfo();
				proj->SetRemote(m_remoteMode);
			}
			
			// current buffer is empty and untouched, so load the file into it
			if ((!m_currentEd->Modified() && 
				 !m_currentEd->HasBeenSaved() && 
				  m_currentEd->GetText().IsEmpty()) ) 
			{
				m_book->SetPageText(m_currentPage, locationPrefix + fileNameNoPath);
				m_currentEd->SetProject(proj);				
			}
			// need to create a new buffer for the file
			else
			{
				ChameleonEditor *edit = new ChameleonEditor (this, m_options, proj, m_book, -1);

				m_currentEd = edit;
				m_currentPage = m_book->GetPageCount();
				m_book->AddPage (m_currentEd, locationPrefix + fileNameNoPath, true);
			}
			
			m_currentEd->LoadFileText(fileContents);
			m_currentEd->SetFilename(newFileName, m_remoteMode);
		}

		if (firstPageNr < 0)
		{
			firstPageNr = m_currentPage;
		}

		m_currentEd->UpdateSyntaxHighlighting();
	}

	// show the active tab, new or otherwise
	if (firstPageNr >= 0) 
	{
		PageHasChanged(firstPageNr);
	}
}



//////////////////////////////////////////////////////////////////////////////
///  private GetFileContents
///  Gets the text of a source file.  Abstracts out opening local / remote files.
///
///  @param  fileToLoad   wxString   The name of the file to open
///  @param  fileContents wxString & Gets the contents of the opened file
///  @param  fileName     wxString & Gets the name of the file (no path)
///
///  @return bool         True if the open succeeded, false if it failed
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonWindow::GetFileContents(wxString fileToLoad, wxString &fileContents, wxString &fileName)
{
	wxFileName fn(fileToLoad);
	if(m_remoteMode)
	{
		wxString remotePath = fn.GetPath(false, wxPATH_UNIX);
		wxString remoteFile = fn.GetFullName();

		if(!m_network->GetFileContents(fn, fileContents))
		{
			CheckNetworkStatus();
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

//////////////////////////////////////////////////////////////////////////////
///  private SaveFile
///  Saves a text file, abstracting out local / remote issues.
///
///  @param  saveas         bool            True if this is explicitly a "Save-As" command and a file dialog must be shown
///  @param  askLocalRemote bool            True if the user should be asked whether to save the file locally or remotely
///  @param  filterType     FileFilterType  The type of files to show in the dialog
///
///  @return bool           Whether the save operation succeeded or not
///
///  @remarks This function is also used for creating new project files, since that's effectively
///  @remarks just saving a text file as well.  
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonWindow::SaveFile(bool saveas, bool askLocalRemote, FileFilterType filterType)
{
	wxString filename;
	wxString fileContents;

	bool originalRemoteMode = m_remoteMode;

	bool doSaveAs = saveas || !m_currentEd->HasBeenSaved() || (m_remoteMode != m_currentEd->LastSavedRemotely());

	bool isSourceFile = !(filterType == FILE_PROJECT);

	wxString filterString = ConstructFilterString(filterType);

	if(doSaveAs && askLocalRemote)
	{
		// default to saving remotely
		m_remoteMode = true;

		if(m_options->GetPerms()->isEnabled(PERM_LOCALMODE))
		{
			wxArrayString choices;
			choices.Add("Remote");
			choices.Add("Local");
			wxString choice = wxGetSingleChoice("Would you like to save this file: \n\n1) Remotely (on the server) \n2) Locally (on the computer you're using)?", 
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
	}

	if(isSourceFile)
	{
		fileContents = m_currentEd->GetText();
	}
	// we must be saving a new project
	else
	{
		fileContents = "[Headers]\n\n[Sources]\n\n[Libraries]\n\n[Other]";
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

			wxString simpleFileName = m_currentEd->GetFilenameString();

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

		if(!CheckForBlankPassword())
		{
			return false;
		}
		

		if(doSaveAs)
		{
			SetStatusText("Doing network operation...", 3);
			if(m_remoteFileDialog->Prepare(false, filterString))
			{
				int result = m_remoteFileDialog->ShowModal();
				SetStatusText(wxEmptyString, 3);
				m_currentEd->SetFocus();

				if(result != wxOK)
				{
					return false;
				}

				remotePath = m_remoteFileDialog->GetRemotePath();
				remoteFile = m_remoteFileDialog->GetRemoteFileName();

				wxString userHome;
				if(!m_network->GetHomeDirPath(userHome))
				{
					CheckNetworkStatus();
				}
				else
				{
					remotePath.Replace("~", userHome);
				}
			}
			else
			{
				CheckNetworkStatus();
				SetStatusText(wxEmptyString, 3);
				return false;				
			}			
		}
		else
		{
			remoteFile = m_currentEd->GetFilenameString();
			remotePath = m_currentEd->GetFilePath();
		}

		wxFileName remoteFN(remotePath, remoteFile);
		wxBeginBusyCursor();
		if(!m_network->SendFileContents(fileContents, remoteFN))
		{
			CheckNetworkStatus();
		}
		wxEndBusyCursor();

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
		if(m_projMultiFiles != NULL)
		{
			CloseProjectFile();
		}

		m_projMultiFiles = new ProjectInfo(false);
		m_projMultiFiles->SetRemote(m_remoteMode);

		wxFileName projectFile(filename);
		m_projMultiFiles->SetProjectFile(projectFile);
		m_projMultiFiles->SetProjectName(projectFile.GetFullName());

		wxTreeItemId rootItem = m_projectTree->GetRootItem();
		m_projectTree->SetItemText(rootItem, m_projMultiFiles->GetProjectName());

	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  private ConstructFilterString
///  Puts together the filter that defines what files are shown in a file dialog
///
///  @param  filterType FileFilterType  The type of filter to construct
///
///  @return wxString   The constructed filter string
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString ChameleonWindow::ConstructFilterString(FileFilterType filterType)
{
	wxString filterString;
	switch(filterType)
	{
	case FILE_ALLSOURCETYPES:
		filterString = m_filterCPPFiles;
		filterString += "|";
		filterString += m_filterHeaderFiles;
		filterString += "|";
		filterString += m_filterAllFiles;
		break;
	case FILE_SOURCES:
		filterString = m_filterCPPFiles;
		break;
	case FILE_HEADERS:
		filterString = m_filterHeaderFiles;
		break;
	case FILE_PROJECT:
		filterString = m_filterProjectFiles;
		break;
	case FILE_LIBRARIES:
		filterString = m_filterLibraryFiles;
		break;
	case FILE_ALLFILES:
	default:
		filterString = m_filterAllFiles;
		break;
	}
	return filterString;
}

// my "I need to try something out, I'll stick it in here" function
void ChameleonWindow::Test(wxCommandEvent& WXUNUSED(event))
{
}


// not currently used.  may come into play later regarding terminal resizing
void ChameleonWindow::CheckSize()
{
	int x, y;
	this->GetClientSize(&x, &y);
	wxString result;
	result << x << ", " << y;
	wxMessageBox(result );
	m_currentEd->SetScrollWidth(x);
	
}

// UI update code begins here

//////////////////////////////////////////////////////////////////////////////
///  private OnIdle
///  Initiates the UI update timer as needed
///
///  @param  event wxIdleEvent & The generated program idle event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnIdle(wxIdleEvent &event)
{
	if (m_updateTimer && !m_updateTimer->IsRunning ()) 
	{
		m_updateTimer->Start (250, wxTIMER_ONE_SHOT);

	}
	event.Skip();
}

//////////////////////////////////////////////////////////////////////////////
///  private OnStatusTimer
///  Initiates UI updates based on the internal timer
///
///  @param  event wxTimerEvent & The generated timer event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnStatusTimer(wxTimerEvent &WXUNUSED(event))
{
	if (m_updateTimer)
	{
		m_updateTimer->Stop();
		UpdateStatusBar();
		OnUpdateSaveUI();
		OnUpdateDebugUI();
		OnUpdateConnectionUI();
		OnUpdateCompileUI();
		OnUpdateProjectUI();
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private OnUpdateDebugUI
///  Updates the debug-related toolbar items
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnUpdateDebugUI()
{
	if(!m_options->GetPerms()->isEnabled(PERM_DEBUG))
	{
		return;
	}
	bool isDebugging = m_debugger->isDebugging();
	bool isPaused = m_debugger->isPaused();

	wxToolBar* tb = GetToolBar();

	if(isDebugging)
	{
		tb->EnableTool(ID_DEBUG_START, false);
		tb->EnableTool(ID_DEBUG_CONTINUE, isPaused);
		tb->EnableTool(ID_DEBUG_STOP, true);

		tb->EnableTool(ID_DEBUG_STEPNEXT, isPaused);
		tb->EnableTool(ID_DEBUG_STEPOVER, isPaused);
		tb->EnableTool(ID_DEBUG_STEPOUT, isPaused);
	}
	else
	{
		// skip ID_DEBUG_START;
		for(int i = ID_DEBUG_IDS_FIRST + 1; i < ID_DEBUG_IDS_LAST; i++)
		{
			tb->EnableTool(i, false);
		}

		ProjectInfo* proj = m_currentEd->GetProject();

		bool canStartDebug = proj->IsCompiled();
		tb->EnableTool(ID_DEBUG_START, canStartDebug);
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private OnUpdateConnectionUI
///  Updates the terminal connection buttons and menu items
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnUpdateConnectionUI()//wxUpdateUIEvent &event)
{
	if(!m_options->GetPerms()->isEnabled(PERM_TERMINAL))
	{
		return;
	}
	bool termConnected = m_terminal->IsConnected();

	wxToolBar* tb = GetToolBar();
	tb->EnableTool(ID_STARTCONNECT, !termConnected);
	tb->EnableTool(ID_DISCONNECT, termConnected);

	wxMenuBar* mb = GetMenuBar();
	WXWidget handle = mb->GetHandle();
	if(handle != NULL)
	{
		wxMenu* toolsMenu = mb->GetMenu(mb->FindMenu("Tools"));
		toolsMenu->Enable(ID_STARTCONNECT, !termConnected);
		toolsMenu->Enable(ID_DISCONNECT, termConnected);	
	}


}

//////////////////////////////////////////////////////////////////////////////
///  private OnUpdateCompileUI
///  Updates the compile button
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnUpdateCompileUI()//wxUpdateUIEvent &event)
{
	if(!m_options->GetPerms()->isEnabled(PERM_COMPILE))
	{
		return;
	}

	ProjectInfo* edProj = m_currentEd->GetProject();
	wxToolBar* tb = GetToolBar();
	wxToolBarToolBase* compileButton = tb->FindById(ID_COMPILE);

	bool canCompile = !edProj->IsCompiled();
	bool currProjIsCompiling = m_compiler->IsCompiling() && edProj->IsBeingCompiled();

	// The other half of the compile cancellation code.  If this is ever activated,
	// then a check needs to be made so that tb->Realize() is only called when it's
	// actually needed.  If it's called every time, there's some nasty flickering
	// in the toolbar.

	/*
	// Choose the image & caption:
	if(currProjIsCompiling)
	{
	wxBitmap bmStopCompile(stop1_xpm);
	compileButton->SetNormalBitmap(bmStopCompile);
	compileButton->SetLabel("Stop");
	}
	else
	{
	wxBitmap bmBuild(build_xpm);
	compileButton->SetNormalBitmap(bmBuild);
	compileButton->SetLabel("Compile");
	}
	//tb->Realize();
	*/

	bool enableButton = canCompile && !currProjIsCompiling; //|| currProjIsCompiling;
	tb->EnableTool(ID_COMPILE, enableButton);
}

//////////////////////////////////////////////////////////////////////////////
///  private UpdateStatusBar
///  Updates the status bar text as needed
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::UpdateStatusBar()
{
	if(m_statusBar == NULL)
	{
		return;
	}

	int pageCount = m_book->GetPageCount();
	if(pageCount < 1 || pageCount <= m_currentPage)
	{
		return;
	}
	wxString tabText = m_book->GetPageText(m_currentPage);
	wxString filename;
	wxString location;

	filename = m_currentEd->GetFileNameAndPath();

	if(tabText.StartsWith("(R)"))
	{			
		location = "Remote";
	}
	else if(tabText.StartsWith("(L)"))
	{
		location = "Local";
	}
	else if(tabText.StartsWith("(?)"))
	{
		filename = "Unsaved file";
		location = "N/A";
	}

	if(filename != m_statusBar->GetStatusText(0))
	{
		m_statusBar->SetStatusText(filename, 0);
	}

	if(location != m_statusBar->GetStatusText(1))
	{
		m_statusBar->SetStatusText(location, 1);
	}

	bool isEdReadOnly = m_currentEd->GetProject()->IsReadOnly();

	wxString editable = isEdReadOnly ? "Read-only" : "Read-write";

	if(editable != m_statusBar->GetStatusText(2))
	{
		m_statusBar->SetStatusText(editable, 2);
	}

	bool synchronousOp = m_network->DoingSynchronousOperation();

	if(synchronousOp)
	{
		wxLogDebug("UI aware of synchronous op");
	}

	wxString netStatus = m_network->GetStatusDetails();
	wxString statusText;
	if(netStatus == "NET_STARTING")
	{
		statusText = "Networking starting";
	}
	else if(netStatus == "NET_GOOD")
	{
		statusText = "Network connected";
	}
	else if(netStatus == "NET_UNKNOWN_HOST")
	{
		statusText = "Can't find host";
	}
	else if(netStatus == "NET_CONN_REFUSED")
	{
		statusText = "Connection refused";
	}
	else if (netStatus = "NET_AUTH_FAILED")
	{
		statusText = "Login failed";
	}
	else if(netStatus == "NET_ERROR_MESSAGE")
	{
		statusText = "Unknown network error";
	
	}

	SetStatusText(statusText, 3);


	int curLine = m_currentEd->GetCurrentLine();
	int curPos = m_currentEd->GetCurrentPos() -m_currentEd->PositionFromLine (-curLine);
	wxString linecol;
	linecol.Printf (_("Line: %d, Col: %d"), curLine, curPos);
	if (linecol != m_statusBar->GetStatusText (4)) 
	{
		SetStatusText (linecol, 4);
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private OnUpdateSaveUI
///  Updates the status of the active tab if modified, as well as enabling the save items.
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnUpdateSaveUI()//wxUpdateUIEvent &event)
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
	GetToolBar()->EnableTool(ID_SAVE, enable);

	wxMenuBar* mb = GetMenuBar();
	WXWidget handle = mb->GetHandle();
	if(handle != NULL)
	{
		mb->FindItem(ID_SAVE)->Enable(enable);
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private OnUpdateProjectUI
///  Updates project-related menu items
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnUpdateProjectUI ()
{
	/*
	bool enable = (m_book->GetPageCount() > 1) ||
		(m_currentEd && (m_currentEd->GetLength() > 0));
	*/
	bool projectOpen = (m_projMultiFiles != NULL);

	wxMenuBar* mb = GetMenuBar();
	WXWidget handle = mb->GetHandle();
	if(handle != NULL)
	{
		mb->FindItem(ID_CLOSE_PROJECT)->Enable(projectOpen);
		mb->FindItem(ID_COMPILE_PROJECT)->Enable(projectOpen);
	}

}

// Gets / sets begin here

//////////////////////////////////////////////////////////////////////////////
///  public SetIntVar
///  A "one-size-fits-all" integer Set routine, to avoid pointless duplication
///
///  @param  variableName int  The ID of the member variable to set
///  @param  value        int  The value to set it to
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::SetIntVar(int variableName, int value)
{
	// figure out which integer I'm setting
	int* target = SelectIntVar(variableName);

	// assuming we assigned it properly, Set it 
	if(target != NULL)
	{
		*target = value;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  public GetIntVar
///  A "one-size-fits-all" integer Get routine, to avoid pointless duplication
///
///  @param  variableName int  The ID of the member variable to return
///
///  @return int          The variable's value
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
int ChameleonWindow::GetIntVar(int variableName)
{
	int* target = SelectIntVar(variableName);

	if(target != NULL)
	{
		return *target;
	}
	else
	{
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private SelectIntVar
///  Internal utility routine used by GetIntVar / SetIntVar
///
///  @param  variableName int  The ID of the variable to be get or set
///
///  @return int *        A pointer to the requested variable
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
int* ChameleonWindow::SelectIntVar(int variableName)
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

//////////////////////////////////////////////////////////////////////////////
///  public IsDebugging
///  Returns the debugger's active status
///
///  @return bool Whether or not the debugger is active
///
///  @remarks Used by ChameleonEditor
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonWindow::IsDebugging()
{
	return m_debugger->isDebugging();
}

//////////////////////////////////////////////////////////////////////////////
///  public IsDebuggerPaused
///  Returns if the debugger is paused or not
///
///  @return bool Whether or not the debugger is paused
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonWindow::IsDebuggerPaused()
{
	return m_debugger->isPaused();
}

//////////////////////////////////////////////////////////////////////////////
///  public PassImageList
///  Allows the RemoteFileDialog to pass along its imagelist for use in the project tree
///
///  @param  imagelist wxImageList * The imagelist to use
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::PassImageList(wxImageList* imagelist)
{
	m_projectTree->AssignImageList(imagelist);
}

// Options evaluation and GUI reconfiguration code begins here

//////////////////////////////////////////////////////////////////////////////
///  public EvaluateOptions
///  Updates the GUI and other program options after the user has closed the 
///  options dialog.  Called even if the user canceled the dialog because 
///  the authorization code could have been changed.
///
///  @return void
///
///  @author Mark Erikson @date 03-29-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::EvaluateOptions()
{
	m_network->PingOptions();

	Permission* perms = m_options->GetPerms();

	UpdateToolbar();
	UpdateMenuBar();
	UpdateTerminalNotebook();

	if(perms->isEnabled(PERM_PROJECTS))
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
		if(m_projMultiFiles != NULL)
		{
			CloseProjectFile(false);
		}
		if(m_projectTree->IsShown())
		{
			m_splitProjectEditor->Unsplit(m_projectTree);
			m_projectTree->Hide();
			m_book->Refresh();
		}
	}

	for(int i = 0; i < (int)m_book->GetPageCount(); i++)
	{
		ChameleonEditor* edit = static_cast<ChameleonEditor*> (m_book->GetPage(i));
		edit->UpdateSyntaxHighlighting();
	}

	int newMaxTermSize = m_options->GetTerminalHistorySize();
	m_termContainer->SetTerminalHistory(newMaxTermSize);

	m_config->Write("Permissions/enabled", perms->getGlobalEnabled());
	m_config->Write("Network/hostname", m_options->GetHostname());
	m_config->Write("Network/username", m_options->GetUsername());
	//m_config->Write("Compiler/mingwpath", m_options->GetMingwPath());
	m_config->Write("Miscellaneous/TerminalHistory", newMaxTermSize);
	

	bool printInColor = (m_options->GetPrintStyle() == wxSTC_PRINT_COLOURONWHITE);
	m_config->Write("Miscellaneous/PrintInColor", printInColor ? "true" : "false");
	
	bool printLineNumbers = m_options->GetLineNumberPrinting();
	m_config->Write("Miscellaneous/PrintLineNumbers", printLineNumbers ? "true" : "false");

	m_config->Flush();
}

//////////////////////////////////////////////////////////////////////////////
///  private UpdateMenuBar
///  Recreates the menus, based on the current permissions
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::UpdateMenuBar()
{
	wxMenuBar* menuBar = GetMenuBar();

	Permission* perms = m_options->GetPerms();

	bool localModeEnabled = perms->isEnabled(PERM_LOCALMODE);
	if(menuBar != NULL)
	{
		SetMenuBar(NULL);
		delete menuBar;
	}

	menuBar = new wxMenuBar();
	SetMenuBar(menuBar);

	wxMenu* menuFile = new wxMenu();

	menuFile->Append(ID_NEW_SOURCE, "&New File\tCtrl-N", "Create a new C++ file");
	if(localModeEnabled)
	{
		menuFile->Append(ID_OPEN_SOURCE_LOCAL, "&Open File (Local)", "Open an existing C++ file");
	}
	
	menuFile->Append(ID_OPEN_SOURCE_REMOTE, "&Open File (Remote)", "Open an existing C++ file");
	menuFile->Append(ID_CLOSEPAGE, "&Close File", "Close the active file");
	menuFile->Append(ID_CLOSEALL, "Close All Files");
	menuFile->AppendSeparator();
	menuFile->Append(ID_SAVE, "&Save File\tCtrl-S", "Save the current file");

	if(localModeEnabled)
	{
		menuFile->Append(ID_SAVE_SOURCE_LOCAL, "Save File &As (Local)", "Save the current file as a different name");
	}
	
	menuFile->Append(ID_SAVE_SOURCE_REMOTE, "Save File &As (Remote)", "Save the current file as a different name");
	menuFile->AppendSeparator();

	menuFile->Append(ID_PRINT_PAGE, "&Print\tCtrl-P", "Print the current file");
	menuFile->Append(ID_PRINT_PREVIEW, "Print preview");
	menuFile->Append(ID_PRINT_SETUP, "Print setup");
	
	menuFile->Append(ID_QUIT, "E&xit\tAlt-F4", "Quit this program");

	menuBar->Append(menuFile, "&File");


	wxMenu* menuEdit = new wxMenu();

	menuEdit->Append(ID_UNDO, "&Undo\tCtrl-Z");
	menuEdit->Append(ID_REDO, "&Redo\tCtrl-Y");
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_CUT, "Cu&t\tCtrl-X");
	menuEdit->Append(ID_COPY, "&Copy\tCtrl-C");
	menuEdit->Append(ID_PASTE, "&Paste\tCtrl-V");
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_FIND, "Find\tCtrl-F");
	menuEdit->Append(ID_REPLACE, "Replace\tCtrl-H");
	
	// TODO Implement these Edit menu items

	/*
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_INDENT, "Indent text\tTab");
	menuEdit->Append(ID_UNINDENT, "Unindent text\tShift-Tab");
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_SELECTALL, "Select all\tCtrl-A");
	menuEdit->Append(ID_SELECTLINE, "Select line");
	*/

	menuBar->Append(menuEdit, "&Edit");

	if(perms->isEnabled(PERM_PROJECTS))
	{
		wxMenu* menuProject = new wxMenu();
		menuProject->Append(ID_NEW_PROJECT, "Create New Project");

		if(localModeEnabled)
		{
			menuProject->Append(ID_OPEN_PROJECT_LOCAL, "Open Project File (Local)");
		}

		menuProject->Append(ID_OPEN_PROJECT_REMOTE, "Open Project File (Remote)");
		menuProject->Append(ID_CLOSE_PROJECT, "Close Project");

		menuProject->AppendSeparator();
		menuProject->Append(ID_COMPILE_PROJECT, "Compile Active Project");

		menuBar->Append(menuProject, "&Projects");
	}

	if(perms->isEnabled(PERM_COMPILE))
	{
		wxMenu* compileMenu = new wxMenu();
		compileMenu->Append(ID_COMPILE, "Compile File");
		menuBar->Append(compileMenu, "&Compile");
	}


	wxMenu* menuTools = new wxMenu();

	menuTools->Append(ID_OPTIONS, "&Options");

	if(perms->isEnabled(PERM_TERMINAL))
	{
		menuTools->AppendSeparator();
		menuTools->Append(ID_STARTCONNECT, "&Connect to server");
		menuTools->Append(ID_DISCONNECT, "&Disconnect from server");
	}
	menuBar->Append(menuTools, "&Tools");

	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(ID_HELP, "Chameleon help contents");
	helpMenu->Append(ID_ABOUT, "&About Chameleon", "Show about dialog");	
	
	menuBar->Append(helpMenu, "&Help");
}

//////////////////////////////////////////////////////////////////////////////
///  private UpdateToolbar
///  Recreates the toolbar, based on the current permissions
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::UpdateToolbar()
{
	Permission* perms = m_options->GetPerms();

	//CleanupDropMenu();

	wxToolBar* t = GetToolBar();
	delete t;
	SetToolBar(NULL);
	int style = wxTB_FLAT | wxTB_HORIZONTAL;

	bool showText = m_options->GetShowToolbarText();
	if(showText)
	{
		style |= wxTB_TEXT;
	}
	m_config->Write("Interface/ShowToolbarText", showText ? "true" : "false");
	t = CreateToolBar(style);//new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
	//SetToolBar(t);

	wxBitmap bmNew(newfile_xpm);
	t->AddTool(ID_NEW_SOURCE, "New", bmNew, "New source file");	
    
	//wxBitmap bmOpen((open_xpm);
	
	if(perms->isEnabled(PERM_LOCALMODE))
	{
		wxBitmap bmOpenLocal(openlocal_xpm);
		t->AddTool(ID_OPEN_SOURCE_LOCAL, "Open (L)", bmOpenLocal, "Open a file from the local computer");
	}	
	wxBitmap bmOpenRemote(openremote_xpm);
	t->AddTool(ID_OPEN_SOURCE_REMOTE, "Open (R)", bmOpenRemote, "Open a file from the remote server");

	wxBitmap bmSave(savefile_xpm);
	t->AddTool(ID_SAVE, "Save", bmSave, "Save the current file");

	if(perms->isEnabled(PERM_TERMINAL))
	{
		t->AddSeparator();

		wxBitmap bmConnect(connect16_xpm);
		t->AddTool(ID_STARTCONNECT, "Connect", bmConnect, "Connect the terminal to the remote server");

		wxBitmap bmDisconnect(disconnect16_xpm);
		t->AddTool(ID_DISCONNECT, "Disconnect", bmDisconnect, "Disconnect the terminal from the remote server");		
	}


	if(perms->isEnabled(PERM_TEST))
	{
		t->AddSeparator();	

		wxBitmap bmTest(button_xpm);
		t->AddTool(ID_TEST, "Test", bmTest);
	}

	if(perms->isEnabled(PERM_COMPILE))
	{
		t->AddSeparator();
		wxBitmap bmCompile(build_xpm);
		t->AddTool(ID_COMPILE, "Compile", bmCompile, "Compile the current project or file");
	}
	
	if(perms->isEnabled(PERM_DEBUG))
	{
		t->AddSeparator();
		wxBitmap bmStart(newstart1_xpm);
		t->AddTool(ID_DEBUG_START, "Start", bmStart, "Start debugging the current project or file");

		wxBitmap bmStop(newstop1_xpm);
		t->AddTool(ID_DEBUG_STOP, "Stop", bmStop, "Stop debugging");

		wxBitmap bmContinue(newcontinue1_xpm);
		t->AddTool(ID_DEBUG_CONTINUE, "Continue", bmContinue, "Resume when paused at a breakpoint");

		wxBitmap bmStepNext(stepnext_xpm);
		t->AddTool(ID_DEBUG_STEPNEXT, "Step next", bmStepNext, 
			"Run the next line of code, stepping into it if it's a function call");

		wxBitmap bmStepOver(stepover_xpm);
		t->AddTool(ID_DEBUG_STEPOVER, "Step over", bmStepOver, 
			"Run the next line of code, but don't go inside if it's a function call");

		wxBitmap bmStepOut(stepout_xpm);
		t->AddTool(ID_DEBUG_STEPOUT, "Step out", bmStepOut,
			"Run all the code in the current function");

		t->AddSeparator();

		wxBitmap bmAddBreakpoint(breakpoint_xpm);
		t->AddTool(ID_DEBUG_ADDEDITORBREAKPOINT, "Add", bmAddBreakpoint, 
			"Add a breakpoint at the current line");

		wxBitmap bmRemoveBreakpoint(breakpoint_octagon_disable_xpm);
		t->AddTool(ID_DEBUG_REMOVEEDITORBREAKPOINT, "Remove", bmRemoveBreakpoint, 
			"Remove a breakpoint at the current line");

		wxBitmap bmClearBreakpoint(breakpoint_crossed_xpm);
		t->AddTool(ID_DEBUG_CLEAREDITORBREAKPOINTS, "Clear", bmClearBreakpoint, 
			"Remove all breakpoints in this file");

		for(int i = ID_DEBUG_IDS_FIRST; i < ID_DEBUG_IDS_LAST; i++)
		{
			t->EnableTool(i, false);
		}
	}

	t->Realize();
	

	m_config->Write("Interface/ShowToolbarText", showText ? "true" : "false");
}

//////////////////////////////////////////////////////////////////////////////
///  private UpdateTerminalNotebook
///  Recreates the notebook containing the terminal and other related widgets
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::UpdateTerminalNotebook()
{
	Permission* perms = m_options->GetPerms();
	int splitterLocation = m_splitEditorOutput->GetSashPosition();

	int numPages = m_noteTerm->GetPageCount();
	for(int i = 0; i < numPages; i++)
	{
		wxNotebookPage* page = m_noteTerm->GetPage(0);
		page->Hide();
		m_noteTerm->RemovePage(0);
	}

	if(perms->isEnabled(PERM_TERMINAL))
	{
		m_termContainer->Show();
		m_noteTerm->AddPage(m_termContainer, "Terminal");
	}
	else
	{
		m_termContainer->Hide();
	}

	bool advCompileEnabled = perms->isEnabled(PERM_ADVANCEDCOMPILE);
	if(perms->isEnabled(PERM_COMPILE))
	{
		m_outputPanel->Show();
		m_noteTerm->AddPage(m_outputPanel, "Compiler Output");
		m_outputPanel->SetAdvanced(advCompileEnabled);
	}	
	else
	{
		m_outputPanel->Hide();
	}

	if(perms->isEnabled(PERM_DEBUG))
	{
		m_watchPanel->Show();
		m_debugTermContainer->Show();
		m_noteTerm->AddPage(m_watchPanel, "Variable Watches");
		m_noteTerm->AddPage(m_debugTermContainer, "Debug I/O");
	}
	else
	{
		m_watchPanel->Hide();
		m_debugTermContainer->Hide();
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
			m_splitEditorOutput->SplitHorizontally(m_splitProjectEditor, m_noteTerm, 0.575f);//-260);//-200);
			m_splitEditorOutput->SetMinimumPaneSize(20);
			//m_splitEditorOutput->SetSashGravity(1.0);
			m_terminal->UpdateSize();
			m_noteTerm->Show();	
		}		
		m_noteTerm->SetSelection(0);
		m_noteTerm->GetPage(0)->SetFocus();
	}
	m_noteTerm->Refresh();
	m_book->Refresh();
}

// Project code begins here

//////////////////////////////////////////////////////////////////////////////
///  private OnTreeItemRightClick
///  Pops up a menu with appropriate items when the project tree is right-clicked.  
///  Also sets the last selected tree item and file type for use in other functions.
///
///  @param  event wxTreeEvent & The generated tree event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnTreeItemRightClick(wxTreeEvent& event)
{
	wxTreeItemId clickedItem = event.GetItem();
	m_clickedTreeItem = clickedItem;
	wxMenu popupMenu;

	wxTreeItemId rootItem = m_projectTree->GetRootItem();
	wxTreeItemId parentItem = m_projectTree->GetItemParent(clickedItem);

	bool rootClicked = (clickedItem == rootItem);

	if(m_projMultiFiles == NULL && !rootClicked)
	{
		wxMessageBox("You do not currently have a project open.", "No project open", 
			wxOK | wxCENTRE | wxICON_EXCLAMATION);
		return;
	}

	if(parentItem == rootItem)
	{
		wxString label = "Add file";
		if(m_clickedTreeItem == m_projectFileFolders[0])
		{		
			m_projectSelectedFolderType = FILE_SOURCES;
			label = "Add source file";
		}
		else if(m_clickedTreeItem == m_projectFileFolders[1])
		{		
			m_projectSelectedFolderType = FILE_HEADERS;
			label = "Add header file";
		}
		else if(m_clickedTreeItem == m_projectFileFolders[2])
		{		
			m_projectSelectedFolderType = FILE_LIBRARIES;
			label = "Add library";
		}
		else if(m_clickedTreeItem == m_projectFileFolders[3])
		{
			m_projectSelectedFolderType = FILE_NONSOURCE;
			label = "Add non-source file";
		}

		popupMenu.Append(ID_PROJECT_ADDFILE, label);
	}
	// user right-clicked a file in the project
	else if(clickedItem == rootItem)
	{
		if(m_projMultiFiles != NULL)
		{
			popupMenu.Append(ID_COMPILE_PROJECT, "Compile this project");
			popupMenu.Append(ID_CLOSE_PROJECT, "Close this project");
		}
		else
		{
			Permission* perms = m_options->GetPerms();
			popupMenu.Append(ID_NEW_PROJECT, "Create a new project");
			if(perms->isEnabled(PERM_LOCALMODE))
			{
				popupMenu.Append(ID_OPEN_PROJECT_LOCAL, "Open a local project");
			}
			popupMenu.Append(ID_OPEN_PROJECT_REMOTE, "Open a remote project");			
		}		
	}
	else
	{
		if(parentItem == m_projectFileFolders[0])
		{
			m_projectSelectedFolderType = FILE_SOURCES;
		}
		else if(parentItem == m_projectFileFolders[1])
		{
			m_projectSelectedFolderType = FILE_HEADERS;
		}
		else if(parentItem == m_projectFileFolders[2])
		{
			m_projectSelectedFolderType = FILE_LIBRARIES;
		}
		else if(parentItem == m_projectFileFolders[3])
		{
			m_projectSelectedFolderType = FILE_NONSOURCE;
		}

		popupMenu.Append(ID_PROJECT_REMOVEFILE, "Remove file from project");

		if(m_projectSelectedFolderType != FILE_LIBRARIES)
		{
			FileNameTreeData* data = static_cast <FileNameTreeData* > (m_projectTree->GetItemData(clickedItem));
			if(m_projMultiFiles->FileIncludedInBuild(data->filename, m_projectSelectedFolderType))
			{
				popupMenu.Append(ID_PROJECT_EXCLUDE_FILE, "Exclude file from compilation");
			}
			else
			{
				popupMenu.Append(ID_PROJECT_INCLUDE_FILE, "Include file in compilation");
			}
		}		
	}

	wxPoint p = event.GetPoint();
	m_projectTree->PopupMenu(&popupMenu, p);
}


//////////////////////////////////////////////////////////////////////////////
///  private OpenProjectFile
///  Opens a project file
///
///  @param  isRemote bool  Whether the user wants to open a remote or local file
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OpenProjectFile(bool isRemote)
{	
	m_remoteMode = isRemote;
	wxPathFormat currentPathFormat = m_remoteMode ? wxPATH_UNIX : wxPATH_DOS;
	
	if(m_projMultiFiles != NULL)
	{
		wxString message = "A project is already open.  Do you want to replace the currently open project";
		message += " with a different one?";
		int result = wxMessageBox(message, "Project already open", wxOK | wxCANCEL | wxICON_QUESTION);

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
	if(m_projMultiFiles != NULL)
	{
		CloseProjectFile();
	}	
	
	m_projMultiFiles = new ProjectInfo(false);

	m_projectTree->DeleteChildren(m_projectFileFolders[0]);
	m_projectTree->DeleteChildren(m_projectFileFolders[1]);
	m_projectTree->DeleteChildren(m_projectFileFolders[2]);

	wxString fileContents;
	wxString fileName;

	GetFileContents(fileNames[0], fileContents, fileName);


	// set the current project's base path based on the project file's directory, 
	// choosing the path separators based on where the file is stored

	wxFileName projectFileName(fileNames[0]);

	m_projMultiFiles->SetProjectFile(projectFileName);
	m_projMultiFiles->SetRemote(m_remoteMode);

	wxTreeItemId rootItem = m_projectTree->GetRootItem();
	m_projectTree->SetItemText(rootItem, fileName);
	m_projectTree->SetItemBold(rootItem);

	wxMemoryInputStream projectFileStream(fileContents, fileContents.Len());
	
	wxFileConfig config(projectFileStream);

	LoadFilesIntoProjectTree("/Sources", FILE_SOURCES, m_projectFileFolders[0], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Headers", FILE_HEADERS, m_projectFileFolders[1], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Libraries", FILE_LIBRARIES, m_projectFileFolders[2], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Other", FILE_NONSOURCE, m_projectFileFolders[3], config, currentPathFormat);

	m_projectTree->Thaw();
}

//////////////////////////////////////////////////////////////////////////////
///  private AddFileToProject
///  Adds a file to the current multi-file project
///
///  @return void
///
///  @remarks This function currently requires the user to have right-clicked in
///  @remarks the project tree.  It might be useful to expand this to, say, 
///  @remarks two overloads and a grunt work function.  One overload would be called
///  @remarks from a menubar item, one would be called from a right-click, and both
///  @remarks would call the actual add function with the right settings
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::AddFileToProject()
{
	m_remoteMode = m_projMultiFiles->IsRemote();
	wxArrayString fileNames = OpenFile(m_projectSelectedFolderType);

	if(fileNames.Count() < 1)
	{
		wxLogDebug("No files selected when adding to project");
		return;
	}

	wxString fileToOpen = fileNames[0];

	if(m_projMultiFiles->FileExistsInProject(fileToOpen))
	{
		wxString message = "The file " + fileToOpen + " already exists in this project, so it was not added.";
		wxMessageBox(message, "File already in project", wxOK | wxCENTRE | wxICON_EXCLAMATION);
		return;
	}

	wxString fileContents;
	wxString fileName;

	m_remoteMode = m_projMultiFiles->IsRemote();

	m_projMultiFiles->AddFileToProject(fileToOpen, m_projectSelectedFolderType);

	int iconIndex = iconIndex = m_extensionMappings[wxFileName(fileToOpen).GetExt()];

	FileNameTreeData* data = new FileNameTreeData();
	data->filename = fileToOpen;

	wxString simpleName = wxFileName(fileToOpen).GetFullName();
	
	m_projectTree->AppendItem(m_clickedTreeItem, simpleName, iconIndex, -1, data);
	m_projectTree->SortChildren(m_clickedTreeItem);
	m_projectTree->Refresh();	
	
	wxFileName fn(fileToOpen);
	int pageNum = GetPageNum(fn);

	if(pageNum != -1)
	{
		ChameleonEditor* edit = static_cast<ChameleonEditor*> (m_book->GetPage(pageNum));
		edit->SetProject(m_projMultiFiles);
	}

	SaveProjectFile();
}

//////////////////////////////////////////////////////////////////////////////
///  private OnTreeItemActivated
///  Attempts to open a file when an item is double-clicked in the project tree
///
///  @param  event wxTreeEvent & The generated tree event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnTreeItemActivated(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();

	wxTreeItemId rootItem = m_projectTree->GetRootItem();

	if( item != rootItem)
	{
		wxTreeItemId parentItem = m_projectTree->GetItemParent(item);

		//if( (parentItem == m_projectFileFolders[0]) ||
		//	(parentItem == m_projectFileFolders[1]) )

		// allow opening anything but .lib files
		if(parentItem != m_projectFileFolders[2])
		{
			FileNameTreeData* data = static_cast <FileNameTreeData* > (m_projectTree->GetItemData(item));
			
			wxArrayString filesToOpen;
			wxString pathname = data->filename;
			filesToOpen.Add(pathname);

			OpenSourceFile(filesToOpen);
		}
		else// if(parentItem == m_projectFileFolders[2])
		{
			wxMessageBox("Only source files can be opened", "Chameleon", wxOK | wxICON_INFORMATION);
			return;
		}
	}
	m_currentEd->Refresh();
	m_book->Refresh();
}

//////////////////////////////////////////////////////////////////////////////
///  private CloseProjectFile
///  Closes the currently open multi-file project
///
///  @param  canUserCancel bool  [=true] Whether or not to let the user cancel the project close
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::CloseProjectFile(bool canUserCancel)
{
	if(!m_appClosing)
	{
		EditorPointerArray edList = m_projMultiFiles->GetEditors();

		if(edList.GetCount() > 0)
		{
			int allowedResponse = wxYES_NO;

			if(canUserCancel)
			{
				allowedResponse |= wxCANCEL;
			}

			int response = wxMessageBox("Do you want to close all files from this project?", 
				"Close Project", allowedResponse | wxCENTER | wxICON_QUESTION);

			if(response == wxYES)
			{
				for(int i = 0; i < (int)edList.GetCount(); i++)
				{
					ChameleonEditor* ed = edList[i];
					int tabnum = m_book->FindPagePosition(ed);
					CloseFile(tabnum);
				}
			}
			else if(response == wxNO)
			{
				for(int i = 0; i < (int)edList.GetCount(); i++)
				{
					ChameleonEditor* ed = edList[i];
					ProjectInfo* proj = new ProjectInfo();
					proj->SetRemote(ed->LastSavedRemotely());
					ed->SetProject(proj);
				}
			}
			else
			{
				return;
			}
		}
	}

	SaveProjectFile();
	
	// Clear out all items in the project tree
	for(int i = 0; i < 4; i++)
	{
		m_projectTree->DeleteChildren(m_projectFileFolders[i]);
	}

	m_projectTree->SetItemText(m_projectTree->GetRootItem(), "No project");

	delete m_projMultiFiles;
	m_projMultiFiles = NULL;
}

//////////////////////////////////////////////////////////////////////////////
///  private SaveProjectFile
///  Saves the currently open multi-file project
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::SaveProjectFile()
{
	wxPathFormat currentPathFormat = (m_projMultiFiles->IsRemote() ? wxPATH_UNIX : wxPATH_DOS);
	wxMemoryInputStream mis("", 0);
	wxFileConfig config(mis);

	config.SetPath("/Headers");
	wxArrayString headers = m_projMultiFiles->GetHeaders();
	for(size_t i = 0; i < headers.Count(); i++)
	{
		wxString headerName;
		headerName << (i + 1);
		wxFileName relativeName(headers[i]);
		config.Write(headerName, relativeName.GetFullPath(currentPathFormat));
	}

	config.SetPath("/Sources");
	wxArrayString sources = m_projMultiFiles->GetSources();
	for(size_t i = 0; i < sources.Count(); i++)
	{
		wxString sourceName;
		sourceName << (i + 1);
		wxFileName relativeName(sources[i]);
		config.Write(sourceName, relativeName.GetFullPath(currentPathFormat));
	}

	config.SetPath("/Libraries");
	wxArrayString libraries = m_projMultiFiles->GetLibraries();
	for(size_t i = 0; i < libraries.Count(); i++)
	{
		wxString libraryName;
		libraryName << (i + 1);
		wxFileName relativeName(libraries[i]);
		config.Write(libraryName, relativeName.GetFullPath(currentPathFormat));
	}

	config.SetPath("/Other");
	wxArrayString other = m_projMultiFiles->GetNonSources();
	for(size_t i = 0; i < other.Count(); i++)
	{
		wxString otherName;
		otherName << (i + 1);
		wxFileName relativeName(other[i]);
		config.Write(otherName, relativeName.GetFullPath(currentPathFormat));
	}

	wxMemoryOutputStream outputStream;
	//config.FlushToStream(outputStream);
	config.Save(outputStream);

	wxString resultContents;
	size_t streamsize = outputStream.GetSize();

	if(streamsize == 0)
	{
		resultContents = "[Headers]\n\n[Sources]\n\n[Libraries]\n\n[Other]";
	}
	else
	{
		char* bufptr = resultContents.GetWriteBuf(streamsize);
		outputStream.CopyTo(bufptr,streamsize);
		resultContents.UngetWriteBuf();
	}

	wxString projBasePath = m_projMultiFiles->GetProjectBasePath();
	wxString projName = m_projMultiFiles->GetProjectFile().GetFullName();
	if(m_projMultiFiles->IsRemote())
	{
		wxFileName fn(projBasePath, projName);
		m_network->SendFileContents(resultContents, fn);
	}
	else
	{
		wxString filename = projBasePath + "\\" + projName;
		wxFile projectFile(filename, wxFile::write);
		projectFile.Write(resultContents);
		projectFile.Close();		
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private LoadFilesIntoProjectTree
///  Loads the given files into the project tree
///
///  @param  configPath          wxString       The internal path the wxConfig should look in for file entries
///  @param  fileType			 FileFilterType The type of files being loaded
///  @param  treeid              wxTreeItemId   The tree ID of the folder to add the files to
///  @param  config              wxFileConfig & The wxFileConfig containing the project file
///  @param  currentPathFormat   wxPathFormat   The path format to use for the filenames
///
///  @return void
///
///  @author Mark Erikson @date 03-29-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::LoadFilesIntoProjectTree(wxString configPath,  FileFilterType fileType, wxTreeItemId treeid, 
										   wxFileConfig& config, wxPathFormat currentPathFormat)
{
	config.SetPath(configPath);

	int numEntries = config.GetNumberOfEntries();

	for(int i = 1; i <= numEntries; i++)
	{
		wxString keyname;
		keyname << i;
		wxString fileName = config.Read(keyname);		

		if(fileName != wxEmptyString)
		{
			wxFileName newFileName(fileName);
			//int iconNum = m_extensionMappings[newFileName.GetExt()];
			int iconNum = m_iconManager->GetIconIndex(newFileName.GetExt());

			FileNameTreeData* data = new FileNameTreeData();
			data->filename = newFileName.GetFullPath(currentPathFormat);

			m_projectTree->AppendItem(treeid, newFileName.GetFullName(), iconNum, -1, data);

			m_projMultiFiles->AddFileToProject(data->filename, fileType);

			int pageNum = GetPageNum(newFileName);

			if(pageNum != -1)
			{
				ChameleonEditor* edit = static_cast<ChameleonEditor*> (m_book->GetPage(pageNum));
				edit->SetProject(m_projMultiFiles);
			}
		}
	}

	m_projectTree->SortChildren(treeid);
}

//////////////////////////////////////////////////////////////////////////////
///  private RemoveFileFromProject
///  Removes a file from the current multi-file project
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::RemoveFileFromProject()
{
	FileNameTreeData* treeData = static_cast<FileNameTreeData*> (m_projectTree->GetItemData(m_clickedTreeItem));

	if(m_projMultiFiles->FileExistsInProject(treeData->filename))
	{
		m_projMultiFiles->RemoveFileFromProject(treeData->filename, m_projectSelectedFolderType);

		wxFileName fn(treeData->filename);
		int pageNum = GetPageNum(fn);

		if(pageNum != -1)
		{
			ChameleonEditor* pEdit = static_cast <ChameleonEditor* >(m_book->GetPage(pageNum));
			ProjectInfo* proj = new ProjectInfo();
			pEdit->SetProject(proj);
		}

		m_projectTree->Delete(m_clickedTreeItem);

		SaveProjectFile();
	}
}

// Other major functions

//////////////////////////////////////////////////////////////////////////////
///  public CheckNetworkStatus
///  Shows an appropriate error message if a network operation failed.
///
///  @return NetworkCallResult Whether to redo the operation or not
///
///  @remarks The NetworkCallResult enum is outdated and unneeded, so this
///  @remarks should probably be revised at some point to get rid of it.
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
NetworkCallResult ChameleonWindow::CheckNetworkStatus()
{
	NetworkStatus result = m_network->GetStatus();

	switch((int)result)
	{
	case NET_UNKNOWN_HOST:
		{			
			wxString hostname = m_options->GetHostname();
			wxString fingerprint = m_network->GetStatusDetails();

			wxString message = "The SSH fingerprint for the server " + hostname + " was not recognized.";
			message += "\nThe fingerprint was " + fingerprint + ".  \nDo you want to cache it?";
			int result = wxMessageBox(message, "Unknown SSH Fingerprint", wxYES_NO | wxICON_QUESTION);

			if(result == wxYES)
			{
				m_network->SSHCacheFingerprint();
				return NETCALL_REDO;
			}
			
			return NETCALL_FAILED;
			break;
		}
	case NET_ERROR_MESSAGE:
		{		
			wxString message = "An unknown network error has occurred.";
			wxString statusDetails = m_network->GetStatusDetails();;
			message += "\nError details: " + statusDetails;
			wxLogDebug("NET_ERROR_MESSAGE: %s", statusDetails);
			wxMessageBox(message, "Unknown network error", wxOK | wxICON_EXCLAMATION);
			return NETCALL_FAILED;
			break;
		}
	case NET_AUTH_FAILED:
		{
			// If everything's still default, go ahead and ask the user for the password.
			if(m_options->GetPassphrase() == "" &&
				m_options->GetHostname() != "127.0.0.1" &&
				m_options->GetUsername() != "username") 
			{
				if(AskUserForPassword())
				{
					return NETCALL_REDO;
				}

			}
			else
			{
				wxString message = "Chameleon was unable to log you in using the current username and password.";
				message += "\nPlease check them in the Options menu and try again.";
				wxMessageBox(message, "Login failed", wxOK | wxICON_EXCLAMATION);
				return NETCALL_FAILED;
			}

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

//////////////////////////////////////////////////////////////////////////////
///  private Compile
///  Compiles the active editor's project
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::Compile()
{
	bool doCompile = true;

	if(!m_compileProject)
	{
		if(!m_currentEd->HasBeenSaved())
		{
			wxMessageBox("Please save this file before trying to compile it.", 
				"Unsaved File", wxOK | wxICON_INFORMATION);

			doCompile = false;
		}
		else if(m_currentEd->Modified())
		{
			// yes, same message... this can later be changed to ask about 
			// saving, then do the appropriate thing.
			wxMessageBox("Please save this file before trying to compile it.", "Unsaved File",
				wxOK | wxICON_INFORMATION);

			doCompile = false;
		}
	}	

	if(doCompile)
	{		
		ProjectInfo* projToCompile;
		if(m_compileProject)
		{
			projToCompile = m_projMultiFiles;
		}
		else
		{
			projToCompile = m_currentEd->GetProject();
		}

		if(!projToCompile->IsRemote())
		{
			wxMessageBox("Local compiling has not been implemented.\nPlease save this file to a network server\nand try again",
						"Can't Compile Locally", wxOK | wxICON_WARNING);
			return;
		}
		m_outputPanel->ClearOutput();

		m_compiler->CompileProject(projToCompile, m_outputPanel);

		int outputIndex = m_noteTerm->FindPagePosition(m_outputPanel);
		m_noteTerm->SetSelection(outputIndex);		
		m_outputPanel->SetFocus();
	}
}

//////////////////////////////////////////////////////////////////////////////
///  public FocusOnLine
///  Moves the cursor to a given file and line, with an optional line marker
///
///  @param  filename     wxString  The filename to look for
///  @param  linenumber   int       The line number to go to
///  @param  showMarker   bool      [=true] Whether or not to show a marker to highlight the line
///  @param  linecontents wxString  [=wxEmptyString] The text that the given line should have in it
///
///  @return void
///
///  @remarks The line contents idea isn't implemented, but could be used to ensure
///  @remarks that the right file was selected if two open files had the same name
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::FocusOnLine(wxString filename, int linenumber, bool showMarker, wxString linecontents /* = wxEmptyString */)
{
	wxFileName fn(filename);

	int tabNum = GetPageNum(fn, false);

	if(tabNum == -1)
	{
		if(m_projMultiFiles != NULL)
		{
			if(m_projMultiFiles->FileExistsInProject(filename))
			{
				wxArrayString filesToOpen;
				filesToOpen.Add(filename);
				OpenSourceFile(filesToOpen);
				tabNum = GetPageNum(fn, false);
			}
		}
	}

	if(tabNum != -1)
	{
		PageHasChanged(tabNum);

		// Adjust for Scintilla's zero-based line numbers
		int adjustedLineNum = linenumber - 1;

		if(linecontents != wxEmptyString)
		{
			wxString edLineContents = m_currentEd->GetLine(adjustedLineNum);
		}

		m_currentEd->FocusOnLine(adjustedLineNum, showMarker);
	}
}

// Find/replace code begins here

//////////////////////////////////////////////////////////////////////////////
///  private OnFindEvent
///  Handles find/replace events
///
///  @param  event wxFindDialogEvent & The generated find/replace event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnFindEvent(wxFindDialogEvent& event)
{
	wxEventType type = event.GetEventType();
	wxString findString = event.GetFindString();
	long flags = event.GetFlags();
	int pos = m_currentEd->GetCurrentPos();

	if ((type == wxEVT_COMMAND_FIND) || 
		(type == wxEVT_COMMAND_FIND_NEXT))
	{
		pos = FindString(findString, pos, flags, TRUE);

		if (pos >= 0)
			m_currentEd->SetFocus();
		else
		{
			wxMessageBox("No more occurrences of: \"" + findString + "\" in this file",
				"Search Ended", wxOK | wxICON_EXCLAMATION, this);			
		}
	}
	else if (type == wxEVT_COMMAND_FIND_REPLACE)
	{
		if (m_currentEd->GetSelectedText() != findString)
		{
			wxBell();
			return;
		}

		int pos = m_currentEd->GetSelectionStart();
		wxString replaceString = event.GetReplaceString();
		m_currentEd->ReplaceSelection(replaceString);
		m_currentEd->EnsureCaretVisible();
		m_currentEd->SetSelection(pos, pos + replaceString.Length());

		m_currentEd->SetFocus();

		// TODO Do a Find after this for the next item automatically?
	}
	else if (type == wxEVT_COMMAND_FIND_REPLACE_ALL)
	{
		wxString replaceString = event.GetReplaceString();
		if (findString == replaceString)
			return;

		wxBusyCursor busy;
		int count = ReplaceAllStrings(findString, replaceString, flags);

		wxMessageBox( wxString::Format("Replaced %d occurrences of\n'%s' with '%s'.", count, findString.c_str(), 
			replaceString.c_str()), 	"Finished replacing", wxOK, this);
	}
	else if (type == wxEVT_COMMAND_FIND_CLOSE)
	{
		if (wxDynamicCast(event.GetEventObject(), wxDialog))
		{
			((wxDialog*)event.GetEventObject())->Destroy();
		}
		m_findReplace = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
///  private FindString
///  Looks for a given string in the current editor
///
///  @param  findString const wxString & The string to find
///  @param  start_pos  int              [=-1] The offset to begin searching (-1 for the whole document)
///  @param  flags      int              [=-1] The selected find/replace options
///  @param  highlight  bool             [=1] Whether or not to select the found text
///
///  @return int        The offset of the found text
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
int ChameleonWindow::FindString(const wxString &findString, int start_pos, int flags, bool highlight)
{
	if (findString.IsEmpty())
		return wxNOT_FOUND;

	int stc_flags = 0;
	if ((flags & wxFR_MATCHCASE) != 0) 
	{
		stc_flags |= wxSTC_FIND_MATCHCASE;
	}
	if ((flags & wxFR_WHOLEWORD) != 0)
	{
		stc_flags |= wxSTC_FIND_WHOLEWORD;
	}

	int pos = start_pos == -1 ? m_currentEd->GetCurrentPos() : start_pos;

	if ((flags & wxFR_DOWN) != 0)
	{
		m_currentEd->SetTargetStart(wxMax(0, pos));
		m_currentEd->SetTargetEnd(wxMax(0, m_currentEd->GetTextLength()));
	}
	else
	{
		if (labs(m_currentEd->GetTargetEnd() - m_currentEd->GetTargetStart()) == long(findString.Length()))
		{
			pos -= findString.Length() + 1; // doesn't matter if it matches or not, skip it
		}

		m_currentEd->SetTargetStart(wxMax(0, pos));
		m_currentEd->SetTargetEnd(0); 
	}

	m_currentEd->SetSearchFlags(stc_flags);
	pos = m_currentEd->SearchInTarget(findString);

	if (pos >= 0)
	{
		if (highlight)
		{
			m_currentEd->EnsureCaretVisible();
			m_currentEd->SetSelection(pos, pos + findString.Length());
		}
	}

	return pos;
}

//////////////////////////////////////////////////////////////////////////////
///  private ReplaceAllStrings
///  Replaces all occurrences of the given string in the current editor
///
///  @param  findString    const wxString & The string to find and replace
///  @param  replaceString const wxString & The string to insert
///  @param  flags         int              [=-1] The selected find/replace flags
///
///  @return int           The number of matches replaced
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
int ChameleonWindow::ReplaceAllStrings(const wxString &findString, const wxString &replaceString, int flags)
{    
	int count = 0;

	if (findString.IsEmpty() || (findString == replaceString))
		return count;

	int cursor_pos = m_currentEd->GetCurrentPos();  // return here when done

	if (m_currentEd->GetSelectedText() == findString)
	{
		++count;
		m_currentEd->ReplaceSelection(replaceString);
	}

	int pos = FindString(findString, -1, flags, TRUE);

	while (pos != -1)
	{
		++count;
		m_currentEd->ReplaceSelection(replaceString);
		pos = FindString(findString, -1, flags, TRUE);
	}

	// return to starting pos or as close as possible
	m_currentEd->GotoPos(wxMin(cursor_pos, m_currentEd->GetLength()));

	return count;
}

// Various utility functions and random other stuff

//////////////////////////////////////////////////////////////////////////////
///  private DeterminePrintSize
///  Returns an appropriate size for printing
///
///  @return wxRect The dimensions of the print area
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxRect ChameleonWindow::DeterminePrintSize () 
{
	wxSize scr = wxGetDisplaySize();

	// determine position and size (shifting 16 left and down)
	wxRect rect = GetRect();
	rect.x += 16;
	rect.y += 16;
	rect.width = wxMin (rect.width, (scr.x - rect.x));
	rect.height = wxMin (rect.height, (scr.x - rect.y));

	return rect;
}

//////////////////////////////////////////////////////////////////////////////
///  private OnSplitterDoubleClick
///  Cancels the ability to "close" a split window by double-clicking the splitter bar
///
///  @param  event wxSplitterEvent & The generated splitter event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnSplitterDoubleClick(wxSplitterEvent &event)
{
	event.Veto();
}

//////////////////////////////////////////////////////////////////////////////
///  private OnSize
///  Not actually used at the moment - could be used to cleanly resize the terminal notebook
///
///  @param  event wxSizeEvent & The generated size event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonWindow::OnSize(wxSizeEvent &event)
{
	event.Skip();	
}

void ChameleonWindow::OnTermResize(wxSplitterEvent &event)
{
}

bool ChameleonWindow::CheckForBlankPassword()
{
	wxString password = m_options->GetPassphrase();
	if(password.IsEmpty())
	{
		bool passwordEntered = AskUserForPassword();
		if(!passwordEntered)
		{
			return false;
		}

		m_network->GetStatus();
	}

	return true;
}

bool ChameleonWindow::AskUserForPassword()
{
	wxTextEntryDialog getPW(this, "Please enter your password and try again.", "Missing Password", "", wxTE_PASSWORD | wxOK | wxCANCEL);
	int oked = getPW.ShowModal();
	if(oked == wxID_OK) 
	{
		m_options->SetPassphrase(getPW.GetValue());
		return true;
	}

	return false;
}

void ChameleonWindow::OnOpenSourceFile(int id )
{
	if( id == ID_OPEN_SOURCE_LOCAL)
	{
		m_remoteMode = false;
	}
	else if(id == ID_OPEN_SOURCE_REMOTE)
	{
		m_remoteMode = true;
	}

	wxArrayString fnames = OpenFile(FILE_ALLSOURCETYPES);

	if(fnames.Count() > 0)
	{
		OpenSourceFile (fnames);
	}
}

void ChameleonWindow::OnSaveSourceFile( int id )
{
	if( id == ID_SAVE_SOURCE_LOCAL)
	{
		m_remoteMode = false;
	}
	else if(id == ID_SAVE_SOURCE_REMOTE)
	{
		m_remoteMode = true;
	}
	SaveFile(true, false, FILE_ALLSOURCETYPES);
}

void ChameleonWindow::OnStartConnect()
{
	if(!CheckForBlankPassword())
	{
		return;
	}
restartConnection:
	NetworkStatus isok = m_network->GetStatus();
	if(isok == NET_GOOD) 
	{ 

		m_terminal->Connect();
		wxLogDebug("Connected: %d", m_terminal->IsConnected());

		// Focus on the terminal
		int terminalIndex = m_noteTerm->FindPagePosition(m_termContainer);
		m_noteTerm->SetSelection(terminalIndex);
		m_terminal->SetFocus();
	}
	else 
	{
		wxLogDebug("Tried to start Terminal with invalid networking status: %d", isok);
		if(CheckNetworkStatus() == NETCALL_REDO)
		{
			goto restartConnection;
		}
	}
}

void ChameleonWindow::OnCopy()
{
	/* HACK!!! 
	* Okay, here's the problem.  When keyboard shortcuts like CTRL-C are listed in the text
	* of a menu item, that shortcut is automatically assigned.  But, this means that the menu's
	* enclosing frame will always capture that shortcut.
	*
	* I'd really like to have CTRL-C available to the editor for "Copy", but also available to 
	* the terminal for killing programs.  My initial experiments didn't come up with anything,
	* but I did come up with this workaround.  It'll do for now.  Basically, if we detect a
	* CTRL-C, we check to see if a terminal is active.  If it is, we fake a CTRL-C keyboard
	* event, and pass it on to the terminal.  Otherwise, we just copy text as usual.
	*/
	wxWindow* focusedWindow = wxWindow::FindFocus();

	if(focusedWindow == m_terminal || focusedWindow == m_debugTerminal)
	{
		wxSSH* focusedTerminal = (focusedWindow == m_terminal) ? m_terminal : m_debugTerminal;

		// magic codes copied from a real CTRL-C event.
		wxKeyEvent ctrlCEvent(wxEVT_CHAR);
		ctrlCEvent.m_controlDown = true;
		ctrlCEvent.m_keyCode = 3;
		ctrlCEvent.m_rawCode = 3;
		ctrlCEvent.m_rawFlags = 3014657;

		focusedTerminal->ProcessEvent(ctrlCEvent);
	}
	else
	{
		m_currentEd->Copy();
	}
}

void ChameleonWindow::OnFindReplace(int id)
{
	if(m_findReplace != NULL)
	{
		bool isReplaceDialog = m_findReplace->GetWindowStyle() & wxFR_REPLACEDIALOG;

		if( (isReplaceDialog && (id == ID_REPLACE)) ||
			(!isReplaceDialog && (id == ID_FIND)) )
		{
			return;
		}				
		else
		{
			delete m_findReplace;
		}
	}

	bool showFind = (id == ID_FIND);
	int dialogFlags = showFind ? 0 : wxFR_REPLACEDIALOG;
	wxString title = showFind ? "Find" : "Replace";

	m_findData.SetFlags(wxFR_DOWN);

	m_findReplace = new wxFindReplaceDialog(this,	&m_findData, title, dialogFlags);
	m_findReplace->Show(TRUE);
}

void ChameleonWindow::OnOptions()
{
	if(m_terminal->IsConnected())
	{
		m_optionsDialog->DisableServerSettings();
	}
	else
	{
		m_optionsDialog->EnableServerSettings();
	}

	m_optionsDialog->InitializeDialog();
	int result = m_optionsDialog->ShowModal();

	Permission* perms = m_options->GetPerms();
	// because the authorization could be updated and followed by a cancel,
	// go ahead and write the authcode out.  Odds are it hasn't changed, but
	// it's worth doing anyway to make sure it's current.
	m_config->Write("Permissions/authorized", perms->GetAuthCode());

	// For the same reason, ALWAYS re-evaluate the options.  If the user canceled
	// the dialog, things won't have changed.
	EvaluateOptions();	

	m_currentEd->SetFocus();
}

void ChameleonWindow::OnProjectIncludeExcludeFile( int id )
{
	bool include = (id == ID_PROJECT_INCLUDE_FILE);
	FileNameTreeData* data = static_cast <FileNameTreeData* > (m_projectTree->GetItemData(m_clickedTreeItem));
	m_projMultiFiles->SetFileBuildInclusion(data->filename, m_projectSelectedFolderType, include);

	wxString extension = wxFileName(data->filename).GetExt();
	int iconIndex;
	if(include)
	{
		iconIndex = m_extensionMappings[extension];
	}
	else
	{
		extension += "_disabled";
		iconIndex = m_extensionMappings[extension];
	}
	m_projectTree->SetItemImage(m_clickedTreeItem, iconIndex);
}
	
void ChameleonWindow::OnPrintPage()
{
	m_currentEd->SetPrintColourMode(m_options->GetPrintStyle());
	wxPrintDialogData printDialogData( *g_printData);
	wxPrinter printer (&printDialogData);
	ChameleonPrintout printout (m_currentEd, m_options);
	if (!printer.Print (this, &printout, true)) 
	{
		if (wxPrinter::GetLastError() == wxPRINTER_ERROR) 
		{
			wxMessageBox ("There was a problem with printing.\nPlease check your printer setup and try again.",
				"Print failed", wxOK | wxICON_WARNING);
			return;
		}
	}
	(*g_printData) = printer.GetPrintDialogData().GetPrintData();
}


void ChameleonWindow::OnPrintPreview()
{
	m_currentEd->SetPrintColourMode(m_options->GetPrintStyle());
	wxPrintDialogData printDialogData( *g_printData);
	printDialogData.SetToPage(999);
	wxPrintPreview *preview = new wxPrintPreview (new ChameleonPrintout (m_currentEd, m_options),
		new ChameleonPrintout (m_currentEd, m_options),
		&printDialogData);
	if (!preview->Ok()) 
	{
		delete preview;
		wxMessageBox ("There was a problem with previewing.\nPlease check your printer setup and try again.",
			"Preview failed", wxOK | wxICON_WARNING);
		return;
	}
	wxRect rect = DeterminePrintSize();
	wxPreviewFrame *frame = new wxPreviewFrame (preview, this, _("Print Preview"));
	frame->SetSize (rect);
	frame->Centre(wxBOTH);
	frame->Initialize();
	frame->Show(true);
}

void ChameleonWindow::OnPrintSetup()
{
	(*g_pageSetupData) = * g_printData;
	wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
	pageSetupDialog.ShowModal();
	(*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
	(*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
}

void ChameleonWindow::OnHelp()
{
	wxString helpFile = "chameleon.chm";
	wxString ext = "chm";
	wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
	if ( !ft )
	{
		return;
	}

	wxString cmd;
	bool ok = ft->GetOpenCommand(&cmd,
		wxFileType::MessageParameters(helpFile, _T("")));
	delete ft;
	if ( !ok )
	{
		return;
	}

	wxProcess *process = new wxProcess();
	long pidLast = wxExecute(cmd, wxEXEC_ASYNC, process);
	if ( !pidLast )
	{
		delete process;
	}

	return;
}



/*
void ChameleonWindow::CleanupDropMenu()
{
	wxToolBar* tb = GetToolBar();

	if(tb == NULL)
	{
		return;
	}

	wxControl* dropControl = tb->FindControl(ID_DEBUG_REMOVEEDITORBREAKPOINT);
	if(dropControl != NULL)
	{
		mmDropMenu* dropMenu = (mmDropMenu*)dropControl;
		wxMenu* menu = dropMenu->GetChild();

		delete dropMenu;
		delete menu;
	}
}
*/