#include "networking.h"
//#include "../common/datastructure.h"
#include <wx/utils.h>

Networking::Networking()
{
	//PscpConnect* scp_pscp;
	ssh_host = "localhost";
	ssh_user = "user";
	ssh_pass = "password";
	downloadDir = ""; // should cause the working directory to be used
	plinkApp = "plink.exe"; // will use the working directory
	pscpApp = "pscp.exe"; // will use the working directory
	isConnected = false;
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
DirListing Networking::GetDirListing(wxString dirPath, bool includeHidden)
{
	// dirPath has no trailing "/"
	return SSHGetDirListing(dirPath, includeHidden);
}

int Networking::SendFileContents(wxString strng, wxString rfile, wxString rpath)
{
	// !!!!INCOMPLETE!!!!
	return 0;
}

//no longer passing files to and fro
//void Networking::SetDefaultDownloadDir(wxString path)
//{
//	if(path.Right(1) != "/") { // precautionary check
//		path += "/";
//	}
//	downloadDir = path;
//}

// if any of these are "" those will be unchanged
void Networking::SetDetails(wxString hostname, wxString username, wxString passphrase)
{
	if(hostname != "") {
		ssh_host = hostname;
		ssh_plink->setHostname(hostname);
	}
	if(username != "") {
		ssh_user = username;
		ssh_plink->setUsername(username);
	}
	if(passphrase != "") {
		ssh_pass = passphrase;
		ssh_plink->setPassphrase(passphrase);
	}

	//if(hostname != "" || username != "") {
	//	//will need to empty directory cache
	//}
}


void Networking::SetPlinkProg(wxString path_name)
{
	// This had better be accurate, because I don't handle an error in this well at all
	plinkApp = path_name;
}

void Networking::SetPscpProg(wxString path_name)
{
	// This had better be accurate, because I don't handle an error in this well at all
	pscpApp = path_name;
}

wxString Networking::GetHomeDirPath() {
	// !!!! I don't know if this has a trailing /
	wxString cmd = "cd ~ && pwd ";
	return SSHSendCommand(cmd);
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
	ssh_plink->sendCommand(command);
	return ssh_plink->getOutput();

#if 0
/*
	// special case:
	if(errlog.Contains("key is not cached") && errlog.Contains("Connection abandoned.") ) {
		isConnected = false;
		delete proc;
		// If plink is run as -batch it will terminate
		//  when "Store key in cache? (y/n) " is asked
		//  so, I need to rerun the process not in batch,
		//  and send the appropriate "n"
		sendCommand(lastcmd, false);
		send("n\n");
	}
*/
#endif
}


