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

void Compiler::SimpleCompileFile(wxString file, wxTextCtrl* textbox)
{
	m_proc = m_network->GetPlinkProcess(this);
	if(m_proc != NULL) {
		wxTextOutputStream os(* (m_proc->GetOutputStream()) );
		os.WriteString("echo S_T_A_R_T_COMPILE ; g++ " + file + " ; echo E_N_D_COMPILE\r");
		m_receivedToken = false;
		m_isCompiling = true;
		m_out = textbox;
		*m_out << "Compiling...\n";
	}
}


void Compiler::SimpleCompileProject(ProjectInfo* proj, wxTextCtrl* textbox)
{
	//
}


//void Compiler::AdvancedCompileFile(wxString file, CompilerWindow* window)
//{
//	//
//}


//void Compiler::AdvancedCompileProject(ProjectInfo* proj, CompilerWindow* window)
//{
//	//
//}


//Private:
void Compiler::OnProcessTerm(wxProcess2EndedEvent& e)
{
	*m_out << "\nDone.";
	m_out = NULL;
	m_isCompiling = false;
}


void Compiler::OnProcessOut(wxProcess2StdOutEvent& e)
{
	if(m_isCompiling) {
		wxString s = e.GetOutput();
		if(!m_receivedToken) {
			if(s.Contains("S_T_A_R_T_COMPILE\r")) {
				s.Remove(0,s.Find("S_T_A_R_T_COMPILE\r")+19);
				m_receivedToken = true;
			}
		}

		if(m_receivedToken) {
			if(s.Contains("E_N_D_COMPILE\r")) {
				s.Remove(s.Find("E_N_D_COMPILE\r"));
				m_isCompiling = false;
				m_receivedToken = false;
				wxTextOutputStream os(* (m_proc->GetOutputStream()) );
				os.WriteString("exit\r");
			}
			
			// Display the output:
			*m_out << s;
		}
	}
	else {
		wxLogDebug("Compiler Recieved Output while not compiling: " + e.GetOutput());
	}

}


void Compiler::OnProcessErr(wxProcess2StdErrEvent& e)
{
	// This means problems with the process (not with gcc) ??
}


