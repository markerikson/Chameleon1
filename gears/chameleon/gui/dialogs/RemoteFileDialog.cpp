/////////////////////////////////////////////////////////////////////////////
// Name:        RemoteFileDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     01/12/04 16:32:27
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "RemoteFileDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/statline.h"
////@end includes

#include "RemoteFileDialog.h"

////@begin XPM images
////@end XPM images

/*!
 * RemoteFileDialog type definition
 */

IMPLEMENT_CLASS( RemoteFileDialog, wxDialog )

/*!
 * RemoteFileDialog event table definition
 */

BEGIN_EVENT_TABLE( RemoteFileDialog, wxDialog )

////@begin RemoteFileDialog event table entries
    EVT_BUTTON( ID_BUTTON2, RemoteFileDialog::OnButtonBackClick )

    EVT_BUTTON( ID_BUTTON, RemoteFileDialog::OnButtonOpenClick )

    EVT_BUTTON( ID_BUTTON1, RemoteFileDialog::OnButtonCancelClick )

////@end RemoteFileDialog event table entries

END_EVENT_TABLE()

/*!
 * RemoteFileDialog constructors
 */

RemoteFileDialog::RemoteFileDialog( )
{
}

RemoteFileDialog::RemoteFileDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * RemoteFileDialog creator
 */

bool RemoteFileDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RemoteFileDialog member initialisation
    m_currentPath = "~/";
////@end RemoteFileDialog member initialisation

////@begin RemoteFileDialog creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end RemoteFileDialog creation
    return TRUE;
}

/*!
 * Control creation for RemoteFileDialog
 */

void RemoteFileDialog::CreateControls()
{    
////@begin RemoteFileDialog content construction

    RemoteFileDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxBoxSizer* item3 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item3, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxStaticText* item4 = new wxStaticText( item1, wxID_STATIC, _("Look in: "), wxDefaultPosition, wxSize(260, -1), 0 );
    item3->Add(item4, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* item5Strings = NULL;
    wxComboBox* item5 = new wxComboBox( item1, ID_COMBOBOX2, _(""), wxDefaultPosition, wxSize(260, -1), 0, item5Strings, wxCB_DROPDOWN );
    m_pathlist = item5;
    item3->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxButton* item6 = new wxButton( item1, ID_BUTTON2, _("Back"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item6, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxListCtrl* item7 = new wxListCtrl( item1, ID_LISTCTRL, wxDefaultPosition, wxSize(450, 280), wxLC_LIST  );
    m_list = item7;
    item2->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item8, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item9 = new wxBoxSizer(wxHORIZONTAL);
    item8->Add(item9, 0, wxGROW|wxALL, 0);

    wxStaticText* item10 = new wxStaticText( item1, wxID_STATIC, _("File &name:"), wxDefaultPosition, wxSize(90, -1), 0 );
    item9->Add(item10, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxTextCtrl* item11 = new wxTextCtrl( item1, ID_TEXTCTRL, _(""), wxDefaultPosition, wxSize(246, -1), 0 );
    m_txtFilename = item11;
    item9->Add(item11, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticLine* item12 = new wxStaticLine( item1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item9->Add(item12, 0, wxGROW|wxLEFT|wxRIGHT, 10);

    wxButton* item13 = new wxButton( item1, ID_BUTTON, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonOpen = item13;
    item9->Add(item13, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* item14 = new wxBoxSizer(wxHORIZONTAL);
    item8->Add(item14, 0, wxGROW|wxALL, 0);

    wxStaticText* item15 = new wxStaticText( item1, wxID_STATIC, _("Files of &type:"), wxDefaultPosition, wxSize(90, -1), 0 );
    item14->Add(item15, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxString* item16Strings = NULL;
    wxComboBox* item16 = new wxComboBox( item1, ID_COMBOBOX1, _(""), wxDefaultPosition, wxSize(246, -1), 0, item16Strings, wxCB_DROPDOWN );
    m_comboFiletypes = item16;
    item14->Add(item16, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticLine* item17 = new wxStaticLine( item1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item14->Add(item17, 0, wxGROW|wxLEFT|wxRIGHT, 10);

    wxButton* item18 = new wxButton( item1, ID_BUTTON1, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonCancel = item18;
    item14->Add(item18, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

////@end RemoteFileDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void RemoteFileDialog::OnButtonOpenClick( wxCommandEvent& event )
{
    // Insert custom code here

	wxString filename = m_txtFilename->GetValue();

	if(filename == wxEmptyString)
	{
		return;
	}

	m_fullPathName.SetName(filename);
	m_fullPathName.SetPath(m_currentPath);

	EndModal(wxOK);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void RemoteFileDialog::OnButtonCancelClick( wxCommandEvent& event )
{
    // Insert custom code here
    EndModal(wxCANCEL);
}

/*!
 * Should we show tooltips?
 */

bool RemoteFileDialog::ShowToolTips()
{
  return TRUE;
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
 */

void RemoteFileDialog::OnButtonBackClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
}

wxString RemoteFileDialog::GetFileName()
{
	return m_fullPathName.GetFullPath();
}


