////////////////////////////////////////////////////////////////////
//
// Notes:
//    gcc || echo token <-- token if gcc had errors
//
////////////////////////////////////////////////////////////////////
#include "compiler.h"


BEGIN_EVENT_TABLE(Compiler, wxEvtHandler)
	EVT_PROCESS2_STDOUT(Compiler::OnProcessOut)
	EVT_PROCESS2_STDERR(Compiler::OnProcessErr)
	EVT_PROCESS2_ENDED(Compiler::OnProcessTerm)
END_EVENT_TABLE()


Compiler::Compiler(Networking* network) {
	m_network = network;
	m_isCompiling = false;
	m_compilePID = -2;
}

void Compiler::SimpleCompileFile(wxString file, wxTextCtrl* textbox) {
	//m_networking->Get??Process(this);
	//m_isCompiling = true;
	wxString s = "Hello from the Compiler!";
	*textbox << s;
}


void Compiler::SimpleCompileProject(ProjectInfo* proj, wxTextCtrl* textbox) {
	//m_networking->Get??Process(this);
	//m_isCompiling = true;
	wxString s = "Hello from the Compiler!";
	*textbox << s;
}


//void Compiler::AdvancedCompileFile(wxString file, CompilerWindow* window) {
//	//
//}


//void Compiler::AdvancedCompileProject(ProjectInfo* proj, CompilerWindow* window) {
//	//
//}


//Private:
void Compiler::OnProcessTerm(wxEvent& e) {
	//
}


void Compiler::OnProcessOut(wxEvent& e) {
	//
}


void Compiler::OnProcessErr(wxEvent& e) {
	//
}


