#ifndef CHAMELEON_NOTEBOOK__H
#define CHAMELEON_NOTEBOOK__H



#include <wx/notebook.h>



class ChameleonWindow;

class ChameleonNotebook : public wxNotebook
{
public:
	ChameleonNotebook(wxWindow* parent, wxWindowID id, 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize, long style = 0, 
						const wxString& name = "notebook");
	~ChameleonNotebook();

	//int HitTest(const wxPoint& pt, long& flags);
	int FindPagePosition(wxNotebookPage* page);

	void OnTabActivate (wxMouseEvent &event);

	void OnSize(wxSizeEvent &event);

private:
	//void CreateBookMenus();

	ChameleonWindow* m_parent;


	DECLARE_EVENT_TABLE()
};

#endif