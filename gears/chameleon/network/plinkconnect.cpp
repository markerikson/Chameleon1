//////////////////////////////////////////
//
// Notes: There is always an available process -- the last in the list.  If I
//            start to use one, I open another one.
//        I designate methods as "ASYNCHRONOUS" if they don't have a wait-loop
//        If I assume that having had at least one connection determines a new
//            one will be ok, this might become asynchornous?
//
// Todo: Determine return status of run programs
//       Change struct ProcessInfo into a class (use good defaults & a destructor)
//       Instead of always killing and opening processes, I could keep some live
//
////////////////////////////////////////
#include <wx/txtstrm.h>
#include <wx/process.h>
#include <wx/listimpl.cpp> // CAREFUL!
#include "plinkconnect.h"
#include "../common/chameleonprocessevent.h"
#include "../common/datastructures.h"
#include "../common/debug.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif


#ifdef _PC_INTERNAL_TIMER_ //#ifdef's not allowed inside Event Table
BEGIN_EVENT_TABLE(PlinkConnect, wxEvtHandler)
	EVT_TIMER(-1, PlinkConnect::OnTimerEvent)
    EVT_END_PROCESS(-1, PlinkConnect::onTerminate)
END_EVENT_TABLE()
#else
BEGIN_EVENT_TABLE(PlinkConnect, wxEvtHandler)
    EVT_END_PROCESS(-1, PlinkConnect::onTerminate)
END_EVENT_TABLE()
#endif

// List & HashMap Types:
WX_DEFINE_LIST(ProcessInfoList);


// ASYNCHRONOUS
PlinkConnect::PlinkConnect(wxString plinkApp, wxString host, wxString user, wxString pass)
{
	m_plinkApp = plinkApp; // essentially this is assumed accurate, and can't change
	m_isConnected = false;

#ifdef _PC_INTERNAL_TIMER_
	m_timer.SetOwner(this);
	bool timerSuccess =	m_timer.Start(POLL_RATE);
	if(!timerSuccess) {
		wxLogDebug("PlinkConnect could not get a timer.\n");
	}
#endif

	m_host = host;
	m_user = user;
	m_pass = pass;

	spawnConnection(); // jumpstart the 1-always-open-connection
					   //   this will also establish my status
}


// SYNCHRONOUS
PlinkConnect::~PlinkConnect()
{
	// Send NO more events:
	for(ProcessInfoList::Node* node = m_processes.GetFirst(); node; node = node->GetNext() ) {
		ProcessInfo* p = node->GetData();
		if(p->owner != NULL) {
			wxLogDebug("BAD BAD - PlinkConnect Deleted with a process still live!");
			p->owner = NULL;
		}
	}

	terminateAllConnections();

	while(m_processes.GetCount() != 0) {
		// wait for the processes to remove themselves
		wxSafeYield();
	}
}


// Anything that is "" will be ignored
// This also acts as a way to restart/reset the connection/s
// ASYNCHRONOUS
void PlinkConnect::setLogin(wxString host, wxString user, wxString pass)
{
	// Set appropriate values:
	if(host != "") {
		m_host = host;
	}
	if(user != "") {
		m_user = user;
	}
	if(pass != "") {
		#ifdef _DEBUG
			if(pass.Contains("\"")) {
				// this user's passphrase just isn't going to work
				wxLogDebug("Passwords with \" will not work! -- Unstable state entered.");
			}
		#endif
		m_pass = "\"" + pass + "\"";
	}

	// No if-changed test - otherwise, why was this method called

	// Reset:
	terminateAllConnections();

	// Start:
	spawnConnection(); // try to start the 1-always-open-connection
}


// This will create a process and add it to the hash of processes, and append
//    it's pid to the tail of the PID list
// ASYNCHRONOUS
//Private:
void PlinkConnect::spawnConnection()
{
	wxProcess* proc = new wxProcess(this);
	proc->Redirect();

	wxString cmd = m_plinkApp
		+ " -pw " + m_pass + " "
		+ m_user + "@" + m_host;

	long pid = wxExecute(cmd, wxEXEC_ASYNC, proc);

	if(pid == 0) {
		//Command could not be executed
		wxLogDebug("Could not start a Plink process -- Command could not be executed.");
		m_message = "Could not start a Plink process -- Command could not be executed.";
		m_isConnected = false;
		//delete proc not needed because ASYNC
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their operating system
		// User has DDE running under windows (OLE deprecated this)
		wxLogDebug("Could not start a Plink process -- DDE in use.");
		m_message = "Could not start a Plink process -- DDE in use.";
		m_isConnected = false;
		//delete proc not needed because ASYNC
	}
	else { // Process is Live
		wxLogDebug("Starting a Plink process...");
		ProcessInfo* p = new ProcessInfo;
		p->pid = pid;
		p->proc = proc;
		p->state = PC_STARTING;
		p->stdinStream = new wxTextOutputStream(*proc->GetOutputStream(), wxEOL_UNIX);
		p->outputBuf = "";
		p->owner = NULL; // set when process gets used
		m_processes.Append(p);
	}

	return;
}


// SYNCHRONOUS
bool PlinkConnect::getIsConnected()
{
	while(m_processes.GetFirst() != NULL && m_processes.GetFirst()->GetData()->state == PC_STARTING) {
		// If the first connection is in the starting
		//   state, wait till "the dust settles"
		wxSafeYield();
	}

	return m_isConnected;
}


// ASYNCHRONOUS
bool PlinkConnect::getIsSettled()
{
	return m_processes.GetFirst() != NULL && m_processes.GetFirst()->GetData()->state != PC_STARTING;
}


//Public:
wxTextOutputStream* PlinkConnect::executeCommand(wxString command, wxEvtHandler* listener)
{
	return executeCmd(command, listener, false);
}


// This method should only be called if IsConnected().
// ASYNCHRONOUS (relatively)
//Private:
wxTextOutputStream* PlinkConnect::executeCmd(wxString command, wxEvtHandler* listener, bool isSynch)
{

	if(!getIsConnected()) { // <-- synchronous
		// Prevent a stupid-user error
		return NULL;
	}

	// Set the owner who will listen for Events
	ProcessInfo* p = (ProcessInfo*)m_processes.Last()->GetData();
	p->owner = listener;
	p->isRunSynch = isSynch;

	// Wrap the command:
	wxString cmd = "echo St_Ar_Tt_oK_eN ; " + command + " ; echo En_Dt_oK_eN\r";
	//wxString cmd = "echo St_Ar_Tt_oK_eN ; " + command + " && echo Ch_Ls_Uc_Es_S ; echo En_Dt_oK_eN\r";
	
	// Send it:
//wxLogDebug("PC(%d) cmd is: %s", p->pid, cmd);
	*(p->stdinStream) << cmd;
	p->state = PC_EXECUTING;

	spawnConnection(); // pre-spawn the next connection

	return 	p->stdinStream;

}


// SYNCHRONOUS
wxString PlinkConnect::executeSyncCommand(wxString command)
{
	executeCmd(command, NULL, true);

	// executeCommand spawns a new process, so my process is the next-to-last one
	ProcessInfo* p = (ProcessInfo*)m_processes.GetLast()->GetPrevious()->GetData();

	while(p->state == PC_BUSY || p->state == PC_EXECUTING) {
		// Perhaps this should terminate after an amount of time
		wxSafeYield();
	}

	terminateConnection(p);

	return p->outputBuf;
}


// Errlog default is ""
// Errlog it is useless except for during PC_STARTING
//Private:
void PlinkConnect::parseOutput(ProcessInfo* p, wxString output, wxString errLog)
{
//	wxLogDebug("Plink stdout: \"" + output + "\"");
//	wxLogDebug("Plink stderr: \"" + errLog + "\"");

	if(p->state != PC_ENDING) {
		p->outputBuf += output;
	}

	if(p->state == PC_STARTING) {
		if(p->outputBuf.Contains("Last login:")) { // <------------------------ FIX ME !!
			// Yeah!  It succeeded
			p->outputBuf = "";
			m_isConnected = true;

			// State transition:
			p->state = PC_READY;
		}
		else if(errLog.Contains("Store key in cache? (y/n)")) {
			// "Store key in cache? (y/n)"
			m_message += errLog;
			terminateConnection(p);
		}
		else {
			// Do nothing.  Just continue waiting.
//*(p->stdinStream) << "echo \"Last Login:\"" << endl;

			// (in the meantime:)
			m_message += errLog;
		}
		
	}

	if(p->state == PC_READY) {
		// nothing to do
//		wxLogDebug("PlinkConnect(%d): extranious output: \"%s\"", p->pid, p->outputBuf);
	}

	if(p->state == PC_EXECUTING) {
		if(p->outputBuf.Contains("St_Ar_Tt_oK_eN\r")) {
			// Scrub the output (remove start-token and prior)
			int end = p->outputBuf.Find("St_Ar_Tt_oK_eN\r")+16;
			p->outputBuf.Remove(0,end);
			p->state = PC_BUSY;
		}
	}

	if(p->state == PC_BUSY) {
		if(p->outputBuf.Contains("En_Dt_oK_eN")) {
			// Scrub the output (remove end-token and beyond)
			int start = p->outputBuf.Find("En_Dt_oK_eN");
			p->outputBuf.Remove(start);
			p->state = PC_ENDING;
		}

		//Throw Appropriate Events:
		if(p->outputBuf != "") {
			if(p->owner != NULL) { // if someone is listening
				ChameleonProcessEvent e(chEVT_PROCESS_STDOUT);
				e.SetString(p->outputBuf);
				p->owner->AddPendingEvent(e);
				// Clear the buffer:
				p->outputBuf = "";
			}
		}
		if(p->state == PC_ENDING) { // ie. just finished
			if(p->owner != NULL) {
				ChameleonProcessEvent e(chEVT_PROCESS_ENDED);
				e.SetInt(0); // <-- this is incomplete
				p->owner->AddPendingEvent(e);
			}

			// proc not being used any more, so terminate it
			if(!p->isRunSynch) {
				terminateConnection(p);
			}
			//else -- it's terminated from within execSynchCommand
			//           to prevent early deletion
		}
	}

	if(p->state == PC_ENDING) {
		// nothing to do
		#ifdef _DEBUG
			wxString temp = output;
			int start = temp.Find("En_Dt_oK_eN");
			if(start != -1) {
				temp.Remove(start);
			}
			
			if(temp != "") {
//				wxLogDebug("PlinkConnect(%d): extranious output: \"%s\"", p->pid, temp);
			}
		#endif
	}

	return;
}


// SYNCHRONOUS (?)
//Private:
void PlinkConnect::terminateConnection(ProcessInfo* p)
{
	if(p->state == PC_BUSY || p->state == PC_EXECUTING) {
		// Kill the plink process
		p->state = PC_ENDING;
		wxKill(p->pid, wxSIGKILL); // SIGTERM (doesn't always work!?)
	}
	else {
		//send exit:
		p->state = PC_ENDING;
		*(p->stdinStream) <<  "exit" << endl;
	}
	
	// All the termination clean-up is done in onTerminate()
}

	
// ASYNCHRONOUS
//Private:
void PlinkConnect::terminateAllConnections()
{
	m_isConnected = false;

	// Walk thru list m_procPID's
	for(ProcessInfoList::Node *node = m_processes.GetFirst(); node; node = node->GetNext() ) {
		ProcessInfo *p = node->GetData();
		terminateConnection(p);
	}
}


void PlinkConnect::PollTick() {
	// Check on all of the processes:
	for(ProcessInfoList::Node *node = m_processes.GetFirst(); node; node = node->GetNext() ) {
		ProcessInfo *p = node->GetData();

		wxProcess* proc = p->proc;
		wxString errout = "";
		wxString output = "";

		wxInputStream* pStdOut = proc->GetInputStream();
		while(proc->IsInputAvailable()) {
			output += pStdOut->GetC();
		}

		if(p->state == PC_STARTING) {
			// Also check STDERR if I'm in the starting phase
			wxInputStream* pStdErr = proc->GetErrorStream();
			while(proc->IsErrorAvailable()) {
				errout += pStdErr->GetC();
			}
		}

		if(output != "" || errout != "") {
//wxLogDebug("ProcessOutput(%d) - \"%s\"", p->pid, output);
			parseOutput(p, output, errout);
		}
	}

}


//Private:
void PlinkConnect::onTerminate(wxProcessEvent& event) {

	PollTick(); // Catch any last Outputs

	// Determine which process:
	long pid = event.GetPid();
	ProcessInfo* p;
	for(ProcessInfoList::Node* node = m_processes.GetFirst(); node; node = node->GetNext() ) {
		p = node->GetData();
		if(p->pid == pid) {
			break;
		}
	}

	//Remove and Delete the process:
	delete p->proc;
	delete p->stdinStream;
	m_processes.DeleteObject(p);
	delete p;

	if(m_processes.GetCount() == 0) {
		m_isConnected = false;
		wxLogDebug("All Plink Processes Terminated");
	}

}

void PlinkConnect::ForceKillProcess(wxTextOutputStream* w)
{
	// Determine which process:
	ProcessInfo* p;
	for(ProcessInfoList::Node* node = m_processes.GetFirst(); node; node = node->GetNext() ) {
		p = node->GetData();
		if( (int)&(*(p->stdinStream)) == (int)(&(*w)) ) { // yeah, this is a hack :(
			break;
		}
	}

	// "terminateConnection(p); will check for any new output before actually killing the proc
	//    and will cause the event.  Setting NULL will prevent this.
	p->owner = NULL;
	terminateConnection(p);
}



#ifdef _PC_INTERNAL_TIMER_
void PlinkConnect::OnTimerEvent(wxTimerEvent &event) {
	PollTick();
}
#endif