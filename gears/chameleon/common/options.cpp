#include "Options.h"

#include "debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


Options::Options() {
	// Set Some Default Values (perhaps these should not be set!)
	m_pscpProg = "pscp.exe";
	m_plinkProg = "plink.exe";
	m_mingwPath = "c:\\program files\\mingw\\";
	m_username = "username";
	m_hostname = "localhost";
	m_password = "password";
	m_remoteCompileOut = "a.out";
	//wxFileName pscpPath(wxGetCwd(), "pscp.exe");
	//SetPscpApp(pscpPath.GetFullPath());
	//wxFileName plinkPath(wxGetCwd(), "plink.exe");
	//SetPlinkApp(plinkPath.GetFullPath());
	//wxFileName mingwPath(wxGetCwd());
	//SetMingwPath(mingwPath.GetFullPath());
}


bool Options::SetPscpApp(wxString path_and_prog) {
	if(wxFileName::FileExists(path_and_prog)) {
		m_pscpProg = path_and_prog;
		// Could even check to make sure it's the right version (I'm dreaming)
		return true;
	}
	//else
		wxLogDebug("\"" + path_and_prog + "\" is an invalid PSCP.");
		return false;
}


bool Options::SetPlinkApp(wxString path_and_prog) {
	if(wxFileName::FileExists(path_and_prog)) {
		m_plinkProg = path_and_prog;
		return true;
	}
	//else
		wxLogDebug("\"" + path_and_prog + "\" is an invalid Plink.");
		return false;
}


bool Options::SetMingwPath(wxString path) {
	if(wxFileName::DirExists(path)) {
		m_mingwPath = path;
		return true;
	}
	//else
		wxLogDebug("\"" + path + "\" is an invalid path.");
		return false;
}


bool Options::SetUsername(wxString user) {
	m_username = user;
	return true;
}


bool Options::SetHostname(wxString host) {
	m_hostname = host;
	return true;
}


bool Options::SetPassphrase(wxString pass) {
	if(pass.Contains('\"')) {
		return false;
	}
	//else
		m_password = pass;
		return true;
}


bool Options::SetRemoteCompileOut(wxString path_and_file) {
	m_remoteCompileOut = path_and_file;
	return true;
}


//wxString GetPscpApp(); <-- Inlined
//wxString GetPlinkApp(); <-- Inlined
//wxString GetMingwPath(); <-- Inlined
//wxString GetUsername(); <-- Inlined
//wxString GetHostname(); <-- Inlined
//wxString GetPassphrase(); <-- Inlined
//wxString GetCompileOut(); <-- Inlined

