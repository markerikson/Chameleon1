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
#include "../../common/Options.h"


////@begin includes
////@end includes

#include <wx/checklst.h>
#include <wx/valtext.h>
#include "OptionsDialog.h"
#include "../ChameleonWindow.h"

#include "../../perms/p.h"
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
    EVT_BUTTON( ID_SETAUTHCODE, OptionsDialog::OnUpdateAuthCode )

    EVT_BUTTON( ID_MINGWBROWSE, OptionsDialog::OnMinGWBrowseClick )

    EVT_BUTTON( ID_BUTTON_OK, OptionsDialog::OnButtonOkClick )

    EVT_BUTTON( ID_BUTTON_CANCEL, OptionsDialog::OnButtonCancelClick )

////@end OptionsDialog event table entries
	EVT_CHAR(OptionsDialog::OnChar)
	EVT_TEXT_ENTER( ID_PROFCODE, OptionsDialog::OnEnter )
	EVT_TEXT_ENTER(ID_HOSTNAME, OptionsDialog::OnEnter)
	EVT_TEXT_ENTER(ID_USERNAME, OptionsDialog::OnEnter)
	EVT_TEXT_ENTER(ID_PASSWORD1, OptionsDialog::OnEnter)
	EVT_TEXT_ENTER(ID_PASSWORD2, OptionsDialog::OnEnter)

END_EVENT_TABLE()

/*!
 * OptionsDialog constructors
 */

OptionsDialog::OptionsDialog( )
{
}

OptionsDialog::OptionsDialog( wxWindow* parent, Options* options, wxWindowID id,  const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
	m_parentFrame = (ChameleonWindow*)parent;
	m_options = options;

	wxTextValidator textval(wxFILTER_EXCLUDE_CHAR_LIST);
	wxStringList exclude;
	exclude.Add("\"");
	m_password1->SetValidator(textval);
	m_password2->SetValidator(textval);
}

/*!
 * OptionsDialog creator
 */

bool OptionsDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin OptionsDialog member initialisation
    m_optionsNotebook = NULL;
    m_checkList = NULL;
    m_txtProfCode = NULL;
    m_butSetAuthCode = NULL;
    m_authCodeLabel = NULL;
    m_hostname = NULL;
    m_username = NULL;
    m_password1 = NULL;
    m_password2 = NULL;
    m_printStyle = NULL;
    m_showToolbarText = NULL;
    m_termHistory = NULL;
    m_txtMingwPath = NULL;
    m_butBrowseMingw = NULL;
////@end OptionsDialog member initialisation

////@begin OptionsDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
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
    wxTextCtrl* item13 = new wxTextCtrl( item4, ID_PROFCODE, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    m_txtProfCode = item13;
    item11->Add(item13, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxButton* item14 = new wxButton( item4, ID_SETAUTHCODE, _("Set authorization code"), wxDefaultPosition, wxSize(120, -1), 0 );
    m_butSetAuthCode = item14;
    item11->Add(item14, 0, wxALIGN_LEFT|wxALL, 5);
    wxStaticText* item15 = new wxStaticText( item4, wxID_STATIC, _("Current authorization code:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add(item15, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxStaticText* item16 = new wxStaticText( item4, wxID_STATIC, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
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
    wxTextCtrl* item21 = new wxTextCtrl( item17, ID_HOSTNAME, _T(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER );
    m_hostname = item21;
    item19->Add(item21, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item22 = new wxStaticText( item17, wxID_STATIC, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item22, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item23 = new wxTextCtrl( item17, ID_USERNAME, _T(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER );
    m_username = item23;
    item19->Add(item23, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item24 = new wxStaticText( item17, wxID_STATIC, _("Password (no quote marks allowed):"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item24, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item25 = new wxTextCtrl( item17, ID_PASSWORD1, _T(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER|wxTE_PASSWORD );
    m_password1 = item25;
    item19->Add(item25, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxStaticText* item26 = new wxStaticText( item17, wxID_STATIC, _("Confirm password:"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add(item26, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxTextCtrl* item27 = new wxTextCtrl( item17, ID_PASSWORD2, _T(""), wxDefaultPosition, wxSize(160, -1), wxTE_PROCESS_ENTER|wxTE_PASSWORD );
    m_password2 = item27;
    item19->Add(item27, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    item3->AddPage(item17, _("Network"));
    wxPanel* item28 = new wxPanel( item3, ID_PANEL3, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* item29 = new wxBoxSizer(wxHORIZONTAL);
    item28->SetSizer(item29);
    item28->SetAutoLayout(TRUE);
    wxBoxSizer* item30 = new wxBoxSizer(wxVERTICAL);
    item29->Add(item30, 1, wxALIGN_TOP, 5);
    wxStaticText* item31 = new wxStaticText( item28, wxID_STATIC, _("Print text in:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add(item31, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxString item32Strings[] = {
        _("Black and white"),
        _("Color")
    };
    wxComboBox* item32 = new wxComboBox( item28, ID_PRINTSTYLE, _("Black and white"), wxDefaultPosition, wxDefaultSize, 2, item32Strings, wxCB_READONLY );
    m_printStyle = item32;
    item32->SetStringSelection(_("Black and white"));
    item30->Add(item32, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxCheckBox* item33 = new wxCheckBox( item28, ID_CHECKBOX, _("Show text on toolbar buttons"), wxDefaultPosition, wxDefaultSize, 0 );
    m_showToolbarText = item33;
    item33->SetValue(FALSE);
    item30->Add(item33, 1, wxGROW|wxALL, 5);
    wxBoxSizer* item34 = new wxBoxSizer(wxHORIZONTAL);
    item30->Add(item34, 0, wxALIGN_LEFT|wxALL, 0);
    wxStaticText* item35 = new wxStaticText( item28, wxID_STATIC, _("Maximum history lines in the terminal:"), wxDefaultPosition, wxDefaultSize, 0 );
    item34->Add(item35, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    wxSpinCtrl* item36 = new wxSpinCtrl( item28, ID_SPINCTRL, _T(""), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 25, 2500, 0 );
    m_termHistory = item36;
    item34->Add(item36, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxBoxSizer* item37 = new wxBoxSizer(wxVERTICAL);
    item30->Add(item37, 0, wxALIGN_CENTER_HORIZONTAL, 5);
    wxStaticText* item38 = new wxStaticText( item28, wxID_STATIC, _("Path to MinGW:"), wxDefaultPosition, wxDefaultSize, 0 );
    item37->Add(item38, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxBoxSizer* item39 = new wxBoxSizer(wxHORIZONTAL);
    item37->Add(item39, 0, wxALIGN_CENTER_HORIZONTAL, 5);
    wxTextCtrl* item40 = new wxTextCtrl( item28, ID_MINGWPATH, _T(""), wxDefaultPosition, wxSize(295, -1), 0 );
    m_txtMingwPath = item40;
    item39->Add(item40, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    wxButton* item41 = new wxButton( item28, ID_MINGWBROWSE, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
    m_butBrowseMingw = item41;
    item39->Add(item41, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    item3->AddPage(item28, _("Miscellaneous"));
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item42 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item42, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* item43 = new wxButton( item1, ID_BUTTON_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item42->Add(item43, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item44 = new wxButton( item1, ID_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item42->Add(item44, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end OptionsDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool OptionsDialog::ShowToolTips()
{
  return TRUE;
}

/*
wxCheckListBox* OptionsDialog::GetListBox()
{
    return this->m_checkList;
}

wxString OptionsDialog::GetServerAddress()
{
    return m_hostname->GetValue();
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
	m_hostname->SetValue(address);
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
*/

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

//////////////////////////////////////////////////////////////////////////////
///  public OnEnter
///  Allows the user to press Enter to close the dialog
///
///  @param  event wxCommandEvent & The generated event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::OnEnter( wxCommandEvent& event )
{
    // Insert custom code here
	event.Skip();

	ExitDialog();
}


//////////////////////////////////////////////////////////////////////////////
///  public ExitDialog
///  Ensures that everything's correct before exiting the dialog
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::ExitDialog()
{
	m_txtProfCode->Clear();

	wxString pwd1 = m_password1->GetValue();
	wxString pwd2 = m_password2->GetValue();

	if(pwd1 == pwd2)
	{
		//Permission* perms = m_options->GetPerms();
		
		if(EvaluateOptions())
		{
			UpdateChecklist();
			EndModal(wxOK);
			m_optionsNotebook->SetSelection(0);
		}
		
	}
	else
	{
		wxMessageBox("Please enter the same password in both fields");
	}
}

/*
wxString OptionsDialog::GetAuthCode()
{
	return m_txtProfCode->GetValue();

	if(authCodeString == wxEmptyString)
	{
		return -1;
	}

	long authCodeLong = 0;

	authCodeString.ToLong(&authCodeLong);
	

	return authCodeLong;

}
*/

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

//////////////////////////////////////////////////////////////////////////////
///  public OnUpdateAuthCode
///  Checks the newly entered authorization code and updates the permissions manager
///
///  @param  event wxCommandEvent & The generated menu event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::OnUpdateAuthCode( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxString newAuthCode = m_txtProfCode->GetValue();
	newAuthCode.MakeUpper();

	Permission* perms = m_options->GetPerms();

	if(!perms->setGlobalAuthorized(newAuthCode))
	{
		
		wxMessageBox("Invalid authorization code.  Please check that it was entered correctly and try again.");
	}
	else
	{
		UpdateChecklist();

		m_txtProfCode->Clear();
		m_authCodeLabel->SetLabel(newAuthCode);

		wxMessageBox("Authorized features updated.");
		
	}

}


//////////////////////////////////////////////////////////////////////////////
///  public EnableServerSettings
///  Enables the network text fields
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::EnableServerSettings()
{
	wxColour white("white");
	m_hostname->SetEditable(true);
	m_hostname->SetBackgroundColour(white);
	m_username->SetEditable(true);
	m_username->SetBackgroundColour(white);
	m_password1->SetEditable(true);
	m_password1->SetBackgroundColour(white);
	m_password2->SetEditable(true);
	m_password2->SetBackgroundColour(white);
}

//////////////////////////////////////////////////////////////////////////////
///  public DisableServerSettings
///  Disables the network text fields
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::DisableServerSettings()
{
	wxColour grey("light grey");
	m_hostname->SetEditable(false);
	m_hostname->SetBackgroundColour(grey);
	m_username->SetEditable(false);
	m_username->SetBackgroundColour(grey);
	m_password1->SetEditable(false);
	m_password1->SetBackgroundColour(grey);
	m_password2->SetEditable(false);
	m_password2->SetBackgroundColour(grey);
}

/*
void OptionsDialog::SetAuthCode(wxString authcode)
{
	m_authCodeLabel->SetLabel(authcode);
}
*/

//////////////////////////////////////////////////////////////////////////////
///  public BrowseForDir
///  Lets the user browse for the MinGW directory
///
///  @param  textbox wxTextCtrl * The textbox to fill
///  @param  name    wxString     The title of the browse dialog
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::BrowseForDir(wxTextCtrl* textbox, wxString title)
{
	wxString currentDir = textbox->GetValue();

	title = "Select the directory where MinGW is installed (usually inside the Chameleon directory)";

	wxString newDir;
	wxString defaultDir;

	if(wxFileName::DirExists(currentDir))
	{
		defaultDir = currentDir;
	}
	else
	{
		defaultDir = wxEmptyString;
	}

	wxString resultDir = wxDirSelector(title, defaultDir);

	if(resultDir != wxEmptyString)
	{
		textbox->SetValue(resultDir);
	}

	
}/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_MINGWBROWSE
 */

//////////////////////////////////////////////////////////////////////////////
///  public OnMinGWBrowseClick
///  Calls the BrowseForDir function
///
///  @param  event wxCommandEvent & The generated menu event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::OnMinGWBrowseClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

    BrowseForDir(m_txtMingwPath, wxEmptyString);
}


//////////////////////////////////////////////////////////////////////////////
///  public EvaluateOptions
///  Validates the options items before exiting the dialog
///
///  @return bool Whether or not the options are valid
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool OptionsDialog::EvaluateOptions()
{
	bool validOptions = true;

	wxString outputMessage = wxEmptyString;

	wxString mingwPath = m_txtMingwPath->GetValue();

	if( (mingwPath != m_options->GetMingwPath()) && !wxFileName::DirExists(mingwPath))
	{
		outputMessage = mingwPath + " is not a valid path.";
		validOptions = false;
	}


	if(validOptions)
	{
		Permission* perms = m_options->GetPerms();
		for(int i = 0; i < m_checkList->GetCount(); i++)
		{
			int mappedPerm = m_permMappings[i];

			if(m_checkList->IsChecked(i))
			{
				perms->enable(mappedPerm);
			}
			else
			{
				perms->disable(mappedPerm);
			}
		}

		m_options->SetHostname(m_hostname->GetValue());
		m_options->SetUsername(m_username->GetValue());
		m_options->SetPassphrase(m_password1->GetValue());
		m_options->SetTerminalHistorySize(m_termHistory->GetValue());

		m_options->SetMingwPath(mingwPath);

		wxString selectedPrintStyleString = m_printStyle->GetValue();
		
		if(selectedPrintStyleString == "Color")
		{
			m_options->SetPrintStyle(wxSTC_PRINT_COLOURONWHITE);
		}
		else
		{
			m_options->SetPrintStyle(wxSTC_PRINT_BLACKONWHITE);
		}

		m_options->SetShowToolbarText(m_showToolbarText->IsChecked());
	}
	else
	{
		wxMessageBox(outputMessage, "Invalid Option", wxOK | wxICON_WARNING);
	}

	return validOptions;	
}

//////////////////////////////////////////////////////////////////////////////
///  public InitializeDialog
///  Sets up the dialog's contents before being displayed
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::InitializeDialog()
{
	Permission* perms = m_options->GetPerms();

	UpdateChecklist();

	m_hostname->SetValue(m_options->GetHostname());
	m_username->SetValue(m_options->GetUsername());
	wxString password = m_options->GetPassphrase();
	m_password1->SetValue(password);
	m_password2->SetValue(password);

	m_txtMingwPath->SetValue(m_options->GetMingwPath());

	m_authCodeLabel->SetLabel(perms->GetAuthCode());

	wxString printStyleString;
	if(m_options->GetPrintStyle() == wxSTC_PRINT_COLOURONWHITE)
	{
		printStyleString = "Color";
	}
	else
	{
		printStyleString = "Black and white";
	}

	m_printStyle->SetValue(printStyleString);
	m_termHistory->SetValue(m_options->GetTerminalHistorySize());

	m_showToolbarText->SetValue(m_options->GetShowToolbarText());

}

//////////////////////////////////////////////////////////////////////////////
///  public UpdateChecklist
///  Updates the items in the permissions checklist, based on the current permissions
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void OptionsDialog::UpdateChecklist()
{
	Permission* perms = m_options->GetPerms();
	m_checkList->Clear();
	m_permMappings.Clear();

	wxString optionname;

	for(int i = PERM_FIRST; i < PERM_LAST; i++)
	{		
		if(perms->isAuthorized(i))
		{
			optionname = perms->getPermName(i);
			m_checkList->Append(optionname);
			int checkIndex = m_permMappings.GetCount();
			m_permMappings.Add(i);

			if(perms->isEnabled(i))
			{
				m_checkList->Check(checkIndex, true);
			}
		}		
	}
}