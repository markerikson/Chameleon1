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
 ***************************/

#include <wx/string.h>
#include "../common/datastructures.h"
#include "plinkconnect.h"
#include "../common/Options.h"
#include "../common/process2.h"
#include <wx/utils.h>
#include <wx/regex.h> // for finding the host fingerprint
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/process.h>
#include "../common/debug.h"


class Networking {
	public:
		// ..strcutors:
		Networking(Options* options);
		~Networking();

		// Setup -- This info is now known thru Options
		//void SetDetails(wxString hostname, wxString username, wxString passphrase); // Preferred
		//void SetDetailsNoStatus(wxString hostname, wxString username, wxString passphrase);

		// Methods
		NetworkStatus GetStatus();
		wxString GetStatusDetails();
		wxString GetHomeDirPath(); // no trailing /
		DirListing GetDirListing(wxString dirPath, bool forceRefresh = false,
															bool includeHidden = false);
		wxString GetFileContents(wxString filename, wxString path);
		void SendFileContents(wxString strng, wxString rfilename, wxString rpath);
		void SSHCacheFingerprint();

		// Processes:
		wxProcess2* GetPlinkProcess(wxEvtHandler* owner);
		wxProcess2* GetLocalProcess(wxEvtHandler* owner);


	private:
		// Methods:
		DirListing SSHGetDirListing(wxString dirPath, bool includeHidden = false);
		wxString SSHSendCommand(wxString command);
		void SCPDoTransfer(wxString from_path_name, wxString to_path_name);
		// Helpers:
		wxArrayString ParseLS(wxString strng, bool includeHidden);
		void DetermineStatusError(wxString errlog, wxString output);
		// Data:
		PlinkConnect* ssh_plink;
		Options* m_options;
		wxString m_currHost, m_currUser, m_currPass;
		NetworkStatus status;
		wxString statusDetails;
		wxString userHomeDir;
};


#endif // __CHAMELEON__NETWORKING__H__
