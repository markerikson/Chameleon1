#include "compilerevent.h"


DEFINE_EVENT_TYPE(wxEVT_COMPILER_ENDED)

wxCompilerEndedEvent::wxCompilerEndedEvent(bool isSuccessful, wxFileName outfile)
{
	m_success = isSuccessful;
	m_resultFile = outfile;
}


bool wxCompilerEndedEvent::wasSuccessful()
{
	return m_success;
}

wxFileName wxCompilerEndedEvent::GetExecFile() {
	return m_resultFile;
}