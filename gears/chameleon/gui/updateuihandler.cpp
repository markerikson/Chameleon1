#include "../common/CommonHeaders.h"

#include "updateuihandler.h"
#include "../editor/stc.h"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UpdateUIHandler::UpdateUIHandler(ChameleonWindow* mf)
{
	mainframe = mf;
}


bool UpdateUIHandler::ProcessEvent(wxEvent& event)
{
	wxEventType et = event.GetEventType();

	/*
	
	if(et == wxEVT_STC_SAVEPOINTREACHED)
	{
		::wxMessageBox("Save point reached message");
	}
	*/
	
	/*
	if(et == wxEVT_STC_CHARADDED)
	{
		::wxMessageBox("Character added");
	}
	*/
	if(et == wxEVT_STC_CHARADDED)
	{
		
		event.Skip();
	}

	return this->wxEvtHandler::ProcessEvent(event);
	

}
