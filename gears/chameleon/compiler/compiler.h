#ifndef _CHAMEL_COMPILER_H_
#define _CHAMEL_COMPILER_H_

#include <wx/wx.h>
#include <wx/txtstrm.h>
#include "../common/process2.h"
#include "../network/networking.h"
#include "../common/projectinfo.h"

class Compiler : public wxEvtHandler
{
	public:
		Compiler(Networking* network);

		void SimpleCompileFile(wxString file, wxTextCtrl* textbox);
		void SimpleCompileProject(ProjectInfo* proj, wxTextCtrl* textbox);
		//void AdvancedCompileFile(wxString file, CompilerWindow* window);
		//void AdvancedCompileProject(ProjectInfo* proj, CompilerWindow* window);

	private:
		void OnProcessTerm(wxProcess2EndedEvent& e);
		void OnProcessOut(wxProcess2StdOutEvent& e);
		void OnProcessErr(wxProcess2StdErrEvent& e);

		// Data:
		Networking* m_network;
		wxTextCtrl* m_out;
		wxProcess2* m_proc;
		bool m_isCompiling;
		bool m_receivedToken;
		long m_compilePID;


	DECLARE_EVENT_TABLE()
};

#endif /* _CHAMEL_COMPILER_H_ */
