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
////@end forward declarations

class ChameleonWindow;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_NOTEBOOK 10001
#define ID_PANEL1 10006
#define ID_CHECKLISTBOX 10007
#define ID_PROFCODE 10004
#define ID_BUTTON 10012
#define ID_PANEL 10005
#define ID_TEXTCTRL1 10008
#define ID_TEXTCTRL2 10009
#define ID_TEXTCTRL3 10010
#define ID_TEXTCTRL4 10011
#define ID_BUTTON_OK 10002
#define ID_BUTTON_CANCEL 10003
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

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
    void OnUpdateAuthCode( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OK
    void OnButtonOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CANCEL
    void OnButtonCancelClick( wxCommandEvent& event );

////@end OptionsDialog event handler declarations

	void OnChar(wxKeyEvent &event);



////@begin OptionsDialog member function declarations

////@end OptionsDialog member function declarations

    
    wxCheckListBox* GetListBox();
    wxString GetServerAddress();
    wxString GetUsername();
    wxString GetPassword1();
    wxString GetPassword2();
	long GetAuthCode();

	void ExitDialog();
	void OnEnter(wxCommandEvent &event);


    void SetServerAddress(wxString address);
    void SetUsername(wxString username);
    void SetPassword1(wxString pwd);
    void SetPassword2(wxString pwd);

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin OptionsDialog member variables
    wxCheckListBox* m_checkList;
    wxTextCtrl* m_txtProfCode;
    wxButton* m_butSetAuthCode;
    wxTextCtrl* m_serverAddress;
    wxTextCtrl* m_username;
    wxTextCtrl* m_password1;
    wxTextCtrl* m_password2;
////@end OptionsDialog member variables

	ChameleonWindow* m_parentFrame;
};

#endif
    // _OPTIONSDIALOG_H_
