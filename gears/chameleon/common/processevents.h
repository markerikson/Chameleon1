//////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCESS_EVENTS__H__
#define __PROCESS_EVENTS__H__

#include <wx/process.h>
#include <wx/event.h>

// -------------------------------------------------------------------
// ProcessEndedEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(EVT_PROCESS_ENDED, wxID_ANY)


class ProcessEndedEvent : public wxEvent
{
	public:
		ProcessEndedEvent(int exitcode);
		int GetExitCode() { return m_exitcode; }
		virtual wxEvent *Clone() const { return new ProcessEndedEvent(*this); }

	private:
		int m_exitcode;

};

typedef void (wxEvtHandler::*ProcessEndedEventFunction)(ProcessEndedEvent&);

#define EVT_PROCESS_ENDED(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			EVT_PROCESS_ENDED, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(ProcessEndedEventFunction)&fn, \
			(wxObject *) NULL),



// -------------------------------------------------------------------
// ProcessStdOutEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(EVT_PROCESS_STDOUT, wxID_ANY)


class ProcessStdOutEvent : public wxEvent
{
	public:
		ProcessStdOutEvent(wxString output);
		wxString GetOutput() { return m_output; }
		virtual wxEvent *Clone() const { return new ProcessStdOutEvent(*this); }

	private:
		wxString m_output;

};

typedef void (wxEvtHandler::*ProcessStdOutEventFunction)(ProcessStdOutEvent&);

#define EVT_PROCESS_STDOUT(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			EVT_PROCESS_STDOUT, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(ProcessStdOutEventFunction)&fn, \
			(wxObject *) NULL),



// -------------------------------------------------------------------
// ProcessStdErrEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(EVT_PROCESS_STDERR, wxID_ANY)


class ProcessStdErrEvent : public wxEvent
{
	public:
		ProcessStdErrEvent(wxString error);
		wxString GetError() { return m_error; }
		virtual wxEvent *Clone() const { return new ProcessStdErrEvent(*this); }

	private:
		wxString m_error;

};

typedef void (wxEvtHandler::*ProcessStdErrEventFunction)(ProcessStdErrEvent&);

#define EVT_PROCESS_STDERR(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			EVT_PROCESS_STDERR, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(ProcessStdErrEventFunction)&fn, \
			(wxObject *) NULL),



#endif

