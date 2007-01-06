#ifndef CHAMELEONWINDOW__H
#define CHAMELEONWINDOW__H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
//#include "wx/wx.h"
#endif

#include "../common/datastructures.h"
#include "../common/DebugEvent.h"
#include <wx/fdrepdlg.h>
#include <wx/treectrl.h>

// forward declarations

class wxFileConfig;
class ChameleonNotebook;
class wxTreeCtrl;
class wxTextCtrl;
class ChameleonEditor;
class wxLogWindow;
class wxTabCtrl;
class IntIntHashmap;
class WindowPointerArray;
class wxPanel;
class wxSplitterWindow;
class RemoteFileDialog;
class OptionsDialog;
class Permission;
class Networking;
class Compiler;
class wxFileConfig;
class wxTelnet;
class wxMenu;
class wxTreeItemId;
class wxImageList;
class wxNotebookEvent;
class wxSplitterEvent;
class wxTreeEvent;
class wxStatusBar;
class ProjectInfo;
class wxSSH;
class Options;
class wxTermContainer;
class Debugger;
class VariableWatchPanel;
class wxTimer;
class wxListCtrl;
class CompilerOutputPanel;
class IconManager;
class wxProportionalSplitterWindow;


//----------------------------------------------------------------------

class MyApp : public wxApp
{
public:
	~MyApp();
	virtual bool OnInit();
	//virtual int OnRun();
	//virtual void OnUnhandledException(); 
};

//----------------------------------------------------------------------





// Define a new frame type: this is going to be our main frame
class ChameleonWindow : public wxFrame
{
public:
	ChameleonWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
	~ChameleonWindow();

	void SetIntVar(int variableName, int value);
	int GetIntVar(int variableName);

	bool IsEnabled(int permission);
	bool InRemoteMode();
	bool IsDebugging();
	bool IsDebuggerPaused();

	void FocusOnLine(wxString filename, int linenumber, bool showMarker = true, wxString linecontents = wxEmptyString);

	NetworkCallResult CheckNetworkStatus();

	void OpenSourceFile(wxArrayString fnames);

	Networking* GetNetworking();

	void PassImageList(wxImageList* imagelist);

private:
	void CheckSize();

	void OnMenuEvent(wxCommandEvent &event);

	void OnHelp();
	void OnPrintSetup();
	void OnPrintPreview();
	void OnPrintPage();
	void OnProjectIncludeExcludeFile( int id );
	void OnOptions();
	void OnFindReplace(int id );
	void OnCopy();
	void OnStartConnect();
	void OnSaveSourceFile( int id );
	void OnOpenSourceFile( int id );
	void OnDebugCommand(wxCommandEvent &event);
	void OnDebugBreakpointCommand(wxCommandEvent &event);

	void OnDebugEvent(wxDebugEvent &event);

	void OnClose(wxCloseEvent& event);
	void Test(wxCommandEvent& event);

	void OnUpdateSaveUI();//wxUpdateUIEvent &event);
	void OnUpdateDebugUI();//wxUpdateUIEvent &event);
	void OnUpdateConnectionUI();//wxUpdateUIEvent &event);
	void OnUpdateCompileUI();//wxUpdateUIEvent &event);
	void OnUpdateProjectUI();//wxUpdateUIEvent &event);	
	void OnIdle(wxIdleEvent &event);
	void UpdateStatusBar();
	
	void OnStatusTimer(wxTimerEvent &event);

	void OnSplitterDoubleClick(wxSplitterEvent &event);
	void OnTermResize(wxSplitterEvent &event);
	void OnSize(wxSizeEvent &event);	

	void OnTreeItemRightClick(wxTreeEvent& event);
	void OnTreeItemActivated(wxTreeEvent &event);

	void OnPageChange (wxNotebookEvent &event);

	void OnFindEvent(wxFindDialogEvent& event);

	void NewFile();
	wxArrayString OpenFile(FileFilterType filterType );
	bool SaveFile(bool saveas, bool askLocalRemote, FileFilterType filterType);
	bool GetFileContents(wxString fileToLoad, wxString &fileContents, wxString &fileName);
	void CloseFile(int pageNr = -1);
	void CloseAllFiles();
	void CloseTab();
	void PageHasChanged (int pageNr = -1);
	int GetPageNum(wxFileName fn, bool compareWholePath = true, int startingTab = 0);
	int HandleModifiedFile(int pageNr, ModifiedFileAction fileAction); 

	void OpenProjectFile(bool isRemote);
	void AddFileToProject();
	void RemoveFileFromProject();
	void SaveProjectFile();
	void CloseProjectFile(bool canUserCancel = true);
	void LoadFilesIntoProjectTree(wxString configPath, FileFilterType fileType, wxTreeItemId treeid, 
								wxFileConfig& config, wxPathFormat currentPathFormat);

	void Compile();	
	
	wxRect DeterminePrintSize();
	
	wxString ConstructFilterString(FileFilterType filterType);
	int FindString(const wxString &findString, int start_pos = -1, int flags = -1, bool highlight = TRUE);
	int ReplaceAllStrings(const wxString &findString, const wxString &replaceString, int flags = -1);

	void EvaluateOptions();

	bool CheckForBlankPassword();
	bool AskUserForPassword();

	void UpdateMenuBar();
	void UpdateToolbar();
	void UpdateTerminalNotebook();

	//void CleanupDropMenu();

	int* SelectIntVar(int variableName);

	/*! Displays the output from wxLogDebug calls */
	wxLogWindow* logWindow;

	// main widgets
	/*! Main editor notebook */
	ChameleonNotebook* m_book;
	/*! Notebook for the terminal and debug-related widgets */
	ChameleonNotebook*  m_noteTerm;
	/*! Pointer to the currently active editor */
	ChameleonEditor* m_currentEd;
	/*! Displays the files in the current project */
	wxTreeCtrl* m_projectTree;
	/*! The status bar */
	wxStatusBar* m_statusBar;
	//wxPanel* panelEd;
	/*! Holds the editor/project group and the output notebook */
	wxProportionalSplitterWindow*  m_splitEditorOutput;
	/*! Holds the project tree and the editor notebook */
	wxSplitterWindow* m_splitProjectEditor;
	/*! Displays the output from the compiler */
	CompilerOutputPanel* m_outputPanel;
	/*! The main terminal widget container*/
	wxTermContainer* m_termContainer;
	/*! The debug I/O widget container*/
	wxTermContainer* m_debugTermContainer;
	/*! Displays watched variables */
	VariableWatchPanel* m_watchPanel;
	/*! The actual terminal widget */
	wxSSH* m_terminal;
	/*! The actual debug I/O widget */
	wxSSH* m_debugTerminal;

	// dialogs
	OptionsDialog*  m_optionsDialog;
	RemoteFileDialog* m_remoteFileDialog;
	wxFindReplaceDialog *m_findReplace;

	/* Responsible for writing the Chameleon settings to the INI file */
	wxFileConfig* m_config;

	//wxImageList* m_tempImageList;
	/*! Used to initiate UI updates */
	wxTimer* m_updateTimer;

	//IntIntHashmap m_permNumMap;
	/*! Maps a file extension to the image list index for that extension's icon */
	StringIntHashmap m_extensionMappings;
	//WindowPointerArray m_infoTabTracker;

	/*! Used to hold data for the find/replace dialogs */
	wxFindReplaceData m_findData;

	/*! The IDs for the file type folders in the project tree */
	wxTreeItemId m_projectFileFolders[4];
	/*! The last clicked item in the project tree */
	wxTreeItemId m_clickedTreeItem;
	/*! The file type that corresponds to the last clicked project folder */
	FileFilterType m_projectSelectedFolderType;

	Networking* m_network;
	Compiler* m_compiler;
	Options* m_options;
	/*! The currently open multi-file project */
	ProjectInfo* m_projMultiFiles;
	Debugger* m_debugger;
	IconManager* m_iconManager;

	int m_numPages;
	/*! Tracks the active editor tab */
	int m_currentPage;
	/*! Tracks the number of new files created - used to identify <untitled> files */
	int m_fileNum;
	/*! The last clicked tab in the editor notebook */
	int m_clickedTabNum;
	int m_splitterPos;

	bool m_appClosing;
	bool m_setSelection;
	bool m_remoteMode;
	bool m_appStarting;
	bool m_currentlyDebugging;
	bool m_compileProject;

	wxString m_filterCPPFiles;
	wxString m_filterHeaderFiles;
	wxString m_filterProjectFiles;
	wxString m_filterLibraryFiles;
	wxString m_filterAllFiles;
	

	DECLARE_EVENT_TABLE()
	
	
};

class wxTreeItemData;

class FileNameTreeData : public wxTreeItemData
{
public:
	wxString filename;
};


#endif
