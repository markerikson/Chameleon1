#ifndef _CHAMEL_COMPILER_H_
#define _CHAMEL_COMPILER_H_

#include <wx/wx.h>
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
		void OnProcessTerm(wxEvent& e);
		void OnProcessOut(wxEvent& e);
		void OnProcessErr(wxEvent& e);

		// Data:
		Networking* m_network;
		bool m_isCompiling;
		long m_compilePID;


	DECLARE_EVENT_TABLE()
};

#endif /* _CHAMEL_COMPILER_H_ */
