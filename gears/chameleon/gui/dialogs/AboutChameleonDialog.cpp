/////////////////////////////////////////////////////////////////////////////
// Name:        AboutChameleonDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     04/20/04 01:22:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "AboutChameleonDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "AboutChameleonDialog.h"

////@begin XPM images
#include "chamlogo_1.xpm"
#include "team_1.xpm"
////@end XPM images

/*!
 * AboutChameleonDialog type definition
 */

IMPLEMENT_CLASS( AboutChameleonDialog, wxDialog )

/*!
 * AboutChameleonDialog event table definition
 */

BEGIN_EVENT_TABLE( AboutChameleonDialog, wxDialog )

////@begin AboutChameleonDialog event table entries
    EVT_BUTTON( ID_BUTTONOK, AboutChameleonDialog::OnButtonOKClick )

////@end AboutChameleonDialog event table entries

END_EVENT_TABLE()

/*!
 * AboutChameleonDialog constructors
 */

AboutChameleonDialog::AboutChameleonDialog( )
{
}

AboutChameleonDialog::AboutChameleonDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * AboutChameleonDialog creator
 */

bool AboutChameleonDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin AboutChameleonDialog member initialisation
////@end AboutChameleonDialog member initialisation

////@begin AboutChameleonDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end AboutChameleonDialog creation
    return TRUE;
}

/*!
 * Control creation for AboutChameleonDialog
 */

void AboutChameleonDialog::CreateControls()
{    
////@begin AboutChameleonDialog content construction

    AboutChameleonDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxNotebook* item3 = new wxNotebook( item1, ID_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_TOP );
    item3->SetForegroundColour(wxColour(255, 255, 255));
    item3->SetBackgroundColour(wxColour(255, 255, 255));
    wxNotebookSizer* item3Sizer = new wxNotebookSizer(item3);
    wxPanel* item4 = new wxPanel( item3, ID_PANEL4, wxDefaultPosition, wxSize(100, 80), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item4->SetForegroundColour(wxColour(255, 255, 255));
    item4->SetBackgroundColour(wxColour(255, 255, 255));
    wxBoxSizer* item5 = new wxBoxSizer(wxVERTICAL);
    item4->SetSizer(item5);
    item4->SetAutoLayout(TRUE);
    wxBitmap item6Bitmap(chamlogo_1_xpm);
    wxStaticBitmap* item6 = new wxStaticBitmap( item4, wxID_STATIC, item6Bitmap, wxDefaultPosition, wxSize(225, 142), 0 );
    item5->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* item7 = new wxStaticText( item4, wxID_STATIC, _("Chameleon\nThe Adaptive Instructional IDE\nversion 0.99\nSenior Presentation Release"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    item7->SetForegroundColour(wxColour(0, 0, 0));
    item7->SetBackgroundColour(wxColour(255, 255, 255));
    item7->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, _T("Verdana")));
    item5->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    item3->AddPage(item4, _("The Program"));
    wxPanel* item8 = new wxPanel( item3, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item8->SetForegroundColour(wxColour(255, 255, 255));
    item8->SetBackgroundColour(wxColour(255, 255, 255));
    wxBoxSizer* item9 = new wxBoxSizer(wxVERTICAL);
    item8->SetSizer(item9);
    item8->SetAutoLayout(TRUE);
    wxStaticText* item10 = new wxStaticText( item8, wxID_STATIC, _("Chameleon was brought to you by:"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->SetForegroundColour(wxColour(0, 0, 0));
    item10->SetBackgroundColour(wxColour(255, 255, 255));
    item10->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Verdana")));
    item9->Add(item10, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxBitmap item11Bitmap(team_1_xpm);
    wxStaticBitmap* item11 = new wxStaticBitmap( item8, wxID_STATIC, item11Bitmap, wxDefaultPosition, wxSize(340, 175), 0 );
    item9->Add(item11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* item12 = new wxStaticText( item8, wxID_STATIC, _("Ben Carhart\n - Requirements Lead, Quality Assurance, Testing Lead\n - Debugger, Permissions\nDavid Czechowski\n - Design Lead, Configuration Management \n - Networking, Compiler\nMark Erikson\n - Team Lead, Project Lead\n - GUI, Editor, Terminal"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    item12->SetForegroundColour(wxColour(0, 0, 0));
    item12->SetBackgroundColour(wxColour(255, 255, 255));
    item12->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Verdana")));
    item9->Add(item12, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    item3->AddPage(item8, _("The Team"));
    wxPanel* item13 = new wxPanel( item3, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item13->SetForegroundColour(wxColour(255, 255, 255));
    item13->SetBackgroundColour(wxColour(255, 255, 255));
    item3->AddPage(item13, _("The Credits"));
    wxPanel* item14 = new wxPanel( item3, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item14->SetForegroundColour(wxColour(255, 255, 255));
    item14->SetBackgroundColour(wxColour(255, 255, 255));
    item3->AddPage(item14, _("The Stats"));
    wxPanel* item15 = new wxPanel( item3, ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item15->SetForegroundColour(wxColour(255, 255, 255));
    item15->SetBackgroundColour(wxColour(255, 255, 255));
    item3->AddPage(item15, _("The Quotes"));
    item2->Add(item3Sizer, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item16 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item16, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* item17 = new wxButton( item1, ID_BUTTONOK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add(item17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    this->SetBackgroundColour(wxColour(255, 255, 255));
////@end AboutChameleonDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool AboutChameleonDialog::ShowToolTips()
{
    return TRUE;
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONOK
 */

void AboutChameleonDialog::OnButtonOKClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();
	EndModal(wxOK);
}


