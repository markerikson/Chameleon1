#include "process2.h"

// Event Table Declaration
BEGIN_EVENT_TABLE(wxProcess2, wxProcess)
	EVT_TIMER(-1, wxProcess2::OnTimerEvent)
END_EVENT_TABLE()


wxProcess2::wxProcess2(wxEvtHandler *parent, int id) 
	: wxProcess(parent, id)
{
	m_timer.SetOwner(this);
	m_timer.Start(POLL_RATE);
	Redirect();

	m_pid = -2;
}

wxProcess2::wxProcess2(int flags) 
	: wxProcess(flags)
{
	m_timer.SetOwner(this);
	m_timer.Start(POLL_RATE);
	Redirect();

	m_pid = -2;
}

wxProcess2::~wxProcess2() {
	// !!User should NEVER use delete explicitly if the process is still alive!!
	//  (call Detach() -- And, try to terminate the process on your own)
	if(m_pid != -2) {
	//if(Exists(m_pid)) { <-- this isn't behaving as expected
		wxLogDebug("Delete called on wxProcess2 while process is still alive.  BAD!");
		//Ideally: don't let this complete
		
		//Forcefully try to kill the process (better than leaving it?)
		// This will cause a Mem Access Violation!
		wxProcess::Kill(m_pid, wxSIGKILL); // or wxSIGTERM??
	}
	
	//m_timer goes out of scope
}

// virtual:
void wxProcess2::Detach()
{
	SetEvtHandlerEnabled(false); // don't recieve output-polling-timer events
	m_timer.Stop();

	wxProcess::Detach(); //SetNextHandler(NULL); (events no longer go anywhere)
}

void wxProcess2::OnTimerEvent(wxTimerEvent &e) {
	PollOutputs();
}

//private:
void wxProcess2::PollOutputs() {
	int pid = 0;
	if(IsErrorAvailable()) {
		wxString err;
		while(IsErrorAvailable()) {
			err += m_errorStream->GetC();
		}
		wxProcess2StdErrEvent event(pid, err);
		AddPendingEvent(event);
	}
	if(IsInputAvailable()) {
		wxString out;
		while(IsInputAvailable()) {
			out += m_inputStream->GetC();
		}
		wxProcess2StdOutEvent* event = new wxProcess2StdOutEvent(pid, out);
		AddPendingEvent(*event);
	}
}


void wxProcess2::OnTerminate(int pid, int status)
{
m_pid = -2;
	if(GetNextHandler() != NULL) { // not been Detach()'ed
		SetEvtHandlerEnabled(false); // stop listening for polling-times
		//PollOutputs(); <-- Uncommenting this will cause Mem Acc Violation
		//PollOutputsAsynch(); <-- maybe if it weren't Asynch  ??

		wxProcess2EndedEvent event(pid);

		//AddPendingEvent(event); <-- can't do if I delete myself, or can I ??
		ProcessEvent(event);
	}
	
	// *At this point, (my) Events will no longer be heard by any one*
	
	// For Consistency's sake I (always) delete myself when the process terminates
	delete this;

}


// I think this should be moved into wxProcess!
void wxProcess2::SetPID(long pid) {
	m_pid = pid;
}

