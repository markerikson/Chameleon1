#ifndef _CHAMEL_COMPILER_H_
#define _CHAMEL_COMPILER_H_

#include <wx/wx.h>
#include <wx/txtstrm.h>
#include <wx/filename.h>
#include "../common/options.h"
#include "../network/networking.h"
#include "../common/projectinfo.h"
#include "../common/chameleonprocessevent.h"

class wxEvtHandler;

class Compiler : public wxEvtHandler
{
	public:
		Compiler(Options* options, Networking* network);

		// Simple Output:
		void CompileFile(wxFileName file, bool isRemote, wxTextCtrl* textbox, wxEvtHandler* h);
		void CompileProject(ProjectInfo* proj, bool isRemote, wxTextCtrl* textbox, wxEvtHandler* h);
		// Advanced Output:
		//void CompileFile(wxString path, wxString file, bool isRemote, CompilerWindow* window);
		//void CompileProject(ProjectInfo* proj, bool isRemote, CompilerWindow* window);

		void SimpleRemoteCompile(wxString path, wxString file, wxTextCtrl* textbox);
		void SimpleRemoteCompile(ProjectInfo* proj, wxTextCtrl* textbox);
		void SimpleLocalCompile(wxString path, wxString file, wxTextCtrl* textbox);
		void SimpleLocalCompile(ProjectInfo* proj, wxTextCtrl* textbox);

		bool IsCompiling() { return m_isCompiling;}

	private:
		//Helper:
		void StartNext();
		void CompileRemoteFile(wxFileName file, wxFileName outfile);
		void CompileLocalFile(wxFileName file, wxFileName outfile);

		//Events:
		void OnProcessTerm(ChameleonProcessEvent& e);
		void OnProcessOut(ChameleonProcessEvent& e);
		void OnProcessErr(ChameleonProcessEvent& e);

		// Data:
		Options* m_options;
		Networking* m_network;
		wxTextCtrl* m_out;
		wxTextOutputStream* m_compilerStdIn; // only used for ^C
		bool m_isCompiling;
		bool m_receivedToken;
		long m_compilePID;
		wxFileName m_currOutfile;
		wxEvtHandler* m_handler;


	DECLARE_EVENT_TABLE()
};

#endif /* _CHAMEL_COMPILER_H_ */
