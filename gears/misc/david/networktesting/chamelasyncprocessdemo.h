#ifndef __CHAMEL_ASYNC_PROCESS_DEMO__H__
#define __CHAMEL_ASYNC_PROCESS_DEMO__H__
//=================================================================
// ChamelAsyncProcessDemo
//
//  This is a demo class to imitate the debuggers event-driven-ness
//    and also to demo proper wxProcess Asynchronosity
//
//=================================================================
#include <wx/event.h>
#include <wx/textctrl.h>
#include <wx/txtstrm.h>
#include <wx/log.h>  // debug/log window
#include "../gears_chameleon/common/process2.h"
#include "../gears_chameleon/common/process2events.h"


class ChamelAsyncProcessDemo : public wxEvtHandler {

	public:
		// ..structors:
		ChamelAsyncProcessDemo(wxTextCtrl* command);
		~ChamelAsyncProcessDemo();

		// Methods:
		void putStdIn(wxString strng);

		bool isLive;

	private:
		// Event Triggered(internal use):
		void onProcessOutputEvent(wxProcess2StdOutEvent &e);
		void onProcessErrOutEvent(wxProcess2StdErrEvent &e);
		//void onProcessTermEvent(wxProcessEvent &e);
		void onProcessTermEvent(wxProcess2EndedEvent &e);

		// Methods:
		void startProcess();

		// Attributes:
		wxProcess2* proc;
		long pid;
		wxTextCtrl* outputScreen;


	
	DECLARE_EVENT_TABLE()
};


#endif //__CHAMEL_ASYNC_PROCESS_DEMO__H__
