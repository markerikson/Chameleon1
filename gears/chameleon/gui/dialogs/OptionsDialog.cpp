/////////////////////////////////////////////////////////////////////////////
// Name:        OptionsDialog.cpp
// Purpose:     
// Author:      Mark Erikson
// Modified by: 
// Created:     11/23/03 16:02:26
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "OptionsDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
#include "wx/wx.h"
#include "wx/notebook.h"
////@end includes

#include <wx/checklst.h>
#include "OptionsDialog.h"

#include "../../common/debug.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////@begin XPM images
////@end XPM images

/*!
 * OptionsDialog type definition
 */

IMPLEMENT_CLASS( OptionsDialog, wxDialog )

/*!
 * OptionsDialog event table definition
 */

BEGIN_EVENT_TABLE( OptionsDialog, wxDialog )

////@begin OptionsDialog event table entries
    EVT_BUTTON( ID_BUTTON_OK, OptionsDialog::OnButtonOkClick )

    EVT_BUTTON( ID_BUTTON_CANCEL, OptionsDialog::OnButtonCancelClick )

////@end OptionsDialog event table entries

END_EVENT_TABLE()

/*!
 * OptionsDialog constructors
 */

OptionsDialog::OptionsDialog( )
{
}

OptionsDialog::OptionsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * OptionsDialog creator
 */

bool OptionsDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin OptionsDialog member initialisation
////@end OptionsDialog member initialisation

////@begin OptionsDialog creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end OptionsDialog creation
    return TRUE;
}

/*!
 * Control creation for OptionsDialog
 */

void OptionsDialog::CreateControls()
{    
////@begin OptionsDialog content construction

    OptionsDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxNotebook* item3 = new wxNotebook( item1, ID_NOTEBOOK, wxDefaultPosition, wxSize(400, 270), wxNB_TOP );
    wxPanel* item4 = new wxPanel( item3, ID_PANEL1, wxDefaultPosition, wxSize(100, 80), wxNO_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* item5 = new wxBoxSizer(wxVERTICAL);
    item4->SetSizer(item5);
    item4->SetAutoLayout(TRUE);
    wxStaticText* item6 = new wxStaticText( item4, wxID_STATIC, _("Chameleon has a variety of features that can be enabled by your professor.\nHere you can see what features are enabled, as well as enter an activation code."), wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    item5->Add(item6, 0, wxGROW|wxLEFT|wxTOP|wxBOTTOM|wxADJUST_MINSIZE, 5);
    wxBoxSizer* item7 = new wxBoxSizer(wxHORIZONTAL);
    item5->Add(item7, 0, wxGROW, 5);
    wxBoxSizer* item8 = new wxBoxSizer(wxVERTICAL);
    item7->Add(item8, 0, wxGROW|wxALL, 0);
    wxStaticText* item9 = new wxStaticText( item4, wxID_STATIC, _("Current authorized features:"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add(item9, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxString* item10Strings = NULL;
    wxCheckListBox* item10 = new wxCheckListBox( item4, ID_CHECKLISTBOX, wxDefaultPosition, wxSize(180, 175), 0, item10Strings, 0 );
    m_checkList = item10;
    item8->Add(item10, 1, wxGROW|wxALL, 5);
    wxBoxSizer* item11 = new wxBoxSizer(wxVERTICAL);
    item7->Add(item11, 0, wxGROW, 5);
    wxStaticText* item12 = new wxStaticText( item4, wxID_STATIC, _("Enter the code from your professor here\n(note: currently disabled):"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item13 = new wxTextCtrl( item4, ID_PROFCODE, _(""), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add(item13, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    item3->AddPage(item4, _("Features"));
    wxPanel* item14 = new wxPanel( item3, ID_PANEL, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* item15 = new wxBoxSizer(wxHORIZONTAL);
    item14->SetSizer(item15);
    item14->SetAutoLayout(TRUE);
    wxBoxSizer* item16 = new wxBoxSizer(wxVERTICAL);
    item15->Add(item16, 0, wxALIGN_TOP, 5);
    wxStaticText* item17 = new wxStaticText( item14, wxID_STATIC, _("Network server address:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item17, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item18 = new wxTextCtrl( item14, ID_TEXTCTRL1, _(""), wxDefaultPosition, wxSize(160, -1), 0 );
    m_serverAddress = item18;
    item16->Add(item18, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item19 = new wxStaticText( item14, wxID_STATIC, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item19, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item20 = new wxTextCtrl( item14, ID_TEXTCTRL2, _(""), wxDefaultPosition, wxSize(160, -1), 0 );
    m_username = item20;
    item16->Add(item20, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item21 = new wxStaticText( item14, wxID_STATIC, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item21, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item22 = new wxTextCtrl( item14, ID_TEXTCTRL3, _(""), wxDefaultPosition, wxSize(160, -1), wxTE_PASSWORD );
    m_password1 = item22;
    item16->Add(item22, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item23 = new wxStaticText( item14, wxID_STATIC, _("Confirm password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item23, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item24 = new wxTextCtrl( item14, ID_TEXTCTRL4, _(""), wxDefaultPosition, wxSize(160, -1), wxTE_PASSWORD );
    m_password2 = item24;
    item16->Add(item24, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    item3->AddPage(item14, _("Network"));
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item25 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item25, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* item26 = new wxButton( item1, ID_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add(item26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item27 = new wxButton( item1, ID_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add(item27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end OptionsDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool OptionsDialog::ShowToolTips()
{
  return TRUE;
}

wxCheckListBox* OptionsDialog::GetListBox()
{
    return this->m_checkList;
}

wxString OptionsDialog::GetServerAddress()
{
    return m_serverAddress->GetValue();
}

wxString OptionsDialog::GetUsername()
{
	return m_username->GetValue();
}

wxString OptionsDialog::GetPassword1()
{
	return m_password1->GetValue();
}

wxString OptionsDialog::GetPassword2()
{
	return m_password2->GetValue();
}

void OptionsDialog::SetServerAddress(wxString address)
{
	m_serverAddress->SetValue(address);
}

void OptionsDialog::SetUsername(wxString username)
{
	m_username->SetValue(username);
}

void OptionsDialog::SetPassword1(wxString pwd)
{
	m_password1->SetValue(pwd);
}

void OptionsDialog::SetPassword2(wxString pwd)
{
	m_password2->SetValue(pwd);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OK
 */

void OptionsDialog::OnButtonOkClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxString pwd1 = m_password1->GetValue();
	wxString pwd2 = m_password2->GetValue();

	if(pwd1 == pwd2)
	{
		EndModal(wxOK);
	}
	else
	{
		wxMessageBox("Please enter the same password in both fields");
	}
	
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CANCEL
 */

void OptionsDialog::OnButtonCancelClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
	EndModal(wxCANCEL);
}

