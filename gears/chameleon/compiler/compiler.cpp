////////////////////////////////////////////////////////////////////
//
// Notes:
//
////////////////////////////////////////////////////////////////////
#include "compiler.h"
#include <wx/filename.h>
#include <wx/txtstrm.h>
#include "compilerevent.h"
#include "../common/chameleonprocessevent.h"
#include "../common/options.h"
#include "../network/networking.h"
#include "../common/projectinfo.h"
#include "../common/fixvsbug.h"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_EVENT_TABLE(Compiler, wxEvtHandler)
	EVT_PROCESS_STDOUT(Compiler::OnProcessOut)
	EVT_PROCESS_STDERR(Compiler::OnProcessErr)
	EVT_PROCESS_ENDED(Compiler::OnProcessTerm)
END_EVENT_TABLE()


Compiler::Compiler(Options* options, Networking* network)
{
	m_options = options;
	m_network = network;
	m_isCompiling = false;
	m_isLinking = false;
	m_compilerStdIn = NULL; // used only for forced terminations
	m_currProj = NULL;
	m_currFileNum = -2;
}

Compiler::~Compiler() {
	if(IsCompiling()) {
		m_network->ForceKillProcess(m_compilerStdIn);
	}
}


void Compiler::CompileProject(ProjectInfo* proj, wxEvtHandler* h)
{
	m_isCompiling = true;
	SetNextHandler(h);
	proj->SetBeingCompiled(true); // prevent user from editing while compiling

	m_currProj = proj;
	m_currFileNum = 0;
	m_compilingStatus = CR_OK; // default

	StartNextFile();
}


//Private:
void Compiler::StartNextFile()
{
	wxArrayString a = m_currProj->GetSourcesToBuild();
	bool isRemote = m_currProj->IsRemote();

	// Create outfile name:
	wxFileName inFile(a.Item(m_currFileNum));
	wxFileName outFile(inFile.GetPath(wxPATH_GET_VOLUME, isRemote ? wxPATH_UNIX : wxPATH_DOS) + "/" + inFile.GetName() + ".o");

	// Start the compile:
	wxString cmd = wxEmptyString;
	if(isRemote) {
		cmd += "g++ "; // compiler (assuming it is in the PATH)
		cmd += " -g -c "; // include gdb info, and don't link
		cmd += " -o " + outFile.GetFullPath(wxPATH_UNIX) + " ";
		cmd +=  inFile.GetFullPath(wxPATH_UNIX);
		cmd +=  " && echo C_O_M_P_I_L_E_SUCCESS || echo C_O_M_P_I_L_E_FAILED";
	}
	else {
		cmd +=  "\"" + m_options->GetMingwPath() + "/bin/g++.exe\" "; // compiler
		cmd +=  " -g -c ";
		cmd +=  " -o " + outFile.GetFullPath(wxPATH_DOS) + " ";
		cmd +=  inFile.GetFullPath(wxPATH_DOS);
		cmd +=  " && echo C_O_M_P_I_L_E_SUCCESS || echo C_O_M_P_I_L_E_FAILED";
	}
	wxLogDebug("Starting to Compile with cmd=" + cmd);
	m_compilerStdIn = m_network->StartCommand(isRemote, cmd, this);

	m_intermediateFiles.Add(outFile.GetFullPath(isRemote ? wxPATH_UNIX : wxPATH_DOS));

	// Signal User
	CompilerEvent e(chEVT_COMPILER_START);
	e.SetRemoteFile(isRemote);
	e.SetFile(inFile);
	AddPendingEvent(e);

	m_currFileNum++; // increment file counter
}


//Private:
void Compiler::StartLinking() {
	bool isRemote = m_currProj->IsRemote();

	// Create outfile name:
	wxString name = m_currProj->GetProjectBasePath();
	if(isRemote) {
		name += "/" + m_currProj->GetProjectName() + ".exe";
	}
	else {
		name += "\\" + m_currProj->GetProjectName() + ".out";
	}
	wxFileName outFile(name);

	// Assemble inFiles list:
	wxString inFiles = wxEmptyString;
	for(int i = 0; i < m_intermediateFiles.Count(); i++) {
		inFiles += " " + m_intermediateFiles.Item(i);
	}
	for(int i = 0; i < m_currProj->GetLibraries().Count(); i++) {
		inFiles += " " + m_currProj->GetLibraries().Item(i);
	}

	// Start the linking:
	wxString cmd = wxEmptyString;
	if(isRemote) {
		cmd += "g++ -g -o " + outFile.GetFullPath(wxPATH_UNIX);
		cmd +=  inFiles;
		cmd +=  " && echo C_O_M_P_I_L_E_SUCCESS || echo C_O_M_P_I_L_E_FAILED";
	}
	else {
		cmd +=  "\"" + m_options->GetMingwPath() + "/bin/g++.exe\" ";
		cmd +=  " -g -o " + outFile.GetFullPath(wxPATH_DOS);
		cmd +=  inFiles;
		cmd +=  " && echo C_O_M_P_I_L_E_SUCCESS || echo C_O_M_P_I_L_E_FAILED";
	}
	wxLogDebug("Starting to Link with cmd=" + cmd);
	m_compilerStdIn = m_network->StartCommand(isRemote, cmd, this);

	m_intermediateFiles.Add(outFile.GetFullPath(isRemote ? wxPATH_UNIX : wxPATH_DOS));

	// Signal User
	CompilerEvent e(chEVT_COMPILER_START);
	e.SetRemoteFile(isRemote);
	e.SetFile(wxFileName("Linking")); // kind of icky

	m_currFileNum++; // increment file counter

}


// This is basically my last step before ending
//Private:
void Compiler::RemoveIntermediateFiles() {
	wxString files = wxEmptyString;
	for(int i = 0; i < m_intermediateFiles.Count(); i++) {
		files += " " + m_intermediateFiles.Item(i);
	}

	if(m_currProj->IsRemote()) {
//		m_network->StartRemoteCommand("rm " + files, this); //pehaps NULL (instead of this)
	}
	else {
//		m_network->StartLocalCommand("del " + files, this);
	}

	m_intermediateFiles.Clear();
}


//Private:
void Compiler::OnProcessTerm(ChameleonProcessEvent& e)
{
	m_compilerStdIn = NULL;

	if(m_isCompiling) {
		if(m_compilingStatus == CR_TERMINATED) {
			//manually terminated
			m_isCompiling = false;
			RemoveIntermediateFiles();
			//signal user
			CompilerEvent e(chEVT_COMPILER_END);
			e.SetResult(CR_TERMINATED);
			AddPendingEvent(e);
			// End
			SetNextHandler(NULL);
		}
		else {
			// have I compiled all the files?
			if( m_currFileNum >= m_currProj->GetSourcesToBuild().GetCount() ) {
				if(m_compilingStatus == CR_OK) {
					m_isCompiling = false;
					m_isLinking = true;
					StartLinking();
				}
				else {
					//signal user
					CompilerEvent e(chEVT_COMPILER_END);
					e.SetResult(CR_ERROR);
					AddPendingEvent(e);
					// End
					SetNextHandler(NULL);
				}
			}
			else {
				//m_currFileNum is incremented at the end of StartNextFile()
				//m_compilingStatus == CR_ERROR || m_compilingStatus == CR_OK
				StartNextFile();
			}
		}
	}
	else if(m_isLinking) {
		m_isLinking = false;

		CompilerEvent e(chEVT_COMPILER_END);
		e.SetResult(CR_ERROR);

		m_currProj->SetCompiled(false); // default
		if(m_compilingStatus == CR_OK) {
			m_currProj->SetCompiled(true);
			m_currProj->SetExecutableName(m_intermediateFiles.Last());
			e.SetFile(m_intermediateFiles.Last());
		}

		m_intermediateFiles.Remove(m_intermediateFiles.Count()-1); // remove the executable

		// End
		AddPendingEvent(e);
		m_currProj->SetBeingCompiled(false);
		RemoveIntermediateFiles();
	}
	else {
		wxLogDebug("Compiler Recieved extranious term event");
		// Probably from the file removal command
	}

	return;
}


void Compiler::HaltCompiling()
{
	if(m_isCompiling) {
		m_compilerStdIn->WriteString((char)3); // send ^C -- break
		m_compilingStatus = CR_TERMINATED;
	}
}


//Private:
void Compiler::OnProcessOut(ChameleonProcessEvent& e)
{
	wxLogDebug("Compiler Received: %s", e.GetString());

	if(m_isCompiling || m_isLinking) {
		wxString s = e.GetString();

		if(s.Contains("C_O_M_P_I_L_E_SUCCESS")) {
			s.Remove(s.Find("C_O_M_P_I_L_E_SUCCESS"));
		}
		else if(s.Contains("C_O_M_P_I_L_E_FAILED")) {
			s.Remove(s.Find("C_O_M_P_I_L_E_FAILED"));
			//remove this filename from the intermediate list
			m_intermediateFiles.Remove(m_intermediateFiles.Count()-1);
			m_compilingStatus = CR_ERROR;
		}

		ParseCompilerMessages(s);
	}
	else {
		wxLogDebug("Compiler Recieved Output while not compiling: " + e.GetString());
	}
}


void Compiler::OnProcessErr(ChameleonProcessEvent& e)
{
	// Doesn't matter where it comes from
	OnProcessOut(e);
}


//Private:
void Compiler::ParseCompilerMessages(wxString s)
{
	CompilerEvent e(chEVT_COMPILER_PROBLEM);
	e.SetGCCOutput(s);
	AddPendingEvent(e);
}