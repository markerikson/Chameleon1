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


//----------------------------------------------------------------------

class MyApp : public wxApp
{
public:
	~MyApp();
	virtual bool OnInit();
	virtual int OnRun();
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
	void ResizeSplitter();

	void OnMenuEvent(wxCommandEvent &event);
	void OnDebugCommand(wxCommandEvent &event);

	void OnDebugEvent(wxDebugEvent &event);

	void OnClose(wxCloseEvent& event);
	void Test(wxCommandEvent& event);

	void OnUpdateSaveUI(wxUpdateUIEvent &event);
	void OnUpdateDebugUI(wxUpdateUIEvent &event);
	void OnUpdateConnectionUI(wxUpdateUIEvent &event);
	void OnUpdateCompileUI(wxUpdateUIEvent &event);
	void OnUpdatePrintPreviewUI(wxUpdateUIEvent &event);	
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
	int HandleModifiedFile(int pageNr, bool closingFile); 

	void OpenProjectFile(bool isRemote);
	void AddFileToProject();
	void RemoveFileFromProject();
	void CloseProjectFile();
	void LoadFilesIntoProjectTree(wxString configPath, FileFilterType fileType, wxTreeItemId treeid, 
								wxFileConfig& config, wxPathFormat currentPathFormat);

	void Compile();	
	
	wxRect DeterminePrintSize();
	
	wxString ConstructFilterString(FileFilterType filterType);
	int FindString(const wxString &findString, int start_pos = -1, int flags = -1, bool highlight = TRUE);
	int ReplaceAllStrings(const wxString &findString, const wxString &replaceString, int flags = -1);

	void EvaluateOptions();

	void UpdateMenuBar();
	void UpdateToolbar();
	void UpdateTerminalNotebook();

	int* SelectIntVar(int variableName);

	// debug log window
	wxLogWindow* logWindow;

	// main widgets
	ChameleonNotebook* m_book;
	ChameleonNotebook*  m_noteTerm;
	ChameleonEditor* m_currentEd;
	wxTreeCtrl* m_projectTree;
	wxStatusBar* m_statusBar;
	wxPanel* panelEd;
	wxSplitterWindow*  m_splitEditorOutput;
	wxSplitterWindow* m_splitProjectEditor;
	//wxTextCtrl* m_compilerTextbox;
	//wxListCtrl* m_compilerList;
	CompilerOutputPanel* m_outputPanel;
	wxTermContainer* m_termContainer;
	VariableWatchPanel* m_watchPanel;
	wxSSH* m_terminal;

	// dialogs
	OptionsDialog*  m_optionsDialog;
	RemoteFileDialog* m_remoteFileDialog;
	wxFindReplaceDialog *m_findReplace;

	wxFileConfig* m_config;
	wxImageList* m_tempImageList;
	wxTimer* m_updateTimer;

	IntIntHashmap m_permNumMap;
	StringIntHashmap m_extensionMappings;
	WindowPointerArray m_infoTabTracker;

	wxFindReplaceData m_findData;

	wxTreeItemId m_projectFileFolders[3];
	wxTreeItemId m_clickedTreeItem;
	FileFilterType m_projectSelectedFolderType;

	Networking* m_network;
	Compiler* m_compiler;
	Options* m_options;
	ProjectInfo* m_projMultiFiles;
	Debugger* m_debugger;

	int m_numPages;
	int m_currentPage;
	int m_fileNum;
	int m_clickedTabNum;
	int m_splitterPos;

	bool m_appClosing;
	bool m_setSelection;
	bool m_remoteMode;
	bool m_appStarting;
	bool m_currentlyDebugging;

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