//////////////////////////////////////////
//
// Notes: This code has Plink.exe used SYNCHRONOUSLY which means
//         that when a command is sent, the main thread waits for it
//         to end before continuing.  This is (sort of) alright
//         because Plink is now run in 'batch' mode, which means it
//         runs whatever command is pass on the commandline, then
//         it terminates.
//
// ToDo: In ScrubLogs
//
////////////////////////////////////////

#include <wx/utils.h>
#include <wx/txtstrm.h>
#include "plinkconnect.h"


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
	pid = -2;
	proc = NULL;
	rin = NULL;
	rerr = NULL;

}

PlinkConnect::~PlinkConnect() {
	// There's nothing to do.
	//proc is almost always NULL
}

//Public:
wxString PlinkConnect::getOutput() {
	// Output 'log' is cleared before each sendCommand()
	return output;
}

//Public:
wxString PlinkConnect::getErrors() {
	// Error Log is cleared before each sendCommand()
	return errlog;
}

//Public:
wxString PlinkConnect::getMessage() {
	return message;
}

//Public:
void PlinkConnect::setPlinkApp(wxString path_name) {
	plinkApp = path_name;
}

//Public:
void PlinkConnect::setHostname(wxString hostname) {
	host = hostname;
}

//Public:
void PlinkConnect::setUsername(wxString username) {
	user = username;
}

//Public:
void PlinkConnect::setPassphrase(wxString passphrase) {
	if(passphrase.Contains("\"")) {
		// this user's passphrase just isn't going to work
	}
	passphrase = "\"" + passphrase + "\"";
	pass = passphrase;
}


//Public:
void PlinkConnect::sendCommand(wxString command) {
	// the choice to run in non-batch mode is not for the weak of heart
	//   it can easily cause the process to never terminate
	sendCommand(command, true);
}

// This function starts a wxProcess to asynchornously execute the command.
// A connection is made, the command is run, then the connection is ended.
//Private:
void PlinkConnect::sendCommand(wxString command, bool isBatch) {
	output = ""; // clear the ouput log
	errlog = ""; // clear the error log
	lastcmd = command;

	proc = new wxProcess(NULL);
	proc->Redirect();

	wxString batch = " -batch ";
	if(!isBatch) { batch = ""; }
	
	wxString cmd = plinkApp
		+ batch
		+ " -pw " + pass
		+ " -l " + user
		+ " " + host + " "
		+ " \"echo cHaMeLeOnCoNnEcTeD ; " //wrapper
		+ " " + command // <--- command
		+ " ; echo cHaMeLeOnDoNe\""; // wrapper

	pid = wxExecute(cmd, wxEXEC_SYNC, proc);

	if (pid == -1) {
		// Bad Exit
		message = "Could not start process.";
		delete proc;
	}
	else {
		// Not bad Exit
		rin = proc->GetInputStream();
		rerr = proc->GetErrorStream();

		// Grab the outputs
		while(!rin->Eof()) {
			output += rin->GetC();
		}
		while(!rerr->Eof()) {
			errlog += rerr->GetC();
		}

		scrubLogs();

		// Set exit status:

		delete proc;
		rin = NULL;
		rerr = NULL;
	}

	return;
}




//Private:
void PlinkConnect::scrubLogs() {
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


// I am assuming!!! that the connection should work, and I'm just doing this to
//  accept the cache.
//Public:
void PlinkConnect::acceptCacheFingerprint() {
	//start the program asynchonously

	// MPE: For some rason, running with this command line doesn't seem to 
	//		get Plink to properly save the key.  The simple command line does.
	//wxString cmd = plinkApp + " -batch -pw password nobody@" + host + " exit";

	// This simple command line does work right
	wxString cmd = plinkApp + " " + host;


	proc = new wxProcess(NULL);
	proc->Redirect();
	long pid = wxExecute(cmd, wxEXEC_ASYNC, proc);

	// MPE: leaving this in here in case David needs it in the future
	/*
#ifdef _DEBUG
	wxString tempOutput = "";
	wxString tempErrlog = "";
	// Grab the outputs
	rin = proc->GetInputStream();
	rerr = proc->GetErrorStream();
	while(!rin->Eof()) {
		tempOutput += rin->GetC();
	}
	while(!rerr->Eof()) {
		tempErrlog += rerr->GetC();
	}
#endif
	*/

	sendToStream("y\n");

	// Sending wxSIGTERM to the process is all that's needed to close it out gracefully.
	// It also takes care of deleting the process, so we don't have to do that manually.
	proc->Kill(pid, wxSIGTERM);

	/*
#ifdef _DEBUG
	while(!rin->Eof()) {
		tempOutput += rin->GetC();
	}
	while(!rerr->Eof()) {
		tempErrlog += rerr->GetC();
	}
	rin = NULL;
	rerr = NULL;
#endif
*/
}

//Private:
void PlinkConnect::sendToStream(wxString strng) {
	// Probably should check for a connection first

	//open
	wxTextOutputStream os(* (proc->GetOutputStream()) );
	//send
	os.WriteString(strng);
	//close
	proc->CloseOutput();
}



