////////////////////////////////////////////////////
//
//
// ToDo: prevent anything external from being used if status == not good
//       (if I use my own GetStatus(), it will make sure m_curr$ are always accurate)
//
//
////////////////////////////////////////////////////

#include "networking.h"

#include "../common/debug.h"



Networking::Networking(Options* options)
{
	m_options = options;
	m_currHost = m_options->GetHostname();
	m_currUser = m_options->GetUsername();
	m_currPass = m_options->GetPassphrase();
	status = NET_ERROR_MESSAGE;
	statusDetails = "SSH Settings Have Not Been Initialized";
	ssh_plink = new PlinkConnect(m_options->GetPlinkApp(), m_currHost, m_currUser, m_currPass);
}


Networking::~Networking()
{
	delete ssh_plink;
}


wxString Networking::GetFileContents(wxString file, wxString path)
{
	wxString cmd = "cd " + path + " && cat " + file + " ";
	return SSHSendCommand(cmd);
}


DirListing Networking::GetDirListing(wxString dirPath, bool forceRefresh, bool includeHidden)
{
	// internally could also manage dirListing caches to increase speed
	// dirPath has no trailing "/"
	return SSHGetDirListing(dirPath, includeHidden);
}


void Networking::SendFileContents(wxString strng, wxString rfile, wxString rpath)
{
	// Save the file someplace:
	wxString fname = wxFileName::CreateTempFileName("chm"); // chm?????.tmp
	if(fname != "") {
		wxFile f(fname, wxFile::write);
		if(f.IsOpened()) {
			bool written = f.Write(strng);
			f.Close();
			if(written) {
				// YEAH!, now I can send the file
				SCPDoTransfer(fname, rpath+"/"+rfile); // also sets Status

				// Remove the temp file
				wxRemoveFile(fname);
			}
			else {
				status = NET_WRITE_ERROR;
				statusDetails = "Could not write to the temp file " + fname;
			}
		}
		else {
			status = NET_WRITE_ERROR;
			statusDetails = "Could not open the temp file " + fname;
		}
	}
	else {
		status = NET_WRITE_ERROR;
		statusDetails = "Could not create a temp file";
	}

	return;
}


wxString Networking::GetHomeDirPath()
{
	if(userHomeDir == wxEmptyString)
	{
		// Does not return a trailing /
		wxString cmd = "cd ~ && pwd ";
		userHomeDir = SSHSendCommand(cmd);
		userHomeDir.Remove(userHomeDir.Length()-1,1); // remove the "\n"
	}
	
	return userHomeDir;
}


//Private:
DirListing Networking::SSHGetDirListing(wxString dirPath, bool includeHidden)
{
	DirListing r;

	// Files:
	wxString files = "";
	wxString dirs = SSHSendCommand("cd " + dirPath
								+ " && find -maxdepth 1 -type f "
								+ " && echo N_E_TwOrKiNg-DiRs"
								+ " && find -maxdepth 1 -type d");
	if(status == NET_GOOD) {
		while(dirs.Left(18) != "N_E_TwOrKiNg-DiRs\n") {
			files += dirs.Left(1); // peek
			dirs.Remove(0,1); // pop
		}
		dirs.Remove(0,18);

		r.fileNames = ParseLS(files, includeHidden);
		if(dirs.Left(2) == ".\n") {
			//Special Case for "." directory
			dirs.Remove(0,2);
		}
		r.dirNames = ParseLS(dirs, includeHidden);
	}
	//else there was an error

	return r;
}


// This parsing can easily be broken!  But, I am pretty confident
//   in the integrity of the string I'm parsing, because I know
//   what's coming in.
// Private:
wxArrayString Networking::ParseLS(wxString strng, bool includeHidden)
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

		while(c1 != '\n') {
			newitem += c1;
			c1 = strng.GetChar(0); // peek
			strng.Remove(0,1); // pop
		}

		if( !(isHidden && !includeHidden) ) {
			r.Add(newitem);
		}
	}

	return r;
}


//Private:
wxString Networking::SSHSendCommand(wxString command)
{
	MaintainSettings();

	if(command == "") {
		// because I'm using boolean logic to determine the success of the command
		//  I need to have at least have true
		command = "true";
	}

	command += " && echo \"C_O_M_P_L_E_T_E_D_OK\"";

	ssh_plink->sendCommand(command);
	wxString output = ssh_plink->getOutput();

	// Confirm that all went according to plan
	if(output.Right(21) == "C_O_M_P_L_E_T_E_D_OK\n") {
		status = NET_GOOD;
		output.Truncate(output.Length()-21); // remove C_O_M_P_L_E_T_E_D_OK\n
	}
	else {
		DetermineStatusError(ssh_plink->getErrors(), ssh_plink->getOutput());
	}

	return output;
}


// Before Entering this code, the errlog and output should be accurate
void Networking::DetermineStatusError(wxString errlog, wxString output)
{
	while(errlog.Right(1) == "\n") {
		errlog.RemoveLast();
	}

	// Default:
	status = NET_ERROR_MESSAGE;
	statusDetails = errlog;

	if(errlog.Contains("key is not cached") && errlog.Contains("Connection abandoned.") ) {
		status = NET_UNKNOWN_HOST;	// and StatusDetail will be the key

		// Screen-scrape for the key:
		// these lines doesn't work properly... returns 0xffffff + 1 + 16, result is 0x00000010
		//int len = 1+ statusDetails.Index("fingerprint is:\n")+16;
		//statusDetails.Remove(0,len);
		/*	MPE:
			This regular expression looks for the following:
			one or more digits (0-9) followed by one more more whitespace characters
			followed by one or more pairs of two hex digits and a colon, followed by two final hex digits
				
			Since I know this is a valid regex, I'm leaving out the .IsValid() check that would otherwise
			be a good idea
		*/
		wxRegEx reFingerprint = "[[:digit:]]+[[:blank:]]+([[:xdigit:]]{2}:)+[[:xdigit:]]{2}";

		if(reFingerprint.Matches(errlog))
		{
			wxString match = reFingerprint.GetMatch(errlog);
			wxLogDebug("Matched fingerprint: \"%s\"", match);
			statusDetails = match;
		}
		else
		{
			wxLogDebug("Failed to match the fingerprint in: %s", errlog);
			statusDetails = "*unknown* - could not parse fingerprint";
		}

	}
	else if(errlog.Contains("Unable to authenticate")) {
		status = NET_AUTH_FAILED;
		statusDetails.RemoveLast(); // "\n"
	}
	else if(errlog.Contains("Permission denied")) {
		status = NET_WRITE_ERROR;
		while(errlog.Right(1) == "\n") {
			errlog.RemoveLast(); // Remove any trailing new-lines for aesthetic purposes
		}
		statusDetails = "Could not write to destination file or directory. (Check permissions)";
	}
	else if(errlog.Contains("Connection refused")) {
		status = NET_CONN_REFUSED;
		while(errlog.Right(1) == "\n") {
			errlog.RemoveLast(); // Remove any trailing new-lines for aesthetic purposes
		}
		statusDetails = errlog;
	}
	else if(false) {
		status = NET_READ_ERROR;
		statusDetails = "Read Error";
	}
	//else -- defaults

	
	return;
}


//returns true if any settings have changed.  I need to do this polling because Options is passive.
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

		// Update all the PlinkProcess's
		//walk the list of pointers

		//Update PlinkConnect
		ssh_plink->setHostname(newH);
		ssh_plink->setUsername(newU);
		ssh_plink->setPassphrase(newP);

		return true;
	}
	//else
	  return false;

}


// I recommend OptionsDialog call this after a change
NetworkStatus Networking::GetStatus()
{
	if(MaintainSettings()) {
		// Update the status
		SSHSendCommand("");
	}

	return status;
}


wxString Networking::GetStatusDetails()
{
	return statusDetails;
}


void Networking::SSHCacheFingerprint()
{
	ssh_plink->acceptCacheFingerprint();
	// Set the status:
	SSHSendCommand("");
}


void Networking::SCPDoTransfer(wxString from_path_name, wxString to_path_name)
{
	// right now this only does local -> remote transfers
	wxString cmd = m_options->GetPscpApp() // + " -l " + ssh_user 
					+ " -pw " + m_currPass + " -batch "
					+ from_path_name + " " 
					+ m_currUser + "@" + m_currHost + ":" + to_path_name;
	wxProcess* proc = new wxProcess();
	proc->Redirect();
	int pid = wxExecute(cmd, wxEXEC_SYNC, proc);

	if (pid == -1) {
		// Bad Exit
		// Could not start process
		status = NET_ERROR_MESSAGE;
		statusDetails = "Could not start the file transfer process.";
		delete proc;
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
		delete proc;
		//delete rin; // don't delete these
		//delete rerr;

		// Determine the success:
		if(output.Contains("100%")) {
			status = NET_GOOD;
			statusDetails = "";
		}
		else {
			DetermineStatusError(errlog, output);
		}
	}

	return;
}


wxProcess2* Networking::GetPlinkProcess(wxEvtHandler* owner)
{
	// I should call GetStatus();

	// Start the new Process
	wxString cmd = m_options->GetPlinkApp();
	if(m_currPass != "") { // Why am I checking for an empty pass?
		cmd += " -pw " + m_currPass;
	}
	if(m_currUser != "") { // Why am I checking for an empty user?
		cmd += " " + m_currUser + "@" + m_currHost;
	}
	else
	{
		cmd += " " + m_currHost;
	}
	
	wxProcess2* proc = new wxProcess2(owner);
	long pid = wxExecute(cmd, wxEXEC_ASYNC, proc);

	if(pid == 0) {
		//Command could not be executed
		wxLogDebug("Could not start a Plink process -- Command could not be executed.");
		delete proc; //proc = NULL;
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their operating system
		// User has DDE running under windows (OLE deprecated this)
		wxLogDebug("Could not start a Plink process(DDE).");
		delete proc; // proc = NULL;
	}
	else { // Process is Live
		proc->SetPID(pid); // Temporary Solution?
		//wxLogDebug("Started a Plink process successfully.");
	}

	// Add this process to an internal list?

	return proc;
}

wxProcess2* Networking::GetLocalProcess(wxEvtHandler* owner) {
	// ::wxGetOsVersion()
	wxString cmd = "cmd.exe"; // "command"

	wxProcess2* proc = new wxProcess2(owner);
	long pid = wxExecute(cmd, wxEXEC_ASYNC, proc);

	if(pid == 0) {
		//Command could not be executed
		wxLogDebug("Could not start a local process.");
		delete proc; //proc = NULL;
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their operating system
		// User has DDE running under windows (OLE deprecated this)
		wxLogDebug("Could not start a local process(DDE).");
		delete proc; // proc = NULL;
	}
	else { // Process is Live
		proc->SetPID(pid); // Temporary Solution?
		wxLogDebug("Started a local process successfully.");
	}

	// Add this process to an internal list?

	return proc;
}