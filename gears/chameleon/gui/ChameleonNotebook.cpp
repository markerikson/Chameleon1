#include "ChameleonNotebook.h"
#include "ChameleonWindow.h"

#include "../common/debug.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_EVENT_TABLE(ChameleonNotebook, wxNotebook)
//EVT_CONTEXT_MENU(ChameleonNotebook::TestMenu)

	EVT_RIGHT_UP ( ChameleonNotebook::OnTabActivate)


END_EVENT_TABLE()

ChameleonNotebook::ChameleonNotebook(wxWindow* parent, wxWindowID id, 
									 const wxPoint& pos /* = wxDefaultPosition */, 
									 const wxSize& size /* = wxDefaultSize */, 
									 long style /* = 0 */, 
									 const wxString& name /* = "notebook" */)
:wxNotebook(parent, id, pos, size, style, name)
{
	m_parent = (ChameleonWindow*)wxTheApp->GetTopWindow();

	// create menu
	m_nonActiveMenu = new wxMenu();
	m_activeMenu = new wxMenu();

	CreateBookMenus();

}

ChameleonNotebook::~ChameleonNotebook () 
{
	delete m_nonActiveMenu;
	delete m_activeMenu;
}

int ChameleonNotebook::HitTest(const wxPoint& pt, long& flags)
{
	TC_HITTESTINFO hitTestInfo;
	hitTestInfo.pt.x = pt.x;
	hitTestInfo.pt.y = pt.y;
	int item = TabCtrl_HitTest( (HWND) GetHWND(), & hitTestInfo ) ;
	flags = 0;

	if ((hitTestInfo.flags & TCHT_NOWHERE) == TCHT_NOWHERE)
		flags |= wxTAB_HITTEST_NOWHERE;
	if ((hitTestInfo.flags & TCHT_ONITEMICON) == TCHT_ONITEMICON)
		flags |= wxTAB_HITTEST_ONICON;
	if ((hitTestInfo.flags & TCHT_ONITEMLABEL) == TCHT_ONITEMLABEL)
		flags |= wxTAB_HITTEST_ONLABEL;

	return item;
}

void ChameleonNotebook::OnTabActivate (wxMouseEvent &event) 
{
//#define TESTMACRO(s) \
//	wxString tempstring(s);  \
//	::wxMessageBox(tempstring);

	

	//TESTMACRO("Testing stuff");
	//::wxMessageBox("OnTabActivate");
	wxPoint pt;
	pt.x = event.GetX();
	pt.y = event.GetY();
	// wxNotebook::HitTest might not be supported
	//#if NOTEBOOK_HITTEST
	long flags = 0;
	int pageNum = this->HitTest (pt, flags);
	if (pageNum < 0) return;
	//m_frame->m_hitNr = pageNr;
	m_parent->SetIntVar(VN_CLICKEDTAB, pageNum);
	/*
	if (pageNum == m_parent->GetIntVar(VN_CURRENTPAGE)) 
	{
		PopupMenu (m_activeMenu, pt);
	}
	else
	{
		PopupMenu (m_nonActiveMenu, pt);
	}
	*/
	PopupMenu(m_activeMenu, pt);
	//#endif
}

void ChameleonNotebook::TestMenu(wxCommandEvent& evt)
{
	::wxMessageBox("Test Menu");
}

void ChameleonNotebook::CreateBookMenus () {

	wxMenuBar *menuBar = m_parent->GetMenuBar();
	wxString label;


	m_nonActiveMenu->Append(-1, "Non-active menu");
	m_activeMenu->Append(ID_CLOSETAB, "Close");
	//m_activeMenu->Append(9997, "Active menu");
	//m_activeMenu->Append(9998, "Active menu - 2");

	// create menu 1
	/*
	if (menuBar->FindItem (wxID_SAVE)) {
		label = menuBar->GetLabel (wxID_SAVE);
		m_bookMenu1->Append (wxID_SAVE, label);
	}
	if (menuBar->FindItem (wxID_SAVEAS)) {
		label = menuBar->GetLabel (wxID_SAVEAS);
		m_bookMenu1->Append (wxID_SAVEAS, label);
	}
	if (menuBar->FindItem (wxID_CLOSE)) {
		label = menuBar->GetLabel (wxID_CLOSE);
		m_bookMenu1->Append (wxID_CLOSE, label);
	}
	m_bookMenu1->AppendSeparator();
	if (menuBar->FindItem (wxID_PRINT)) {
		label = menuBar->GetLabel (wxID_PRINT);
		m_bookMenu1->Append (wxID_PRINT, label);
	}

	// create menu 2
	m_bookMenu2->Append (myID_PAGEACTIVE, _("Activate"));
#if NOTEBOOK_DELETEPAGE
	m_bookMenu2->Append (myID_PAGECLOSE, _("Close"));
#endif
	*/

}

