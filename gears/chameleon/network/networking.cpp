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


// This is a useless function
bool Networking::IsConnected()
{
	return isConnected;
}

wxString Networking::GetFile(wxString file, wxString path)
{
	// !!!!INCOMPLETE!!!!
	return "F:\\Projects\\School\\cs4810\\gears\\chameleon\\gui\\Chameleon.cpp";
}


//no trailing "/"
// internally could also manage dirListing caches to increase speed
DirListing Networking::GetDirListing(wxString dirPath)
{
	return GetSSHDirListing(dirPath);
}

int Networking::SendFile(wxString lfile, wxString lpath, wxString rfile, wxString rpath)
{
	// !!!!INCOMPLETE!!!!
	return 0;
}

//no trailing "/" ???
void Networking::SetDefaultDownloadDir(wxString path)
{
	if(path.Right(1) != "/") { // precautionary check
		path += "/";
	}
	downloadDir = path;
}

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

// This had better be accurate, because I don't handle an error in this well at all
void Networking::SetPscpProg(wxString path_name)
{
	pscpApp = path_name;
}

DirListing Networking::GetSSHDirListing(wxString dirPath)
{
	DirListing r;

	// Files:
	ssh_plink->sendCmd("cd " + dirPath + " && find -maxdepth 1 -type f");
	wxString files = ssh_plink->getOutput();
	r.fileNames = ParseLS(files);
	// Directories:
	ssh_plink->sendCmd("cd " + dirPath + " && find -maxdepth 1 -type d");
	wxString dirs = ssh_plink->getOutput();
	if(dirs.Left(2) == ".\n") {
		dirs.Remove(0,2);
	}
	r.dirNames = ParseLS(dirs);

	return r;
}


// This parsing can easily be broken!  But, I should be pretty confident
//   in the integrity of the, because I know what's coming in.
// Private:
//wxArrayString Networking::ParseLS(wxString strng, bool includeHidden);
wxArrayString Networking::ParseLS(wxString strng) {
	wxArrayString r;
	r.Empty();

	while(!strng.IsEmpty()) {

		if(strng.Length() < 2) {
			return r;
		}
		// Get the "./"
		//char c1 = strng.Remove(0,1);
		//char c2 = strng.Remove(0,1);
		char c1 = strng.GetChar(0); // peek
		char c2 = strng.GetChar(1); // peek
		strng.Remove(0,2); // pop
		if(c1 != '.' && c2 != '/' || strng.IsEmpty()) {
			return r;
		}

		wxString newitem = "";

		//c1 = strng.Remove(0,1);
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
			//c1 = strng.Remove(0,1);
		}

		if(!isHidden) {
			r.Add(newitem);
		}
	}

	return r;
}


