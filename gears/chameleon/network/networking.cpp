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


//////////////////////////////////////////////////////////////////////////////
///  public constructor Networking
///  <TODO: insert text here>
///
///  @param  options Options * <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
///  public destructor ~Networking
///  <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
Networking::~Networking()
{
	delete m_plinks;
}


//////////////////////////////////////////////////////////////////////////////
///  public GetHomeDirPath
///  <TODO: insert text here>
///
///  @param  homeDir wxString & <TODO: insert text here>
///
///  @return bool    <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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
		else {
			// pull for 'cache'
			path = m_userHomeDir;
			success = true;
		}
	}

	return success;
}


//////////////////////////////////////////////////////////////////////////////
///  public GetFileContents
///  <TODO: insert text here>
///
///  @param  file     wxFileName  <TODO: insert text here>
///  @param  contents wxString &  <TODO: insert text here>
///
///  @return bool     <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
///  public SendFileContents
///  <TODO: insert text here>
///
///  @param  strng wxString    <TODO: insert text here>
///  @param  file  wxFileName  <TODO: insert text here>
///
///  @return bool  <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
///  public GetDirListing
///  <TODO: insert text here>
///
///  @param  dirPath       wxString     <TODO: insert text here>
///  @param  list          DirListing & <TODO: insert text here>
///  @param  forceRefresh  bool         [=false] <TODO: insert text here>
///  @param  includeHidden bool         [=false] <TODO: insert text here>
///
///  @return bool          <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////
///  private SSHGetHomeDirPath
///  <TODO: insert text here>
///
///  @param  homeDir wxString & <TODO: insert text here>
///
///  @return bool    <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool Networking::SSHGetHomeDirPath(wxString &path) {
	wxString cmd = "cd ~ && pwd";
	
	wxString output;

	if( SSHExecSyncCommand(cmd, output) ) {
		output.Remove(output.Length()-2); // remove pwd's EOL("\r\n")
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
//////////////////////////////////////////////////////////////////////////////
///  private SSHGetFileContents
///  <TODO: insert text here>
///
///  @param  file     wxString   <TODO: insert text here>
///  @param  contents wxString & <TODO: insert text here>
///
///  @return bool     <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool Networking::SSHGetFileContents(wxString file, wxString &contents)
{
	wxString cmd = "cat " + file;
	wxString output;

	if( SSHExecSyncCommand(cmd, output) ) {

		/////////////
		output.Replace("\r\n", "\n", true);
		/////////////

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
//////////////////////////////////////////////////////////////////////////////
///  private SCPDoTransfer
///  <TODO: insert text here>
///
///  @param  from_path_name wxString  <TODO: insert text here>
///  @param  to_path_name   wxString  <TODO: insert text here>
///
///  @return bool           <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool Networking::SCPDoTransfer(wxString from_path_name, wxString to_path_name)
{
	// right now this only does local -> remote transfers
	wxString cmd = m_options->GetPscpApp() // + " -l " + ssh_user 
					+ " -pw " + m_currPass + " -batch "
					+ from_path_name + " " 
					+ m_currUser + "@" + m_currHost + ":" + to_path_name;
					//+ " && echo Chameleon-Transfer-Success";
	wxProcess* proc = new wxProcess();
	proc->Redirect();
	int exitcode = wxExecute(cmd, wxEXEC_SYNC, proc);

	// Determine success:
	bool success = false;

	if(exitcode == -1) { // Bad Exit -- Could not start process
		m_statusDetails = "Could not start the file transfer process.";
		//success = false;
	}
	else if(exitcode == 0) {
		// Assumed good
		success = true;
		m_statusDetails = "";
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
//////////////////////////////////////////////////////////////////////////////
///  private SSHGetDirListing
///  <TODO: insert text here>
///
///  @param  dirPath       wxString     <TODO: insert text here>
///  @param  listing       DirListing & <TODO: insert text here>
///  @param  includeHidden bool         [=false] <TODO: insert text here>
///
///  @return bool          <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool Networking::SSHGetDirListing(wxString dirPath, DirListing &listing,
									bool includeHidden)
{
	wxString cmd = "cd " + dirPath
					+ " && find -maxdepth 1 -type f "
					+ " && echo N_E_TwOrKiNg-DiRs"
					+ " && find -maxdepth 1 -type d";
	wxString output;
	
	if( SSHExecSyncCommand(cmd, output) ) {
		// if returned true, the token will be there
		int tokenPos = output.Find("N_E_TwOrKiNg-DiRs\r\n");
		wxString files = output.Left(tokenPos); // +1
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
//////////////////////////////////////////////////////////////////////////////
///  private ParseFindOutput
///  <TODO: insert text here>
///
///  @param  strng         wxString  <TODO: insert text here>
///  @param  includeHidden bool      <TODO: insert text here>
///
///  @return wxArrayString <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxArrayString Networking::ParseFindOutput(wxString strng, bool includeHidden)
{
	wxArrayString r;
	r.Empty();

	while(strng.Length() > 2) {

		// Get the "./"
		char c1 = strng.GetChar(0); // peek
		char c2 = strng.GetChar(1); // peek
		strng.Remove(0,2); // pop

		if(c1 == '.' && c2 == '\r') {
			// Special case for . direcrtory
			strng = "\r" + strng; // push it back on
			c2 = '/';
		}

		//if(c1 != '.' && c2 != '/') {
		//	return r; // unexpected
		//}

		// Queue-up/Initialize for the loop
		// There should be at least 2 left: \r\n		
		c1 = strng.GetChar(0);
		c2 = strng.GetChar(1);
		strng.Remove(0,2);

		wxString newitem = "";

		while(c1 != '\r' && c2 != '\n') {
			newitem += c1;
			c1 = c2;
			c2 = strng.GetChar(0); // peek
			strng.Remove(0,1); // pop
		}

		if(newitem != "") {
			if(newitem.Left(1) != "." || includeHidden) {
				r.Add(newitem);
			}
		}
	}

	return r;
}


//Private:
//////////////////////////////////////////////////////////////////////////////
///  private SSHExecSyncCommand
///  <TODO: insert text here>
///
///  @param  command wxString   <TODO: insert text here>
///  @param  output  wxString & <TODO: insert text here>
///
///  @return bool    <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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
		m_plinks->setLogin(newH, newU, newP); // this restarts the network
		m_status = NET_STARTING;

		return true;
	}
	//else
	  return false;

}


// I might recommend OptionsDialog call this after a change
//////////////////////////////////////////////////////////////////////////////
///  public GetStatus
///  <TODO: insert text here>
///
///  @return NetworkStatus <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
NetworkStatus Networking::GetStatus()
{
	if(MaintainSettings() || m_status == NET_STARTING) {
		// Update the status
		if(m_plinks->getIsConnected()) {
			m_status = NET_GOOD;
		}
		else {
			wxString message = m_plinks->getMessage();

			m_status = NET_ERROR_MESSAGE; //default
			m_statusDetails = message;

			if(message.Contains("key is not cached")) {
				// host finger print not in cache
				m_status = NET_UNKNOWN_HOST;

				// Grab the Fingerprint:
				wxRegEx reFingerprint = "[[:digit:]]+[[:blank:]]+([[:xdigit:]]{2}:)+[[:xdigit:]]{2}";

				if(reFingerprint.Matches(message))
				{
					//wxString match = reFingerprint.GetMatch(message);
					//wxLogDebug("Matched fingerprint: \"%s\"", match);
					m_statusDetails = reFingerprint.GetMatch(message);
				}
				else
				{
					//wxLogDebug("Failed to match the fingerprint in: %s", errlog);
					m_statusDetails = "*unknown* - could not parse fingerprint";
				}
			}
			else if(message.Contains("Unable to authenticate")) {
				// user+pass did not work on host
				m_status = NET_AUTH_FAILED;
			}
			else if(message.Contains("Connection refused")) {
				// host wouldn't allow a connection
				m_status = NET_CONN_REFUSED;
			}
		}
	}
	return m_status;
}


//////////////////////////////////////////////////////////////////////////////
///  public GetStatusDetails
///  <TODO: insert text here>
///
///  @return wxString <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString Networking::GetStatusDetails()
{
	return m_statusDetails;
}


// I am assuming!!! that the connection should work, and I'm just doing this to
//  accept the cache.
//Public:
//////////////////////////////////////////////////////////////////////////////
///  public SSHCacheFingerprint
///  <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void Networking::SSHCacheFingerprint()
{
	//Start the program asynchonously, answer "y" to caching, then Kill it:
	wxString cmd = m_options->GetPlinkApp() + " " + m_currHost;
	wxProcess* proc = new wxProcess(NULL);
	proc->Redirect();
	long pid = wxExecute(cmd, wxEXEC_ASYNC, proc);
	wxTextOutputStream pout(*proc->GetOutputStream(), wxEOL_UNIX);
	pout << "y\n";

	// Sending wxSIGTERM should close it out gracefully, but it doesn't terminate (ODD)
	// And, sending SIGKILL too soon ends the process before it caches the fingerprint
	// So, pause for 1/4 a second, then send SIGKILL
	wxUsleep(250);
	wxKill(pid, wxSIGKILL);

	m_plinks->setLogin("","",""); // terminate all connections, and do a spawn
	m_status = NET_STARTING;

	// It also takes care of deleting the process, so we don't have to do that manually.
	//delete proc; // self-destructs
}


//Private:
void Networking::onTerm(wxProcessEvent &e) {
	//
}


//Private:
void Networking::onTimerTick(wxTimerEvent &e) {
	//
}


// This function can NOT be SYNCHRONOUS (especially not locking -- wxSafeYield)
//////////////////////////////////////////////////////////////////////////////
///  public PingOptions
///  <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void Networking::PingOptions() {
	// If nothing has changed, nothing will happen.
	if( MaintainSettings() || m_status == NET_STARTING) {
		// If settings have changed, the changes will be propagated to PlinkConnect, then
		//     (asynchronously) PlinkConnect will spawn a connection (if possible)
		if(m_plinks->getIsSettled()) {
			// only do this if I know I will can an immediate response
			GetStatus();
		}
	}
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
///  public StartCommand
///  <TODO: insert text here>
///
///  @param  isRemote             bool           <TODO: insert text here>
///  @param  cmd                  wxString       <TODO: insert text here>
///  @param  owner                wxEvtHandler * <TODO: insert text here>
///
///  @return wxTextOutputStream * <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxTextOutputStream* Networking::StartCommand(bool isRemote, wxString cmd, wxEvtHandler* owner)
{
	if(isRemote) {
		return StartRemoteCommand(cmd, owner);
	}
	else {
		return StartLocalCommand(cmd, owner);
	}
}


//////////////////////////////////////////////////////////////////////////////
///  public StartRemoteCommand
///  <TODO: insert text here>
///
///  @param  cmd                  wxString       <TODO: insert text here>
///  @param  owner                wxEvtHandler * <TODO: insert text here>
///
///  @return wxTextOutputStream * <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxTextOutputStream* Networking::StartRemoteCommand(wxString cmd, wxEvtHandler* owner)
{
	return m_plinks->executeCommand(cmd, owner);
}


//////////////////////////////////////////////////////////////////////////////
///  public StartLocalCommand
///  <TODO: insert text here>
///
///  @param  cmd                  wxString       <TODO: insert text here>
///  @param  owner                wxEvtHandler * <TODO: insert text here>
///
///  @return wxTextOutputStream * <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxTextOutputStream* Networking::StartLocalCommand(wxString cmd, wxEvtHandler* owner)
{
	wxLogDebug("Local Process Execution still missing.");
	return NULL;
}


// A very simple method
//////////////////////////////////////////////////////////////////////////////
///  public ExecuteCommand
///  <TODO: insert text here>
///
///  @param  isRemote bool      <TODO: insert text here>
///  @param  cmd      wxString  <TODO: insert text here>
///
///  @return wxString <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString Networking::ExecuteCommand(bool isRemote, wxString cmd)
{
	if(isRemote) {
		return ExecuteRemoteCommand(cmd);
	}
	else {
		return ExecuteLocalCommand(cmd);
	}
}


//////////////////////////////////////////////////////////////////////////////
///  public ExecuteRemoteCommand
///  <TODO: insert text here>
///
///  @param  cmd      wxString  <TODO: insert text here>
///
///  @return wxString <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString Networking::ExecuteRemoteCommand(wxString cmd)
{
	return m_plinks->executeSyncCommand(cmd);
}


//////////////////////////////////////////////////////////////////////////////
///  public ExecuteLocalCommand
///  <TODO: insert text here>
///
///  @param  cmd      wxString  <TODO: insert text here>
///
///  @return wxString <TODO: insert text here>
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString Networking::ExecuteLocalCommand(wxString cmd)
{
	wxLogDebug("Local Process Execution still missing.");
	return wxEmptyString;
}


// This is certainly not a common-sense thing to pass when desiring to terminate
//    a process
//////////////////////////////////////////////////////////////////////////////
///  public ForceKillProcess
///  <TODO: insert text here>
///
///  @param  w    wxTextOutputStream * <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void Networking::ForceKillProcess(wxTextOutputStream* w)
{
	//Walk m_lprocesses:
	bool isLocal = false;
	//for(ProcessInfoList::Node* node = m_processes.GetFirst(); node; node = node->GetNext() ) {
	//	p = node->GetData();
	//	if(p->pid == pid) {
	//		isLocal = true;
	//		break;
	//	}
	//}

	if(isLocal) {
		//
	}
	else {
		m_plinks->ForceKillProcess(w);
	}
}




