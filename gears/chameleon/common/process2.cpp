#include "process2.h"

// Event Table Declaration
BEGIN_EVENT_TABLE(wxProcess2, wxEvtHandler)
	EVT_TIMER(-1, wxProcess2::OnTimerEvent)
END_EVENT_TABLE()


wxProcess2::wxProcess2(wxEvtHandler *parent, int id) 
	: wxProcess(parent, id)
{
	m_timer.SetOwner(this);
	m_timer.Start(POLL_RATE);
	Redirect();
}

wxProcess2::wxProcess2(int flags) 
	: wxProcess(flags)
{
	m_timer.SetOwner(this);
	m_timer.Start(POLL_RATE);
	Redirect();
}

wxProcess2::~wxProcess2() 
{
	//SetEvtHandlerEnabled(false); // don't recieve output-polling-timer events
	//m_timer.Stop(); // done when out-scoped/deleted
	//delete m_timer;
}

void wxProcess2::OnTimerEvent(wxTimerEvent &e) {
	PollOutputs();
}

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
	SetEvtHandlerEnabled(false); // stop listening for polling-times
	// Poll Outputs one last time   <-------------- buggy?
	//PollOutputs();

	wxProcess2EndedEvent event(pid);
	//wxProcessEvent event(m_id, pid, status);

	// One approach:  ('this' deletes itself)
	ProcessEvent(event);
	delete this;
	//Synchronous

	// Second approach: (owner responsible for deleting 'this')
	//AddPendingEvent(event);
	//Asynchronous -- But, it appears buggy!?
}

