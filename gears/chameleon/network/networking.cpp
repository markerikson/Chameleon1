////////////////////////////////////////////////////
//
//
// ToDo: prevent anything from being used before SetPlinkProg is called
//
//
////////////////////////////////////////////////////

#include "networking.h"
//#include "../common/datastructure.h"
#include <wx/utils.h>
#include <wx/regex.h> // for finding the host fingerprint
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/process.h>
#include "../common/debug.h"
#include "../common/Options.h"


Networking::Networking(Options* options)
{
	m_options = options;
	//ssh_host = "localhost";
	//ssh_user = "user";
	//ssh_pass = "password";
	//downloadDir = ""; // should cause the working directory to be used
	//plinkApp = "plink.exe"; // will use the working directory
	//pscpApp = "pscp.exe";
	status = NET_GOOD;
	ssh_plink = new PlinkConnect(m_options->plinkPath, m_options->hostname, m_options->username, m_options->password);
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


void Networking::SetDetailsNoStatus(wxString hostname, wxString username, wxString passphrase) 
{
	/*
	if(!hostname.IsEmpty()) {
		ssh_host = hostname;
		ssh_plink->setHostname(hostname);
	}
	if(!username.IsEmpty()) {
		ssh_user = username;
		ssh_plink->setUsername(username);
	}
	if(!passphrase.IsEmpty()) {
		ssh_pass = passphrase;
		ssh_plink->setPassphrase(passphrase);
	}
	*/

	userHomeDir = wxEmptyString;

	//if(hostname != "" || username != "") {
	//	//will need to empty directory cache
	//}
}


//Maybe this should be requirements for the constructor -- or maybe not, because (in theory)
//  Networking doesn't have to be SSH
//If any of these are "" those will be unchanged
void Networking::SetDetails(wxString hostname, wxString username, wxString passphrase)
{
	SetDetailsNoStatus(hostname, username, passphrase);

	// Set the status
	SSHSendCommand("");
}


/*
void Networking::SetPlinkProg(wxString path_name)
{
	// bool wxFileExists(const wxString& filename)
	plinkApp = path_name;
	ssh_plink->setPlinkApp(plinkApp);
}
*/


/*
void Networking::SetPscpProg(wxString path_name)
{
	// bool wxFileExists(const wxString& filename)
	pscpApp = path_name;
}
*/


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
//   in the integrity of the strng I'm parsing, because I know
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

wxString Networking::SSHSendCommand(wxString command)
{
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



NetworkStatus Networking::GetStatus()
{
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


// I have decided, because SCP is always a 1 shot thing that 
void Networking::SCPDoTransfer(wxString from_path_name, wxString to_path_name)
{
	// right now this only does local -> remote transfers
	
	/*
	wxString cmd = pscpApp // + " -l " + ssh_user 
					+ " -pw " + ssh_pass + " -batch "
					+ from_path_name + " " 
					+ ssh_user + "@" + ssh_host + ":" + to_path_name;
	*/

	wxString cmd = m_options->pscpPath 
					+ " -pw " + m_options->password 
					+ " - batch "
					+ from_path_name + " "
					+ m_options->username + "@" + m_options->hostname + ":" + to_path_name;
					
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

/*
wxString Networking::GetPlinkProg()
{
	return plinkApp;
}
*/