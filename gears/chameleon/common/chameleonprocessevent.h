//////////////////////////////////////////////////////////////////////
//
//   ChameleonProcessEvent
//
//////////////////////////////////////////////////////////////////////

#ifndef __CHAMELEON_PROCESS_EVENT__H__
#define __CHAMELEON_PROCESS_EVENT__H__

#include <wx/process.h>
#include <wx/event.h>

// -------------------------------------------------------------------
// ProcessEvent
// -------------------------------------------------------------------
DECLARE_EVENT_TYPE(chEVT_PROCESS_ENDED, wxID_ANY)
DECLARE_EVENT_TYPE(chEVT_PROCESS_STDOUT, wxID_ANY)
DECLARE_EVENT_TYPE(chEVT_PROCESS_STDERR, wxID_ANY)


class ChameleonProcessEvent : public wxEvent
{
	public:
		ChameleonProcessEvent(wxEventType eventtype);

		void SetInt(int i) {m_int = i; }
		void SetString(wxString s) { m_string = s; }
		int GetInt() { return m_int; }
		wxString GetString() { return m_string; }

		virtual wxEvent *Clone() const { return new ChameleonProcessEvent(*this); }

	private:
		//int m_type;
		int m_int;
		wxString m_string;

};

typedef void (wxEvtHandler::*ChameleonProcessEventFunction)(ChameleonProcessEvent&);

#define EVT_PROCESS_ENDED(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			chEVT_PROCESS_ENDED, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(ChameleonProcessEventFunction)&fn, \
			(wxObject *) NULL),

#define EVT_PROCESS_STDOUT(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			chEVT_PROCESS_STDOUT, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(ChameleonProcessEventFunction)&fn, \
			(wxObject *) NULL),

#define EVT_PROCESS_STDERR(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			chEVT_PROCESS_STDERR, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(ChameleonProcessEventFunction)&fn, \
			(wxObject *) NULL),


#endif // __CHAMELEON_PROCESS_EVENT__H__