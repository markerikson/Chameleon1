#include "ChameleonPrintout.h"
#include "../editor/editor.h"
#include "wx/printdlg.h"  // wxPageSetupDialog
#include "wx/progdlg.h"   // wxProgressDialog

#include "debug.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern wxPrintData *g_printData;
extern wxPageSetupData *g_pageSetupData;


//////////////////////////////////////////////////////////////////////////////
///  public constructor ChameleonPrintout
///  <TODO: insert text here>
///
///  @param  edit  ChameleonEditor * <TODO: insert text here>
///  @param  title wxChar *          [=""] <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
ChameleonPrintout::ChameleonPrintout (ChameleonEditor *edit, wxChar *title)
: wxPrintout(title) {
	m_edit = edit;
	m_printed = 0;

}

//////////////////////////////////////////////////////////////////////////////
///  public OnPrintPage
///  <TODO: insert text here>
///
///  @param  page int  <TODO: insert text here>
///
///  @return bool <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonPrintout::OnPrintPage (int page) {

	wxDC *dc = GetDC();
	if (!dc) return false;

	// scale DC
	PrintScaling (dc);

	// TODO Need to figure out how to preview the appropriate page...
	// right now it just does from the start
	// print page
	if (page == 1) m_printed = 0;
	m_printed = m_edit->FormatRange (1, m_printed, m_edit->GetLength(),
		dc, dc, m_printRect, m_pageRect);

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  public OnBeginDocument
///  <TODO: insert text here>
///
///  @param  startPage int  <TODO: insert text here>
///  @param  endPage   int  <TODO: insert text here>
///
///  @return bool      <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonPrintout::OnBeginDocument (int startPage, int endPage) {

	m_printed = 0;
	if (!wxPrintout::OnBeginDocument (startPage, endPage)) {
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
///  public GetPageInfo
///  <TODO: insert text here>
///
///  @param  minPage     int * <TODO: insert text here>
///  @param  maxPage     int * <TODO: insert text here>
///  @param  selPageFrom int * <TODO: insert text here>
///  @param  selPageTo   int * <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonPrintout::GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) {

	// initialize values
	*minPage = 0;
	*maxPage = 0;
	*selPageFrom = 0;
	*selPageTo = 0;

	// scale DC if possible
	wxDC *dc = GetDC();
	if (!dc) return;
	PrintScaling (dc);

	// get print page informations and convert to printer pixels
	wxSize ppiScr;
	GetPPIScreen (&ppiScr.x, &ppiScr.y);
	wxSize page = g_pageSetupData->GetPaperSize();
	page.x = static_cast<int> (page.x * ppiScr.x / 25.4);
	page.y = static_cast<int> (page.y * ppiScr.y / 25.4);
	m_pageRect = wxRect (0,
		0,
		page.x,
		page.y);

	// get margins informations and convert to printer pixels
	int  top = 25; // default 25
	int  bottom = 25; // default 25
	int  left = 20; // default 20
	int  right = 20; // default 20
	wxPoint (top, left) = g_pageSetupData->GetMarginTopLeft();
	wxPoint (bottom, right) = g_pageSetupData->GetMarginBottomRight();
	top = static_cast<int> (top * ppiScr.y / 25.4);
	bottom = static_cast<int> (bottom * ppiScr.y / 25.4);
	left = static_cast<int> (left * ppiScr.x / 25.4);
	right = static_cast<int> (right * ppiScr.x / 25.4);
	m_printRect = wxRect (left,
		top,
		page.x - (left + right),
		page.y - (top + bottom));

	// count pages
	while (HasPage (*maxPage)) {
		m_printed = m_edit->FormatRange (0, m_printed, m_edit->GetLength(),
			dc, dc, m_printRect, m_pageRect);
		*maxPage += 1;
	}
	if (*maxPage > 0) *minPage = 1;
	*selPageFrom = *minPage;
	*selPageTo = *maxPage;
}

//////////////////////////////////////////////////////////////////////////////
///  public HasPage
///  <TODO: insert text here>
///
///  @param  page int  <TODO: insert text here>
///
///  @return bool <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonPrintout::HasPage (int WXUNUSED(page)) {

	return (m_printed < m_edit->GetLength());
}

//////////////////////////////////////////////////////////////////////////////
///  private PrintScaling
///  <TODO: insert text here>
///
///  @param  dc   wxDC * <TODO: insert text here>
///
///  @return bool <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonPrintout::PrintScaling (wxDC *dc){

	// check for dc, return if none
	if (!dc) return false;

	// get printer and screen sizing values
	wxSize ppiScr;
	GetPPIScreen (&ppiScr.x, &ppiScr.y);
	if (ppiScr.x == 0) { // most possible guess 96 dpi
		ppiScr.x = 96;
		ppiScr.y = 96;
	}
	wxSize ppiPrt;
	GetPPIPrinter (&ppiPrt.x, &ppiPrt.y);
	if (ppiPrt.x == 0) { // scaling factor to 1
		ppiPrt.x = ppiScr.x;
		ppiPrt.y = ppiScr.y;
	}
	wxSize dcSize = dc->GetSize();
	wxSize pageSize;
	GetPageSizePixels (&pageSize.x, &pageSize.y);

	// set user scale
	float scale_x = (float)(ppiPrt.x * dcSize.x) /
		(float)(ppiScr.x * pageSize.x);
	float scale_y = (float)(ppiPrt.y * dcSize.y) /
		(float)(ppiScr.y * pageSize.y);
	dc->SetUserScale (scale_x, scale_y);

	return true;
}

