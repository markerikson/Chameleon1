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
#include <wx/regex.h>
#include "../common/debug.h"


Networking::Networking()
{
	ssh_host = "localhost";
	ssh_user = "user";
	ssh_pass = "password";
	downloadDir = ""; // should cause the working directory to be used
	plinkApp = "plink.exe"; // will use the working directory
	//isConnected = false; // no longer needed
	status = NET_GOOD;
	ssh_plink = new PlinkConnect(plinkApp, ssh_host, ssh_user, ssh_pass);
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


// internally could also manage dirListing caches to increase speed
DirListing Networking::GetDirListing(wxString dirPath, bool forceRefresh, bool includeHidden)
{
	// dirPath has no trailing "/"
	return SSHGetDirListing(dirPath, includeHidden);
}


void Networking::SendFileContents(wxString strng, wxString rfile, wxString rpath)
{
	//wxString cmd = "cd " + rpath + " && echo \"" + strng + "\" > " + rfile + " ";

	// The "<<ENDOFFILE" says that all input on stdin will be piped to cat until
	// it reaches the characters ENDOFFILE
	wxString cmd = "cd " + rpath + " && cat > " + rfile + " <<ENDOFFILE";

	// tack the markers onto the file contents
	strng += "\nENDOFFILE";

	// send an enter to finish the command, send the file contents, send an
	// enter to finish the command
	cmd += "\n" + strng + "\n";
	SSHSendCommand(cmd);

	return;
}


//Maybe this should be requirements for the constructor -- or maybe not, because (in theory) Networking doesn't have to be SSH
void Networking::SetDetailsNoStatus(wxString hostname, wxString username, wxString passphrase) 
{
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

	//if(hostname != "" || username != "") {
	//	//will need to empty directory cache
	//}
}


// if any of these are "" those will be unchanged
void Networking::SetDetails(wxString hostname, wxString username, wxString passphrase)
{
	SetDetailsNoStatus(hostname, username, passphrase);

	// Set the status
	SSHSendCommand("");
}


void Networking::SetPlinkProg(wxString path_name)
{
	// This had better be accurate, because I don't handle an error in this well at all
	plinkApp = path_name;
}


wxString Networking::GetHomeDirPath() {
	// Does not return a trailing /
	wxString cmd = "cd ~ && pwd ";
	wxString r = SSHSendCommand(cmd);
	r.Remove(r.Length()-1,1); // remove the "\n"
	return r;
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
wxArrayString Networking::ParseLS(wxString strng, bool includeHidden){
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

wxString Networking::SSHSendCommand(wxString command) {
	command += " && echo \"C_O_M_P_L_E_T_E_D_OK\"";

	ssh_plink->sendCommand(command);
	wxString output = ssh_plink->getOutput();

	// Confirm that all went according to plan
	if(output.Right(21) == "C_O_M_P_L_E_T_E_D_OK\n") {
		status = NET_GOOD;
		output.Truncate(output.Length()-21); // remove C_O_M_P_L_E_T_E_D_OK\n
	}
	else {
		wxString errlog = ssh_plink->getErrors();
		wxString message = ssh_plink->getMessage();
		statusDetails = errlog;
		statusDetails.RemoveLast(); // "\n"

		if(errlog.Contains("key is not cached") && errlog.Contains("Connection abandoned.") ) {
			status = NET_UNKNOWN_HOST;	// and StatusDetail will be the key

			// Screen-scrape for the key:
			// these lines doesn't work properly... returns 0xffffff + 1 + 16, result is 0x00000010
			//int len = 1+ statusDetails.Index("fingerprint is:\n")+16;
			//statusDetails.Remove(0,len);
			/*	MPE:
				This regular expression looks for the following:
				one or more digits (0-9) followed by one more more whitespace characters
				followed by 15 pairs of two hex digits and a colon, followed by two final hex digits
				
				Since I know this is a valid regex, I'm leaving out the .IsValid() check that would otherwise
				be a good idea
			*/
			wxRegEx reFingerprint = "[[:digit:]]+[[:blank:]]+([[:xdigit:]]{2}:){15}[[:xdigit:]]{2}";

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
		//else if() {
		//	NET_READ_ERROR,
		//}
		//else if() {
		//	NET_WRITE_ERROR
		//}
		else {
			status = NET_ERROR_MESSAGE;
		}
	}

	return output;
}


NetworkStatus Networking::GetStatus() {
	return status;
}


wxString Networking::GetStatusDetails() {
	return statusDetails;
}


void Networking::SSHCacheFingerprint() {
	ssh_plink->acceptCacheFingerprint();
	// Set the status
	SSHSendCommand("");
}
