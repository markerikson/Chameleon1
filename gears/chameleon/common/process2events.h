//////////////////////////////////////////////////////////////////////
//
// wxProcess2Events: wxProcess2EndedEvent, wxProcess2StdOutEvent, &
//                           wxProcess2StdErrEvent
//
// Notes:  All of these are _very_ similar.  But, I have chosen to 
//           split them into separate classes so that the any of them
//           can be ignored by a listener, but not attaching
//
// ToDo: I should have an EndStatus in the EndedEvent (like wxProcessEvent)
//
//////////////////////////////////////////////////////////////////////

#ifndef __WXPROCESS2_EVENTS__H__
#define __WXPROCESS2_EVENTS__H__

#include <wx/process.h>
#include <wx/event.h>

// -------------------------------------------------------------------
// wxProcess2EndedEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(wxEVT_PROCESS2_ENDED, wxID_ANY)


class wxProcess2EndedEvent : public wxEvent
{
	public:
		wxProcess2EndedEvent(long pid);
		long GetPid();
		virtual wxEvent *Clone() const { return new wxProcess2EndedEvent(*this); }

	public:
		long m_long_pid;

};

typedef void (wxEvtHandler::*wxProcess2EndedEventFunction)(wxProcess2EndedEvent&);
#define EVT_PROCESS2_ENDED(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			wxEVT_PROCESS2_ENDED, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)&fn, \
			(wxObject *) NULL),



// -------------------------------------------------------------------
// wxProcess2StdOutEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(wxEVT_PROCESS2_STDOUT, wxID_ANY)


class wxProcess2StdOutEvent : public wxEvent
{
	public:
		wxProcess2StdOutEvent(long pid, wxString out);
		long GetPid();
		wxString GetOutput();
		virtual wxEvent *Clone() const { return new wxProcess2StdOutEvent(*this); }

	public:
		wxString m_output;
		long m_long_pid;

};
//typedef void (wxEvtHandler::*wxStyledTextEventFunction)(wxStyledTextEvent&);
typedef void (wxEvtHandler::*wxProcess2StdOutEventFunction)(wxProcess2StdOutEvent&);

#define EVT_PROCESS2_STDOUT(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			wxEVT_PROCESS2_STDOUT, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(wxProcess2StdOutEventFunction)&fn, \
			(wxObject *) NULL),




// -------------------------------------------------------------------
// wxProcess2StdErrEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(wxEVT_PROCESS2_STDERR, wxID_ANY)


class wxProcess2StdErrEvent : public wxEvent
{
	public:
		wxProcess2StdErrEvent(long pid, wxString err);
		long GetPid();
		wxString GetError();
		virtual wxEvent *Clone() const { return new wxProcess2StdErrEvent(*this); }

	public:
		wxString m_error;
		long m_long_pid;

};

typedef void (wxEvtHandler::*wxProcess2StdErrEventFunction)(wxProcess2StdErrEvent&);
#define EVT_PROCESS2_STDERR(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			wxEVT_PROCESS2_STDERR, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)&fn, \
			(wxObject *) NULL),




#endif // __WXPROCESS2_EVENTS__H__

