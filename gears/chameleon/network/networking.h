#ifndef __CHAMELEON__NETWORKING__H__
#define __CHAMELEON__NETWORKING__H__

/***********************
 *
 * Internally, I will keep the trailing / on directories, but...
 * Externally, I will receive pathes without trailing /
 *
 * When I have path_name that means path+name (ie. "c:/windows/plink.exe")
 * 
 * Caution: sendFileContents will overwrite anything in it's way.
 *
 * 1/18/04 - No longer passing files to/fro, instead passing the contents of the files
 *
 * Todo: I'm not following CS StyleGuide for method naming.  I don't know why I did
 *       this.  I'll change it at somepoint.  (Which means I'll be responsible for
 *       changing it in everyone else's code.)
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
		//void SetPscpProg(wxString path_name);  // No longer using
		void SetDetails(wxString hostname, wxString username, wxString passphrase);

		// Methods
		NetworkStatus GetStatus();
		wxString GetHomeDirPath();
		DirListing GetDirListing(wxString dirPath, bool forceRefresh = false, bool includeHidden = false);
		wxString GetFileContents(wxString filename, wxString path);
		void SendFileContents(wxString strng, wxString rfilename, wxString rpath);


	private:
		// Methods:
		DirListing SSHGetDirListing(wxString dirPath, bool includeHidden = false);
		wxString SSHSendCommand(wxString command);
		wxArrayString ParseLS(wxString strng, bool includeHidden);
		// Data:
		//bool isConnected;
		PlinkConnect* ssh_plink;
		wxString ssh_host, ssh_user, ssh_pass;
		wxString plinkApp;// pscpApp;
		wxString downloadDir;
		NetworkStatus status;
};


#endif // __CHAMELEON__NETWORKING__H__
