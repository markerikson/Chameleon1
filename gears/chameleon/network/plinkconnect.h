#ifndef __PLINK_CONNECT__H__
#define __PLINK_CONNECT__H__

//=================================================================
// PlinkConnect
//
// ToDo: I could probably do away with proc
//
//=================================================================
#include <wx/process.h>
#include <wx/event.h>


class PlinkConnect {

	public:
		// ..structors:
		PlinkConnect(wxString plinkApplication, wxString hostname,
					wxString username, wxString passphrase);
		~PlinkConnect();
		// Methods:
		void sendCommand(wxString command);
		void acceptCacheFingerprint();

		// gets/sets
		wxString getOutput();
		wxString getErrors();
		wxString getMessage();
		void setPlinkApp(wxString path_name); //path+name
		void setHostname(wxString hostname);
		void setUsername(wxString username);
		void setPassphrase(wxString passphrase);

	private:
		// Methods:
		void sendCommand(wxString command, bool isBatch);
		void sendToStream(wxString strng);
		void scrubLogs();
		// Data:
		wxString plinkApp, host, user, pass;
		wxProcess* proc; // I don't really need this
		long pid;
		wxString output;
		wxString errlog;
		wxString message;
		wxString lastcmd;
		// Streams:
		wxInputStream* rin;
		wxInputStream* rerr;

};


#endif //__PLINK_CONNECT__H__
