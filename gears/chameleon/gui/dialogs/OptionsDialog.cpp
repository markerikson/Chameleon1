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
#include "wx/grid.h"
////@end includes

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

    wxNotebook* item3 = new wxNotebook( item1, ID_NOTEBOOK, wxDefaultPosition, wxSize(400, 260), wxNB_TOP );
    wxPanel* item4 = new wxPanel( item3, ID_PANEL, wxDefaultPosition, wxSize(100, 100), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* item5 = new wxBoxSizer(wxHORIZONTAL);
    item4->SetSizer(item5);
    item4->SetAutoLayout(TRUE);
    wxBoxSizer* item6 = new wxBoxSizer(wxVERTICAL);
    item5->Add(item6, 0, wxALIGN_TOP|wxALL, 0);
    wxGrid* item7 = new wxGrid( item4, ID_GRID, wxDefaultPosition, wxSize(385, 231), wxSUNKEN_BORDER );
    m_grid = item7;
    item7->SetDefaultColSize(100);
    item7->SetDefaultRowSize(16);
    item7->SetColLabelSize(20);
    item7->SetRowLabelSize(160);
    item7->CreateGrid(16, 2, wxGrid::wxGridSelectCells);
    item6->Add(item7, 0, wxGROW|wxALL, 0);
    item3->AddPage(item4, _("Features"));
    item2->Add(item3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item8, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* item9 = new wxButton( item1, ID_BUTTON, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add(item9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item10 = new wxButton( item1, ID_BUTTON1, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add(item10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end OptionsDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool OptionsDialog::ShowToolTips()
{
  return TRUE;
}

wxGrid* OptionsDialog::GetGrid()
{
    return this->m_grid;
}