/***********************
 *
 * Consideration needs to be made for (perceived) overwriting of files
 *
 * Internally, I will keep the trailing / on directories, but...
 * Externally, I will receive pathes without trailing /
 *
 * When I have path_name that means path+name (ie. "c:/windows/plink.exe")
 *
 * 1/18/04 - No longer passing files to/fro, instead passing the contents of the files
 *
 ***************************/

#include <wx/string.h>
#include "../common/datastructures.h"
#include "plinkconnect.h"


class Networking {
	public:
		// ..strcutors
		Networking();
		~Networking();

		//Setup
		void SetPlinkProg(wxString path_name);
		void SetPscpProg(wxString path_name);
		void SetDetails(wxString hostname, wxString username, wxString passphrase);
		//void SetDefaultDownloadDir(wxString path);

		// Methods
		//bool IsConnected(); No longer maintaining a live connection
		wxString GetHomeDirPath();
		DirListing GetDirListing(wxString dirPath, bool includeHidden = false);
		// Pass strings back and forth:
		wxString GetFileContents(wxString file, wxString path);
		int SendFileContents(wxString strng, wxString rfile, wxString rpath);
		// As opposed to:
		//wxString GetFile(wxString file, wxString path); // returns path+name of file locally
		//int SendFile(wxString lfile, wxString lpath, wxString rfile, wxString rpath);


	private:
		// Methods:
		DirListing SSHGetDirListing(wxString dirPath, bool includeHidden = false);
		wxString SSHSendCommand(wxString command);
		wxArrayString ParseLS(wxString strng, bool includeHidden);
		// Data:
		bool isConnected;
		PlinkConnect* ssh_plink;
		//PscpConnect* scp_pscp;
		wxString ssh_host, ssh_user, ssh_pass;
		wxString plinkApp, pscpApp;
		wxString downloadDir;
};
