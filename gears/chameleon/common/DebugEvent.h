#ifndef __DEBUGEVENT__H__
#define __DEBUGEVENT__H__

#include <wx/event.h>
#include <wx/dynarray.h>


DECLARE_EVENT_TYPE(wxEVT_DEBUG, wxID_ANY)


class wxDebugEvent : public wxEvent
{
	public:
		wxDebugEvent();
		virtual wxEvent *Clone() const { return new wxDebugEvent(*this); }

		void SetLineNumber(int line);
		void SetStatus(int status);
		void SetVariableNames(wxArrayString names);
		void SetVariableValues(wxArrayString values);
		void SetSourceFilenames(wxArrayString filenames);
		void SetErrorMessage(wxString message);
		void SetExecutableFilename(wxString filename);
		void SetRemoteMode(bool remote);

		int GetLineNumber() { return m_lineNumber;}
		int GetStatus() { return m_status;}
		wxArrayString GetVariableNames() { return m_variableNames; }
		wxArrayString GetVariableValues() { return m_variableValues; }
		wxArrayString GetSourceFilenames() { return m_sourceFilenames; }
		wxString GetErrorMessage() {return m_errorMessage; }
		wxString GetExecutableFilename() { return m_executableFilename;}
		bool IsRemote() { return m_remoteMode; }

public:
		int m_lineNumber;
		int m_status;
		wxArrayString m_variableNames;
		wxArrayString m_variableValues;
		wxArrayString m_sourceFilenames;
		wxString m_errorMessage;
		wxString m_executableFilename;
		bool m_remoteMode;


};

typedef void (wxEvtHandler::*wxDebugEventFunction)(wxDebugEvent&);

#define EVT_DEBUG(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			wxEVT_DEBUG, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(wxDebugEventFunction)&fn, \
			(wxObject *) NULL),


#endif // __WXPROCESS2_EVENTS__H__

