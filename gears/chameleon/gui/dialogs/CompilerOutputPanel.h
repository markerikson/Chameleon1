/////////////////////////////////////////////////////////////////////////////
// Name:        CompilerOutputPanel.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     04/12/04 19:39:08
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _COMPILEROUTPUTPANEL_H_
#define _COMPILEROUTPUTPANEL_H_

#ifdef __GNUG__
#pragma interface "CompilerOutputPanel.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/wx.h"
#include "wx/grid.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define SYMBOL_COMPILEROUTPUTPANEL_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_COMPILEROUTPUTPANEL_TITLE _("CompilerOutputPanel")
#define SYMBOL_COMPILEROUTPUTPANEL_IDNAME ID_DIALOG
#define SYMBOL_COMPILEROUTPUTPANEL_SIZE wxSize(400, 300)
#define SYMBOL_COMPILEROUTPUTPANEL_POSITION wxPoint(0, 0)
#define ID_COMPILERTEXT 10001
#define ID_COMPILERGRID 10002
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

class CompilerEvent;
class wxGridEvent;
class ChameleonWindow;

/*!
 * CompilerOutputPanel class declaration
 */

class CompilerOutputPanel: public wxPanel
{    
    DECLARE_CLASS( CompilerOutputPanel )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CompilerOutputPanel( );
    CompilerOutputPanel( wxWindow* parent, ChameleonWindow* mainFrame, wxWindowID id = SYMBOL_COMPILEROUTPUTPANEL_IDNAME, const wxString& caption = SYMBOL_COMPILEROUTPUTPANEL_TITLE, const wxPoint& pos = SYMBOL_COMPILEROUTPUTPANEL_POSITION, const wxSize& size = SYMBOL_COMPILEROUTPUTPANEL_SIZE, long style = SYMBOL_COMPILEROUTPUTPANEL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_COMPILEROUTPUTPANEL_IDNAME, const wxString& caption = SYMBOL_COMPILEROUTPUTPANEL_TITLE, const wxPoint& pos = SYMBOL_COMPILEROUTPUTPANEL_POSITION, const wxSize& size = SYMBOL_COMPILEROUTPUTPANEL_SIZE, long style = SYMBOL_COMPILEROUTPUTPANEL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin CompilerOutputPanel event handler declarations

////@end CompilerOutputPanel event handler declarations

////@begin CompilerOutputPanel member function declarations

////@end CompilerOutputPanel member function declarations

	virtual void SetFocus();

	bool IsAdvanced() { return m_isAdvanced; }

	void Stuff(wxWindow* parent, ChameleonWindow* mainFrame, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style );

	void ClearOutput();
	void SetAdvanced(bool advanced);

	void OnCompilerStart(CompilerEvent& event);
	void OnCompilerProblem(CompilerEvent &event);
	void OnCompilerEnd(CompilerEvent &event);
	void OnGridDoubleClick(wxGridEvent &event);

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin CompilerOutputPanel member variables
    wxBoxSizer* m_sizer;
    wxTextCtrl* m_textbox;
    wxGrid* m_grid;
////@end CompilerOutputPanel member variables

	ChameleonWindow* m_mainFrame;

	bool m_isAdvanced;
	int m_numErrors;
};

#endif
    // _COMPILEROUTPUTPANEL_H_
