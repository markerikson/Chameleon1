#ifndef __COMPILER__ENDED__EVENT__
#define __COMPILER__ENDED__EVENT__

#include <wx/event.h>
#include <wx/filename.h>
#include "../common/debug.h"

DECLARE_EVENT_TYPE(wxEVT_COMPILER_ENDED, wxID_ANY)


class wxCompilerEndedEvent : public wxEvent
{
	public:
		wxCompilerEndedEvent(bool isSuccessful, wxFileName outfile);
		bool wasSuccessful();
		wxFileName GetExecFile();
		virtual wxEvent *Clone() const { return new wxCompilerEndedEvent(*this); }

	public:
		bool m_success;
		wxFileName m_resultFile;

};

typedef void (wxEvtHandler::*wxCompilerEndedEventFunction)(wxCompilerEndedEvent&);

#define EVT_COMPILER_ENDED(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			wxEVT_COMPILER_ENDED, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(wxCompilerEndedEventFunction)&fn, \
			(wxObject *) NULL),


#endif  // __COMPILER__ENDED__EVENT__

