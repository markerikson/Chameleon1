////////////////////////////////////////////////////////////////////
//
// Notes:
//    gcc || echo token <-- token if gcc had errors
//
////////////////////////////////////////////////////////////////////
#include "compiler.h"
#include "compilerevent.h"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


void Compiler::CompileFile(wxFileName file, bool isRemote, wxTextCtrl* textbox, wxEvtHandler* h) {
	m_out = textbox;
	SetNextHandler(h);
	wxString outfile = file.GetName();

	// Start a process and point m_proc at it:
	if(isRemote) {
		m_proc = m_network->GetPlinkProcess(this);
		outfile += ".out"; // extension
	}
	else { // Local:
		m_proc = m_network->GetLocalProcess(this);
		outfile += ".exe"; // extension
	}

	// Signal the user, and get going
	if(m_proc != NULL) {
		*m_out << "Compiling...\n";
		*m_out << file.GetFullName() << "\n";

		//Unfortunately the shells are not identical:
		if(isRemote) {
			CompileRemoteFile(file, outfile);
		}
		else { // isLocal
			CompileLocalFile(file, outfile);
		}
	}
	else {
		*m_out << "Error: Could not start a process to begin compiling.\n";

		// Remember, for a remote compilation to be started, the person needs to
		//   have a remote file open (aka. successful user/pw/connection)

#ifdef _DEBUG
		if(isRemote)
			wxLogDebug("Compiler could not get a remote process.");
		else
			wxLogDebug("Compiler could not get a local process.");
#endif
	}

}


void Compiler::CompileProject(ProjectInfo* proj, bool isRemote, wxTextCtrl* textbox, wxEvtHandler* h) {
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
void Compiler::CompileRemoteFile(wxFileName file, wxString outfile)
{
	wxString cmd;
	wxString blank = ""; // oddity
	wxTextOutputStream os( *(m_proc->GetOutputStream()) );

	// Enter appropriate directory:
	cmd = "cd " + file.GetPath(0, wxPATH_UNIX) + "\r";
	os.WriteString(cmd); // SEND


	cmd = blank + "echo S_T_A_R_T_COMPILE ; "
		+ " g++ " // compiler (assuming it is in the PATH)
		+ " -g "
		+ " -o " + outfile + " "
		+ file.GetFullName()
		+ " && echo C_O_M_P_I_L_E_SUCCESS ; echo E_N_D_COMPILE\r";
	os.WriteString(cmd); // SEND

	m_receivedToken = false;
	m_isCompiling = true;
}


// This assumes that I already have a live process (m_proc)
//Private:
void Compiler::CompileLocalFile(wxFileName file, wxString outfile)
{
	wxString cmd;
	wxString blank = ""; // oddity
	wxTextOutputStream os( *(m_proc->GetOutputStream()) );

	// Enter appropriate directory:
	cmd = file.GetVolume() + ":\n"; // change to the drive
	cmd += "cd " + file.GetPath(0, wxPATH_DOS) + "\n";
	os.WriteString(cmd); // SEND


	cmd = blank + "echo S_T_A_R_T_COMPILE & "
		+ "\"" + m_options->GetMingwPath() + "g++.exe\" " // compiler
		+ " -g "
		+ " -o " + outfile + " "
		+ file.GetFullName()
		+ " & echo E_N_D_COMPILE\n";
	cmd = "";
	os.WriteString(cmd); // SEND

	m_receivedToken = false;
	m_isCompiling = true;
}


//Private:
void Compiler::OnProcessTerm(wxProcess2EndedEvent& e)
{
	*m_out << "Done.";
	m_out = NULL;
	m_isCompiling = false;
}


void Compiler::OnProcessOut(wxProcess2StdOutEvent& e)
{
//wxLogDebug("--------------------------------------------\nCompiler Recieved Output\n--------------------------------------------\n" + e.GetOutput());
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
				bool success = false;
				if(s.Contains("C_O_M_P_I_L_E_SUCCESS")) {
					s.Remove(s.Find("C_O_M_P_I_L_E_SUCCESS"), 21);
					success = true;
				}	
				s.Remove(s.Find("E_N_D_COMPILE\r"));
				m_isCompiling = false;
				m_receivedToken = false;

				// Signal
				wxCompilerEndedEvent e(success);
				ProcessEvent(e); // synchronous because I detach in the very next line
				SetNextHandler(NULL);

				// Terminate the process:
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


