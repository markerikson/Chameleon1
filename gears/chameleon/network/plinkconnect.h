#ifndef __PLINK_CONNECT__H__
#define __PLINK_CONNECT__H__
//=================================================================
// PlinkConnect
//=================================================================
#include <wx/txtstrm.h>
#include <wx/process.h>
#include <wx/event.h>


const char PLINK_LOCATION[] = "plink.exe";
const char HOSTNAME[] = "163.11.160.218";
const char USERNAME[] = "danroeber";
const char PASSPHRASE[] = "dayspring";


class PlinkConnect : public wxEvtHandler {

	public:
		// ..structors:
		PlinkConnect();
		~PlinkConnect();
		// Methods:
		wxString send(wxString cmd);
		wxString getOutput();
		bool getIsConnected();
		// Event Handlers:
		void onProcessTermEvent(wxProcessEvent &UNUSED);
		//void onTimerEvent(wxEvent &UNUSED);

	private:
		void connect();
		wxProcess* proc;
		bool isConnected;
		long pid;
		wxString plinkApp, host, user, pass;
		wxInputStream* rin;
		wxTextInputStream* rerr;

	public:
		DECLARE_EVENT_TABLE()
};


#endif //__PLINK_CONNECT__H__
