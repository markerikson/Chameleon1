//////////////////////////////////////////////////////////////////////
//
// wxProcess2Events: wxProcess2EndedEvent, wxProcess2StdOutEvent, &
//                           wxProcess2StdErrEvent
//
// Notes:
//
//////////////////////////////////////////////////////////////////////

#include "DebugEvent.h"

#include "debug.h"

#ifdef _DEBUG

#define new DEBUG_NEW

#endif

//-------------------------------------------------------------------
// wxProcess2EndedEvent
//-------------------------------------------------------------------
DEFINE_EVENT_TYPE(wxEVT_DEBUG)
wxDebugEvent::wxDebugEvent()
{
	wxEvent::m_eventType = wxEVT_DEBUG;
}

void wxDebugEvent::SetLineNumber(int line)
{
	m_lineNumber = line;
}

void wxDebugEvent::SetStatus(int status)
{
	m_status = status;
}

void wxDebugEvent::SetVariableNames(wxArrayString names)
{
	m_variableNames = names;
}

void wxDebugEvent::SetVariableValues(wxArrayString values)
{
	m_variableValues = values;
}

void wxDebugEvent::SetErrorMessage(wxString message)
{
	m_errorMessage = message;
}

void wxDebugEvent::SetExecutableFilename(wxString filename)
{
	m_executableFilename = filename;
}

void wxDebugEvent::SetRemoteMode(bool remote)
{
	m_remoteMode = remote;
}

void wxDebugEvent::SetSourceFilenames(wxArrayString filenames)
{
	m_sourceFilenames = filenames;
}
