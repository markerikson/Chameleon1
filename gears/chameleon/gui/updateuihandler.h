#include <wx/event.h>
#include "../editor/stc.h"

class ChameleonWindow;

class UpdateUIHandler : public wxEvtHandler
{

public:
	UpdateUIHandler(ChameleonWindow* mf);
	virtual bool ProcessEvent(wxEvent& event);

private:
	ChameleonWindow* mainframe;

};
