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
    item3->SetForegroundColour(wxColour(0, 0, 0));
    item3->SetBackgroundColour(wxColour(255, 255, 255));
    wxNotebookSizer* item3Sizer = new wxNotebookSizer(item3);
    wxPanel* item4 = new wxPanel( item3, ID_PROGPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item4->SetForegroundColour(wxColour(0, 0, 0));
    item4->SetBackgroundColour(wxColour(255, 255, 255));
    wxBoxSizer* item5 = new wxBoxSizer(wxVERTICAL);
    item4->SetSizer(item5);
    item4->SetAutoLayout(TRUE);
    wxBitmap item6Bitmap(chamlogo_1_xpm);
    wxStaticBitmap* item6 = new wxStaticBitmap( item4, wxID_STATIC, item6Bitmap, wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add(item6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* item7 = new wxStaticText( item4, wxID_STATIC, _("Chameleon\nThe Adaptive Instructional IDE\nversion 1.0 (Final Release)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    item7->SetForegroundColour(wxColour(0, 0, 0));
    item7->SetBackgroundColour(wxColour(255, 255, 255));
    item7->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE, _T("Verdana")));
    item5->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    wxStaticText* item8 = new wxStaticText( item4, wxID_STATIC, _("Chameleon is licensed under the General Public License, \navailable at http://www.gnu.org/licenses/gpl.html"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Arial")));
    item5->Add(item8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);
    item3->AddPage(item4, _("The Program"));
    wxPanel* item9 = new wxPanel( item3, ID_TEAMPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item9->SetForegroundColour(wxColour(255, 255, 255));
    item9->SetBackgroundColour(wxColour(255, 255, 255));
    wxBoxSizer* item10 = new wxBoxSizer(wxVERTICAL);
    item9->SetSizer(item10);
    item9->SetAutoLayout(TRUE);
    wxStaticText* item11 = new wxStaticText( item9, wxID_STATIC, _("Chameleon was brought to you by:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->SetForegroundColour(wxColour(0, 0, 0));
    item11->SetBackgroundColour(wxColour(255, 255, 255));
    item11->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Verdana")));
    item10->Add(item11, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);
    wxBitmap item12Bitmap(team_1_xpm);
    wxStaticBitmap* item12 = new wxStaticBitmap( item9, wxID_STATIC, item12Bitmap, wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* item13 = new wxStaticText( item9, wxID_STATIC, _("Ben Carhart\n - Requirements Lead, Quality Assurance, Testing Lead\n - Debugger, Permissions\nDavid Czechowski\n - Design Lead, Configuration Management \n - Networking, Compiler\nMark Erikson\n - Team Lead, Project Lead\n - GUI, Editor, Terminal"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    item13->SetForegroundColour(wxColour(0, 0, 0));
    item13->SetBackgroundColour(wxColour(255, 255, 255));
    item13->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Verdana")));
    item10->Add(item13, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    item3->AddPage(item9, _("The Team"));
    wxPanel* item14 = new wxPanel( item3, ID_CREDITSPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item14->SetForegroundColour(wxColour(0, 0, 0));
    item14->SetBackgroundColour(wxColour(255, 255, 255));
    wxBoxSizer* item15 = new wxBoxSizer(wxVERTICAL);
    item14->SetSizer(item15);
    item14->SetAutoLayout(TRUE);
    wxStaticText* item16 = new wxStaticText( item14, wxID_STATIC, _("Chameleon wouldn't have been possible without the tools,\nexamples, parts, and programs we were able to use.  \nWe'd like to recognize them here:\n\n* The wxWidgets toolkit, which made development\n   a lot easier than it could have been.\n* Neil Hodgson, author of the Scintilla editor widget.\n* Robin Dunn, who created the wxStyledTextCtrl \n   wrapper for Scintilla.\n* Simon Tatham, author of the incredible Putty suite of \n   SSH clients.  Without his Plink tool, our project \n   would have been effectively impossible.\n* Otto Wyss, creator of the wxGuide example program .\n* Derry Bryson, creator of the wxTerm terminal widget\n   class, and Timothy Miller, who wrote the GTerm \n   core that wxTerm is based on.\n* Jan van de Baard, who wrote some checksum \n   routines that came in handy.\n* Anyone else whose code we ever looked at or used."), wxDefaultPosition, wxDefaultSize, 0 );
    item16->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Arial")));
    item15->Add(item16, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    item3->AddPage(item14, _("The Credits"));
    wxPanel* item17 = new wxPanel( item3, ID_STATSPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    item17->SetForegroundColour(wxColour(0, 0, 0));
    item17->SetBackgroundColour(wxColour(255, 255, 255));
    wxBoxSizer* item18 = new wxBoxSizer(wxVERTICAL);
    item17->SetSizer(item18);
    item17->SetAutoLayout(TRUE);
    wxStaticText* item19 = new wxStaticText( item17, wxID_STATIC, _("* Total lines of code: ~24,000\n   - ~8,300 statements\n   - ~8,000 lines of comments\n* ~820 CVS commmits, totaling 41,000 lines of changes\n* Number of source files: ~100\n* Project length: 221 days (9/18/03 - 4/26/04)\n* Number of times Mark broke CVS: at least 15\n* Number of times Ben had to rewrite the debugger: 4\n* Number of bugs fixed the night before the final \n   presentation: 15\n* Number of bugs Mark introduced the night before the \n   final presentation that showed up during the demo: 2\n* Number of beta releases: 5\n* Number of beta comments we got back: 2\n* Number of times David asked to discuss something\n  over the phone instead of AIM: too many to count\n* Number of all-nighters: technically none\n* Number of times one of us was up past 5:00 AM\n   writing code: we lost track\n* Amount of caffeine consumed: immeasurable"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, FALSE, _T("Arial")));
    item18->Add(item19, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    item3->AddPage(item17, _("The Stats"));
    item2->Add(item3Sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item20 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item20, 0, wxALIGN_RIGHT|wxALL, 0);

    wxButton* item21 = new wxButton( item1, ID_BUTTONOK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item20->Add(item21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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


