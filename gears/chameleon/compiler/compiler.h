#ifndef _CHAMEL_COMPILER_H_
#define _CHAMEL_COMPILER_H_

#include <wx/wx.h>
#include <wx/txtstrm.h>
#include "../common/process2.h"
#include "../common/options.h"
#include "../network/networking.h"
#include "../common/projectinfo.h"

class Compiler : public wxEvtHandler
{
	public:
		Compiler(Options* options, Networking* network);

		// Simple Output:
		void CompileFile(wxString path, wxString filename, bool isRemote, wxTextCtrl* textbox);
		void CompileProject(ProjectInfo* proj, bool isRemote, wxTextCtrl* textbox);
		// Advanced Output:
		//void CompileFile(wxString path, wxString file, bool isRemote, CompilerWindow* window);
		//void CompileProject(ProjectInfo* proj, bool isRemote, CompilerWindow* window);

		void SimpleRemoteCompile(wxString path, wxString file, wxTextCtrl* textbox);
		void SimpleRemoteCompile(ProjectInfo* proj, wxTextCtrl* textbox);
		void SimpleLocalCompile(wxString path, wxString file, wxTextCtrl* textbox);
		void SimpleLocalCompile(ProjectInfo* proj, wxTextCtrl* textbox);

	private:
		void OnProcessTerm(wxProcess2EndedEvent& e);
		void OnProcessOut(wxProcess2StdOutEvent& e);
		void OnProcessErr(wxProcess2StdErrEvent& e);

		void CompileRemoteFile(wxString filename, wxString outfile);
		void CompileLocalFile(wxString filename, wxString outfile);

		// Data:
		Options* m_options;
		Networking* m_network;
		wxTextCtrl* m_out;
		wxProcess2* m_proc;
		bool m_isCompiling;
		bool m_receivedToken;
		long m_compilePID;


	DECLARE_EVENT_TABLE()
};

#endif /* _CHAMEL_COMPILER_H_ */
