#include "updateuihandler.h"

UpdateUIHandler::UpdateUIHandler(ChameleonWindow* mf)
{
	mainframe = mf;
}


bool UpdateUIHandler::ProcessEvent(wxEvent& event)
{
	/*
	wxEventType et = event.GetEventType();
	if(et == wxEVT_STC_SAVEPOINTREACHED)
	{
		::wxMessageBox("Save point reached message");
	}
	*/
	return this->wxEvtHandler::ProcessEvent(event);
	

}
