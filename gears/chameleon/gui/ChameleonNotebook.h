#ifndef CHAMELEON_NOTEBOOK__H
#define CHAMELEON_NOTEBOOK__H







class ChameleonWindow;

class ChameleonNotebook : public wxNotebook
{
public:
	ChameleonNotebook(wxWindow* parent, wxWindowID id, 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxDefaultSize, long style = 0, 
						const wxString& name = "notebook");
	~ChameleonNotebook();

	int HitTest(const wxPoint& pt, long& flags);
	void TestMenu(wxCommandEvent& evt);

	void OnTabActivate (wxMouseEvent &event);

	void OnSize(wxSizeEvent &event);

private:
	void CreateBookMenus();



	ChameleonWindow* m_parent;

	wxMenu* m_nonActiveMenu;
	wxMenu* m_activeMenu;


	DECLARE_EVENT_TABLE()
};

/*
class AppBook: public wxNotebook {

public:
	//! constructor
	AppBook (wxWindow *parent,
		wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0);

	//! destructor
	~AppBook ();

	// event handlers
	void OnTabActivate (wxMouseEvent &event);

private:
	AppFrame *m_frame;

	// context menu of the tab region
	wxMenu *m_bookMenu1;
	wxMenu *m_bookMenu2;
	void CreateBookMenus ();

	DECLARE_EVENT_TABLE()
};
*/


#endif