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
#include "wx/wx.h"
#include "wx/notebook.h"
////@end includes


/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
////@end forward declarations

class ChameleonWindow;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define SYMBOL_OPTIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_OPTIONSDIALOG_TITLE _("Options")
#define SYMBOL_OPTIONSDIALOG_IDNAME ID_DIALOG
#define SYMBOL_OPTIONSDIALOG_SIZE wxSize(420, 315)
#define SYMBOL_OPTIONSDIALOG_POSITION wxPoint(0, 0)
#define ID_NOTEBOOK 10001
#define ID_PANEL1 10006
#define ID_CHECKLISTBOX 10007
#define ID_PROFCODE 10004
#define ID_SETAUTHCODE 10012
#define ID_PANEL 10005
#define ID_HOSTNAME 10008
#define ID_USERNAME 10009
#define ID_PASSWORD1 10010
#define ID_PASSWORD2 10011
#define ID_PANEL2 10013
#define ID_MINGWPATH 10014
#define ID_MINGWBROWSE 10015
#define ID_PANEL3 10016
#define ID_PRINTSTYLE 10017
#define ID_BUTTON_OK 10002
#define ID_BUTTON_CANCEL 10003
////@end control identifiers

//#include <wx/wx.h>

class wxTextCtrl;
class wxButton;
class wxCheckListBox;
class wxString;
class Options;

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
    OptionsDialog( wxWindow* parent, Options* options, wxWindowID id = -1,  const wxString& caption = _("Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = -1, const wxString& caption = _("Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );

    /// Creates the controls and sizers
    void CreateControls();

////@begin OptionsDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SETAUTHCODE
    void OnUpdateAuthCode( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MINGWBROWSE
    void OnMinGWBrowseClick( wxCommandEvent& event );

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
	wxString GetAuthCode();
	wxString GetMingwPath(){ return m_txtMingwPath->GetValue(); }

	void BrowseForDir(wxTextCtrl* textbox, wxString name);
	bool EvaluateOptions();

	void InitializeDialog();
	void UpdateChecklist();
	void ExitDialog();
	void OnEnter(wxCommandEvent &event);


    void SetServerAddress(wxString address);
    void SetUsername(wxString username);
    void SetPassword1(wxString pwd);
    void SetPassword2(wxString pwd);
	void SetAuthCode(wxString authcode);

	void EnableServerSettings();
	void DisableServerSettings();

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin OptionsDialog member variables
    wxNotebook* m_optionsNotebook;
    wxCheckListBox* m_checkList;
    wxTextCtrl* m_txtProfCode;
    wxButton* m_butSetAuthCode;
    wxStaticText* m_authCodeLabel;
    wxTextCtrl* m_hostname;
    wxTextCtrl* m_username;
    wxTextCtrl* m_password1;
    wxTextCtrl* m_password2;
    wxTextCtrl* m_txtMingwPath;
    wxButton* m_butBrowseMingw;
    wxComboBox* m_printStyle;
////@end OptionsDialog member variables

	ChameleonWindow* m_parentFrame;
	Options* m_options;

	wxArrayInt m_permMappings;
};

#endif
    // _OPTIONSDIALOG_H_
