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


Compiler::Compiler(Options* options, Networking* network) {
	m_options = options;
	m_network = network;
	m_isCompiling = false;
	m_compilePID = -2;
}


void Compiler::CompileFile(wxString path, wxString filename, bool isRemote, wxTextCtrl* textbox) {
	m_out = textbox;

	// I really wish I could merge these two:
	if(isRemote) {
		m_proc = m_network->GetPlinkProcess(this);
		if(m_proc != NULL) {
			*m_out << "Compiling...\n";
			wxTextOutputStream os(* (m_proc->GetOutputStream()) );
			os.WriteString("cd " + path + "\r");
			CompileRemoteFile(filename, m_options->GetRemoteCompileOut());
		}
		else {
			wxLogDebug("Compiler could not get a process.");
		}
	}
	else { // Local:
		m_proc = m_network->GetLocalProcess(this);
		if(m_proc != NULL) {
			*m_out << "Compiling...\n";
			wxTextOutputStream os(* (m_proc->GetOutputStream()) );
			os.WriteString("cd " + path + "\r");
			CompileLocalFile(filename, m_options->GetLocalCompileOut());
		}
		else {
			wxLogDebug("Compiler could not get a local process.");
		}
	}
}


void Compiler::CompileProject(ProjectInfo* proj, bool isRemote, wxTextCtrl* textbox) {
	m_out = textbox;

	if(isRemote) {
		// Remote Compilation:
		*m_out << "Can't compile remote projects yet.";

		//walk through the list of files in the project and call SimpleCompileFile on each
	}
	else { // Local Compilation:
		*m_out << "Can't compile local projects yet.";
	}
}


// This assumes that I already have a live process (m_proc)
//Private:
void Compiler::CompileRemoteFile(wxString filename, wxString outfile) {

	wxString blank = ""; // ummm this is weird
	wxString cmd = blank + "echo S_T_A_R_T_COMPILE ; "
		+ " g++ " // compiler
		+ " -g "
		+ " -o " + outfile + " "
		+ filename
		+ " ; echo E_N_D_COMPILE\r";

	wxTextOutputStream os(* (m_proc->GetOutputStream()) );
	os.WriteString(cmd);

	m_receivedToken = false;
	m_isCompiling = true;
}


//Private:
void Compiler::CompileLocalFile(wxString filename, wxString outfile)
{
	//
}


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


