/////////////////////////////////////////////////////////////////////////////
// Name:        ProfOptionsDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     02/09/04 19:36:08
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "ProfOptionsDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
#include "wx/wx.h"
////@end includes

#include "ProfOptionsDialog.h"
#include "../perms/p.h"
#include <stdlib.h>

////@begin XPM images

////@end XPM images

/*!
 * ProfOptionsDialog type definition
 */

IMPLEMENT_CLASS( ProfOptionsDialog, wxDialog )

/*!
 * ProfOptionsDialog event table definition
 */

BEGIN_EVENT_TABLE( ProfOptionsDialog, wxDialog )

////@begin ProfOptionsDialog event table entries
    EVT_BUTTON( ID_GENERATE, ProfOptionsDialog::OnGenerateClick )

    EVT_BUTTON( ID_EXITBUTTON, ProfOptionsDialog::OnExitbuttonClick )

////@end ProfOptionsDialog event table entries

	EVT_CLOSE(ProfOptionsDialog::OnQuit)

END_EVENT_TABLE()

/*!
 * ProfOptionsDialog constructors
 */

ProfOptionsDialog::ProfOptionsDialog( )
{
}

ProfOptionsDialog::ProfOptionsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * ProfOptionsDialog creator
 */

bool ProfOptionsDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ProfOptionsDialog member initialisation
////@end ProfOptionsDialog member initialisation

////@begin ProfOptionsDialog creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end ProfOptionsDialog creation

	for(int i = PERM_FIRST; i < PERM_LAST; i++)
	{
		m_chklstModules->Append(GlobalPermStrings[i]);
	}

	srand(time(0));
    return TRUE;
}

/*!
 * Control creation for ProfOptionsDialog
 */

void ProfOptionsDialog::CreateControls()
{    
////@begin ProfOptionsDialog content construction

    ProfOptionsDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxBoxSizer* item3 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item4 = new wxBoxSizer(wxVERTICAL);
    item3->Add(item4, 0, wxALIGN_TOP|wxALL, 5);

    wxStaticText* item5 = new wxStaticText( item1, wxID_STATIC, _("Available modules:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString* item6Strings = NULL;
    wxCheckListBox* item6 = new wxCheckListBox( item1, ID_CHECKLISTBOX, wxDefaultPosition, wxSize(200, 240), 0, item6Strings, 0 );
    m_chklstModules = item6;
    item4->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxVERTICAL);
    item3->Add(item7, 0, wxALIGN_TOP|wxALL, 5);

    wxStaticText* item8 = new wxStaticText( item1, wxID_STATIC, _("Generated activation code:"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add(item8, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item9 = new wxTextCtrl( item1, ID_TXTCODE, _(""), wxDefaultPosition, wxSize(140, -1), 0 );
    m_txtGeneratedCode = item9;
    item7->Add(item9, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxCheckBox* item10 = new wxCheckBox( item1, ID_RANDOMIZE, _("Randomize code"), wxDefaultPosition, wxSize(100, -1), 0 );
    m_chkRandomize = item10;
    item10->SetValue(FALSE);
    item7->Add(item10, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* item11 = new wxButton( item1, ID_GENERATE, _("Generate code"), wxDefaultPosition, wxSize(90, -1), 0 );
    m_genButton = item11;
    item7->Add(item11, 0, wxALIGN_LEFT|wxALL, 5);

    wxButton* item12 = new wxButton( item1, ID_EXITBUTTON, _("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
    m_exitButton = item12;
    item7->Add(item12, 0, wxALIGN_RIGHT|wxTOP, 140);

////@end ProfOptionsDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_GENERATE
 */

void ProfOptionsDialog::OnGenerateClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	unsigned int newCode = 0;

	for(int i = 0; i < m_chklstModules->GetCount(); i++)
	{
		if(m_chklstModules->IsChecked(i))
		{
			newCode |= 1 << i;
		}
	}

	if(m_chkRandomize->IsChecked())
	{
		int bitcounter = sizeof(int) * 8;
		for(bitcounter; bitcounter >= PERM_LAST; bitcounter--)
		{
			if(rand() % 2 == 0)
			{
				newCode |= 1 << bitcounter;
			}

		}
	}
	wxString message;
	message.Printf("%u", newCode);
	
	m_txtGeneratedCode->SetValue(message);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_EXITBUTTON
 */

void ProfOptionsDialog::OnExitbuttonClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
	Destroy();
}

/*!
 * Should we show tooltips?
 */

bool ProfOptionsDialog::ShowToolTips()
{
  return TRUE;
}

void ProfOptionsDialog::OnQuit(wxCommandEvent &event)
{
	Destroy();
}