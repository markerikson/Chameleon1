#include <wx/event.h>


class ChameleonWindow;

class UpdateUIHandler : public wxEvtHandler
{

public:
	UpdateUIHandler(ChameleonWindow* mf);
	virtual bool ProcessEvent(wxEvent& event);

private:
	ChameleonWindow* mainframe;

};
