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
	message = "";
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

// This function starts a wxProcess to asynchornously execute the command.
// A connection is made, the command is run, then the connection is ended.
// So the output will not be accurate until the connection has ended.
void PlinkConnect::sendCmd(wxString command) {
	//default to trying a command as a batch job
	sendCommand(command, true);
}

// Private:
void PlinkConnect::sendCommand(wxString command, bool isBatch) {
	output = ""; // clear the ouput log
	errlog = ""; // clear the error log
	lastcmd = command;

	proc = new wxProcess(this);
	proc->Redirect();

	wxString batch = " -batch ";
	if(!isBatch) { batch = ""; }
	
	wxString cmd = plinkApp
		+ batch
		+ " -pw " + pass
		+ " -l " + user
		+ " " + host + " "
		+ " \"echo cHaMeLeOnCoNnEcTeD && " //wrapper
		+ " " + command // <--- command
		+ " && echo cHaMeLeOnDoNe\""; // wrapper

	pid = wxExecute(cmd, wxEXEC_ASYNC, proc); //wxEXEC_NOHIDE

	if(pid == 0) {
		isConnected = false;
		message = "Connection Failed.  Possibly plink.exe wasn't found.";
		delete proc;
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their system
		// User has DDE running under windows (OLE deprecated this)
		isConnected = false;
		message = "Get a real operating system.  www.kernel.org --djc";
		delete proc;
	}
	else { // Process is Live!
		rin = proc->GetInputStream();
		rerr = proc->GetErrorStream();

		isConnected = true;
	}

	return;
}




//  It is my understanding that this will is not as thread independant as I thought
//  (ie. the thread really isn't independant until after (at least) the function
//    that created it has ended -- sendCmd in my case)
// Only used for Event Handling
void PlinkConnect::onProcessTermEvent(wxProcessEvent &event) {
	// Get any output.
	// Surprisingly, this is actually where all the output
	//  is gathered!  Because the connection ends when the
	//  command ends I wait to gather the output here.
	while(!rin->Eof()) {
		output += rin->GetC();
	}
	while(!rerr->Eof()) {
		errlog += rerr->GetC();
	}

	// special case:
	if(errlog.Contains("key is not cached") && errlog.Contains("Connection abandoned.") ) {
		isConnected = false;
		delete proc;
		// If plink is run as -batch it will terminate
		//  when "Store key in cache? (y/n) " is asked
		//  so, I need to rerun the process not in batch,
		//  and send the appropriate "n"
		sendCommand(lastcmd, false);
		send("n\n");
	}

	else { // Process has finished it's job

		cleanlogs();

		// Set exit status:
		//event.m_id event.pid event.status

		isConnected = false;
		delete proc;
		//delete rin;  // this is just a pointer to an internal of proc's
		//delete rerr; // ditto
	}

	return;
}



// Private:
void PlinkConnect::cleanlogs() {
	// Clean the output and determine the success of the command:

	if(errlog.Contains("key is not cached")) {
		message = "The server's host key is not cached in the registry.";
	}
	else if(errlog.Contains("FATAL ERROR")) {
		// ie. "FATAL ERROR: Network error: Connection refused"
		// ie. "FATAL ERROR: Unable to authenticate"
		message = errlog;
	}
	if(output.Contains("cHaMeLeOnCoNnEcTeD\n") ) {
		// Yeah!  It succeeded
		//message = "success"; // would overwrite fingerpring caching message
		int start = output.Find("cHaMeLeOnCoNnEcTeD\n") + 19;
		int len = output.Find("cHaMeLeOnDoNe\n") - start;
		output = output.Mid(start, len);
	}

	return;
}


// Private
void PlinkConnect::send(wxString strng) {
	if(isConnected) {
		//open
		wxTextOutputStream os(* (proc->GetOutputStream()) );
		//send
		os.WriteString(strng);
		//close
		proc->CloseOutput();
	}
}

// Public:
bool PlinkConnect::getIsConnected() {
	return isConnected;
}

// Public:
wxString PlinkConnect::getOutput() {
	// Output will not be complete until isConnected == false
	//output.Remove(output.Len());
	return output;
}

// Public:
wxString PlinkConnect::getErrors() {
	// The error log will not be complete until isConnected == false
	return errlog;
}

// Public:
wxString PlinkConnect::getMessage() {
	return message;
}

