#include "processevents.h"
#include "../common/debug.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

DEFINE_EVENT_TYPE(EVT_PROCESS_ENDED)
DEFINE_EVENT_TYPE(EVT_PROCESS_STDOUT)
DEFINE_EVENT_TYPE(EVT_PROCESS_STDERR)

ProcessEndedEvent::ProcessEndedEvent(int exitcode) : wxEvent(0, EVT_PROCESS_ENDED)
{
	m_exitcode = exitcode;
}
ProcessStdOutEvent::ProcessStdOutEvent(wxString output) : wxEvent(0, EVT_PROCESS_STDOUT)
{
	m_output = output;
}
ProcessStdErrEvent::ProcessStdErrEvent(wxString error) : wxEvent(0, EVT_PROCESS_STDERR)
{
	m_error = error;
}
