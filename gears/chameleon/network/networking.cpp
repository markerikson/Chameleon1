////////////////////////////////////////////////////
//
//
// ToDo: prevent anything external from being used if status == not good
//       (if I use my own GetStatus(), it will make sure m_curr$ are always accurate)
//       Move MaintainSettings into GetStatus.
//
//
////////////////////////////////////////////////////
#include <wx/regex.h> // for finding the host fingerprint
#include <wx/file.h>
#include <wx/process.h>
#include <wx/txtstrm.h>
#include "networking.h"
#include "plinkconnect.h"
#include "../common/datastructures.h"
#include "../common/Options.h"
#include "../common/debug.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

BEGIN_EVENT_TABLE(Networking, wxEvtHandler)
	EVT_TIMER(-1, Networking::onTimerTick)
    EVT_END_PROCESS(-1, Networking::onTerm)
END_EVENT_TABLE()


Networking::Networking(Options* options)
{
	m_options = options;
	m_currHost = m_options->GetHostname();
	m_currUser = m_options->GetUsername();
	m_currPass = m_options->GetPassphrase();
	m_plinks = new PlinkConnect(m_options->GetPlinkApp(), m_currHost,
								m_currUser, m_currPass);

	m_status = NET_STARTING;
	//GetStatus(); <-- no need to be pre-emptive (here it's best not to be)
}


Networking::~Networking()
{
	delete m_plinks;
}


bool Networking::GetHomeDirPath(wxString &path)
{
	bool success = false;

	if(GetStatus() == NET_GOOD) {
		// mini-caching:
		if(m_userHomeDir == wxEmptyString)
		{
			if( SSHGetHomeDirPath(path) ) {
				m_userHomeDir = path;
				success = true;
				m_statusDetails = "";
			}
		}
	}

	return success;
}


bool Networking::GetFileContents(wxFileName file, wxString &contents)
{
	if(GetStatus() == NET_GOOD) {
		if( SSHGetFileContents(file.GetFullPath(wxPATH_UNIX), contents) ) {
			m_statusDetails = "";
			return true;
		}
	}
	//else
		return false;
}


bool Networking::SendFileContents(wxString strng, wxFileName file)
{
	bool success = false;

	if(GetStatus() == NET_GOOD) {
		// Save the file someplace:
		wxString fname = wxFileName::CreateTempFileName("chm"); // chm?????.tmp
		if(fname != "") {
			wxFile f(fname, wxFile::write);
			if(f.IsOpened()) {
				bool written = f.Write(strng);
				f.Close();
				if(written) {
					// YEAH!, now I can send the file
					if( SCPDoTransfer(fname, file.GetFullPath(wxPATH_UNIX)) ) {
						success = true;
						m_statusDetails = "";
					}

					// Remove the temp file:
					wxRemoveFile(fname);
				}
				else {
					//success = false;
					m_statusDetails = "Could not write to the temp file " + fname;
				}
			}
			else {
				//success = false;
				m_statusDetails = "Could not open the temp file " + fname;
			}
		}
		else {
			//success = false;
			m_statusDetails = "Could not create a temp file";
		}
	}

	return success;
}


bool Networking::GetDirListing(wxString dirPath, DirListing &listing, bool forceRefresh,
							   bool includeHidden)
{
	if(GetStatus() == NET_GOOD) {
		// internally manage dirListing caches to increase speed
		//if(in cache) {
		//	blah
		//}
		//else
		if( SSHGetDirListing(dirPath, listing, includeHidden) ) {
			m_statusDetails = "";
			return true;
		}
	}
	//else
		return false;
}


//Private:
bool Networking::SSHGetHomeDirPath(wxString &path) {
	wxString cmd = "cd ~ && pwd && echo Get-tingH_O_M_E_DIR";
	
	wxString output;

	if( SSHExecSyncCommand(cmd, output) ) {
		output.Remove(output.Length()-1); // remove pwd's EOL
		path = output;
		m_statusDetails = "";
		return true;
	}
	//else
		m_statusDetails = output;
		path = "~";
		return false;
}


//Private:
bool Networking::SSHGetFileContents(wxString file, wxString &contents)
{
	wxString cmd = "cat " + file + " && echo Fi_Le_Li_St-YEAH";
	wxString output;

	if( SSHExecSyncCommand(cmd, output) ) {

		output.Remove(output.Length()-1); // remove cat's EOL
		contents = output;
		m_statusDetails = "";
		return true;
	}
	//else
		m_statusDetails = output;
		contents = "";
		return false;

}


// Re-write this to use: "output = ExecuteLocalCommand(cmd);"
//Private:
bool Networking::SCPDoTransfer(wxString from_path_name, wxString to_path_name)
{
	// right now this only does local -> remote transfers
	wxString cmd = m_options->GetPscpApp() // + " -l " + ssh_user 
					+ " -pw " + m_currPass + " -batch "
					+ from_path_name + " " 
					+ m_currUser + "@" + m_currHost + ":" + to_path_name
					+ " && echo Chameleon-Transfer-Success";
	wxProcess* proc = new wxProcess();
	proc->Redirect();
	int pid = wxExecute(cmd, wxEXEC_SYNC, proc);

	// Determine success:
	bool success = false;

	if (pid == -1) { // Bad Exit -- Could not start process
		m_statusDetails = "Could not start the file transfer process.";
		//success = false;
	}
	else { // Not bad Exit(hopefully good):
		// Grab the outputs:
		wxString output = "";
		wxString errlog = "";
		wxInputStream* rin = proc->GetInputStream();
		wxInputStream* rerr = proc->GetErrorStream();
		while(!rin->Eof()) {
			output += rin->GetC();
		}
		while(!rerr->Eof()) {
			errlog += rerr->GetC();
		}

		// Determine the success:
		if(output.Contains("Chameleon-Transfer-Success")) {
			success = true;
			m_statusDetails = "";
		}
		else {
			m_statusDetails = output + errlog;
		}
	}
	delete proc;

	return success;
}


// It would be nice to use: "ls --color=never -Q --format=vertical -1 -A", but I can't
//    tell from this what is a dir, and what is a file.  I could later use that with "-l"
//    and parse for the "d"
//Private:
bool Networking::SSHGetDirListing(wxString dirPath, DirListing &listing,
									bool includeHidden)
{
	wxString cmd = "cd " + dirPath
					+ " && find -maxdepth 1 -type f "
					+ " && echo N_E_TwOrKiNg-DiRs"
					+ " && find -maxdepth 1 -type d";
	wxString output;
	
	if( SSHExecSyncCommand(cmd, output) ) {

		int tokenPos = output.Find("N_E_TwOrKiNg-DiRs\r\n");
		wxString files = output.Left(tokenPos+1);
		wxString dirs = output.Mid(tokenPos+19);

		listing.fileNames = ParseFindOutput(files, includeHidden);
		listing.dirNames = ParseFindOutput(dirs, includeHidden);

		return true;
	}
	//else
		m_statusDetails = output;
		return false;
}


// This parsing can easily be broken!  But, I am pretty confident
//   in the integrity of the string I'm parsing, because I know
//   what's coming in.
// Private:
wxArrayString Networking::ParseFindOutput(wxString strng, bool includeHidden)
{
	wxArrayString r;
	r.Empty();

	while(!strng.IsEmpty()) {

		if(strng.Length() < 2) {
			return r;
		}
		// Get the "./"
		char c1 = strng.GetChar(0); // peek
		char c2 = strng.GetChar(1); // peek
		strng.Remove(0,2); // pop
		if(c1 != '.' && c2 != '/' || strng.IsEmpty()) {
			return r;
		}

		wxString newitem = "";

		c1 = strng.GetChar(0); // peek
		strng.Remove(0,1); // pop
		bool isHidden = false;
		if(c1 == '.') {
			isHidden = true;
		}

		while(c1 != '\r') {
			newitem += c1;
			c1 = strng.GetChar(0); // peek
			strng.Remove(0,1); // pop
		}

		strng.Remove(0,1); // '\n'

		if(!isHidden || includeHidden) {
			r.Add(newitem);
		}
	}

	return r;
}


//Private:
bool Networking::SSHExecSyncCommand(wxString command, wxString &output) {
	command += " && echo Su_CC_ess-CMD";

	output = m_plinks->executeSyncCommand(command);

	int tokenPos = output.Find("Su_CC_ess-CMD");
	if(tokenPos != -1) {
		output.Remove(tokenPos);
		return true;
	}
	// else
		return false;	
}


// Not meant to update my status -- just my settings (if necessary)
// This returns true if any settings have changed.  I need to do this polling because
//   Options is passive.
//Private:
bool Networking::MaintainSettings() {
	wxString newH = m_options->GetHostname();
	wxString newU = m_options->GetUsername();
	wxString newP = m_options->GetPassphrase();

	if(newH != m_currHost || newU != m_currUser || newP != m_currPass ) {
		// I don't check for plinkApp changes because I don't keep track of those
		// Something has changed
		m_currHost = newH;
		m_currUser = newU;
		m_currPass = newP;

		//Update PlinkConnect
		m_plinks->setLogin(newH, newU, newP);

		return true;
	}
	//else
	  return false;

}


// I might recommend OptionsDialog call this after a change
NetworkStatus Networking::GetStatus()
{
	if(MaintainSettings()) {
		// Update the status
		if(m_plinks->getIsConnected()) {
			m_status = NET_GOOD;
		}
		else {
			m_status = NET_ERROR_MESSAGE; //default

			wxString message = m_plinks->getMessage();
			if(message.Contains("")) {
				// host finger print not in cache
				m_status = NET_UNKNOWN_HOST;
				m_statusDetails = "";
			}
			else if(message.Contains("")) {
				// host wouldn't allow a connection
				m_status = NET_CONN_REFUSED;
			}
			else if(message.Contains("")) {
				// user+pass did not work on host
				m_status = NET_AUTH_FAILED;
			}
		}
	}

	return m_status;
}


wxString Networking::GetStatusDetails()
{
	return m_statusDetails;
}


// I am assuming!!! that the connection should work, and I'm just doing this to
//  accept the cache.
//Public:
void Networking::SSHCacheFingerprint()
{
	//Start the program asynchonously, answer "y" to caching, then Kill it:
	wxString cmd = m_options->GetPlinkApp() + " " + m_currHost;
	wxProcess* proc = new wxProcess(NULL);
	proc->Redirect();
	long pid = wxExecute(cmd, wxEXEC_ASYNC, proc);
	wxTextOutputStream pout(*proc->GetOutputStream(), wxEOL_UNIX);
	pout << "y\n";

	// Sending wxSIGTERM to the process is all that's needed to close it out gracefully.
	// It also takes care of deleting the process, so we don't have to do that manually.
	wxKill(pid, wxSIGTERM);

}


//Private:
void Networking::onTerm(wxProcessEvent &e) {
	//
}


//Private:
void Networking::onTimerTick(wxTimerEvent &e) {
	//
}


void Networking::PingOptions() {
	if(MaintainSettings()) {
		//GetStatus(); <-- best not to be pre-emptive
	}
	//else nothing to update
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

wxTextOutputStream* Networking::StartCommand(bool isRemote, wxString cmd, wxEvtHandler* owner)
{
	if(isRemote) {
		return StartRemoteCommand(cmd, owner);
	}
	else {
		return StartLocalCommand(cmd, owner);
	}
}


wxTextOutputStream* Networking::StartRemoteCommand(wxString cmd, wxEvtHandler* owner)
{
	return m_plinks->executeCommand(cmd, owner);
}


wxTextOutputStream* Networking::StartLocalCommand(wxString cmd, wxEvtHandler* owner)
{
	wxLogDebug("Local Process Execution still missing.");
	return NULL;
}


// A very simple method
wxString Networking::ExecuteCommand(bool isRemote, wxString cmd)
{
	if(isRemote) {
		return ExecuteRemoteCommand(cmd);
	}
	else {
		return ExecuteLocalCommand(cmd);
	}
}


wxString Networking::ExecuteRemoteCommand(wxString cmd)
{
	return m_plinks->executeSyncCommand(cmd);
}


wxString Networking::ExecuteLocalCommand(wxString cmd)
{
	wxLogDebug("Local Process Execution still missing.");
	return wxEmptyString;
}


// This is certainly not a common-sense thing to pass when desiring to terminate
//    a process
void Networking::ForceKillProcess(wxTextOutputStream* w)
{
	wxLogDebug("David is a dingbat, and NEEDS to implement this!");
}




