/***********************
 *
 * Alternative's that may be considered.  My prefernce(for now), are the ones being used.
 *   wxString GetFile(wxString path_file);
 *   wxString GetFileContents(wxString file, wxString path);
 *   wxString GetFileContents(wxString path_file);
 *   int SendFile(wxString lpath_file, wxString rpath_file);
 *   int SendFileContents(wxString file, wxString path);
 *   wxString GetFileContents(wxString path_file);
 *
 * Consideration needs to be made for (perceived) overwriting of files
 *
 *
 * Internally, I will keep the trailing / on directories, but...
 * Externally, I will receive pathes without trailing /
 *
 * When I have path_name that means path+name (ie. "c:/windows/plink.exe")
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
		void SetDefaultDownloadDir(wxString path);    //no trailing "/" ???

		// Methods
		bool IsConnected();
		wxString GetHomeDirPath();
		DirListing GetDirListing(wxString dirPath);
		wxString GetFile(wxString file, wxString path); // returns path+filename location of file locally
		//wxString GetFile(wxString path_name); // for consistency's sake?
		int SendFile(wxString lfile, wxString lpath, wxString rfile, wxString rpath);
		//int SendFile(wxString lpath_name, wxString rpath_name); // consistancy?

		// SSH Specific
		//No longer maintaining a live connection,
		//  all commands done in 'batch' mode
		//int SSHConnect();
		//int SSHDisconnect();


	private:
		// Methods:
		DirListing GetSSHDirListing(wxString dirPath);
		wxArrayString ParseLS(wxString strng);
		// Data:
		bool isConnected;
		PlinkConnect* ssh_plink;
		//PscpConnect* scp_pscp;
		wxString ssh_host, ssh_user, ssh_pass;
		wxString plinkApp, pscpApp;
		wxString downloadDir;
};
