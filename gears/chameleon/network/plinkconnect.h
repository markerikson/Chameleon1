#ifndef __PLINK_CONNECT__H__
#define __PLINK_CONNECT__H__

//=================================================================
// PlinkConnect
//=================================================================
#include <wx/txtstrm.h>
#include <wx/process.h>
#include <wx/event.h>


class PlinkConnect : public wxEvtHandler {

	public:
		// ..structors:
		PlinkConnect(wxString plinkApplication, wxString hostname,
					wxString username, wxString passphrase);
		~PlinkConnect();
		// Methods:
		bool getIsConnected();
		void sendCmd(wxString command);
		wxString getOutput();
		wxString getErrors();
		wxString getMessage();

		// Event Handlers:
		void onProcessTermEvent(wxProcessEvent &UNUSED);
		//void onTimerEvent(wxEvent &UNUSED);

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
		// Streams:
		wxInputStream* rin;
		wxInputStream* rerr;

		//to be removed:
		bool confirmConnected();

	public:
		DECLARE_EVENT_TABLE()
};


#endif //__PLINK_CONNECT__H__
