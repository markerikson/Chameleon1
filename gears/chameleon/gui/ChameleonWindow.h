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



#include <wx/wfstream.h>
#include <wx/frame.h>
#include <wx/toolbar.h>
#include <wx/bitmap.h>
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <wx/splitter.h>
#include <wx/grid.h>
#include <wx/dynarray.h>
#include <wx/notebook.h>
#include <wx/tabctrl.h>
#include <wx/log.h>
#include <wx/filename.h>
#include <wx/socket.h>
#include <wx/checklst.h>

#include "new.xpm"
#include "open.xpm"
#include "save.xpm"
#include "build.xpm"
#include "button.xpm"

#include "start.xpm"
#include "stop.xpm"
#include "stepnext.xpm"
#include "stepout.xpm"
#include "stepover.xpm"
#include "pause.xpm"

#include "moz.xpm"
//#include "mbrowser.xpm"


//#include "stc.h"
class wxGridCellEditorEvtHandler;
#include "../editor/editor.h"
#include "updateuihandler.h"
#include "../common/datastructures.h"
#include "wxtelnet.h"
#include "dialogs/OptionsDialog.h"
#include "dialogs/RemoteFileDialog.h"
//#include "GridCellEditorEvtHandler.h"
//#include "GridCellCheckboxRenderer.h"
#include "../perms/p.h"

#include "ChameleonNotebook.h"

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

	bool IsEnabled(int permission);

	void EvaluateOptions();



private:
	void CheckSize();
	void ResizeSplitter();

	void CloseApp();


	void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnSave(wxCommandEvent &event);
	void OnSaveAs(wxCommandEvent &event);
	void SaveFileLocal(bool saveas);
	void OpenFile(wxCommandEvent& event);
	void Test(wxCommandEvent& event);
	//void SaveFileAs(wxCommandEvent& event);
	void OnUpdateSave(wxUpdateUIEvent &event);
	void OnUndo(wxCommandEvent &event);
	void OnRedo(wxCommandEvent &event);
	void OnCloseWindow(wxCloseEvent& event);
	void OnConnect(wxCommandEvent &event);
	void OnToolsOptions(wxCommandEvent &event);

	void OnGridClicked(wxGridEvent& event);

	void OnPageChange (wxNotebookEvent &event);
	void OnPageClose(wxCommandEvent& event);
	

	// file manipulation functions
	void OnFileNew (wxCommandEvent &event);
	void OnFileOpen (wxCommandEvent &event);
	void OnFileSave (wxCommandEvent &event);
	void OnFileSaveAs (wxCommandEvent &event);
	void OnFileClose (wxCommandEvent &event);
	void OnFileRecents (wxCommandEvent &event);

	// edit menu functions
	void OnCut(wxCommandEvent &event);
	void OnCopy(wxCommandEvent &event);
	void OnPaste(wxCommandEvent &event);



	void OpenFile(wxArrayString fnames);
	void CloseFile(int pageNr = -1);
	void CloseTab();
	void PageHasChanged (int pageNr = -1);
	int GetPageNum(const wxString& fname);

	void AddDebugButtons();
	void InitializeOptionsDialog();

	int* TargetInt(int variableName);


	//wxStyledTextCtrl* ed;
	//wxNotebook* noteEd;
	//wxNotebook* noteTerm;

	wxLogWindow* logWindow;

	ChameleonNotebook* m_book;
	ChameleonNotebook*  m_noteTerm;
	ChameleonEditor* m_currentEd;
	wxTabCtrl* tc;
	wxPanel* panelEd;
	wxSplitterWindow*  m_split;
	//wxTextCtrl* textbox;
	UpdateUIHandler*  uih;
	wxTelnet*  m_telnet;
	OptionsDialog*  m_optionsDialog;
	RemoteFileDialog* m_remoteFiles;
	wxGrid* m_optGrid;


	IntIntHashmap m_permNumMap;
	//wxPanel* tcPanel1;

	//wxBoxSizer* sizerPanel;
	//wxBoxSizer* sizerTab;

	Permission*  m_perms;

	//wxArrayPtrVoid* docArray; 
	//scintillaDocPageHash* docHash;

	int m_numPages;
	int m_currentPage;
	int m_fileNum;
	int m_clickedTabNum;

	bool m_appClosing;
	bool m_setSelection;


	//ChameleonEditor* m_edit;

	wxArrayString* m_openFiles;


	wxString saveFileName;

	DECLARE_EVENT_TABLE()
};



// IDs for the controls and the menu commands


/*
enum Permissions
{
	PERM_1,
	PERM_2,
};
*/


#endif