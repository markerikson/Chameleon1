#ifndef __DEBUGEVENT__H__
#define __DEBUGEVENT__H__

#include <wx/event.h>
#include <wx/dynarray.h>
#include "datastructures.h"
#include "ProjectInfo.h"

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
		void SetVariableTypes(wxArrayString types);
		//void SetSourceFilenames(wxArrayString filenames);
		void SetSourceFilename(wxString filename);
		void SetErrorMessage(wxString message);
		//void SetRemoteMode(bool remote);
		void SetFileBreakpoints(FileBreakpointHash filebreakpoints);
		void SetProject(ProjectInfo* project);

		int GetLineNumber() { return m_lineNumber;}
		int GetStatus() { return m_status;}
		wxArrayString GetVariableNames() { return m_variableNames; }
		wxArrayString GetVariableValues() { return m_variableValues; }
		wxArrayString GetVariableTypes() { return m_variableTypes; }
		//wxArrayString GetSourceFilenames() { return m_sourceFilenames; }
		wxString GetSourceFilename() { return m_filename; }
		wxString GetErrorMessage() {return m_errorMessage; }
		bool IsRemote() { return m_project->isRemote; }
		FileBreakpointHash GetFileBreakpoints() { return m_filebreakpoints; }
		ProjectInfo* GetProject() { return m_project; }

public:
		int m_lineNumber;
		int m_status;
		wxArrayString m_variableNames;
		wxArrayString m_variableValues;
		wxArrayString m_variableTypes;
		//wxArrayString m_sourceFilenames;
		wxString m_filename;
		wxString m_errorMessage;
		//wxString m_executableFilename;
		bool m_remoteMode;
		FileBreakpointHash m_filebreakpoints;
		ProjectInfo* m_project;


};

typedef void (wxEvtHandler::*wxDebugEventFunction)(wxDebugEvent&);

#define EVT_DEBUG(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
			wxEVT_DEBUG, wxID_ANY, wxID_ANY, \
			(wxObjectEventFunction)(wxEventFunction)(wxDebugEventFunction)&fn, \
			(wxObject *) NULL),


#endif // __WXPROCESS2_EVENTS__H__

