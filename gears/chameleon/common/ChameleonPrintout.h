#ifndef CHAMELEON_PRINTOUT__H
#define CHAMELEON_PRINTOUT__H


#include "wx/print.h"

class ChameleonEditor;

class ChameleonPrintout: public wxPrintout {

public:

	//! constructor
	ChameleonPrintout (ChameleonEditor *edit, wxChar *title = _T(""));

	//! event handlers
	bool OnPrintPage (int page);
	bool OnBeginDocument (int startPage, int endPage);

	//! print functions
	bool HasPage (int page);
	void GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

private:
	ChameleonEditor *m_edit;
	int m_printed;
	wxRect m_pageRect;
	wxRect m_printRect;

	bool PrintScaling (wxDC *dc);
};



#endif