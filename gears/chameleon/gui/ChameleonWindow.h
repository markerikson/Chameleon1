#ifndef CHAMELEONWINDOW__H
#define CHAMELEONWINDOW__H


/////////////////////////////////////////////////////////////////////////////
// Name:        stctest.cpp
// Purpose:     sample of using wxStyledTextCtrl
// Author:      Robin Dunn
// Modified by:
// Created:     3-Feb-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "stctest.cpp"
#pragma interface "stctest.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../common/datastructures.h"
#include "../common/DebugEvent.h"
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

//----------------------------------------------------------------------

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

//----------------------------------------------------------------------





// Define a new frame type: this is going to be our main frame
class ChameleonWindow : public wxFrame
{
public:
	ChameleonWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
	~ChameleonWindow();


	//void SetNumPages(int newnum);
	void SetIntVar(int variableName, int value);
	int GetIntVar(int variableName);

	//void SetBoolVar(int variableName, bool value);
	//bool GetBoolVar(int variableName);

	bool IsEnabled(int permission);
	bool InRemoteMode();
	bool IsDebugging();
	bool IsDebuggerPaused();

	void EvaluateOptions();
	bool UpdateAuthCode();

	NetworkCallResult CheckNetworkStatus();

	void OpenSourceFile(wxArrayString fnames);

	//ChameleonEditor* GetCurrentEditor();
	Networking* GetNetworking();

	void PassImageList(wxImageList* imagelist);



private:
	void CheckSize();
	void ResizeSplitter();

	void CloseApp();


	void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnCloseProject(wxCommandEvent &event);
	void OnAbout(wxCommandEvent& event);
	void OnSave(wxCommandEvent &event);
	void OnSaveAs(wxCommandEvent &event);
	//void SaveFileLocal(bool saveas);
	wxArrayString OpenFile(FileFilterType filterType );
	bool SaveFile(bool saveas, bool askLocalRemote, FileFilterType filterType);
	void Test(wxCommandEvent& event);
	//void SaveFileAs(wxCommandEvent& event);
	void OnUpdateSave(wxUpdateUIEvent &event);
	void OnUpdateDebug(wxUpdateUIEvent &event);
	void OnUpdateConnection(wxUpdateUIEvent &event);
	void OnUpdateCompile(wxUpdateUIEvent &event);
	void OnUndo(wxCommandEvent &event);
	void OnRedo(wxCommandEvent &event);
	void OnCloseWindow(wxCloseEvent& event);
	void OnConnect(wxCommandEvent &event);
	void OnDisconnect(wxCommandEvent &event);
	void OnToolsOptions(wxCommandEvent &event);
	void OnCompile(wxCommandEvent &event);
	void OnSplitterDoubleClick(wxSplitterEvent &event);
	void OnSize(wxSizeEvent &event);

	void OnTreeItemRightClick(wxTreeEvent& event);
	void OnTreeItemActivated(wxTreeEvent &event);
	void OnOpenProjectFile(wxCommandEvent &event);
	void OnAddFileToProject(wxCommandEvent &event);
	void OnRemoveFileFromProject(wxCommandEvent &event);

	void OnPageChange (wxNotebookEvent &event);
	void OnPageClose(wxCommandEvent& event);
	

	// file manipulation functions
	void OnFileNew (wxCommandEvent &event);
	void OnFileOpen (wxCommandEvent &event);
	void OnFileSave (wxCommandEvent &event);
	void OnFileSaveAs (wxCommandEvent &event);
	void OnFileClose (wxCommandEvent &event);
	void OnFileCloseAll(wxCommandEvent &event);
	void OnFileNewProject(wxCommandEvent &event);
	void OnFileRecents (wxCommandEvent &event);

	void OnPrint(wxCommandEvent &event);
	void OnPrintSetup(wxCommandEvent &event);
	void OnPrintPreview(wxCommandEvent &event);
	void OnPrintPreviewUI(wxUpdateUIEvent &event);

	void OnDebugCommand(wxCommandEvent &event);
	void OnDebugEvent(wxDebugEvent &event);

	// edit menu functions
	void OnCut(wxCommandEvent &event);
	void OnCopy(wxCommandEvent &event);
	void OnPaste(wxCommandEvent &event);

	bool GetFileContents(wxString fileToLoad, wxString &fileContents, wxString &fileName);

	
	void CloseFile(int pageNr = -1);
	void CloseAllFiles();
	void CloseProjectFile();
	int HandleModifiedFile(int pageNr, bool closingFile); 
	void LoadFilesIntoProjectTree(wxString configPath, FileFilterType fileType, wxTreeItemId treeid, 
								wxFileConfig& config, wxPathFormat currentPathFormat);
	void CloseTab();
	void PageHasChanged (int pageNr = -1);
	wxRect DeterminePrintSize();
	//void PrintSetup();
	int GetPageNum(wxFileName fn, bool compareWholePath = true, int startingTab = 0);
	wxString ConstructFilterString(FileFilterType filterType);
	void FocusOnLine(wxString filename, int linenumber, wxString linecontents = wxEmptyString);

	void UpdateMenuBar();
	void UpdateToolbar();
	void UpdatePermsList();
	void UpdateTerminalNotebook();

	int* SelectIntVar(int variableName);
	//bool* SelectBoolVar(int variableName);


	//wxStyledTextCtrl* ed;
	//wxNotebook* noteEd;
	//wxNotebook* noteTerm;

	wxLogWindow* logWindow;

	ChameleonNotebook* m_book;
	ChameleonNotebook*  m_noteTerm;
	ChameleonEditor* m_currentEd;

	wxTreeCtrl* m_projectTree;
	wxStatusBar* m_statusBar;
	wxPanel* panelEd;
	wxSplitterWindow*  m_splitEditorOutput;
	wxSplitterWindow* m_splitProjectEditor;
	wxTextCtrl* m_compilerTextbox;
	//UpdateUIHandler*  uih;
	//wxTelnet*  m_telnet;
	wxTermContainer* m_termContainer;
	VariableWatchPanel* m_watchPanel;
	wxSSH* m_terminal;

	OptionsDialog*  m_optionsDialog;
	RemoteFileDialog* m_remoteFileDialog;
	//wxGrid* m_optGrid;

	//wxIniConfig* m_config;
	wxFileConfig* m_config;
	wxImageList* m_tempImageList;


	IntIntHashmap m_permNumMap;
	WindowPointerArray m_infoTabTracker;

	wxTreeItemId m_projectFileFolders[3];
	wxTreeItemId m_clickedTreeItem;
	FileFilterType m_projectSelectedFolderType;

	//wxMenu m_projectPopupMenu;
	//wxPanel* tcPanel1;

	//wxBoxSizer* sizerPanel;
	//wxBoxSizer* sizerTab;

	//Permission*  m_perms;
	Networking* m_network;
	Compiler* m_compiler;
	Options* m_options;
	//ProjectInfo* m_currentProjectInfo;
	ProjectInfo* m_projMultiFiles;
	Debugger* m_debugger;

	//wxArrayPtrVoid* docArray; 
	//scintillaDocPageHash* docHash;

	int m_numPages;
	int m_currentPage;
	int m_fileNum;
	int m_clickedTabNum;
	

	bool m_appClosing;
	bool m_setSelection;
	bool m_remoteMode;
	bool m_appStarting;
	bool m_bProjectOpen;
	bool m_currentlyDebugging;


	//ChameleonEditor* m_edit;

	wxArrayString m_compilationSources;

	wxString m_filterCPPFiles;
	wxString m_filterHeaderFiles;
	wxString m_filterProjectFiles;
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