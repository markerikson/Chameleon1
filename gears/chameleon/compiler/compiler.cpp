////////////////////////////////////////////////////////////////////
//
// Notes:
//    gcc || echo token <-- token if gcc had errors
//
////////////////////////////////////////////////////////////////////
#include "compiler.h"
#include "compilerevent.h"
#include "../common/chameleonprocessevent.h"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_EVENT_TABLE(Compiler, wxEvtHandler)
	EVT_PROCESS_STDOUT(Compiler::OnProcessOut)
	EVT_PROCESS_STDERR(Compiler::OnProcessErr)
	EVT_PROCESS_ENDED(Compiler::OnProcessTerm)
END_EVENT_TABLE()


Compiler::Compiler(Options* options, Networking* network) {
	m_options = options;
	m_network = network;
	m_isCompiling = false;
	m_compilerStdIn = NULL;
	m_out = NULL;
	//m_receivedToken = false;
	//m_currOutfile = "";
	m_handler = NULL;
}


// Remember, for a remote compilation to be started, the person needs to
//   have a remote file open (aka. successful user/pw/connection)
void Compiler::CompileFile(wxFileName file, bool isRemote, wxTextCtrl* textbox,
						   wxEvtHandler* h) {

	m_out = textbox;
	SetNextHandler(h);
	wxString outfile = file.GetName();

	// Set the output file name:
	if(isRemote) {
		outfile += ".out"; // extension
	}
	else { // Local:
		outfile += ".exe"; // extension
	}

	// Signal the user, and get going
	//proj->SetReadOnly(true);
	*m_out << "Compiling...\n";
	*m_out << file.GetFullName() << "\n";
	m_currOutfile = file;
	m_currOutfile.SetFullName(outfile);

	m_isCompiling = true;

	//Unfortunately the shells are not identical:
	if(isRemote) {
		CompileRemoteFile(file, outfile);
	}
	else { // isLocal
		CompileLocalFile(file, outfile);
	}

}


void Compiler::CompileProject(ProjectInfo* proj, bool isRemote, wxTextCtrl* textbox,
							  wxEvtHandler* h) {
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
void Compiler::CompileRemoteFile(wxFileName infile, wxFileName outfile)
{
	wxString cmd = wxEmptyString;
	cmd += "echo S_T_A_R_T_COMPILE ; ";
	cmd += " g++ "; // compiler (assuming it is in the PATH)
	cmd += " -g "; // include gdb info
	cmd += " -o " + outfile.GetFullPath() + " ";
	cmd +=  infile.GetFullPath(wxPATH_UNIX);
	cmd +=  " && echo C_O_M_P_I_L_E_SUCCESS ; echo E_N_D-COMPILE\r";

	m_compilerStdIn = m_network->StartRemoteCommand(cmd, this);
}


//Private:
void Compiler::CompileLocalFile(wxFileName infile, wxFileName outfile)
{
	wxString cmd = wxEmptyString;
	cmd += "echo S_T_A_R_T_COMPILE & ";
	cmd +=  "\"" + m_options->GetMingwPath() + "g++.exe\" "; // compiler
	cmd +=  " -g "; // include gdb info
	cmd +=  " -o " + outfile.GetFullPath() + " ";
	cmd +=  infile.GetFullPath();
	cmd +=  " & echo E_N_D_COMPILE\n";

	m_compilerStdIn = m_network->StartLocalCommand(cmd, this);
}


//Private:
void Compiler::OnProcessTerm(ChameleonProcessEvent& e)
{
	m_out = NULL;
	m_isCompiling = false;
	m_compilerStdIn = NULL;
}


void Compiler::OnProcessOut(ChameleonProcessEvent& e)
{
//wxLogDebug("--------------------------------------------\nCompiler Recieved Output\n--------------------------------------------\n" + e.GetOutput());
	if(m_isCompiling) {
		wxString s = e.GetString();
		if(!m_receivedToken) {
			if(s.Contains("S_T_A_R_T_COMPILE\r")) {
				s.Remove(0,s.Find("S_T_A_R_T_COMPILE\r")+19);
				m_receivedToken = true;
			}
		}

		if(m_receivedToken) {
			if(s.Contains("E_N_D_COMPILE\r")) {
				
				bool success = false;
				wxString resultText = "Done.";
				if(s.Contains("C_O_M_P_I_L_E_SUCCESS")) {
					s.Remove(s.Find("C_O_M_P_I_L_E_SUCCESS"), 21);
					success = true;
					resultText = m_currOutfile.GetFullName() + " - 0 Errors.";
				}
				else {
					resultText = "\nDone.";
				}

				s.Remove(s.Find("E_N_D_COMPILE\r"));
				m_isCompiling = false;
				m_receivedToken = false;

				// Signal Chameleon
				CompilerEvent e(chEVT_COMPILER_END);
				if(success) {
					e.SetResult(CR_OK);
				}
				else {
					e.SetResult(CR_ERROR);
				}
				e.SetFile(wxFileName(m_currOutfile));
				ProcessEvent(e); // synchronous because I detach in the very next line
				SetNextHandler(NULL);

				// Signal the User
				*m_out << resultText;

				// Process will Terminate:
			}
			
			// Display the output:
			*m_out << s;
		}
	}
	else {
		wxLogDebug("Compiler Recieved Output while not compiling: " + e.GetString());
	}

}


void Compiler::OnProcessErr(ChameleonProcessEvent& e)
{
	wxLogDebug("Compiler Received STDERR: " + e.GetString());
}


