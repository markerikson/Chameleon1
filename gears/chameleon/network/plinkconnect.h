#ifndef __PLINK_CONNECT__H__
#define __PLINK_CONNECT__H__

//=================================================================
// PlinkConnect
//=================================================================
#include <wx/process.h>
#include <wx/event.h>


class PlinkConnect : public wxEvtHandler {

	public:
		// ..structors:
		PlinkConnect(wxString plinkApplication, wxString hostname,
					wxString username, wxString passphrase);
		~PlinkConnect();
		// Methods:
		void sendCmd(wxString command);
		bool isBusy();

		// gets/sets
		wxString getOutput();
		wxString getErrors();
		wxString getMessage();
		void setPlinkApp(wxString path_name); //path+name
		void setHostname(wxString hostname);
		void setUsername(wxString username);
		void setPassphrase(wxString passphrase);


		// Event Handlers:
		//void onProcessTermEvent(wxProcessEvent &event);  // Dont't forget an Event in Event Table
		void onProcessTermEvent();

	private:
		// Methods:
		void send(wxString strng);
		void sendCommand(wxString command, bool isBatch);
		void cleanlogs();
		// Data:
		wxString plinkApp, host, user, pass;
		wxProcess* proc;
		bool isConnected;
		long pid;
		wxString output;
		wxString errlog;
		wxString message;
		wxString lastcmd;
		// Streams:
		wxInputStream* rin;
		wxInputStream* rerr;

		//to be removed:
		bool confirmConnected();

	public:
		DECLARE_EVENT_TABLE()
};


#endif //__PLINK_CONNECT__H__
