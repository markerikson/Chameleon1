/////////////////////////////////////////////////////////////////////////////
// Name:        VariableWatchPanel.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     03/29/04 21:43:35
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "VariableWatchPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include <wx/textdlg.h>

#include "VariableWatchPanel.h"
#include "../ChameleonWindow.h"


////@begin XPM images
////@end XPM images

/*!
 * VariableWatchPanel type definition
 */

IMPLEMENT_CLASS( VariableWatchPanel, wxPanel )

/*!
 * VariableWatchPanel event table definition
 */

BEGIN_EVENT_TABLE( VariableWatchPanel, wxPanel )

////@begin VariableWatchPanel event table entries
    EVT_BUTTON( ID_ADDWATCH, VariableWatchPanel::OnAddwatchClick )

    EVT_BUTTON( ID_REMOVEWATCH, VariableWatchPanel::OnRemovewatchClick )

////@end VariableWatchPanel event table entries

END_EVENT_TABLE()

/*!
 * VariableWatchPanel constructors
 */

VariableWatchPanel::VariableWatchPanel( )
{
}

VariableWatchPanel::VariableWatchPanel( wxWindow* parent, ChameleonWindow* mainframe, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, pos, size, style);

	m_mainFrame = mainframe;

/*
	m_grid->SetRowLabelSize(40);
	m_grid->SetColumnWidth(0, 160);
	wxSize s = parent->GetClientSize();
	int totalwidth = s.GetWidth();
	m_grid->SetColumnWidth(1, 400);

	m_grid->SetColLabelValue(0, "Name");
	m_grid->SetColLabelValue(1, "Value");

	int numRows = m_grid->GetRows();

	if(numRows >0)
	{
		m_grid->DeleteRows(0, numRows);
	}
*/
	wxListItem itemCol;
	itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_WIDTH;
	itemCol.m_text = _T("Name");
	itemCol.m_width = 120;
	m_list->InsertColumn(0, itemCol);
	itemCol.m_text = _T("Type");
	itemCol.m_width = 80;
	m_list->InsertColumn(1, itemCol);
	itemCol.m_text = _T("Value");
	itemCol.m_width = 400;
	m_list->InsertColumn(2, itemCol);

	wxListItem testitem;
	testitem.m_mask = wxLIST_MASK_TEXT;
	testitem.m_itemId = 0;
	testitem.m_text = "var1";
	testitem.m_col = 0;
	m_list->InsertItem(testitem);
	testitem.m_text = "string";
	testitem.m_col = 1;
	m_list->SetItem(testitem);
	testitem.m_text = "blah blah blah blahdy blah";
	testitem.m_col = 2;
	m_list->SetItem(testitem);
}

/*!
 * VariableWatchPanel creator
 */

bool VariableWatchPanel::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin VariableWatchPanel member initialisation
    m_list = NULL;
////@end VariableWatchPanel member initialisation

////@begin VariableWatchPanel creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    Centre();
////@end VariableWatchPanel creation
    return TRUE;
}

/*!
 * Control creation for VariableWatchPanel
 */

void VariableWatchPanel::CreateControls()
{    
////@begin VariableWatchPanel content construction

    VariableWatchPanel* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxListCtrl* item3 = new wxListCtrl( item1, ID_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_HRULES|wxSIMPLE_BORDER );
    m_list = item3;
    item2->Add(item3, 1, wxGROW, 5);

    wxBoxSizer* item4 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item4, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxButton* item5 = new wxButton( item1, ID_ADDWATCH, _("Add watch"), wxDefaultPosition, wxSize(80, -1), 0 );
    item4->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item6 = new wxButton( item1, ID_REMOVEWATCH, _("Remove watch"), wxDefaultPosition, wxSize(80, -1), 0 );
    item4->Add(item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end VariableWatchPanel content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADDWATCH
 */

void VariableWatchPanel::OnAddwatchClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxString varName = wxGetTextFromUser("Variable name to watch:", "Add Variable Watch");
	if(varName != wxEmptyString)
	{
		wxDebugEvent dbg;

		dbg.SetId(ID_DEBUG_ADD_WATCH);
		dbg.SetStatus(ID_DEBUG_ADD_WATCH);

		wxArrayString vars;
		vars.Add(varName);
		dbg.SetVariableNames(vars);
		m_mainFrame->AddPendingEvent(dbg);
		
	}
	


}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_REMOVEWATCH
 */

void VariableWatchPanel::OnRemovewatchClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool VariableWatchPanel::ShowToolTips()
{
    return TRUE;
}

void VariableWatchPanel::UpdateVariableInfo(wxDebugEvent event)
{
	wxArrayString names = event.GetVariableNames();
	wxArrayString values = event.GetVariableValues();
	wxArrayString types = event.GetVariableTypes();

	wxSortedArrayString sortedNames(names);

	m_list->DeleteAllItems();

	for(int i = 0; i < (int)sortedNames.GetCount(); i++)
	{
		wxString name = sortedNames[i];
		int idx = names.Index(name);
		wxString value = values[idx];
		wxString type = types[idx];

		wxListItem testitem;
		testitem.m_mask = wxLIST_MASK_TEXT;
		testitem.m_itemId = i;
		testitem.m_text = name;
		testitem.m_col = 0;
		m_list->InsertItem(testitem);
		testitem.m_text = type;
		testitem.m_col = 1;
		m_list->SetItem(testitem);
		testitem.m_text = value;
		testitem.m_col = 2;
		m_list->SetItem(testitem);
	}
}