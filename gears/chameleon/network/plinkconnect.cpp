//////////////////////////////////////////
//
//
//
// Notes: upon completion of the constructor, PlinkConnect
//        will be sitting with clear output, and input at
//        a blank command line
//
//
////////////////////////////////////////

#include <wx/utils.h>
#include <wx/txtstrm.h>
#include "plinkconnect.h"


// Event Table Declaration
BEGIN_EVENT_TABLE(PlinkConnect, wxEvtHandler)
	EVT_END_PROCESS(-1, PlinkConnect::onProcessTermEvent)
END_EVENT_TABLE()


PlinkConnect::PlinkConnect(wxString plinkApplication, wxString hostname,
				wxString username, wxString passphrase) {
	// Set default values:
	plinkApp = plinkApplication;
	host = hostname;
	user = username;
	pass = passphrase;
	output = "";
	errlog = "";
	proc = NULL;
	isConnected = false;
	pid = -2;
	rin = NULL;
	rerr = NULL;

	// Catch Events
	//wxEvtHandler::SetEvtHandlerEnabled(true);  <-- default
}

PlinkConnect::~PlinkConnect() {
	if(isConnected) {
		// do I need to wxExecute::Kill() the process?
		delete proc;
	}
}

// This function creates a wxProcess to asynchornously execute the command in it's
//   own thread.  It gives it a kick start by making sure that the connection does
//   successfully connect.  After this it is finished and output is left for
// 
void PlinkConnect::sendCmd(wxString command) {
	proc = new wxProcess(this);
	proc->Redirect();

	wxString cmd = plinkApp + " -pw " + pass + " "+ user + "@" + host
					+ " \"echo cHaMeLeOnCoNnEcTeD && "
					+ " ls" // <-- command
					//"find -maxdepth 1 -type f"
					+ " \"";

	pid = wxExecute(cmd, wxEXEC_ASYNC, proc); //wxEXEC_NOHIDE

	if(pid == 0) {
		isConnected = false;
		output = "Connection Failed.  Possibly plink.exe wasn't found.";
		delete proc;
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their system
		// User has DDE running under windows (OLE deprecated this)
		isConnected = false;
		output = "Get a real operating system.  www.kernel.org";
		delete proc;
	}
	else { // Process is Live!  Good so far...
		rin = proc->GetInputStream();
		rerr = proc->GetErrorStream();

		isConnected = true; // conditionally

		// Assume either will connect, or process will term(isConnected becomes false)
		// !!!WARNINING!!!  may get stuck here
		char c = 'a', e = 'a';
		while(isConnected && (c != '\n' && output.Right(19) != "cHaMeLeOnCoNnEcTeD\n") ) {

			//if(!rin->Eof()) {
			//if(rin != NULL) {
			if(proc->IsInputAvailable()) {
				//rin = proc->GetInputStream();
				c = rin->GetC();  // store the character for speed increases
				output += c;
			}
			if(proc->IsErrorAvailable()) {
				wxInputStream* rer = proc->GetErrorStream();
				e = rerr->GetC();  // store the character for speed increases
				errlog += e;
			}

			if(e == ' ' && errlog.Right(13) ==  "cache? (y/n) ") {
				// ..."Store key in cache? (y/n) "
				errlog = "";
				send("n\n");
			}
			if(errlog.Left(11) == "FATAL ERROR") {
			//	// ie. "FATAL ERROR: Network error: Connection refused"
				isConnected = false; // leave cleanup for processterm()
			}
			if(errlog.Left(11) == "FATAL ERROR") {
				// ie. "FATAL ERROR: Unable to authenticate"
				isConnected = false; // leave cleanup for processterm()
			}
		}
		// If successful connection, erase the signal text
		if(isConnected) {
			//output.Remove(output.Len()-18, 18);// -= "cHaMeLeOnCoNnEcTeD";
			output = "";
		}
	}

	//return;
}




//  Assumes isConnected was true.
//  This may be a problem if the process terminates when establishing a connection
// Public:    (only used for event handling)
void PlinkConnect::onProcessTermEvent(wxProcessEvent &event) {
	// Get any remaining output
	// event.m_id event.pid event.status
	while(!rin->Eof()) {
		output += rin->GetC();
	}
	while(!rerr->Eof()) {
		errlog += rerr->GetC();
	}

	isConnected = false;
	delete proc;
	//delete rin;  // this is just a pointer to an internal of proc's
	//delete rerr; // ditto

}

// Private
void PlinkConnect::send(wxString cmd) {
	if(isConnected) {
		//open
		wxTextOutputStream os(* (proc->GetOutputStream()) );
		//send
		os.WriteString(cmd);
		//close
		proc->CloseOutput();
	}
}

// Public:
bool PlinkConnect::getIsConnected() {
	// if called during the split second the connection is being
	//  started, this may return a true-negative (true, but not
	//  really connected [yet]).  <-- not thread safe (?)
	return isConnected;
}

// Public:
wxString PlinkConnect::getOutput() {
	return output;
}

// Public:
wxString PlinkConnect::getErrors() {
	return errlog;
}



