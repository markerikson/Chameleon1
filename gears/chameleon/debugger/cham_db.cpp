/*****************************************************************\
*Wrapper Code file
*
*author: Ben Carhart
*date started: 1/11/04
*date finished:
*
*description: Code for the implementation of a wrapper for GDB
*revisions:
*BSC-02/13/04-> Removed all constructors & added 4 private func.
*               to aid in general stuff.
*BSC-02/27/04-> Added Dave Czechowski's ASYNC code which rendered
*               much of my code obsolete.  Backed up files &
*               deleted all commented code to start over.  :(
*BSC-02/29/04-> Added carriage returns [\r] on all my commands. I
*               also added a soft-error-reset as well as a flush-
*               buffer function to clear out unwanted input.
*BSC-03/9/04-> Gosh I added so much i can't list it here.  I added
*              a class-specific status variable, new arrays, a few
*              new functions... re-did a few functions, changed
*              some logic.  I feel like I'm debugging without
*              compiling!  AUGH!
*BSC-03/19/04->Alright... i'm adapting my code into Dave's window
*              API so I can test it.  All portions with "cout"
*              debug statements have been altered & "dave's code"
*              shows obvious places i've moded.
\*****************************************************************/

#include "cham_db.h"
//#include <wx/process.h>
#include <wx/file.h>
//#include <iostream>

//using namespace std;

// Event Table Declaration
BEGIN_EVENT_TABLE(Debugger, wxEvtHandler)
	EVT_PROCESS2_ENDED(Debugger::onProcessTermEvent)
	EVT_PROCESS2_STDOUT(Debugger::onProcessOutputEvent)
	EVT_PROCESS2_STDERR(Debugger::onProcessErrOutEvent)
END_EVENT_TABLE()

//straight up constructor
Debugger::Debugger(bool mode, wxString fName, wxString execThis, wxTextCtrl* outBox)
{
	//dave code
	outputScreen = outBox;

	//generic initialization
	isRemote = mode;			//false = local
	procLives = false;
	classStatus = START;
	
	currFile = fName;
	
	histPointer = 0;
	breakpointCount = 0;
	deadBreakpoints = 0;
	errorCount = 0;
	currDebugLine = 0;
	gdbVarIndex = 0;
	

	firstExecString = execThis;

	if(isRemote)
	{returnChar = "\r";}
	else
	{returnChar = "\n";}

	if(fName != "")
	{
		status = DEBUG_STOPPED;
		fileIsSet = true;
	}
	else
	{
		status = DEBUG_NEED_FILE;
		fileIsSet = false;
	}

	varCount = 0;

	//begin funky process stuff
	debugProc = new wxProcess2(this);
	
	//initial commands to GDB
	command = execThis;
	updateHistory(command);
	
	//this is dave's \/ 
	//pid = wxExecute("plink.exe -pw dayspring danroeber@163.11.160.218 gdb -q", wxEXEC_ASYNC, debugProc);

	//this is me.
	pid = wxExecute(execThis, wxEXEC_ASYNC, debugProc);

	if (pid == 0)
	{
		//Debug started unsuccessfully
		status = DEBUG_DEAD;
		classStatus = STOP;
		error = "Unable to launch.  Process ID not assigned.";
		makeGenericError("Launch_fail");
		delete debugProc;
	}
	else if(pid == -1)
	{
		status = DEBUG_DEAD;
		classStatus = STOP;
		error = "Unable to launch process.  -1 PID assigned";
		makeGenericError("Get a real operating system!");
		delete debugProc;
	}
	else
	{
		int currInit = 0;
		wxArrayString initString;

		//dave code
		(*outputScreen) << "Started Process.  PID: " << pid << "\n";

		procLives = true;

		streamIn = debugProc->GetInputStream();		//GDB to me
		streamError = debugProc->GetErrorStream();	//problems

		//initialize GDB
		initString.Add("set prompt " + PROMPT_CHAR + returnChar);
		initString.Add("set print pretty on" + returnChar);
		initString.Add("set print address off" + returnChar);
		initString.Add("set confirm off" + returnChar);

		//send all initlization commands to GDB
		while(currInit < int(initString.GetCount()))
		{
			sendCommand(initString.Item(currInit));
			currInit++;
		}
		
		if(fileIsSet)
		{
			command = "file " + currFile + returnChar;
			sendCommand(command);
		}//end set-file conditional
	}//end GDB execution failure-conditional
}
//end constructors

//destructor
Debugger::~Debugger()
{
	//initial stuff to cleanly exit (sorta)
	bool alright = resetStatus();

	if(numBreaks() > 0)
	{
		command = "delete break" + returnChar;
		sendCommand(command);
	}

	if(fileIsSet)
	{
		command = "file" + returnChar;
		sendCommand(command);
	}

	command = "quit" + returnChar;
	sendCommand(command);

	//this extra command will actually be universal if we start a CMD
	//session in windows.  A CMD session isn't neccesary really, but it's a
	//thought.  If we're running GDB directly, "exit" is redundant and prob.
	//won't do anything.
	if(isRemote)
	{
		command = "exit" + returnChar;
		sendCommand(command);
	}

	//attempt kill of the process
	int success = debugProc->Kill(pid, wxSIGTERM);
	switch(success)	//check success
	{
		case wxKILL_BAD_SIGNAL: 
			makeGenericError("failure on soft kill: bad signal");
			debugProc->Kill(pid, wxSIGKILL);	//slay the beast
			break;
		case wxKILL_ACCESS_DENIED:
			makeGenericError("could not kill process: access denied");
			break;
		case wxKILL_NO_PROCESS:
			makeGenericError("tried to kill non-existent process?!?");
			break;
		case wxKILL_ERROR: 
			makeGenericError("failure on soft kill: unknown error");
			debugProc->Kill(pid, wxSIGKILL);	//KILL KILL KILL!!
			break;
	};

	//dump history & errors into debug file
	wxFile dumpFile("dump.txt");
	dumpFile.Write("dump v0.1a");
	for(int off = 0; off < MAX_HIST_ITEMS; off++)
	{
		dumpFile.Write(getHistoryItem(off));
	}

	dumpFile.Write("Errors:");
	for(int o = 0; o < errorCount; o++)
		dumpFile.Write(errorHist[o]);
	dumpFile.Close();

	//kill pointers
	delete debugProc;
	delete streamIn;
	delete streamError;
}//end ~Debugger

//PUBLIC FUNCTIONS:
//-----------------
//PROGRAM STATUS
//--------------
//This is just general running information I thought would be
//useful to know while using the class.

//setMode(): switches between local debug & remote debug.
//  LOCAL = false;
//  REMOTE = true;
void Debugger::setMode(bool mode)
{
	if(status == DEBUG_RUNNING)
	{
	}
	else if(status == DEBUG_NEED_FILE)
	{
	}
	else if(status == DEBUG_ERROR || status == DEBUG_DEAD)
	{
	}
	else
	{
	}
	isRemote = mode;
}

//currMode(): returns current mode
bool Debugger::getMode()
{
	return(isRemote);
}

//setFile(): set's the current file to be debugged.
bool Debugger::setFile(wxString fName)
{
	if(fileIsSet)
	{
		//Worry about switching files in the middle of a debug here
		return(false);
	}
	else
	{
		if(fName.IsEmpty())
		{
			return(false);
		}
		else
		{
			classStatus = NEW_FILE;
			currFile = fName;
			command = "file " + currFile + returnChar;
			sendCommand(command);

			status = DEBUG_STOPPED;
			fileIsSet = true;
		}//end file-empty-check
	}
	return(true);
}

//currFile(): returns the currently loaded file
wxString Debugger::getFile()
{
	return(currFile);
}

//progStatus(): returns current debug status
int Debugger::programStatus()
{
	return(classStatus);
}

//genericStatus(): returns the generic debugger status
int Debugger::genericStatus()
{
	return(status);
}

//errorMsg(): returns the most recent error message string
wxString Debugger::errorMsg()
{
	return(error);
}

//resetStatus(): resets this class's status and the child process's
bool Debugger::resetStatus()
{
	if(status == DEBUG_RUNNING)
	{
		//if it's running, kill it...
		bool success = stop(true);
		
		if(success)
		{
			status = DEBUG_STOPPED;
		}
		else	//this code should NEVER be done....
		{
			status = DEBUG_ERROR;
			error = "failed to STOP() debug";
			addErrorHist("resetStatus() <o.0>:");
		}
	}

	//i figure that if someone wants to reset the Debugger, then
	//darn it EVERYTHING is gonna get reset. so....
	//do a full reset of GDB

	if(status != DEBUG_DEAD)
	{
		wxArrayString debugReset;

		//if the file was just loaded... no need to kill something not
		//running.  ^_^  + it generates an error
		if(fileIsSet)
		{
			if(status != DEBUG_STOPPED)
			{
				debugReset.Add("kill" + returnChar);
			}

			debugReset.Add("file" + returnChar);
			debugReset.Add("file " + currFile + returnChar);

			//now change the status
			status = DEBUG_STOPPED;
		}

		if(numBreaks() > 0)
		{
			debugReset.Add("delete breakpoint" + returnChar);
		}

		//end reset commands...
		classStatus = HARD_RESET;

		for(int c = 0; c < int(debugReset.GetCount()); c++)
		{
			sendCommand(debugReset[c]);
		}
	
		error = "";
		command = "";
		data.Empty();
		varNames.Empty();
		varValue.Empty();
		gdbVarIndex = 0;

		return(true);
	}

	//this means the proc is dead so there's nothing to reset
	return(false);
}

//clearError(): does a "soft" reset by just changing the status variable.
//  I included this because I thought just having a hard-reset was a little
//  extreme considering the soft errors generated by bad breakpoints
void Debugger::clearError()
{
	if(fileIsSet)
	{
		status = DEBUG_WAIT;
	}
	else
	{
		status = DEBUG_NEED_FILE;
	}

	classStatus = SOFT_RESET;
}

//setProcess(): re-directs or changes the process pointer.
void Debugger::setProcess(wxString nExec)
{
	//begin with a hard reset of everything.
	status = DEBUG_RUNNING;
		
	resetStatus();
	bool success = stop(false);

	if(!success)
	{debugProc->Kill(pid, wxSIGKILL);} //KILL KILL KILL!!

	delete debugProc;
	delete streamIn;
	delete streamError;
	
	debugProc = new wxProcess2(this);

	updateHistory("new proc->" + nExec);

	//begin fresh...
	classStatus = START;
	pid = wxExecute(nExec, wxEXEC_ASYNC, debugProc);
	if (pid == 0)
	{
		//Debug started unsuccessfully
		status = DEBUG_DEAD;
		classStatus = STOP;
		error = "Unable to execute "+nExec+".  Process ID not assigned.";
		makeGenericError("Launch_fail");
		delete debugProc;
	}
	else if(pid == -1)
	{
		status = DEBUG_DEAD;
		classStatus = STOP;
		error = "Unable to launch process.  -1 PID assigned";
		makeGenericError("Get a real operating system!");
		delete debugProc;
	}
	else	//process started
	{
		int currInit = 0;
		wxArrayString initString;

		procLives = true;
		classStatus = NEW_PROC;
		status = DEBUG_STOPPED;
		firstExecString = nExec;

		streamIn = debugProc->GetInputStream();		//GDB to me
		streamError = debugProc->GetErrorStream();	//problems

		//initialize GDB
		initString.Add("set prompt " + PROMPT_CHAR + returnChar);
		initString.Add("set print pretty on" + returnChar);
		initString.Add("set print address off" + returnChar);
		initString.Add("set confirm off" + returnChar);

		//send all initlization commands to GDB
		while(currInit < int(initString.GetCount()))
		{
			sendCommand(initString.Item(currInit));
			currInit++;
		}
		
		if(fileIsSet)
		{
			command = "file " + currFile + returnChar;
			sendCommand(command);
		}//end set-file conditional
		else
		{
			status = DEBUG_NEED_FILE;
		}
	}//end execution failure-conditional
}
//END PROGRAM STATUS

//BREAKPOINT FUNCTIONALITY
//------------------------
//i have 3 variables to use for breakpoints:
//	[breakpointCount] = how many breakpoints
//  [deadBreakpoints] = how many are deleted
//	[breakpointLoc] = corresponding line #'s
//GDB loops breakpoint #s continuously, so unfortuneatly
//breakpointLoc is continuously growing (until a useful
//fix is found).  GDB also keeps track of both elements
//that these variables track, but it's easier for me
//to track them than to parse through what GDB gives
//me in response to what i want.

//setBreak(): accepts a line number with which to break on
void Debugger::setBreak(int lineNum)
{
	if(fileIsSet)
	{
		classStatus = S_BREAK;
		command.Printf("break %d%s", lineNum, returnChar.c_str());
	
		sendCommand(command);
	
		breakpointLoc.Add(lineNum);
		breakpointCount++;
	}
}

//enableBreak(): enables a breakpoint at a given line number.  This is
//  nearly identical to disable break since the only difference is the
//  command being sent.  ^.^
void Debugger::enableBreak(int lineNum)
{
	//this is the same as disableBreak().  only opposite.  -o.0-
	if(status == DEBUG_STOPPED ||
		status == DEBUG_WAIT ||
		status == DEBUG_BREAK)
	{
		classStatus = E_BREAK;
		//get the breakpoint number by locating it using wxArrayInt
		//note: at this point [breakNum] is actually an INDEX
		int breakNum = breakpointLoc.Index(lineNum, false);

		if(breakNum != wxNOT_FOUND)
		{
			//we found it!  
			breakNum = breakpointLoc[breakNum];
			command.Printf("enable %d%s", breakNum, returnChar.c_str());
			sendCommand(command);
		}
		else
		{
			//raise an event to Mark??
			status = DEBUG_ERROR;
			error.Printf("Breakpoint not found for line number %d", lineNum);
			addErrorHist("Enable_breakpoint_fail:");
		}
	}
}

//disableBreak(): disables a breakpoint at a given line number
void Debugger::disableBreak(int lineNum)
{
	if(status != DEBUG_DEAD &&
		status != DEBUG_NEED_FILE &&
		status != DEBUG_RUNNING &&
		status != DEBUG_ERROR)
	{
		classStatus = D_BREAK;
		//get the breakpoint number by locating it using wxArrayInt
		//note: at this point [breakNum] is actually an INDEX
		int breakNum = breakpointLoc.Index(lineNum, false);

		if(breakNum != wxNOT_FOUND)
		{
			//we found it!  
			breakNum = breakpointLoc[breakNum];
			command.Printf("disable %d%s", breakNum, returnChar.c_str());
			sendCommand(command);
		}
		else
		{
			//raise an event to Mark??
			status = DEBUG_ERROR;
			error.Printf("Breakpoint not found for line number %d", lineNum);
			addErrorHist("Disable_breakpoint_fail:");
		}
	}//end file-set-check
}

//killBreak(): deletes a breakpoint at line X
void Debugger::killBreak(int lineNum)
{
	if(status == DEBUG_STOPPED ||
		status == DEBUG_WAIT ||
		status == DEBUG_BREAK)
	{
		//see [disableBreak()]
		int breakNum = breakpointLoc.Index(lineNum, false);

		if(breakNum != wxNOT_FOUND)
		{
			classStatus = K_BREAK;
			breakNum = breakpointLoc[breakNum];

			command.Printf("delete %d%s", breakNum, returnChar.c_str());
			sendCommand(command);

			deadBreakpoints++;
		}
		else
		{
			//raise an event here too for Mark
			status = DEBUG_ERROR;
			error.Printf("Breakpoint not found for line number %d", lineNum);
			addErrorHist("Delete_breakpoint_fail:");
		}
	}
}

//numBreaks(): returns the # of breakpoints
int Debugger::numBreaks()
{
	return(breakpointCount - deadBreakpoints);
}
//END BREAKPOINT SECTION

//STEP FUNCTIONALITY
//------------------
//This section handles step management; the jump-over, step into, things
//like that.  The "go" function is also here.  It will get private var's
//as neccesary.

//step(): steps a single line (where code is being executed), or into
//  a function if that's the line.
void Debugger::step()
{

	//check program status
	if(status == DEBUG_STOPPED)
	{
		command = "run" + returnChar;
		status = DEBUG_RUNNING;
		classStatus = GO;
		sendCommand(command);

	}
	else if(status == DEBUG_WAIT || status == DEBUG_BREAK)
	{
		command = "step" + returnChar;
		status = DEBUG_RUNNING;
		classStatus = STEP;
		sendCommand(command);

	}
	//if not stopped or waiting, then program status = error
}

//stepOver(): step a line, execute over functions. Nearly identical to [step()]
void Debugger::stepOver()
{
	//check program status
	if(status == DEBUG_WAIT || status == DEBUG_BREAK)
	{
		command = "next" + returnChar;
		status = DEBUG_RUNNING;
		classStatus = STEP;
		sendCommand(command);
	}
	//if not waiting, then program status is not good for a step-over
}

//stepOut(): finishes executing a function and stop on next line
//the command = finish
void Debugger::stepOut()
{
	//check program status
	if(status == DEBUG_WAIT || status == DEBUG_BREAK)
	{
		command = "finish" + returnChar;
		status = DEBUG_RUNNING;
		classStatus = STEP_OUT;
		sendCommand(command);
	}

	//if not stopped or waiting, then program status is not good for stepOut
}

//jump(): jumps to a given [lineNum] and then RUNs from there.  I might
//  modify this function to accept a boolean to STEP or RUN from the given
//  line number.
void Debugger::jump(int lineNum)
{
	/* will be completed if needed*/
}

// go(): run the program using GDB.
void Debugger::go()
{
	//check to make sure the program is suitable to BE run...
	if(status != DEBUG_ERROR ||
		status != DEBUG_RUNNING ||
		status != DEBUG_NEED_FILE ||
		status != DEBUG_DEAD)
	{
		classStatus = GO;
		status = DEBUG_RUNNING;

		command = "run" + returnChar;
		sendCommand(command);
	}//end status check
}//wow, tough

bool Debugger::stop(bool pleaseRestart)
{
	if(status == DEBUG_RUNNING || status == DEBUG_ERROR)
	{
		debugProc->Kill(pid, wxSIGKILL);	//KILL KILL KILL!!
	} 
	else if(status == DEBUG_DEAD)
	{
		//um... hello!
	}
	else
	{
		command = "quit" + returnChar;
		sendCommand(command);
	}

	classStatus = STOP;
	status = DEBUG_STOPPED;

	if(pleaseRestart)
	{
		delete debugProc;
		delete streamIn;
		delete streamError;
	
		debugProc = new wxProcess2(this);

		updateHistory("new proc->" + firstExecString);

		//begin fresh...
		classStatus = START;
		pid = wxExecute(firstExecString, wxEXEC_ASYNC, debugProc);
		if (pid == 0)
		{
			//Debug started unsuccessfully
			status = DEBUG_DEAD;
			classStatus = STOP;
			error = "Unable to execute " + firstExecString + ".  Process ID not assigned.";
			makeGenericError("Launch_fail");
			delete debugProc;
		}
		else if(pid == -1)
		{
			status = DEBUG_DEAD;
			classStatus = STOP;
			error = "Unable to launch process.  -1 PID assigned";
			makeGenericError("Get a real operating system!");
			delete debugProc;
		}
		else	//process started
		{
			int currInit = 0;
			wxArrayString initString;

			procLives = true;
			classStatus = NEW_PROC;
			status = DEBUG_STOPPED;

			streamIn = debugProc->GetInputStream();		//GDB to me
			streamError = debugProc->GetErrorStream();	//problems

			//initialize GDB
			initString.Add("set prompt " + PROMPT_CHAR + returnChar);
			initString.Add("set print pretty on" + returnChar);
			initString.Add("set print address off" + returnChar);
			initString.Add("set confirm off" + returnChar);

			//send all initlization commands to GDB
			while(currInit < int(initString.GetCount()))
			{
				sendCommand(initString.Item(currInit));
				currInit++;
			}
		
			if(fileIsSet)
			{
				command = "file " + currFile + returnChar;
				sendCommand(command);
			}//end set-file conditional
			else
			{
				status = DEBUG_NEED_FILE;
			}
		}//end execution failure-conditional
	}//end pleaseRestart conditional
	else
	{
		procLives = false;
		status = DEBUG_ERROR;
		classStatus = STOP;
	}

	return(true);
}

//kill(): questions?
void Debugger::kill()
{
	classStatus = STOP;
	status = DEBUG_STOPPED;

	debugProc->Kill(pid, wxSIGKILL);	//KILL KILL KILL!!
}

//cont(): continues running a stopped program
void Debugger::cont()
{
	//continue will only work correctly on these two status items
	if(status == DEBUG_WAIT || status == DEBUG_BREAK)
	{
		command = "continue" + returnChar;
		sendCommand(command);

	}
}

//VARIABLE FUNCTIONALITY
//----------------------
// there are only 2 functions here: a snoopVar and a setVar.
// do you need anything else?  ^_^

//snoopVar(): returns the variable's value, and if oneShot = false
//  will add it to the watch list.
void Debugger::snoopVar(wxString varName, bool oneShot = true)
{
	if(status == DEBUG_STOPPED || 
		status == DEBUG_WAIT ||
		status == DEBUG_BREAK)
	{
		classStatus = WATCH_VAR;
		command.Printf("print %s%s", varName.c_str(), returnChar.c_str());
		//sendCommand(command);

		if(!oneShot)
		{
			wxString tmp;
			tmp.Printf("display %s%s", varName.c_str(), returnChar.c_str());
			command.Append(tmp.c_str());
			//sendCommand(command);
		}

		sendCommand(command);
	}//end status check
}

void Debugger::setVar(wxString varName, wxString newValue)
{
}

void Debugger::removeVar(wxString varName)
{
}

//PRIVATE FUNCTIONS:
//------------------

//The following 3 are from Dave Czechowski.  KUDOS!

void Debugger::onProcessOutputEvent(wxProcess2StdOutEvent &e)
{
	//variables to hold stuff for parsing...
	wxString tempHold;
	int classStatusBackup = 0;

	//STEP parsing...
	wxString firstCase, secondCase, thirdCase;
	int initPos, finalPos;

	//!!END variables!!//

	status = DEBUG_WAIT;

	//data now has the output...
	tempHold = e.GetOutput();
	data.Add(tempHold);
	
	//data.Add(e.GetOutput());

	//~~DEBUG CODE~~//
	(*outputScreen)<<"--Output:\n"<<tempHold<<"\n--end output--\n";
	tempHold.Empty();

	//What we're doing with output events:
	//1) See if the captured string has a [$] all by itself at the end
	//2) If so, begin parsing.  Set [classStatus = PARSING]
	//3) If not, wait for another output
	for(int i = 0; 
	    i < int(data.GetCount()) && classStatusBackup == 0; 
	    i++)
	{
		tempHold = data[i];
		if(tempHold.Last() == PROMPT_CHAR)
		{
			tempHold.Empty();

			for(int j = 0; j <= i; j++)
			{tempHold << data[j];}

			classStatusBackup = GO;
		}
	}

	//check to see if something good was found.
	if(classStatusBackup != 0)
	{
		classStatusBackup = classStatus;
		classStatus = WAITING;
	}
	
	//i need to do parsing here & sending mark stuff i guess...
	switch(classStatus)
	{
		case START:			//program start
			flushBuffer();
			break;

		case E_BREAK:		//enable break
		case D_BREAK:		//disable break
		case K_BREAK:		//kill break
		case S_BREAK:		//set break
		case GO:
		case JUMP:
		case SOFT_RESET:
		case HARD_RESET:
		case NEW_FILE:		//new file to debug
		case NEW_MODE:		//change in program mode
			break;

		case NEW_PROC:		//change in process pointer
			flushBuffer();
			break;

		case VAR_ASSIGN:
			//the only time this status occurs is after a "print var = val"
			//First we check for text.  This means an invalid assignment
			//Next we check for the same things WATCH_VAR does...
			// so we can probably remove this "break" and let it flow into
			// WATCH_VAR...
			break;

		case WATCH_VAR:
			//the only time this status occurs is after a "print var"
			//(gdb) command "whatis" returns variable type
			//in this situation we are looking for:
			// ints - "$n = val"
			// array- "$n = {val, val...}"
			// string-"$n = {static bleh bleh...", re-format to:
			// str#2- "$n = "text""
			// obj -  "$n = {varName = val, varName = val, ..} (varName can be of any listed type...)
			// double-"$n = val.val"
			// char[]-"$n = val 'char'"
			// char - "$n = "text""
			//once this is parsed, send off the event with the varName / value
			// pair in the [topVarIndex] position.  BUT DO NOT CHANGE
			// [topVarIndex]
			//If we only get text in the stream, raise an error; we tried
			// to snoop a non-existant variable.
			break;

		case STEP:
			//looking for this heirarchy:
			// 1) the word "Program" as this indicates a fatal error
			//  2) the pattern "f_name.ext:ln"
			//   3) a line number
			//case 1, reset statuses and send an error
			//case 2, update the filename and line number vars to send
			//case 3, update the line number and send
			//once one of these items is found, do a check on [topVarIndex > 0]
			//and update the array;

			firstCase = tempHold.Left(14);
			
			initPos = (tempHold.Find(" at ")) + 4;	//-1 if not there...
			if(initPos >= 4)
			{
				finalPos = tempHold.Find(returnChar);
				secondCase = tempHold.Mid(initPos, (initPos - finalPos));
			}
			else
			{
				secondCase = "blank";
			}

			initPos = tempHold.Find(char(32));
			thirdCase = tempHold.Mid(0, initPos - 1);

			//begin testing cases
			if(firstCase == "Program receiv" ||
				firstCase == "Program exited")
			{
				status = DEBUG_ERROR;
				classStatus = STOP;
				error = "Problem with program execution: " + tempHold;
				makeGenericError("Fault in parsing output");
				break;
			}
			else if(secondCase != "blank")
			{
				int colonPos = secondCase.Find(':');
				wxString tmp;
				wxString mark_fileName = "blank";
				long mark_lineNum = 0;

				if(colonPos > 0)
				{
					mark_fileName = secondCase.Mid(0, colonPos - 1);
					tmp = secondCase.Mid(colonPos + 1, finalPos);
					tmp.ToLong(&mark_lineNum);

					/*DEBUG CODE*/
					(*outputScreen)<<"Case 2:\n";
					(*outputScreen)<<"Captured filename: "<<mark_fileName.c_str()<<"\n";
					(*outputScreen)<<"Captured line num: "<<mark_lineNum<<"\n";
				}
				else
				{
					status = DEBUG_ERROR;
					classStatus = STOP;
					error = "Error on parsing: no colon in string";
					makeGenericError("Parse error: STEP case 2");
					break;
				}
			}
			else
			{
				long mark_lineNum = 0;
				thirdCase.ToLong(&mark_lineNum);

				/*DEBUG CODE*/
				(*outputScreen)<<"Case 3:\n";
				(*outputScreen)<<"Captured line num: "<<mark_lineNum<<"\n";
			}
			break;

		case STEP_OUT:
			//looking for: f_name.ext:ln
			//where f_name.ext = the filename
			//      ln = the line number to re-focus on in f_name
			//if [topVarIndex > 0] we need to grab some variables & ship 'em
			break;

		case CONTINUE:
			break;

		case WAITING:		//i'm waiting...
			classStatus = classStatusBackup;
			break;

		case STOP:
			status = DEBUG_ERROR;
			classStatus = STOP;
			error = "Extreme debug error.";
			makeGenericError("See previous error messages");
			data.Empty();
			break;

		default:
			status = DEBUG_ERROR;
			classStatus = STOP;
			error = "[classStatus] value not recognized";
			makeGenericError("failure on output parse:");
			data.Empty();
	}
}

void Debugger::onProcessErrOutEvent(wxProcess2StdErrEvent &e)
{
	status = DEBUG_ERROR;

	//error now has the output
	error = e.GetError();
	addErrorHist("Event-Generated error");

	(*outputScreen)<<"Event Error: "<<error<<"\n";

	//again, some event stuff here...
}

void Debugger::onProcessTermEvent(wxProcess2EndedEvent &e) 
{
	//the process has been killed (aka GDB quit for some reason)
	if(e.GetPid() == pid) 
	{
		(*outputScreen)<<"Process "<<pid<<" terminated\n";

		status = DEBUG_DEAD;
		classStatus = STOP;
		streamError = NULL;
		streamIn = NULL;
		pid = -2; // a nothing pid
		procLives = false;
		delete debugProc;
		delete streamIn;
		delete streamError;
	}
}

//flushBuffer(): it simply erases the data array.
void Debugger::flushBuffer()
{
	data.Empty();
}

//getResult(): handles getting the streamed output... i think...
wxString Debugger::getResult(int debugFlag)
{
	return("0");
}

//sendCommand(): handles sending a command to the stream.
void Debugger::sendCommand(wxString send)
{
	if(procLives)
	{
		wxTextOutputStream streamOut(*(debugProc->GetOutputStream()));//me to GDB
		streamOut.WriteString(send);
		send = send + "<- sent";
	}
	updateHistory(send);
}

//makeGenericError(): grabs error stream and input stream and combines them
//  into one very long error.  Possibly redundant.
void Debugger::makeGenericError(wxString comment)
{
	status = DEBUG_ERROR;	//if not already set...
	addErrorHist(comment);
}

//addErrorHist(): updates the error history
void Debugger::addErrorHist(wxString comment)
{
	errorCount++;
	errorHist.Add(comment + "-> " + error);
}

//updateHistory: updates the history storage and pointer
void Debugger::updateHistory(wxString addMe)
{
	commandHistory[histPointer] = addMe;
	histPointer++;

	if(histPointer >= MAX_HIST_ITEMS)
		histPointer = 0;
}

//getHistoryItem: returns the last history item offset by [offset].
//eg: this will return [commandHistory[histPointer - 1]] which is the last
//    command.  To get the one before, offset = 1, before that offset = 2
wxString Debugger::getHistoryItem(int offset = 0)
{
	int offsetPointer = histPointer;

	//last command at back of array
	if(offsetPointer == 0)
	{
		offsetPointer = MAX_HIST_ITEMS;
	}
	//adjust offset to wrap around array
	else if((offsetPointer - offset) < 0)
	{
		offset = offset - histPointer;
		offsetPointer = MAX_HIST_ITEMS;
	}
	else	//offset is legal, adjust for the fact that histPointer points
	{		//to next history position.
		offset += 1;
	}

	return(commandHistory[offsetPointer - offset]);
}

//eof