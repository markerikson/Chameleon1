/////////////////////////////////////////////////////////////////////////////
// Name:        VariableWatchPanel.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     03/29/04 21:43:35
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _VARIABLEWATCHPANEL_H_
#define _VARIABLEWATCHPANEL_H_

#ifdef __GNUG__
#pragma interface "VariableWatchPanel.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/wx.h"
#include "wx/listctrl.h"
////@end includes

#include "../../common/DebugEvent.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxListCtrl;
////@end forward declarations
class ChameleonWindow;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_VARWATCHDIALOG 10000
#define SYMBOL_VARIABLEWATCHPANEL_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_VARIABLEWATCHPANEL_TITLE _("VariableWatch")
#define SYMBOL_VARIABLEWATCHPANEL_IDNAME ID_VARWATCHDIALOG
#define SYMBOL_VARIABLEWATCHPANEL_SIZE wxDefaultSize
#define SYMBOL_VARIABLEWATCHPANEL_POSITION wxPoint(0, 0)
#define ID_LISTCTRL 10001
#define ID_ADDWATCH 10002
#define ID_REMOVEWATCH 10003
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * VariableWatchPanel class declaration
 */

class VariableWatchPanel: public wxPanel
{    
    DECLARE_CLASS( VariableWatchPanel )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    VariableWatchPanel( );
    VariableWatchPanel( wxWindow* parent, ChameleonWindow* mainframe, wxWindowID id = SYMBOL_VARIABLEWATCHPANEL_IDNAME, const wxPoint& pos = SYMBOL_VARIABLEWATCHPANEL_POSITION, const wxSize& size = SYMBOL_VARIABLEWATCHPANEL_SIZE, long style = SYMBOL_VARIABLEWATCHPANEL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_VARIABLEWATCHPANEL_IDNAME, const wxPoint& pos = SYMBOL_VARIABLEWATCHPANEL_POSITION, const wxSize& size = SYMBOL_VARIABLEWATCHPANEL_SIZE, long style = SYMBOL_VARIABLEWATCHPANEL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin VariableWatchPanel event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADDWATCH
    void OnAddwatchClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_REMOVEWATCH
    void OnRemovewatchClick( wxCommandEvent& event );

////@end VariableWatchPanel event handler declarations

////@begin VariableWatchPanel member function declarations

////@end VariableWatchPanel member function declarations

	void UpdateVariableInfo(wxDebugEvent event);
    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin VariableWatchPanel member variables
    wxListCtrl* m_list;
////@end VariableWatchPanel member variables
	ChameleonWindow* m_mainFrame;
};

#endif
    // _VARIABLEWATCHPANEL_H_
