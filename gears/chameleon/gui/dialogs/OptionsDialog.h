/////////////////////////////////////////////////////////////////////////////
// Name:        OptionsDialog.h
// Purpose:     
// Author:      Mark Erikson
// Modified by: 
// Created:     11/23/03 16:02:26
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _OPTIONSDIALOG_H_
#define _OPTIONSDIALOG_H_

#ifdef __GNUG__
#pragma interface "OptionsDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_NOTEBOOK 10001
#define ID_PANEL 10004
#define ID_GRID 10005
#define ID_BUTTON 10002
#define ID_BUTTON1 10003
////@end control identifiers

/*!
 * OptionsDialog class declaration
 */

class OptionsDialog: public wxDialog
{    
    DECLARE_CLASS( OptionsDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    OptionsDialog( );
    OptionsDialog( wxWindow* parent, wxWindowID id = -1, const wxString& caption = _("Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = -1, const wxString& caption = _("Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );

    /// Creates the controls and sizers
    void CreateControls();

////@begin OptionsDialog event handler declarations

////@end OptionsDialog event handler declarations

////@begin OptionsDialog member function declarations

////@end OptionsDialog member function declarations

    wxGrid* GetGrid();

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin OptionsDialog member variables
    wxGrid* m_grid;
////@end OptionsDialog member variables
};

#endif
    // _OPTIONSDIALOG_H_
