#include <wx/utils.h>
#include <wx/txtstrm.h>
#include "plinkconnect.h"
#include <iostream>


// Event Table Declaration
BEGIN_EVENT_TABLE(PlinkConnect, wxEvtHandler)
	EVT_END_PROCESS(-1, PlinkConnect::onProcessTermEvent)
END_EVENT_TABLE()


PlinkConnect::PlinkConnect() {
	// Set default values:
	plinkApp = PLINK_LOCATION;
	host = HOSTNAME;
	user = USERNAME;
	pass = PASSPHRASE;
	
	isConnected = false;

	pid = -1;
	rin = NULL;
	rerr = NULL;

	// try connecting
	connect();

	// Catch Events
	//wxEvtHandler::SetEvtHandlerEnabled(true);  <-- default
}

PlinkConnect::~PlinkConnect() {
	if(isConnected) {
		delete proc;
	}
}

void PlinkConnect::connect() {
	proc = new wxProcess(this);
	proc->Redirect();

	wxString command = plinkApp +" "+ user + "@" + host + " -pw " + pass;

	pid = wxExecute(command, wxEXEC_ASYNC, proc);

	if(pid == 0) {
		isConnected = false;
		delete proc;
	}
	else if (pid == -1) {
		isConnected = false;
		delete proc;
		// BAD ERROR!  User ought to upgrade their system
		// User has DDE running under windows (OLE deprecated this)
	}
	else {
		isConnected = true;
		rin = proc->GetInputStream();
		rerr = new wxTextInputStream( * proc->GetErrorStream() );
	}
}


void PlinkConnect::onProcessTermEvent(wxProcessEvent &event) {
	isConnected = false;
	delete proc;
	delete rerr;

	//this is where I can try to reconnect again
}

wxString PlinkConnect::send(wxString cmd) {
	wxString output = "";

	if(isConnected) {
		//open
		wxTextOutputStream rin(* (proc->GetOutputStream()) );
		//send
		rin.WriteString(cmd);
		//close
		proc->CloseOutput();
		
		output = getOutput();
	}

	return output;
}

bool PlinkConnect::getIsConnected() {
	return isConnected;
}

wxString PlinkConnect::getOutput() {
	wxString returnValue = "no input available\n";

	if(isConnected) {
		while(proc->IsInputAvailable()) {
			wxInputStream* rin = proc->GetInputStream();
			returnValue = returnValue + rin->GetC();
		}
	}
	//else

	return returnValue;
}
