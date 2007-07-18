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
//#include "AddVariableWatch.h"
#include "../ChameleonWindow.h"
#include "../../common/fixvsbug.h"


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

    EVT_BUTTON( ID_CLEARALLWATCHES, VariableWatchPanel::OnClearallwatchesClick )

////@end VariableWatchPanel event table entries

END_EVENT_TABLE()

/*!
 * VariableWatchPanel constructors
 */


VariableWatchPanel::VariableWatchPanel( )
{
}

VariableWatchPanel::VariableWatchPanel( wxWindow* parent, wxEvtHandler* mainframe, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, pos, size, style);

	m_parentEventHandler = mainframe;

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

    wxListCtrl* item3 = new wxListCtrl( item1, ID_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxSIMPLE_BORDER );
    m_list = item3;
    item2->Add(item3, 1, wxGROW, 5);

    wxBoxSizer* item4 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item4, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxButton* item5 = new wxButton( item1, ID_ADDWATCH, _("Add watch"), wxDefaultPosition, wxSize(80, -1), 0 );
    item4->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item6 = new wxButton( item1, ID_REMOVEWATCH, _("Remove watch"), wxDefaultPosition, wxSize(80, -1), 0 );
    item4->Add(item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item7 = new wxButton( item1, ID_CLEARALLWATCHES, _("Clear watches"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add(item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end VariableWatchPanel content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ADDWATCH
 */

void VariableWatchPanel::OnAddwatchClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	AddWatch();

	
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_REMOVEWATCH
 */

void VariableWatchPanel::OnRemovewatchClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	RemoveWatch();
}

/*!
 * Should we show tooltips?
 */

bool VariableWatchPanel::ShowToolTips()
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
///  public AddWatch
///  Adds a variable watch to the debugger
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void VariableWatchPanel::AddWatch()
{
	wxString varName = wxGetTextFromUser("Variable name to watch:", "Add Variable Watch");

	//AddVariableWatchDialog avwd(this);

	//int result = avwd.ShowModal();
	


	//if(result == wxOK)
	if(varName != wxEmptyString)
	{
		wxDebugEvent dbg;

		dbg.SetId(ID_DEBUG_ADD_WATCH);

		/*
		//wxString varName = avwd.GetVariableName();
		//wxString funcName = avwd.GetFunctionName();
		//wxString className = wxEmptyString;

		if(avwd.FunctionInClass())
		{
			className = avwd.GetClassName();
		}
		*/

		wxArrayString vars;
		vars.Add(varName);
		dbg.SetVariableNames(vars);
		//dbg.SetFunctionName(funcName);
		//dbg.SetClassName(className);
		m_parentEventHandler->AddPendingEvent(dbg);		

		
	}
}

void VariableWatchPanel::AddWatchedVariables(wxDebugEvent debug)
{	
	wxArrayString variableNames = debug.GetVariableNames();
	
	for(int i = 0; i < variableNames.size(); i++)
	{
		wxString varName = variableNames[i];
		m_list->InsertItem(m_list->GetItemCount(), varName);
	}
}

//////////////////////////////////////////////////////////////////////////////
///  public RemoveWatch
///  Removes a variable watch from the debugger
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void VariableWatchPanel::RemoveWatch()
{
	if(m_list->GetSelectedItemCount() > 0)
	{
		int num = -1;
		int index = m_list->GetNextItem(num, wxLIST_NEXT_ALL,	wxLIST_STATE_SELECTED);

		wxListItem item;
		item.SetId(index);		
		item.m_mask = wxLIST_MASK_TEXT;

		item.m_col = 0;
		m_list->GetItem(item);
		wxString name = item.m_text;

		wxDebugEvent dbg;

		dbg.SetId(ID_DEBUG_REMOVE_WATCH);
		wxArrayString vars;
		vars.Add(name);
		dbg.SetVariableNames(vars);
		m_parentEventHandler->AddPendingEvent(dbg);

		m_list->DeleteItem(index);
	}
}

//////////////////////////////////////////////////////////////////////////////
///  public UpdateVariableInfo
///  Updates the displayed values of the variables being watched
///
///  @param  event wxDebugEvent  The event containing the variable info
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void VariableWatchPanel::UpdateVariableInfo(wxDebugEvent event)
{
	wxArrayString names = event.GetVariableNames();
	wxArrayString values = event.GetVariableValues();
	wxArrayString types = event.GetVariableTypes();

	//wxSortedArrayString sortedNames(names);

	//m_list->DeleteAllItems();

	wxListItem textColorItem;
	textColorItem.SetTextColour(wxColour("black"));
	textColorItem.m_col = 2;

	// reset all variable value text to black
	for(int i = 0; i < m_list->GetItemCount(); i++)
	{
		textColorItem.m_itemId = i;
		//m_list->SetItem(textColorItem);
	}

	textColorItem.SetTextColour(wxColour("red"));

	wxListItem retrievalItem;
	retrievalItem.m_mask = wxLIST_MASK_TEXT;
	retrievalItem.m_col = 1;

	// update any new/changed values
	for(int i = 0; i < (int)names.GetCount(); i++)
	{
		wxString name = names[i];
		int idx = names.Index(name);

		if(idx >= values.GetCount())
		{
			continue;
		}

		wxString value = values[idx];
		wxString type = types[idx];

		int nameLineNum = m_list->FindItem(-1, name);		
		
		// found the item's name
		if(nameLineNum > -1)
		{
			//retrievalItem.SetId(nameLineNum);
			//retrievalItem.m_col = 1;
			//m_list->GetItem(retrievalItem);
			//wxString listedType = retrievalItem.m_text;

			m_list->SetItem(nameLineNum, 1, type);
			retrievalItem.m_itemId = nameLineNum;
			retrievalItem.m_col = 2;
			m_list->GetItem(retrievalItem);

			wxString listedValue = retrievalItem.m_text;

			if(listedValue == value)
			{
				//retrievalItem.SetTextColour(wxColour("black"));
				//m_list->SetItem(textColorItem);
				m_list->SetItemTextColour(nameLineNum, wxColour("black"));
			}		
			else
			{
				m_list->SetItem(nameLineNum, 2, value);
				//retrievalItem.SetTextColour(wxColour("red"));
				m_list->SetItemTextColour(nameLineNum, wxColour("red"));
				
			}

			
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
///  public ClearVariableValues
///  Gives all displayed variables an empty value
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void VariableWatchPanel::ClearVariableValues()
{
	for(int i = 0; i < m_list->GetItemCount(); i++)
	{
		m_list->SetItem(i, 2, wxEmptyString);
	}
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_CLEARALLWATCHES
 */

//////////////////////////////////////////////////////////////////////////////
///  public OnClearallwatchesClick
///  Removes all variables being watched
///
///  @param  event wxCommandEvent & The generated button event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void VariableWatchPanel::OnClearallwatchesClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	int numItems = m_list->GetItemCount();

	wxListItem selectionItem;
	selectionItem.m_mask = wxLIST_MASK_STATE;
	selectionItem.m_stateMask = wxLIST_STATE_SELECTED;
	selectionItem.m_state = wxLIST_STATE_SELECTED;
	selectionItem.SetId(0);

	for(int i = 0; i < numItems; i++)
	{
		m_list->SetItem(selectionItem);
		RemoveWatch();
	}
}


