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

#include "../../common/CommonHeaders.h"

////@begin includes
#include "wx/wx.h"
#include "wx/notebook.h"
////@end includes

#include <wx/checklst.h>
#include "OptionsDialog.h"
#include "../ChameleonWindow.h"

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
    EVT_BUTTON( ID_BUTTON, OptionsDialog::OnUpdateAuthCode )

    EVT_BUTTON( ID_BUTTON_OK, OptionsDialog::OnButtonOkClick )

    EVT_BUTTON( ID_BUTTON_CANCEL, OptionsDialog::OnButtonCancelClick )

////@end OptionsDialog event table entries
	EVT_CHAR(OptionsDialog::OnChar)
	EVT_TEXT_ENTER( ID_PROFCODE, OptionsDialog::OnEnter )
	EVT_TEXT_ENTER(ID_TEXTCTRL1, OptionsDialog::OnEnter)
	EVT_TEXT_ENTER(ID_TEXTCTRL2, OptionsDialog::OnEnter)
	EVT_TEXT_ENTER(ID_TEXTCTRL3, OptionsDialog::OnEnter)
	EVT_TEXT_ENTER(ID_TEXTCTRL4, OptionsDialog::OnEnter)

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
	m_parentFrame = (ChameleonWindow*)parent;
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
    m_optionsNotebook = item3;
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
    wxStaticText* item12 = new wxStaticText( item4, wxID_STATIC, _("Enter the code from your professor here:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item13 = new wxTextCtrl( item4, ID_PROFCODE, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    m_txtProfCode = item13;
    item11->Add(item13, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxButton* item14 = new wxButton( item4, ID_BUTTON, _("Set authorization code"), wxDefaultPosition, wxSize(120, -1), 0 );
    m_butSetAuthCode = item14;
    item11->Add(item14, 0, wxALIGN_LEFT|wxALL, 5);
    wxStaticText* item15 = new wxStaticText( item4, wxID_STATIC, _("Current authorization code:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add(item15, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxStaticText* item16 = new wxStaticText( item4, wxID_STATIC, _(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_authCodeLabel = item16;
    item11->Add(item16, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);
    item3->AddPage(item4, _("Features"));
    wxPanel* item17 = new wxPanel( item3, ID_PANEL, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* item18 = new wxBoxSizer(wxHORIZONTAL);
    item17->SetSizer(item18);
    item17->SetAutoLayout(TRUE);
    wxBoxSizer* item19 = new wxBoxSizer(wxVERTICAL);
    item18->Add(item19, 0, wxALIGN_TOP, 5);
    wxStaticText* item20 = new wxStaticText( item17, wxID_STATIC, _("Network server address:"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item20, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item21 = new wxTextCtrl( item17, ID_TEXTCTRL1, _(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER );
    m_serverAddress = item21;
    item19->Add(item21, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item22 = new wxStaticText( item17, wxID_STATIC, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item22, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item23 = new wxTextCtrl( item17, ID_TEXTCTRL2, _(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER );
    m_username = item23;
    item19->Add(item23, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item24 = new wxStaticText( item17, wxID_STATIC, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item24, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item25 = new wxTextCtrl( item17, ID_TEXTCTRL3, _(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER|wxTE_PASSWORD );
    m_password1 = item25;
    item19->Add(item25, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item26 = new wxStaticText( item17, wxID_STATIC, _("Confirm password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item26, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item27 = new wxTextCtrl( item17, ID_TEXTCTRL4, _(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER|wxTE_PASSWORD );
    m_password2 = item27;
    item19->Add(item27, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    item3->AddPage(item17, _("Network"));
    wxPanel* item28 = new wxPanel( item3, ID_PANEL2, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* item29 = new wxBoxSizer(wxHORIZONTAL);
    item28->SetSizer(item29);
    item28->SetAutoLayout(TRUE);
    wxBoxSizer* item30 = new wxBoxSizer(wxVERTICAL);
    item29->Add(item30, 0, wxALIGN_TOP, 5);
    wxStaticText* item31 = new wxStaticText( item28, wxID_STATIC, _("Path to MinGW:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add(item31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxBoxSizer* item32 = new wxBoxSizer(wxHORIZONTAL);
    item30->Add(item32, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxTextCtrl* item33 = new wxTextCtrl( item28, ID_TEXTCTRL, _(""), wxDefaultPosition, wxSize(180, -1), 0 );
    m_txtMingwPath = item33;
    item32->Add(item33, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxButton* item34 = new wxButton( item28, ID_BUTTON1, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    m_butBrowseMingw = item34;
    item32->Add(item34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* item35 = new wxStaticText( item28, wxID_STATIC, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add(item35, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxStaticText* item36 = new wxStaticText( item28, wxID_STATIC, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add(item36, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxStaticText* item37 = new wxStaticText( item28, wxID_STATIC, _("Confirm password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add(item37, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    item3->AddPage(item28, _("Compiler"));
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item38 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item38, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* item39 = new wxButton( item1, ID_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item38->Add(item39, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item40 = new wxButton( item1, ID_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item38->Add(item40, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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

	ExitDialog();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CANCEL
 */

void OptionsDialog::OnButtonCancelClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
	EndModal(wxCANCEL);
	m_optionsNotebook->SetSelection(0);
}

void OptionsDialog::OnChar(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_RETURN)
	{
		event.Skip();
	}

}
/*!
 * wxEVT_COMMAND_TEXT_ENTER event handler for ID_PROFCODE
 */

void OptionsDialog::OnEnter( wxCommandEvent& event )
{
    // Insert custom code here
	event.Skip();

	ExitDialog();
}


void OptionsDialog::ExitDialog()
{
	m_txtProfCode->Clear();

	wxString pwd1 = m_password1->GetValue();
	wxString pwd2 = m_password2->GetValue();

	if(pwd1 == pwd2)
	{
		EndModal(wxOK);
		m_optionsNotebook->SetSelection(0);
	}
	else
	{
		wxMessageBox("Please enter the same password in both fields");
	}
}

wxString OptionsDialog::GetAuthCode()
{
	return m_txtProfCode->GetValue();
/*
	if(authCodeString == wxEmptyString)
	{
		return -1;
	}

	long authCodeLong = 0;

	authCodeString.ToLong(&authCodeLong);
	

	return authCodeLong;
*/
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void OptionsDialog::OnUpdateAuthCode( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	if(!m_parentFrame->UpdateAuthCode())
	{
		wxMessageBox("Invalid authorization code.  Please check that it was entered correctly and try again.");
	}
	else
	{
		wxMessageBox("Authorized features updated.");
		m_txtProfCode->Clear();
	}

}


void OptionsDialog::EnableServerSettings()
{
	wxColour white("white");
	m_serverAddress->SetEditable(true);
	m_serverAddress->SetBackgroundColour(white);
	m_username->SetEditable(true);
	m_username->SetBackgroundColour(white);
	m_password1->SetEditable(true);
	m_password1->SetBackgroundColour(white);
	m_password2->SetEditable(true);
	m_password2->SetBackgroundColour(white);
}

void OptionsDialog::DisableServerSettings()
{
	wxColour grey("light grey");
	m_serverAddress->SetEditable(false);
	m_serverAddress->SetBackgroundColour(grey);
	m_username->SetEditable(false);
	m_username->SetBackgroundColour(grey);
	m_password1->SetEditable(false);
	m_password1->SetBackgroundColour(grey);
	m_password2->SetEditable(false);
	m_password2->SetBackgroundColour(grey);
}

void OptionsDialog::SetAuthCode(wxString authcode)
{
	m_authCodeLabel->SetLabel(authcode);
}