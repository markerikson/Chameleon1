#ifndef __CHAMELEON__NETWORKING__H__
#define __CHAMELEON__NETWORKING__H__

/***********************
 *
 * Internally, I (presently) am ignorant regarding  the trailing / on directories, but...
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
		void SetDetails(wxString hostname, wxString username, wxString passphrase);

		// Methods
		NetworkStatus GetStatus();
		wxString GetStatusDetails();
		wxString GetHomeDirPath(); // no trailing /
		DirListing GetDirListing(wxString dirPath, bool forceRefresh = false, bool includeHidden = false);
		wxString GetFileContents(wxString filename, wxString path);
		void SendFileContents(wxString strng, wxString rfilename, wxString rpath);
		void SSHCacheFingerprint();


	private:
		// Methods:
		DirListing SSHGetDirListing(wxString dirPath, bool includeHidden = false);
		wxString SSHSendCommand(wxString command);
		wxArrayString ParseLS(wxString strng, bool includeHidden);
		// Data:
		PlinkConnect* ssh_plink;
		wxString ssh_host, ssh_user, ssh_pass;
		wxString plinkApp;
		wxString downloadDir;
		NetworkStatus status;
		wxString statusDetails;
};


#endif // __CHAMELEON__NETWORKING__H__
