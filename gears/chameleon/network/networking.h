#ifndef __CHAMELEON__NETWORKING__H__
#define __CHAMELEON__NETWORKING__H__
///////////////////////////////////////
//
// Internally, I (presently) am ignorant of trailing "/"s on directories
//
// Caution: sendFileContents will overwrite anything in it's way.
//
///////////////////////////////////////
#include <wx/wx.h>
#include <wx/string.h>
//#include <wx/utils.h> ???
#include <wx/filename.h>
#include "../common/datastructures.h"
#include "../common/debug.h"
class wxProcess;
class wxProcessEvent;
class PlinkConnect;
class Options;
class wxTextOutputStream;


class Networking : public wxEvtHandler {
	public:
		// ..strcutors:
		Networking(Options* options);
		~Networking();

		// Methods:
		NetworkStatus GetStatus();
		wxString GetStatusDetails();
		void SSHCacheFingerprint();
		//void PingOptions();

		// Remote Specific:
		bool GetHomeDirPath(wxString &homeDir);
		bool GetFileContents(wxFileName file, wxString &contents);
		bool SendFileContents(wxString strng, wxFileName file);
		bool GetDirListing(wxString dirPath, DirListing &list, bool forceRefresh = false,
							bool includeHidden = false);

		// Processes:
		wxTextOutputStream* StartRemoteCommand(wxEvtHandler* owner);
		wxTextOutputStream* StartLocalCommand(wxEvtHandler* owner);
		//wxString ExecuteRemoteCommand(wxString cmd);
		//wxString ExecuteLocalCommand(wxString cmd);
		//wxString ExecuteCommand(bool isRemote, wxString cmd);
		wxTextOutputStream* StartCommand(bool isRemote, wxEvtHandler* owner);
		void ForceKillProcess(wxTextOutputStream* w); // <-- ICKY ICKY ICKY

	private:
		// Events:
		void onTerm(wxProcessEvent &e);
		void onTimerTick(wxTimerEvent &e);

		// Methods:
		bool SSHGetHomeDirPath(wxString &homeDir);
		bool SSHGetFileContents(wxString file, wxString &contents);
		bool SCPDoTransfer(wxString from_path_name, wxString to_path_name);
		bool SSHGetDirListing(wxString dirPath, DirListing &listing,
								bool includeHidden = false);

		// Helpers:
		bool MaintainSettings();
		wxArrayString ParseFindOutput(wxString strng, bool includeHidden);
		bool SSHExecSyncCommand(wxString command, wxString &output);

		// Data:
		PlinkConnect* m_plinks;
		Options* m_options;
		wxString m_currHost, m_currUser, m_currPass;
		NetworkStatus m_status;
		wxString m_statusDetails;
		wxString m_userHomeDir; // mini-cache


		DECLARE_EVENT_TABLE()
};


#endif // __CHAMELEON__NETWORKING__H__
