#define CHAMELEON__CPP

//#define DEBUG_TRACE_MEMORY_LEAKS

#ifdef DEBUG_TRACE_MEMORY_LEAKS
#include <windows.h>
#include "../common/StackWalker.h"
#endif

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#include "../common/StackTrace.h"
//#include "../common/CommonHeaders.h"
#include "ChameleonWindow.h"
#include <wx/statusbr.h>
#include <wx/choicdlg.h>
#include <wx/msgdlg.h>
#include <wx/msw/tbar95.h>
#include <wx/print.h>
#include <wx/printdlg.h>

#include <math.h>

#include "../common/fixvsbug.h"
#include "../editor/editor.h"
//#include "updateuihandler.h"
#include "ChameleonNotebook.h"
#include "dialogs/OptionsDialog.h"
#include "dialogs/RemoteFileDialog.h"
#include "dialogs/wxTermContainer.h"
#include "dialogs/VariableWatchPanel.h"
#include "dialogs/CompilerOutputPanel.h"
#include "../common/ChameleonPrintout.h"
#include "../perms/p.h"
#include "../common/ProjectInfo.h"
#include "../network/networking.h"
#include "../compiler/compiler.h"
#include "../common/Options.h"
#include "../debugger/cham_db.h"
#include "../common/DebugEvent.h"
//#include "wxtelnet.h"
#include "wxssh.h"
#include "../compiler/compilerevent.h"

/*
#include "../common/exceptions/se_translator.h"
#include "../common/exceptions/sym_engine.h"
#include "../common/exceptions/exception2.h"
#include "../common/exceptions/exception_trap.h"
#include "../common/exceptions/unhandled_report.h"
*/

#include "new.xpm"
#include "open.xpm"
#include "save.xpm"

#include "build.xpm"
#include "compilestop.xpm"
#include "button.xpm"

#include "connect16.xpm"
#include "disconnect16.xpm"

//#include "start.xpm"
//#include "stop.xpm"
#include "newstart1.xpm"
#include "newcontinue1.xpm"
#include "newstop1.xpm"
#include "stepnext.xpm"
#include "stepout.xpm"
#include "stepover.xpm"
//#include "pause.xpm"

#include "moz.xpm"

#define ID_RANDOMTEXTLABEL 9991


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
	//EVT_MENU						(ID_CLOSETAB, ChameleonWindow::CloseTab)
	EVT_MENU						(ID_CLOSETAB, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_CLOSEALL, ChameleonWindow::OnMenuEvent)
	EVT_UPDATE_UI					(ID_SAVE, ChameleonWindow::OnUpdateSaveUI)
	EVT_MENU						(ID_STARTCONNECT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_DISCONNECT, ChameleonWindow::OnMenuEvent)
	EVT_MENU						(ID_COMPILE, ChameleonWindow::OnMenuEvent)
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
	EVT_UPDATE_UI_RANGE				(ID_DEBUG_IDS_FIRST, ID_DEBUG_IDS_LAST - 1, ChameleonWindow::OnUpdateDebugUI)
	EVT_UPDATE_UI_RANGE				(ID_STARTCONNECT, ID_DISCONNECT, ChameleonWindow::OnUpdateConnectionUI)
	EVT_UPDATE_UI					(ID_COMPILE, ChameleonWindow::OnUpdateCompileUI)
	EVT_DEBUG						(ChameleonWindow::OnDebugEvent)
	EVT_IDLE						(ChameleonWindow::OnIdle)
	EVT_TIMER						(ID_STATUSTIMER, ChameleonWindow::OnStatusTimer)

	
END_EVENT_TABLE()

//se_translator stackTraceTranslator;
//exception_trap<unhandled_report> trap(true, unhandled_report());


IMPLEMENT_APP(MyApp)
//----------------------------------------------------------------------
// `Main program' equivalent: the program execution "starts" here

bool MyApp::OnInit()
{
	//OnlyInstallUnhandeldExceptionFilter();
#ifdef DEBUG_TRACE_MEMORY_LEAKS
	//InitAllocCheck(ACOutput_XML);
#endif
    ChameleonWindow *frame = new ChameleonWindow("Chameleon",
                                 wxPoint(5, 5), wxSize(400, 300));

    frame->Show(TRUE);
    return TRUE;
}

int MyApp::OnRun()
{
	//se_translator translator;
	try
	{
		return wxApp::OnRun();
	}
	
	catch (...) 
	{
		int q = 41;
		q++;

		return 1;
	}
	
	/*
	catch(se_translator::access_violation& ex)
	{			
		cerr	<< ex.name() << " at 0x" << std::hex << ex.address() 
			<< ", thread attempts to " << (ex.is_read_op() ? "read" : "write") 
			<< " at 0x" << std::right << ex.inaccessible_address() << std::endl
			<< "stack trace : " << std::endl;
		sym_engine::stack_trace(cerr, ex.info()->ContextRecord);	
		return -1;
	}
	catch(exception2& ex)
	{
		cerr  << ex.what() << std::endl
			<< "stack trace :" << std::endl
			<< ex.stack_trace() << std::endl;
		return -1;
	}
	*/
}

MyApp::~MyApp()
{
#ifdef DEBUG_TRACE_MEMORY_LEAKS
	//DeInitAllocCheck();
#endif
}

//----------------------------------------------------------------------


#pragma hdrstop


// frame constructor
ChameleonWindow::ChameleonWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	//m_appStarting = true;
	wxStopWatch stopwatch;

	//_crtBreakAlloc = 1740;
	
	// should be approximately 80x15 for the terminal
	this->SetSize(800, 600);

	// Show a log window for all debug messages
#ifdef _DEBUG
	logWindow = new wxLogWindow(this, "Debug messages");
	wxLog::SetActiveTarget(logWindow);
#endif

	m_updateTimer = NULL;

	wxIcon icon(moz_xpm);
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
	//m_perms = new Permission(defaultAuthorizedCode, defaultEnableCode);

	wxFileName configName(wxGetHomeDir(), "chameleon.ini");

	m_config = new wxFileConfig("Chameleon", wxEmptyString, configName.GetFullPath());

	if(configName.FileExists())
	{
		m_options->SetHostname(m_config->Read("Network/hostname"));
		m_options->SetUsername(m_config->Read("Network/username"));
		m_options->SetMingwPath(m_config->Read("Compiler/mingwpath"));

		bool printInColor = (m_config->Read("Miscellaneous/PrintInColor", "true") == "true");
		int printStyle = printInColor ? wxSTC_PRINT_COLOURONWHITE : wxSTC_PRINT_BLACKONWHITE;
		m_options->SetPrintStyle(printStyle);

		bool showToolbarText = (m_config->Read("Interface/ShowToolbarText", "true") == "true");
		m_options->SetShowToolbarText(showToolbarText);
		
		authorizedCode = m_config->Read("Permissions/authorized", defaultAuthorizedCode);
		enabledCode = m_config->Read("Permissions/enabled", defaultEnableCode);
		
		if(!perms->setGlobalAuthorized(authorizedCode))
		{
			wxLogDebug("Authcode initialization failed!  Code: %s", authorizedCode.c_str());
			authorizedCode = "0";
		}
		else
		{
			
		}
		perms->setGlobalEnabled(enabledCode);		
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
	m_debugger = new Debugger(m_network, this);

	m_optionsDialog = new OptionsDialog(this, m_options, ID_OPTIONSDIALOG, "Options");

	m_splitEditorOutput = new wxSplitterWindow(this, ID_SPLITEDITOROUTPUT);
	m_splitProjectEditor = new wxSplitterWindow(m_splitEditorOutput, ID_SPLITPROJECTEDITOR);
	m_book = new ChameleonNotebook(m_splitProjectEditor, ID_NOTEBOOK_ED);

	m_noteTerm = new ChameleonNotebook(m_splitEditorOutput, ID_NOTEBOOK_TERM);
	m_termContainer = new wxTermContainer(m_noteTerm, ID_TERM_CONTAINER);
	m_terminal = new wxSSH(m_termContainer, ID_TERMINAL, m_network, wxPoint(0, 0));
	m_terminal->set_mode_flag(GTerm::CURSORINVISIBLE);
	m_termContainer->SetTerminal(m_terminal);		

	//m_compilerTextbox = new wxTextCtrl(m_noteTerm, ID_COMPILERTEXTBOX, wxEmptyString, wxDefaultPosition,
	//								wxDefaultSize, wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY);
	//m_compilerList = new wxListCtrl( m_noteTerm, ID_COMPILERLISTCTRL, wxDefaultPosition, wxDefaultSize, 
	//									wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxSIMPLE_BORDER );

	//m_compilerTextbox->Hide();
	m_outputPanel = new CompilerOutputPanel(m_noteTerm, this, ID_COMPILEROUTPUT);
	m_outputPanel->SetAdvanced(perms->isEnabled(PERM_ADVANCEDCOMPILE));

	/*
	wxListItem itemCol;
	itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH;
	itemCol.m_text = _T("File");
	itemCol.m_width = 120;
	m_list->InsertColumn(0, itemCol);
	itemCol.m_text = _T("Line");
	itemCol.m_width = 80;
	m_list->InsertColumn(1, itemCol);
	itemCol.m_text = _T("Error");
	itemCol.m_width = 400;
	m_list->InsertColumn(2, itemCol);
	*/

	m_watchPanel = new VariableWatchPanel(m_noteTerm, this, ID_VARWATCHPANEL);


	// project setup
	m_projectTree = new wxTreeCtrl(m_splitProjectEditor, ID_PROJECTTREE);

	m_remoteFileDialog = new RemoteFileDialog(this, ID_REMOTEFILEDIALOG);
	m_remoteFileDialog->SetNetworking(m_network);

	wxTreeItemId rootNode = m_projectTree->AddRoot("No project open", ICON_FOLDERCLOSED, ICON_FOLDEROPEN);
	m_projectFileFolders[0] = m_projectTree->AppendItem(rootNode, "Source files", ICON_FOLDERCLOSED, ICON_FOLDEROPEN);
	m_projectFileFolders[1] = m_projectTree->AppendItem(rootNode, "Header files", ICON_FOLDERCLOSED, ICON_FOLDEROPEN);
	m_projectFileFolders[2] = m_projectTree->AppendItem(rootNode, "Libraries", ICON_FOLDERCLOSED, ICON_FOLDEROPEN);

	m_currentPage = 0;
	m_fileNum = 0;

	m_appClosing = false;
	m_setSelection = false;

	//wxToolBar* toolBar = CreateToolBar(wxTB_FLAT | wxTB_TEXT);

	//SetToolBar(toolBar);

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
	//m_bProjectOpen = false;	
}

ChameleonWindow::~ChameleonWindow()
{
	m_config->Flush();

	//delete m_perms;
	

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

	//m_outputPanel->Reparent(NULL);
	//m_compilerTextbox->Reparent(NULL);
	//m_watchPanel->Reparent(NULL);

	/*
#ifdef _DEBUG
	//logWindow->Reparent(NULL);
	wxLog::SetActiveTarget(NULL);
	delete logWindow;
#endif
	*/

	
	//delete m_outputPanel;
	//delete m_compilerTextbox;
	//delete m_watchPanel;

	
	if(m_noteTerm->DestroyChildren())
	{
		wxLogDebug("Successfully destroyed notebook children");
	}
	else
	{
		wxLogDebug("Notebook children destroy failed");
	}
	

	delete m_optionsDialog;
	delete m_compiler;
	delete m_config;
	delete m_options;
	delete m_debugger;
	delete m_network;
	
	
}

void ChameleonWindow::OnMenuEvent(wxCommandEvent &event)
{
	int id = event.GetId();

	switch(id)
	{
		case ID_NEW_SOURCE:
			NewFile();
			break;

		case ID_OPEN_SOURCE_LOCAL:
		case ID_OPEN_SOURCE_REMOTE:
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
				//PageHasChanged (m_currentPage);
			}
			break;
		}

		case ID_SAVE:
			SaveFile(false, true, FILE_ALLSOURCETYPES);
			break;

		case ID_SAVE_SOURCE_LOCAL:
		case ID_SAVE_SOURCE_REMOTE:
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
			break;
		}

		case ID_NEW_PROJECT:
			SaveFile(true, true, FILE_PROJECT);
			break;

		case ID_OPEN_PROJECT_LOCAL:
		case ID_OPEN_PROJECT_REMOTE:
			OpenProjectFile(id == ID_OPEN_PROJECT_REMOTE);
			break;

		case ID_CLOSE_PROJECT:
			CloseProjectFile();
			break;

		case ID_CLOSETAB:
		{
			CloseTab();
			/*
			if (!m_setSelection)
			{
				PageHasChanged();
			}
			*/
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
			CloseAllFiles();
			break;


		case ID_STARTCONNECT:
		{
			NetworkStatus isok = m_network->GetStatus();
			if(isok == NET_GOOD) 
			{ 
				m_terminal->Connect(wxEmptyString, wxEmptyString, wxEmptyString);
				wxLogDebug("Connected: %d", m_terminal->IsConnected());
				//Highlight Terminal:
				int terminalIndex = m_noteTerm->FindPagePosition(m_termContainer);
				m_noteTerm->SetSelection(terminalIndex);
				m_terminal->SetFocus();
			}
			else 
			{
				wxLogDebug("Tried to start Terminal with invalid networking status: %d", isok);
				CheckNetworkStatus();//  <--- causes some problems
			}
			break;
		}

		case ID_DISCONNECT:
			m_terminal->Disconnect();
			break;

		case ID_QUIT:
			Close(TRUE);
			break;

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
			m_currentEd->Copy();
			break;

		case ID_CUT:
			m_currentEd->Cut();
			break;

		case ID_PASTE:
			m_currentEd->Paste();
			break;

		case ID_FIND:
		case ID_REPLACE:
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

			break;
		}

		case ID_ABOUT:
		{
			wxString msg;
			msg.Printf( _T("Chameleon: The Adaptive IDE "));
			msg += CHAMELEON_VERSION_NUM;

			wxMessageBox(msg, "About Chameleon", wxOK | wxICON_INFORMATION, this);
			break;
		}
		
		case ID_OPTIONS:
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

			if(result == wxOK)
			{
				EvaluateOptions();			
			}

			m_currentEd->SetFocus();
			break;
		}

		case ID_COMPILE:
			Compile();
			break;

		case ID_PROJECT_ADDFILE:
			AddFileToProject();
			break;

		case ID_PROJECT_REMOVEFILE:
			RemoveFileFromProject();

		case ID_PROJECT_EXCLUDE_FILE:
		case ID_PROJECT_INCLUDE_FILE:
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
			break;
		}

		case ID_PRINT_PAGE:
		{
			m_currentEd->SetPrintColourMode(m_options->GetPrintStyle());
			wxPrintDialogData printDialogData( *g_printData);
			wxPrinter printer (&printDialogData);
			ChameleonPrintout printout (m_currentEd);
			if (!printer.Print (this, &printout, true)) 
			{
				if (wxPrinter::GetLastError() == wxPRINTER_ERROR) 
				{
					wxMessageBox ("There was a problem with printing.\nPlease check your printer setup and try again.",
									"Print failed", wxOK);
					return;
				}
			}
			(*g_printData) = printer.GetPrintDialogData().GetPrintData();
			break;
		}

		case ID_PRINT_PREVIEW:
		{
			m_currentEd->SetPrintColourMode(m_options->GetPrintStyle());
			wxPrintDialogData printDialogData( *g_printData);
			wxPrintPreview *preview = new wxPrintPreview (new ChameleonPrintout (m_currentEd),
				new ChameleonPrintout (m_currentEd),
				&printDialogData);
			if (!preview->Ok()) 
			{
				delete preview;
				wxMessageBox ("There was a problem with previewing.\nPlease check your printer setup and try again.",
								"Preview failed", wxOK);
				return;
			}
			wxRect rect = DeterminePrintSize();
			wxPreviewFrame *frame = new wxPreviewFrame (preview, this, _("Print Preview"));
			frame->SetSize (rect);
			frame->Centre(wxBOTH);
			frame->Initialize();
			frame->Show(true);
			break;
		}

		case ID_PRINT_SETUP:
		{
			(*g_pageSetupData) = * g_printData;
			wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
			pageSetupDialog.ShowModal();
			(*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
			(*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
			break;
		}
	}
}

// create a new blank file
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

		wxRegEx reModeTest = "\\((R|L|\\?)\\) ";

		if(reModeTest.Matches(tabText))
		{
			wxString modeString = reModeTest.GetMatch(tabText, 1);

			if(modeString == "R")
			{
				m_remoteMode = true;
			}
			else if(modeString == "L")
			{
				m_remoteMode = false;
			}
			// else assume unsaved file and don't change anything
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

	//if(m_bProjectOpen)
	if(m_projMultiFiles != NULL)
	{
		CloseProjectFile();
	}

	if(m_terminal->IsConnected())
	{
		m_terminal->Disconnect();
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
		//wxString currentFileName = wxEmptyString;
		wxFileName currentFileName;
		if ((m_book->GetPageCount() > 1) || m_appClosing) 
		{
			//currentFileName = m_currentEd->GetFileNameAndPath();
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
			//wxString currentEditorFileName = m_currentEd->GetFileNameAndPath();
			//if(currentFileName != wxEmptyString)
			if(currentFileName.IsOk())
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

		int result = wxMessageBox (_(saveMessage), _("Close"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
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

			wxString userHome;
			if(!m_network->GetHomeDirPath(userHome))
			{
				// TODO error stuff here
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
			wxEndBusyCursor();
			CheckNetworkStatus();
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
		wxFileName newFileName(fnames[n]);
		int pageNr = GetPageNum(newFileName);

		if(!GetFileContents(fnames[n], fileContents, fileNameNoPath))
		{
			return;
		}

		// TODO Empty files?
		/*
		if(fileContents == wxEmptyString)
		{
			return;
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
	}

	// show the active tab, new or otherwise
	if (firstPageNr >= 0) 
	{
		//m_currentPage = firstPageNr;
		//m_setSelection = true;
		//m_book->SetSelection (m_currentPage);
		//m_setSelection = false;
		PageHasChanged(firstPageNr);
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
		//fileContents = m_network->GetFileContents(remoteFile, remotePath);
		if(!m_network->GetFileContents(fn, fileContents))
		{
			// TODO error stuff here
			CheckNetworkStatus();
		}
		wxEndBusyCursor();
		
		/*
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
		*/
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

void ChameleonWindow::OnPageChange (wxNotebookEvent &WXUNUSED(event)) 
{
	if (!m_setSelection)
	{
		PageHasChanged();
	}
}

void ChameleonWindow::OnDebugCommand(wxCommandEvent &event)
{
	int eventID = event.GetId();

	wxDebugEvent debugEvent;

	debugEvent.SetId(eventID);
	debugEvent.SetProject(m_currentEd->GetProject());
	wxArrayString sourcefiles;

	switch(eventID)
	{
		case ID_DEBUG_START:
		{
			FileBreakpointHash bphash;
			if(m_projMultiFiles != NULL)
			{
				debugEvent.SetProject(m_projMultiFiles);
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
				wxFileName sourceFile = m_currentEd->GetFileName();
				wxString sourceFileName = sourceFile.GetFullPath(wxPATH_UNIX);
				wxArrayInt linenumbers = m_currentEd->GetBreakpoints();
				bphash[sourceFileName] = linenumbers;
			}
			debugEvent.SetFileBreakpoints(bphash);

			break;
		}	
	}

	m_debugger->AddPendingEvent(debugEvent);

}


// my "I need to try something out, I'll stick it in here" function
void ChameleonWindow::Test(wxCommandEvent& WXUNUSED(event))
{
	CompilerEvent c1(chEVT_COMPILER_START);
	wxFileName fn = m_currentEd->GetFileName();
	c1.SetFile(fn);
	m_outputPanel->ProcessEvent(c1);

	CompilerEvent c2(chEVT_COMPILER_PROBLEM);
	bool isRemote = m_currentEd->LastSavedRemotely();
	int line = 52;
	wxString message = "Your compilation screwed up somehow";
	wxString gccout = "Insert gobbledygook from GCC here";
	c2.SetFile(fn);
	c2.SetRemoteFile(isRemote);
	c2.SetInt(line);
	c2.SetMessage(message);
	c2.SetGCCOutput(gccout);
	m_outputPanel->ProcessEvent(c2);

	CompilerEvent c3(chEVT_COMPILER_END);
	fn.SetExt("out");
	c3.SetFile(fn);
	c3.SetResult(CR_OK);
	m_outputPanel->ProcessEvent(c3);

	CompilerEvent c4(chEVT_COMPILER_END);
	c4.SetFile(fn);
	c4.SetResult(CR_ERROR);
	m_outputPanel->ProcessEvent(c4);

	

	CompilerEvent c5(chEVT_COMPILER_PROBLEM);
	c5.SetFile(fn);
	c5.SetRemoteFile(isRemote);
	c5.SetInt(line);
	c5.SetMessage(message);
	c5.SetGCCOutput(gccout);
	m_outputPanel->ProcessEvent(c5);

	CompilerEvent c6(chEVT_COMPILER_START);
	c6.SetFile(fn);
	m_outputPanel->ProcessEvent(c6);

	CompilerEvent c7(chEVT_COMPILER_END);
	c7.SetFile(fn);
	c7.SetResult(CR_TERMINATED);
	m_outputPanel->ProcessEvent(c7);


	


	//TextManager tm = m_terminal->GetTM();
	//tm.PrintToBitmap();
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

/*
// zap the program frame
void ChameleonWindow::OnCloseWindow(wxCloseEvent& event)
{
	this->Destroy();
}
*/

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
void ChameleonWindow::OnUpdateSaveUI(wxUpdateUIEvent &event)
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

bool ChameleonWindow::SaveFile(bool saveas, bool askLocalRemote, FileFilterType filterType)
{
	wxString filename;
	wxString fileContents;

	bool originalRemoteMode = m_remoteMode;

	bool doSaveAs = saveas || !m_currentEd->HasBeenSaved() || (m_remoteMode != m_currentEd->LastSavedRemotely());

	//bool remoteEnabled = m_perms->isEnabled(PERM_REMOTELOCAL);
	bool isSourceFile = !(filterType == FILE_PROJECT);

	wxString filterString = ConstructFilterString(filterType);
	
	// assume for the moment that we won't have remote mode as a permission
	if(doSaveAs && askLocalRemote)
	{
		// default to saving remotely
		m_remoteMode = true;

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

				wxString userHome;
				if(!m_network->GetHomeDirPath(userHome))
				{
					// TODO error stuff here
					CheckNetworkStatus();
				}
				else
				{
					remotePath.Replace("~", userHome);
				}
			}
			else
			{
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
			// TODO error stuff here
			CheckNetworkStatus();
		}
		wxEndBusyCursor();
		
		/*
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
		*/

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
		//if(m_bProjectOpen)
		if(m_projMultiFiles != NULL)
		{
			CloseProjectFile();
		}
		

		m_projMultiFiles = new ProjectInfo;
		//m_bProjectOpen = true;

		m_projMultiFiles->SetRemote(m_remoteMode);
		wxFileName projectPath(filename);
		m_projMultiFiles->SetProjectName(projectPath.GetFullName());
		wxString path = projectPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR,
											m_remoteMode ? wxPATH_UNIX : wxPATH_DOS);
		m_projMultiFiles->SetProjectPath(path);

		wxTreeItemId rootItem = m_projectTree->GetRootItem();
		m_projectTree->SetItemText(rootItem, projectPath.GetFullName());
	}

	return true;
}

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

void ChameleonWindow::ResizeSplitter()
{

}

// I've got severable integer variables I need to access at various points,
// so rather than doing a separate Get/Set for each of them, I did this
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

// same as SetIntVar, only getting
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

// figure out which integer value I'm wanting to Get/Set
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

bool ChameleonWindow::IsEnabled(int permission)
{

	return m_options->GetPerms()->isEnabled(permission);
}

bool ChameleonWindow::InRemoteMode()
{
	return m_remoteMode;
}

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
	menuFile->Append(ID_SAVE_SOURCE_LOCAL, "Save File &As (Local)", "Save the current file as a different name");
	menuFile->Append(ID_SAVE_SOURCE_REMOTE, "Save File &As (Remote)", "Save the current file as a different name");
	menuFile->AppendSeparator();

	if(perms->isEnabled(PERM_PROJECTS))
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

	if(perms->isEnabled(PERM_TERMINAL))
	{
		menuTools->AppendSeparator();

		menuTools->Append(ID_STARTCONNECT, "&Connect to server");
		menuTools->Append(ID_DISCONNECT, "&Disconnect from server");
	}

	menuBar->Append(menuTools, "&Tools");


	if(perms->isEnabled(PERM_COMPILE))
	{
		wxMenu* compileMenu = new wxMenu();
		compileMenu->Append(ID_COMPILE, "Compile File");
		menuBar->Append(compileMenu, "&Compile");
	}


	/*
	if(perms->isEnabled(PERM_PROJECTS))
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
	Permission* perms = m_options->GetPerms();

	wxToolBar* t = GetToolBar();
	//t->ClearTools();
	delete t;
	SetToolBar(NULL);
	int style = wxTB_FLAT | wxTB_HORIZONTAL;

	bool showText = m_options->GetShowToolbarText();
	if(showText)
	{
		style |= wxTB_TEXT;
	}
	m_config->Write("Interface/ShowToolbarText", showText ? "true" : "false");
	t = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
	SetToolBar(t);

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
	t->AddTool(ID_NEW_SOURCE, "New", bmNew, "New source file");	

    
	wxBitmap bmOpen(open_xpm);

	if(perms->isEnabled(PERM_LOCALMODE))
	{
		t->AddTool(ID_OPEN_SOURCE_LOCAL, "Open (L)", bmOpen, "Open a file from the local computer");
	}
	
	t->AddTool(ID_OPEN_SOURCE_REMOTE, "Open (R)", bmOpen, "Open a file from the remote server");

	wxBitmap bmSave(save_xpm);
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

		for(int i = ID_DEBUG_IDS_FIRST; i < ID_DEBUG_IDS_LAST; i++)
		{
			t->EnableTool(i, false);
		}
	}

	//int toolbarStyle = t->GetWindowStyle();
/*
	long style = t->GetWindowStyle();
	//style &= ~(wxTB_NOICONS | wxTB_TEXT);

	bool showText = m_options->GetShowToolbarText();
	if(showText)
	{
		if(!(style & wxTB_TEXT))
		{
			style |= wxTB_TEXT;
		}
		
	}	
	else
	{
		if(style & wxTB_TEXT)
		{
			style &= ~wxTB_TEXT;
		}
	}

	t->SetWindowStyle(style);
	*/
	t->Realize();
	

	m_config->Write("Interface/ShowToolbarText", showText ? "true" : "false");

	

}


//////////////////////////////////////////////////////////////////////////////
///  public EvaluateOptions
///  Updates the GUI and saves the config file whenever the user clicks
///  OK in the Options dialog.
///
///  @return void
///
///  @author Mark Erikson @date 03-29-2004
//////// comment generated by Comment Maker from www.FeinSoftware.com /////////
void ChameleonWindow::EvaluateOptions()
{
	// update permissions settings from the options dialog

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

	m_config->Write("Permissions/enabled", perms->getGlobalEnabled());
	m_config->Write("Network/hostname", m_options->GetHostname());
	m_config->Write("Network/username", m_options->GetUsername());
	m_config->Write("Compiler/mingwpath", m_options->GetMingwPath());

	bool printInColor = (m_options->GetPrintStyle() == wxSTC_PRINT_COLOURONWHITE);

	m_config->Write("Miscellaneous/PrintInColor", printInColor ? "true" : "false");

	m_config->Flush();
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
		/*
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
		*/
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

	//int currentSashPos = m_splitEditorOutput->GetSashPosition();
	//if(currentSashPos < m_splitterPos)

}

void ChameleonWindow::OnTermResize(wxSplitterEvent &event)
{
	//m_splitterPos = event.GetSashPosition();
}

void ChameleonWindow::PassImageList(wxImageList* imagelist)
{
	m_projectTree->AssignImageList(imagelist);
}

void ChameleonWindow::OnTreeItemRightClick(wxTreeEvent& event)
{
	//if(!m_bProjectOpen)
	
    
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

		popupMenu.Append(ID_PROJECT_ADDFILE, label);

	}
	// user right-clicked a file in the project
	else if(clickedItem == rootItem)
	{
		if(m_projMultiFiles != NULL)
		{
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

//void ChameleonWindow::OnOpenProjectFile(wxCommandEvent &event)
void ChameleonWindow::OpenProjectFile(bool isRemote)
{	
	m_remoteMode = isRemote;
	wxPathFormat currentPathFormat = m_remoteMode ? wxPATH_UNIX : wxPATH_DOS;
	
	//if(m_bProjectOpen)
	if(m_projMultiFiles != NULL)
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
	//if(m_bProjectOpen)
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

	m_projMultiFiles->SetProjectPath(projectFileName.GetPath(currentPathFormat));
	m_projMultiFiles->SetProjectName(projectFileName.GetFullName());
	m_projMultiFiles->SetRemote(m_remoteMode);

	wxTreeItemId rootItem = m_projectTree->GetRootItem();
	m_projectTree->SetItemText(rootItem, fileName);
	m_projectTree->SetItemBold(rootItem);

	wxMemoryInputStream projectFileStream(fileContents, fileContents.Len());
	
	wxFileConfig config(projectFileStream);


	LoadFilesIntoProjectTree("/Sources", FILE_SOURCES, m_projectFileFolders[0], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Headers", FILE_HEADERS, m_projectFileFolders[1], config, currentPathFormat);
	LoadFilesIntoProjectTree("/Libraries", FILE_LIBRARIES, m_projectFileFolders[2], config, currentPathFormat);

	m_projectTree->Thaw();

	//m_bProjectOpen = true;
}

// TODO Expand this to be two functions: a default version used with the
//		tree and an overload that can be used with a project menu
//void ChameleonWindow::OnAddFileToProject(wxCommandEvent &event)
void ChameleonWindow::AddFileToProject()
{
	//wxLogDebug("event id: %d", event.GetId());
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
	//GetFileContents(fileNames[0], fileContents, fileName);

	m_projMultiFiles->AddFileToProject(fileToOpen, m_projectSelectedFolderType);

	//int iconIndex = m_remoteFileDialog->GetIconIndex(wxFileName(fileToOpen).GetExt());
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
}

void ChameleonWindow::OnTreeItemActivated(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();

	wxTreeItemId rootItem = m_projectTree->GetRootItem();

	if( item != rootItem)
	{
		wxTreeItemId parentItem = m_projectTree->GetItemParent(item);

		if( (parentItem == m_projectFileFolders[0]) ||
			(parentItem == m_projectFileFolders[1]) )
		{
			FileNameTreeData* data = static_cast <FileNameTreeData* > (m_projectTree->GetItemData(item));
			
			wxArrayString filesToOpen;
			//wxFileName remoteFile(m_currentProjectInfo->projectBasePath, data->filename);
			//wxString pathname = remoteFile.GetFullPath( (m_currentProjectInfo->isRemote ? wxPATH_UNIX : wxPATH_DOS));
			wxString pathname = data->filename;
			wxLogDebug("Activated filename: %s", pathname);
			filesToOpen.Add(pathname);

			OpenSourceFile(filesToOpen);
		}
		else if(parentItem == m_projectFileFolders[2])
		{
			wxMessageBox("Only source files can be opened", "Chameleon");
			return;
		}
	}
	m_currentEd->Refresh();
	m_book->Refresh();
}

void ChameleonWindow::CloseProjectFile()
{
	if(!m_appClosing)
	{
		int response = wxMessageBox("Do you want to close all files from this project?", 
									"Close Project", wxYES_NO | wxCANCEL | wxCENTER);

		EditorPointerArray edList = m_projMultiFiles->GetEditors();

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
				ed->SetProject(proj);
			}
		}
		else
		{
			return;
		}

	}
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
	

	wxString projBasePath = m_projMultiFiles->GetProjectBasePath();
	wxString projName = m_projMultiFiles->GetProjectName();
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
	
	m_projectTree->DeleteChildren(m_projectFileFolders[0]);
	m_projectTree->DeleteChildren(m_projectFileFolders[1]);
	m_projectTree->DeleteChildren(m_projectFileFolders[2]);

	m_projectTree->SetItemText(m_projectTree->GetRootItem(), "No project");

	delete m_projMultiFiles;
	m_projMultiFiles = NULL;
	//m_bProjectOpen = false;
}

//////////////////////////////////////////////////////////////////////////////
///  private LoadFilesIntoProjectTree
///  <TODO: insert text here>
///
///  @param  configPath          wxString       <TODO: insert text here>
///  @param  configEntryBaseName wxString       <TODO: insert text here>
///  @param  treeid              wxTreeItemId   <TODO: insert text here>
///  @param  config              wxFileConfig & <TODO: insert text here>
///  @param  currentPathFormat   wxPathFormat   <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 03-29-2004
//////// comment generated by Comment Maker from www.FeinSoftware.com /////////
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
			//newFileName.MakeAbsolute(m_projMultiFiles->projectBasePath);
			int iconNum = m_extensionMappings[newFileName.GetExt()];//m_remoteFileDialog->GetIconIndex(newFileName.GetExt());

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

void ChameleonWindow::Compile()
{
	

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
		
		ProjectInfo* projToCompile = m_currentEd->GetProject();
		m_outputPanel->ClearOutput();

		m_compiler->CompileProject(projToCompile, m_outputPanel);

//		wxFileName editorFile = m_currentEd->GetFileName();
//		//editorFile.SetFullName(m_options->GetRemoteCompileOut());
//		editorFile.SetExt("out");
//		wxString fullpath = editorFile.GetFullPath(m_remoteMode ? wxPATH_UNIX : wxPATH_DOS);
//		//fullpath.Replace("~", m_network->GetHomeDirPath());
//		m_currentEd->SetExecutableFilename(fullpath);

		//Highlight CompilerOutputPanel:
		int outputIndex = m_noteTerm->FindPagePosition(m_outputPanel);
		m_noteTerm->SetSelection(outputIndex);
	}
}

void ChameleonWindow::RemoveFileFromProject()
{
	FileNameTreeData* treeData = static_cast<FileNameTreeData*> (m_projectTree->GetItemData(m_clickedTreeItem));

	if(m_projMultiFiles->FileExistsInProject(treeData->filename))
	{
		m_projMultiFiles->RemoveFileFromProject(treeData->filename, m_projectSelectedFolderType);
		m_projectTree->Delete(m_clickedTreeItem);

		wxFileName fn(treeData->filename);
		int pageNum = GetPageNum(fn);
		
		if(pageNum != -1)
		{
			ChameleonEditor* pEdit = static_cast <ChameleonEditor* >(m_book->GetPage(pageNum));
			ProjectInfo* proj = new ProjectInfo();
			pEdit->SetProject(proj);
		}
	}
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
		m_terminal->Show();
		m_noteTerm->AddPage(m_termContainer, "Terminal");
	}
	else
	{
		m_termContainer->Hide();
		m_terminal->Hide();
	}

	bool advCompileEnabled = perms->isEnabled(PERM_ADVANCEDCOMPILE);
	bool basicCompileEnabled = perms->isEnabled(PERM_COMPILE);

	//int compilerTextboxIdx = m_noteTerm->FindPagePosition(m_compilerTextbox);
	//bool compilerTextboxShown = (compilerTextboxIdx != -1);
	if(perms->isEnabled(PERM_COMPILE))
	{
		m_noteTerm->AddPage(m_outputPanel, "Compiler output");
		m_outputPanel->SetAdvanced(advCompileEnabled);
		m_outputPanel->ClearOutput();
		m_outputPanel->Show();
	}	
	else
	{
		m_outputPanel->Hide();
	}
	

	if(perms->isEnabled(PERM_DEBUG))
	{
		m_noteTerm->AddPage(m_watchPanel, "Watches");
		m_watchPanel->Show();
	}
	else
	{
		m_watchPanel->Hide();
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
			m_splitEditorOutput->SplitHorizontally(m_splitProjectEditor, m_noteTerm, -260);//-200);
			m_splitEditorOutput->SetMinimumPaneSize(20);
			m_terminal->UpdateSize();
			m_noteTerm->Show();	
		}		
	}
	m_noteTerm->Refresh();


	if(perms->isEnabled(PERM_PROJECTS))
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

void ChameleonWindow::OnDebugEvent(wxDebugEvent &event)
{
	int eventID = event.GetId();

	wxLogDebug("Debug event: %d", eventID);

	switch(eventID)
	{
		case ID_DEBUG_EXIT_ERROR:
		case ID_DEBUG_EXIT_NORMAL:
		{
			/*
			wxToolBar* tb = GetToolBar();

			for(int i = ID_DEBUG_IDS_FIRST; i < ID_DEBUG_IDS_LAST; i++)
			{
				tb->EnableTool(i, false);
			}
			*/

			// This is an absurdly naive way to handle removing the focused line
			// marker, but it's easy and shouldn't take up much time
			int numPages = m_book->GetPageCount();
			for(int i = 0; i < numPages; i++)
			{
				ChameleonEditor* pEdit = static_cast <ChameleonEditor* >(m_book->GetPage(i));
				pEdit->MarkerDeleteAll(MARKER_FOCUSEDLINE);
			}
			wxLogDebug("Debugger exit");

			break;
		}
		case ID_DEBUG_BREAKPOINT:
		{	
			//wxArrayString sourceFiles = event.GetSourceFilenames();

			wxString file = event.GetSourceFilename();//sourceFiles[0];

			int linenumber = event.GetLineNumber();

			wxLogDebug("File: \"%s\", line: %d", file, linenumber);
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
				//wxArrayString sources = event.GetSourceFilenames();
				wxString filename = event.GetSourceFilename();//sources[0];

				//filename.Replace("~", m_network->GetHomeDirPath());

				//sources.Clear();
				//sources.Add(filename);
				event.SetSourceFilename(filename);
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
		default:
			wxLogDebug("Default DebugEvent.  Value: %d", eventID);
			break;
	}
}

void ChameleonWindow::OnUpdateDebugUI(wxUpdateUIEvent &event)
{
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

		// TODO Better determination (project-based is needed too)
		// TODO The debugger still isn't responding properly... status issues?
		bool canStartDebug = !m_currentEd->Modified() && 
							m_currentEd->HasBeenCompiled() &&
							m_currentEd->HasBeenSaved();
		tb->EnableTool(ID_DEBUG_START, canStartDebug);
	}
}

void ChameleonWindow::OnUpdateConnectionUI(wxUpdateUIEvent &event)
{
	bool termConnected = m_terminal->IsConnected();

	wxToolBar* tb = GetToolBar();
	tb->EnableTool(ID_STARTCONNECT, !termConnected);
	tb->EnableTool(ID_DISCONNECT, termConnected);

	wxMenuBar* mb = GetMenuBar();
	wxMenu* toolsMenu = mb->GetMenu(mb->FindMenu("Tools"));

	toolsMenu->Enable(ID_STARTCONNECT, !termConnected);
	toolsMenu->Enable(ID_DISCONNECT, termConnected);
	
}

void ChameleonWindow::OnUpdateCompileUI(wxUpdateUIEvent &event)
{
	bool canCompile = false;
	ProjectInfo* edProj = m_currentEd->GetProject();

	
	wxToolBar* tb = GetToolBar();
	/*
	wxToolBarToolBase* compileButton = tb->FindById(ID_COMPILE);
	
	bool isCompiling = m_compiler->IsCompiling();

	wxBitmap bmBuild(build_xpm);

	bool currentProjCompiling = isCompiling && edProj->IsBeingCompiled();

	if(currentProjCompiling)
	{
		wxBitmap bmStopCompile(stop1_xpm);
		compileButton->SetNormalBitmap(bmStopCompile);
		compileButton->SetLabel("Cancel");		
	}
	else
	{
		compileButton->SetNormalBitmap(bmBuild);
		compileButton->SetLabel("Compile");
	}

	bool enableButton = currentProjCompiling || !isCompiling;
	*/

	//tb->EnableTool(ID_COMPILE, enableButton);
	tb->EnableTool(ID_COMPILE, !edProj->IsCompiled() && !edProj->IsBeingCompiled());
}

void ChameleonWindow::OnStatusTimer(wxTimerEvent &WXUNUSED(event)) 
{
	if (m_updateTimer)
	{
		m_updateTimer->Stop();
	}
}

void ChameleonWindow::OnIdle(wxIdleEvent &event)
{
	if (m_updateTimer && !m_updateTimer->IsRunning ()) 
	{
		m_updateTimer->Start (100, wxTIMER_ONE_SHOT);
		UpdateStatusBar();
	}
	event.Skip();
	
}

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

	wxRegEx reModeTest = "\\((R|L|\\?)\\) ";

	if(reModeTest.Matches(tabText))
	{
		wxString modeString = reModeTest.GetMatch(tabText, 1);

		if( (modeString == "R") ||
			(modeString == "L") )
		{
			filename = m_currentEd->GetFileNameAndPath();
			bool isRemote = (modeString == "R");
			location = isRemote ? "Remote" : "Local";
		}
		else if(modeString == "?")
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
	}

	bool isEdReadOnly = m_currentEd->GetProject()->IsReadOnly();

	wxString editable = isEdReadOnly ? "Read-only" : "Read-write";

	if(editable != m_statusBar->GetStatusText(2))
	{
		m_statusBar->SetStatusText(editable, 2);
	}

	//m_statusBar->SetStatusText("Waiting for network response...", 3);

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
///  private FocusOnLine
///  <TODO: insert text here>
///
///  @param  filename     wxString  <TODO: insert text here>
///  @param  linenumber   int       <TODO: insert text here>
///  @param  linecontents wxString  [=wxEmptyString] <TODO: insert text here>
///
///  @return void
///
///  @author www.FeinSoftware.com @date 03-29-2004
//////// comment generated by Comment Maker from www.FeinSoftware.com /////////
void ChameleonWindow::FocusOnLine(wxString filename, int linenumber, bool showMarker, wxString linecontents /* = wxEmptyString */)
{
	wxFileName fn(filename);

	int tabNum = GetPageNum(fn, false);

	if(tabNum != -1)
	{
		PageHasChanged(tabNum);

		// Adjust for Scintilla's zero-based line numbers
		int adjustedLineNum = linenumber - 1;

		if(linecontents != wxEmptyString)
		{
			wxString edLineContents = m_currentEd->GetLine(adjustedLineNum);
		}

		m_currentEd->FocusOnLine(adjustedLineNum);
	}
}

bool ChameleonWindow::IsDebugging()
{
	return m_debugger->isDebugging();
}

bool ChameleonWindow::IsDebuggerPaused()
{
	return m_debugger->isPaused();
}

void ChameleonWindow::OnUpdatePrintPreviewUI (wxUpdateUIEvent &event) 
{
	event.Enable ((m_book->GetPageCount() > 1) ||
		(m_currentEd && (m_currentEd->GetLength() > 0)));
}

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

		//        FIXME - find no more occurances is annoying
		//if (pos < 0)
		//{
		           
		//}

		if (pos >= 0)
			m_currentEd->SetFocus();
		else
		{
			wxBell();                 // bell ok to signify no more occurances?

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