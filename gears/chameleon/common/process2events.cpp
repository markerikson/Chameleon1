//////////////////////////////////////////////////////////////////////
//
// wxProcess2Events: wxProcess2EndedEvent, wxProcess2StdOutEvent, &
//                           wxProcess2StdErrEvent
//
// Notes:
//
//////////////////////////////////////////////////////////////////////

#include "process2events.h"

//-------------------------------------------------------------------
// wxProcess2EndedEvent
//-------------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_PROCESS2_ENDED);

wxProcess2EndedEvent::wxProcess2EndedEvent(long pid)
{
	wxEvent::m_eventType = wxEVT_PROCESS2_ENDED;
	m_long_pid = pid;
}


long wxProcess2EndedEvent::GetPid()
{
	return m_long_pid;
}

//-------------------------------------------------------------------
// wxProcess2StdOutEvent
//-------------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_PROCESS2_STDOUT);

wxProcess2StdOutEvent::wxProcess2StdOutEvent(long pid, wxString out)
{
	wxEvent::m_eventType = wxEVT_PROCESS2_STDOUT;
	m_long_pid = pid;
	m_output = out;
}


long wxProcess2StdOutEvent::GetPid()
{
	return m_long_pid;
}


wxString wxProcess2StdOutEvent::GetOutput()
{
	return m_output;
}


//-------------------------------------------------------------------
// wxProcess2StdErrEvent
//-------------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_PROCESS2_STDERR);

wxProcess2StdErrEvent::wxProcess2StdErrEvent(long pid, wxString err)
{
	wxEvent::m_eventType = wxEVT_PROCESS2_STDERR;
	m_long_pid = pid;
	m_error = err;
}


long wxProcess2StdErrEvent::GetPid()
{
	return m_long_pid;
}


wxString wxProcess2StdErrEvent::GetError()
{
	return m_error;
}