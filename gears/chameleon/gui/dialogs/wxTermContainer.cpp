/////////////////////////////////////////////////////////////////////////////
// Name:        wxTermContainer.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     03/18/04 01:08:08
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wxTermContainer.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
#include "wx/wx.h"
#include "wx/scrolbar.h"
////@end includes

#include "wxTermContainer.h"
#include "../wxTerm.h"

////@begin XPM images

////@end XPM images

/*!
 * wxTermContainer type definition
 */

IMPLEMENT_CLASS( wxTermContainer, wxPanel )

/*!
 * wxTermContainer event table definition
 */

BEGIN_EVENT_TABLE( wxTermContainer, wxPanel )

////@begin wxTermContainer event table entries
    EVT_COMMAND_SCROLL_LINEUP( ID_SCROLLBAR, wxTermContainer::OnScrollbarScrollLineUp )
    EVT_COMMAND_SCROLL_LINEDOWN( ID_SCROLLBAR, wxTermContainer::OnScrollbarScrollLineDown )
    EVT_COMMAND_SCROLL_PAGEUP( ID_SCROLLBAR, wxTermContainer::OnScrollbarScrollPageUp )
    EVT_COMMAND_SCROLL_PAGEDOWN( ID_SCROLLBAR, wxTermContainer::OnScrollbarScrollPageDown )
    EVT_COMMAND_SCROLL_THUMBTRACK( ID_SCROLLBAR, wxTermContainer::OnScrollbarScrollThumbtrack )

////@end wxTermContainer event table entries

  EVT_SIZE						(wxTermContainer::OnSize)

END_EVENT_TABLE()

/*!
 * wxTermContainer constructors
 */

wxTermContainer::wxTermContainer( )
{
}

wxTermContainer::wxTermContainer( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: m_terminal(NULL)
{
    Create(parent, id, pos, size, style);
	
}

/*!
 * wxTermContainer creator
 */

bool wxTermContainer::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxTermContainer member initialisation
////@end wxTermContainer member initialisation


////@begin wxTermContainer creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    Centre();
////@end wxTermContainer creation
    return TRUE;
}

/*!
 * Control creation for wxTermContainer
 */

void wxTermContainer::CreateControls()
{    
////@begin wxTermContainer content construction

    wxTermContainer* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxHORIZONTAL);
	m_sizer = item2;
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

	/*
    wxTerm* item3 = new wxTerm( item1, ID_PANEL, wxDefaultPosition, wxSize(379, 80), wxNO_BORDER|wxTAB_TRAVERSAL );
    m_terminal = item3;
    item2->Add(item3, 1, wxGROW, 5);
	*/

	//item2->Add(m_terminal, 1, wxGROW, 0);

    wxScrollBar* item4 = new wxScrollBar( item1, ID_SCROLLBAR, wxPoint(this->GetSize().GetX() - 18, 0), wxSize(18, -1), wxSB_VERTICAL );
    m_scrollbar = item4;
    item4->SetScrollbar(90, 10, 100, 10);
    item2->Add(item4, 0, wxGROW, 5);

////@end wxTermContainer content construction
}

/*!
 * wxEVT_SCROLL_LINEUP event handler for ID_SCROLLBAR
 */

void wxTermContainer::OnScrollbarScrollLineUp( wxScrollEvent& event )
{
    // Insert custom code here
    event.Skip();

	//wxLogDebug("OnScrollbarScrollPageUp()");
	m_terminal->ScrollTerminal(1, true);
}

/*!
 * wxEVT_SCROLL_LINEDOWN event handler for ID_SCROLLBAR
 */

void wxTermContainer::OnScrollbarScrollLineDown( wxScrollEvent& event )
{
    // Insert custom code here
    event.Skip();
	m_terminal->ScrollTerminal(1, false);

}

/*!
 * wxEVT_SCROLL_PAGEUP event handler for ID_SCROLLBAR
 */

void wxTermContainer::OnScrollbarScrollPageUp( wxScrollEvent& event )
{
    // Insert custom code here
    event.Skip();
	m_terminal->ScrollTerminal(10, true);

}

/*!
 * wxEVT_SCROLL_PAGEDOWN event handler for ID_SCROLLBAR
 */

void wxTermContainer::OnScrollbarScrollPageDown( wxScrollEvent& event )
{
    // Insert custom code here
    event.Skip();
	m_terminal->ScrollTerminal(10, false);
}

/*!
 * wxEVT_SCROLL_THUMBTRACK event handler for ID_SCROLLBAR
 */

void wxTermContainer::OnScrollbarScrollThumbtrack( wxScrollEvent& event )
{
    // Insert custom code here
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool wxTermContainer::ShowToolTips()
{
  return TRUE;
}

void wxTermContainer::SetTerminal(wxTerm* terminal)
{
	m_terminal = terminal;

	m_sizer->Prepend(m_terminal, 1, wxGROW, 0);

	int termHeight = m_terminal->Height();
	m_scrollbar->SetScrollbar(100 - termHeight, termHeight, 100, 10);
}

void wxTermContainer::OnSize(wxSizeEvent &event)
{
	int newHeight = 0;
	int numLinesReceived = 0;

	if(m_terminal != NULL)
	{
		m_terminal->UpdateSize(newHeight, numLinesReceived);
	}
	

}