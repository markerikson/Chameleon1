/////////////////////////////////////////////////////////////////////////////
// Name:        RemoteFileDialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     01/12/04 16:32:27
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _REMOTEFILEDIALOG_H_
#define _REMOTEFILEDIALOG_H_

#ifdef __GNUG__
#pragma interface "RemoteFileDialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

#include <wx/filename.h>

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxListCtrl;
////@end forward declarations

class ChameleonEditor;
class ChameleonWindow;
class Networking;
class wxListEvent;

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_BUTTON2 10007
#define ID_LISTCTRL 10001
#define ID_TXTFILENAME 10008
#define ID_BUTTON 10003
#define ID_COMBOBOX1 10004
#define ID_BUTTON1 10005
////@end control identifiers

/*!
 * RemoteFileDialog class declaration
 */

class RemoteFileDialog: public wxDialog
{    
    DECLARE_CLASS( RemoteFileDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RemoteFileDialog( );
    RemoteFileDialog( wxWindow* parent, 
						wxWindowID id = -1, 
						const wxString& caption = _("Open/Save"), 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize, 
						long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );


    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = -1, const wxString& caption = _("Open/Save"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU );

    /// Creates the controls and sizers
    void CreateControls();

////@begin RemoteFileDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
    void OnButtonUpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL
    void OnItemSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_LISTCTRL
    void OnItemActivated( wxListEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
    void OnButtonOpenClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
    void OnButtonCancelClick( wxCommandEvent& event );

////@end RemoteFileDialog event handler declarations

////@begin RemoteFileDialog member function declarations

    wxString GetCurrentPath() const { return m_currentPath ; }
    void SetCurrentPath(wxString value) { m_currentPath = value ; }

////@end RemoteFileDialog member function declarations

	void SetNetworking(Networking* network);
	wxString GetLocalFileNameAndPath();
	wxString GetRemoteFileNameAndPath();

	void OpenRemoteFile();

	void ShowDirectory(wxString dirname);
	void LoadTestData();

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin RemoteFileDialog member variables
    wxListCtrl* m_list;
    wxTextCtrl* m_txtFilename;
    wxButton* m_buttonOpen;
    wxComboBox* m_comboFiletypes;
    wxButton* m_buttonCancel;
    wxString m_currentPath;
////@end RemoteFileDialog member variables

	wxFileName m_localFileNamePath;
	wxFileName m_remoteFileNamePath;
	wxSortedArrayString m_currentDirs;
	wxSortedArrayString m_currentFiles;
	ChameleonWindow* m_parentFrame;
	Networking* m_network;
};

#endif
    // _REMOTEFILEDIALOG_H_
