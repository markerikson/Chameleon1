#ifndef __PLINK_CONNECT__H__
#define __PLINK_CONNECT__H__

//=================================================================
// PlinkConnect
//
// Notes: This redesign allows P.C. to take full responsibility for
//         the remote processes.
//
//=================================================================

#define _PC_INTERNAL_TIMER_  //for testing purposes
#ifdef _PC_INTERNAL_TIMER_
	#define POLL_RATE 10 //milliseconds
#endif

#include <wx/wx.h>
class wxProcessEvent;
class wxTextOutputStream;


enum PCConnState { // this is used exclusively inside PlinkConnect
	PC_STARTING,	// Waiting for Login confirmation
	PC_READY,		// Connection has been established. Nothing going
					//   on.  Just waiting to be used.
	PC_EXECUTING,	// Very short phase between sending the command
					//   and getting the starttoken
	PC_BUSY,		// Have received the starttoken, so all output is
					//   sent to the "owner"
	PC_ENDING		// Have received the endtoken.  Now in doing term
					//   sequence
};

typedef struct
{
	long pid;
	wxProcess* proc;
	PCConnState state;
	wxTextOutputStream* stdinStream;
	wxString outputBuf;
	wxEvtHandler* owner;
} ProcessInfo;

WX_DECLARE_LIST(ProcessInfo, ProcessInfoList);


class PlinkConnect : public wxEvtHandler {

	public:
		// ..structors:
		PlinkConnect(wxString plinkApp, wxString host, wxString user, wxString pass);
		~PlinkConnect();

		// Methods:
		wxTextOutputStream* executeCommand(wxString command, wxEvtHandler* listener);
		wxString executeSyncCommand(wxString command);

		// Polling:
		void PollTick();

		// Gets/Sets
		void setLogin(wxString hostname, wxString username, wxString passphrase);
		bool getIsConnected();
		wxString getMessage() { return m_message; }

	private:
		// Methods:
		void spawnConnection();
		void parseOutput(ProcessInfo* proc, wxString output, wxString errlog = "");
		void terminateConnection(ProcessInfo* proc);
		void terminateAllConnections();

		// Events:
		void onTerminate(wxProcessEvent& event);

		// Data:
		wxString m_plinkApp, m_host, m_user, m_pass;
		wxString m_message;
		bool m_isConnected; // overall Plink-Network status
		ProcessInfoList m_processes;


#ifdef _PC_INTERNAL_TIMER_
		wxTimer m_timer;
		void OnTimerEvent(wxTimerEvent& event);
#endif

	
		DECLARE_EVENT_TABLE()
};


#endif //__PLINK_CONNECT__H__
