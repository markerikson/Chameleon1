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

#include "new.xpm"
#include "open.xpm"
#include "save.xpm"
#include "build.xpm"
#include "button.xpm"

//#include "stc.h"
#include "../editor/editor.h"
#include "updateuihandler.h"

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




private:
	void CheckSize();
	void ResizeSplitter();


	void OnQuit(wxCommandEvent& event);
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



	//wxStyledTextCtrl* ed;
	ChameleonEditor* ed;
	wxSplitterWindow* split;
	wxTextCtrl* textbox;
	UpdateUIHandler* uih;

	wxString saveFileName;

	DECLARE_EVENT_TABLE()
};


// IDs for the controls and the menu commands
enum
{
	// menu items
	ID_QUIT = 5200,

	ID_NEW,
	ID_ABOUT,
	ID_OPEN,
	ID_SAVE,


	ID_ED,

	ID_UNDO,
	ID_REDO,

	ID_COPY,
	ID_CUT,
	ID_PASTE,

	ID_COMPILE,
	ID_TEST,
};

BEGIN_EVENT_TABLE(ChameleonWindow, wxFrame)
EVT_MENU			(ID_OPEN, ChameleonWindow::OpenFile)
EVT_MENU            (ID_QUIT,  ChameleonWindow::OnQuit)
EVT_MENU            (ID_ABOUT, ChameleonWindow::OnAbout)
EVT_MENU			(ID_TEST, ChameleonWindow::Test)
EVT_MENU			(ID_SAVE, ChameleonWindow::OnSave)
//
//EVT_SIZE			(ChameleonWindow::ResizeSplitter)
EVT_UPDATE_UI		(ID_SAVE, ChameleonWindow::OnUpdateSave)

EVT_MENU			(ID_UNDO, ChameleonWindow::OnUndo)
EVT_MENU			(ID_REDO, ChameleonWindow::OnRedo)
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)
