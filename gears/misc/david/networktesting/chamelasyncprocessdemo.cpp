#include "chamelasyncprocessdemo.h"

// Event Table Declaration
BEGIN_EVENT_TABLE(ChamelAsyncProcessDemo, wxEvtHandler)
	//EVT_END_PROCESS(-1, ChamelAsyncProcessDemo::onProcessTermEvent)
	EVT_PROCESS2_ENDED(ChamelAsyncProcessDemo::onProcessTermEvent)
	EVT_PROCESS2_STDOUT(ChamelAsyncProcessDemo::onProcessOutputEvent)
	EVT_PROCESS2_STDERR(ChamelAsyncProcessDemo::onProcessErrOutEvent)
END_EVENT_TABLE()


ChamelAsyncProcessDemo::ChamelAsyncProcessDemo(wxTextCtrl* outBox) {
	proc = NULL;
	pid = -2; // default (-1 can mean something)
	outputScreen = outBox;

	startProcess();
}

ChamelAsyncProcessDemo::~ChamelAsyncProcessDemo() {
	// At this point, no more event interactions will occur.  So, it would be
	//    impossible to know if an 'exit' or 'close' command worked.
	// Likewise, I don't believe it would be appropriate to try to send an
	//    'exit' or 'quit' command.  - DJC????
	if(isLive) {
		SetEvtHandlerEnabled(false); // don't try to catch any more events (specifically terminate)

		int i = wxKill(pid);
		switch (i) {
			case wxKILL_OK:
				// no error
				wxLogDebug("Successfully sent SIGKILL to process(%d)", pid);
				break;
			case wxKILL_BAD_SIGNAL:
				// no such signal
				wxLogDebug("Error killing process(%d) -- No SIGKILL signal.", pid);
				break;
			case wxKILL_ACCESS_DENIED:
				// permission denied
				wxLogDebug("Error killing process(%d) -- Permission Denied.", pid);
				break;
			case wxKILL_NO_PROCESS:
				// no such process
				wxLogDebug("Error killing process(%d) -- No such process.", pid);
				break;
			case wxKILL_ERROR:
				// another, unspecified error
				wxLogDebug("Error killing process(%d) -- wxKILL_ERROR, unspecified.", pid);
				break;
			default:
				// should never get here
				wxLogDebug("Error killing process(%d) -- Unknown wxKill return value.", pid);
				break;
		}

		//delete proc;
	}
}


void ChamelAsyncProcessDemo::putStdIn(wxString strng) {
	if(isLive) {
		wxTextOutputStream os(* (proc->GetOutputStream()) );
		//send
		os.WriteString(strng);
		//proc->CloseOutput();  //<-- don't do this until the very last writing (or never)
	}
}


//private:
void ChamelAsyncProcessDemo::onProcessOutputEvent(wxProcess2StdOutEvent &e) {
//	if(isLive) {
//	if(e.GetPid() == pid) {
		wxString m = e.GetOutput();
		(*outputScreen) << "Process(" << pid << ")-StdOut----------------------------------------------------------------------------------------------------\n"
						<< m
						<< "\n---------------------------------------------------------------------------------------------------------------------------------------\n";
//	}
	//else it's someone else's event
//	}
if(e.GetOutput().Contains("exit")) {
	int i;
	while(isLive) {
		i = 5;
		wxSafeYield();
	}
	i = 7;
}

}


//private:
void ChamelAsyncProcessDemo::onProcessErrOutEvent(wxProcess2StdErrEvent &e) {
//	if(isLive) {
//	if(e.GetPid() == pid) {
		wxString m = e.GetError();
		(*outputScreen) << "Process(" << pid << ")-StdErr----------------------------------------------------------------------------------------------------\n"
						<< m
						<< "\n---------------------------------------------------------------------------------------------------------------------------------------\n";
//	}
	//else it's someone else's event
//	}
}


//private:
void ChamelAsyncProcessDemo::onProcessTermEvent(wxProcess2EndedEvent &e) {
//void ChamelAsyncProcessDemo::onProcessTermEvent(wxProcessEvent &e) {
	if(e.GetPid() == pid) {
		(*outputScreen) << "Process(" << pid << ")-Terminated\n";
		pid = -2; // a nothing pid
		isLive = false;
		//delete proc;
		//wxLogDebug("Deleted proc(%d)\n", pid);
	}
	//else it's someone else's event
}


//private:
void ChamelAsyncProcessDemo::startProcess() {
	// Start the new Process
	proc = new wxProcess2(this);
	pid = wxExecute("plink.exe -pw dayspring danroeber@163.11.160.218", wxEXEC_ASYNC, proc); // || wxEXEC_NOHIDE

	if(pid == 0) {
		//Command could not be executed
		(*outputScreen) << "Process could not be executed.\n";
		delete proc;
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their operating system
		// User has DDE running under windows (OLE deprecated this)
		(*outputScreen) << "Get a real operating system!\n";
		delete proc;
	}
	else { // Process is Live!
		isLive = true;
		(*outputScreen) << "Started Process.  PID: " << pid << "\n";
	}
}