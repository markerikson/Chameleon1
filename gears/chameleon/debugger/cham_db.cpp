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
#include <wx/wfstream.h>
#include <wx/RegEx.h>
//#include <iostream>

//using namespace std;

//vent handling
BEGIN_EVENT_TABLE(Debugger, wxEvtHandler)
	EVT_PROCESS2_ENDED(Debugger::onProcessTermEvent)
	EVT_PROCESS2_STDOUT(Debugger::onProcessOutputEvent)
	EVT_PROCESS2_STDERR(Debugger::onProcessErrOutEvent)

	EVT_DEBUG(Debugger::onDebugEvent)
END_EVENT_TABLE()

//straight up constructor
Debugger::Debugger(wxTextCtrl* outBox, Networking* networking, wxEvtHandler* pointer)
{
	flushPrivateVar();

	outputScreen = outBox;
	guiPointer = pointer;
	procLives = false;
	status = DEBUG_DEAD;
	classStatus = STOP;
	myConnection = networking;
}
//end constructors

//destructor
Debugger::~Debugger()
{
	//initial stuff to cleanly exit (sorta)
	stop(false);

	wxString tmp;
	//dump history & errors into debug file
	wxFile dumpFile;
	dumpFile.Create("dump.txt",true);

	tmp = "dump v0.1a / ";
	dumpFile.Write(tmp.c_str(), tmp.Len());

	for(int off = 0; off < histCount; off++)
	{
		tmp = getHistoryItem(off) + " / ";
		dumpFile.Write(tmp.c_str(), tmp.Len());
	}

	tmp = " *-- Errors: / ";
	dumpFile.Write(tmp.c_str(), tmp.Len());
	for(int o = 0; o < errorCount; o++)
	{
		tmp = errorHist[o] + " / ";
		dumpFile.Write(tmp.c_str(), tmp.Len());
	}

	//delete outputScreen;
	//delete guiPointer;
	//if(debugProc != NULL) {delete debugProc;}
}//end ~Debugger

//onDebugEvent(): catches when Mark sends me something...
void Debugger::onDebugEvent(wxDebugEvent &event)
{
	//all the variables i need here:
	int eventCommand = event.GetId();
	int eventLineNum = event.GetLineNumber();
	bool mode = event.IsRemote();

	wxArrayString srcFiles = event.GetSourceFilenames();
	wxArrayInt breakpointLines;

	FileBreakpointHash breakpointList = event.GetFileBreakpoints();
	
	wxString tmp;
	wxString firstFile = srcFiles[0];
	wxString sendAllBreakpoints;
	wxString execFile = event.GetExecutableFilename();

	//loop vars
	int i, j;

	switch(eventCommand)
	{
	case ID_DEBUG_START:
		//start process
		startProcess(true, mode, execFile, "gdb -q", outputScreen);

		//set breakpoints
		for(i = 0; i < (int)srcFiles.GetCount(); i++)
		{
			tmp = srcFiles[i];
			if(breakpointList.find(tmp) != breakpointList.end())
			{
				breakpointLines = breakpointList[tmp];

				for(j = 0; j < (int)breakpointLines.GetCount(); j++)
				{
					sendAllBreakpoints<<"break \""<<tmp<<":"<<breakpointLines[j]<<"\""<<returnChar;
					numBreakpoints++;
					gdbBreakpointNum++;
				}
			}//end if
		}


		sendCommand("list"+returnChar);
		sendCommand(sendAllBreakpoints);
		sendCommand("something"+returnChar);
		sendCommand("done"+returnChar);	//tag... not really a command
		break;

	case ID_DEBUG_STOP:
		stop(false);
		break;

	case ID_DEBUG_STEPNEXT:
		//(*outputScreen)<<"<event> received step command <event>\n";
		step();
		break;

	case ID_DEBUG_STEPOVER:
		stepOver();
		break;

	case ID_DEBUG_STEPOUT:
		stepOut();
		break;

	case ID_DEBUG_ADD_BREAKPOINT:
		setBreak(firstFile, eventLineNum);
		break;

	case ID_DEBUG_REMOVE_BREAKPOINT:
		killBreak(firstFile, eventLineNum);
		break;

	//case ID_DEBUG_RUNTOCURSOR:
	default:
		tmp = error;
		eventLineNum = status;
		error = "Unrecognized Debug Event";
		makeGenericError("onDebugEvent: ");
		error = tmp;
		status = eventLineNum;
		break;
	}
}

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

//for Mark...
bool Debugger::isDebugging()
{
	return(status == DEBUG_RUNNING);
}

bool Debugger::isPaused()
{
	return(status == DEBUG_WAIT || status == DEBUG_BREAK);
}
//--//

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
	return(fileIsSet);
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
		stop(true);
		
		status = DEBUG_STOPPED;
	}

	//i figure that if someone wants to reset the Debugger, then
	//darn it EVERYTHING is gonna get reset. so....
	//do a full reset of GDB

	if(status != DEBUG_DEAD)
	{
		wxArrayString debugReset;

		if(numBreak() > 0)
		{
			debugReset.Add("delete breakpoint" + returnChar);
			gdbBreakpointNum = 0;
			numBreakpoints = 0;
			lineToNum.empty();
		}

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
		if(classStatus == START || classStatus == HARD_RESET)
		{
			status = DEBUG_STOPPED;
		}
		else
		{
			status = DEBUG_WAIT;
		}
	}
	else
	{
		status = DEBUG_NEED_FILE;
	}

	classStatus = SOFT_RESET;
}

//setProcess(): re-directs or changes the process pointer.
void Debugger::setProcess(bool newIsRemote, wxString newFname, wxString nExec)//, wxTextCtrl *outbox)
{
	//begin with a hard reset of everything.
	stop(false);
	flushPrivateVar();
	
	//Option 1
	//startProcess(true,newIsRemote, newFname, nExec, newOutbox);
	
	//Option 2 (outputscreen used to copy into itself)
	startProcess(true,newIsRemote, newFname, nExec, outputScreen);
}


void Debugger::startProcess(bool fullRestart, bool mode, wxString fName, wxString execThis, wxTextCtrl* outBox)
{
	if(fullRestart)
	{
		//dave code
		outputScreen = outBox;

		//generic initialization
		isRemote = mode;			//false = local
		currFile = fName;
	
		histCount = 0;
	
		numBreakpoints = 0;
		gdbBreakpointNum = 1;
	
		errorCount = 0;

		currDebugLine = 0;
		
		gdbVarIndex = 0;
		varCount = 0;
	
		firstExecString = execThis;
	}

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
	
	procLives = false;
	classStatus = NEW_PROC;
	
	//begin funky process stuff
	if(isRemote)
	{
		debugProc = myConnection->GetPlinkProcess(this);

		pid = debugProc->GetPID();
		if(pid > 0) {procLives = true;}

		command = execThis + returnChar;
		if(procLives)
		{
			wxTextOutputStream streamOut(*(debugProc->GetOutputStream()));//me to GDB
			streamOut.WriteString(command);
			command = command + "<- sent";
		//(*outputScreen)<<send<<"\n";
		}
		else
		{
			command = command + "<- NOT SENT: PROC DEAD";
		}
		updateHistory(command);
	}
	else
	{
		debugProc = new wxProcess2(this);
		pid = wxExecute(execThis, wxEXEC_ASYNC, debugProc);
		command = execThis;
		updateHistory(command);
	}

	//initial commands to GDB
	if (pid == 0)
	{
		//Debug started unsuccessfully
		status = DEBUG_DEAD;
		classStatus = STOP;
		error = "Unable to launch.  Process ID not assigned.";
		makeGenericError("Launch_fail");
		//delete debugProc;
	}
	else if(pid == -1)
	{
		status = DEBUG_DEAD;
		classStatus = STOP;
		error = "Unable to launch process.  -1 PID assigned";
		makeGenericError("Get a real operating system!");
		//delete debugProc;
	}
	else
	{
		//int currInit = 0;
		wxArrayString initString;
		wxString tmp;

		//dave code
		//(*outputScreen) << "Started Process.  PID: " << pid << "\n";

		procLives = true;
		classStatus = START;

		streamIn = debugProc->GetInputStream();		//GDB to me
		streamError = debugProc->GetErrorStream();	//problems

		//initialize GDB
		tmp.Printf("set prompt %s%s", PROMPT_CHAR.c_str(), returnChar.c_str());
		initString.Add(tmp);
		initString.Add("set print pretty on" + returnChar);
		initString.Add("set print address off" + returnChar);
		initString.Add("set confirm off" + returnChar);

		//send all initlization commands to GDB
		for(int currInit = 0; currInit < int(initString.GetCount()); currInit++)
		{
			//(*outputScreen)<<"INIT COMM: "<<initString[currInit];
			sendCommand(initString.Item(currInit));
		}
		
		if(fileIsSet)
		{
			command = "file " + currFile + returnChar;
			sendCommand(command);
		}//end set-file conditional
	}//end GDB execution failure-conditional
}
//END PROGRAM STATUS

//BREAKPOINT FUNCTIONALITY
//------------------------
//i used to have 3 variables to use for breakpoints.
//Now I don't know.  ^_^  We re-designed things.

//setBreak(): accepts a line number with which to break on
void Debugger::setBreak(wxString srcFile, int lineNum)
{
	if(fileIsSet &&
		(status == DEBUG_WAIT || status == DEBUG_BREAK))
	{
		//int currArrayPos = 0;

		if(numBreak() == 0 && gdbBreakpointNum == 1)
		{
			classStatus = START;	//ignore the output

			command = "list" + returnChar;
			sendCommand(command);
			gdbBreakpointNum = 1;
		}
		
		classStatus = S_BREAK;
		command.Printf("break \"%s:%d\"%s", srcFile, lineNum, returnChar.c_str());	
		sendCommand(command);
	
		//breakpointNum[numBreakpoints] = gdbBreakpointNum;
		//currArrayPos = lineToNum[srcFile].lineNumbers.GetCount();
		lineToNum[srcFile].lineNumbers.Add(lineNum);
		lineToNum[srcFile].gdbNumbers.Add(gdbBreakpointNum);
		numBreakpoints++;
		gdbBreakpointNum++;
	}
}

//enableBreak(): enables a breakpoint at a given line number.
void Debugger::enableBreak(wxString srcFile, int lineNum)
{
	int equivNum = findBreakpoint(srcFile, lineNum);

	if(equivNum == -1)
	{
		wxString baka = error;
		int suBaka = status;

		error.Printf("Failed to locate breakpoint in %s at %d", srcFile.c_str(), lineNum);
		makeGenericError("enableBreak: ");

		error = baka;
		status = suBaka;
	}
	else
	{
		command.Printf("enable %d%s", equivNum, returnChar.c_str());
		sendCommand(command);

		classStatus = E_BREAK;
	}
}

//disableBreak(): disables a breakpoint at a given line number
void Debugger::disableBreak(wxString srcFile, int lineNum)
{
	int equivNum = findBreakpoint(srcFile, lineNum);

	if(equivNum == -1)
	{
		wxString baka = error;
		int suBaka = status;

		error.Printf("Failed to locate breakpoint in %s at %d", srcFile.c_str(), lineNum);
		makeGenericError("disableBreak: ");

		error = baka;
		status = suBaka;
	}
	else
	{
		command.Printf("disable %d%s", equivNum, returnChar.c_str());
		sendCommand(command);

		classStatus = D_BREAK;
	}
}

//killBreak(): deletes a breakpoint at line X
void Debugger::killBreak(wxString srcFile, int lineNum)
{
	int equivNum = findBreakpoint(srcFile, lineNum, true);
	if(equivNum == -1)
	{
		wxString baka = error;
		int suBaka = status;

		error.Printf("Failed to locate breakpoint in %s at %d", srcFile.c_str(), lineNum);
		makeGenericError("killBreak: ");

		error = baka;
		status = suBaka;
	}

	classStatus = K_BREAK;
}

//findBreakpoint(): finds a breakpoint within the hash & returns the GDB #
int Debugger::findBreakpoint(wxString fName, int lineNum, bool andRemove)
{
	//altered... added wxArrayInt & took (lineToNum[fName].lineNumbers) out
	bool found = false;
	int equivNum = 0;
	int arrayCount = lineToNum[fName].lineNumbers.GetCount();
	wxArrayInt tmp = lineToNum[fName].lineNumbers;

	for(int i = 0; i < arrayCount && found == false; i++)
	{
		if(tmp[i] == lineNum)
		{
			found = true;
			equivNum = lineToNum[fName].gdbNumbers[i];
		}
	}//end for loop

	if(found)
	{
		if(andRemove)
		{
			lineToNum[fName].lineNumbers.RemoveAt(i - 1);
			lineToNum[fName].gdbNumbers.RemoveAt(i - 1);

			command.Printf("delete break %d%s", equivNum, returnChar.c_str());
			sendCommand(command);

			numBreakpoints--;
		}

		return(equivNum);
	}
	return(-1);
}

//numBreaks(): returns the # of breakpoints
int Debugger::numBreak()
{
	return(numBreakpoints);
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
		classStatus = STEP_OVER;
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

void Debugger::stop(bool pleaseRestart)
{
	int statBackup = status;
	int classStatB = classStatus;

	if(status == DEBUG_RUNNING || status == DEBUG_ERROR)
	{
		debugProc->Kill(pid, wxSIGKILL);	//KILL KILL KILL!!
	} 
	else if(status == DEBUG_DEAD)
	{
		error = "tried to stop dead process";
		makeGenericError("stop(): ");
		status = DEBUG_DEAD;
	}
	else
	{
		//try to stop as cleanly as possible?
		if(numBreak() > 0)
		{
			command = "delete breakpoint" + returnChar;
			sendCommand(command);
		}

		if(fileIsSet)
		{
			command = "file" + returnChar;
			sendCommand(command);
		}

		command = "quit" + returnChar;
		sendCommand(command);

		//temporary---?
		//debugProc->Detach();
	}

	//is this appropriate for me???
	if(isRemote)
	{
		if(statBackup != DEBUG_RUNNING &&
			statBackup != DEBUG_ERROR &&
			statBackup != DEBUG_DEAD)
		{
			command = "exit" + returnChar;
			sendCommand(command);
		}
	}

	classStatus = STOP;
	status = DEBUG_DEAD;

	procLives = false;

	if(pleaseRestart)
	{	
		startProcess(true, isRemote, currFile, firstExecString, outputScreen);
	}//end pleaseRestart conditional
}

//kill(): questions?
void Debugger::kill()
{
	classStatus = STOP;
	status = DEBUG_STOPPED;

	procLives = false;

	debugProc->Kill(pid, wxSIGKILL);	//KILL KILL KILL!!

	flushBuffer();
	flushPrivateVar();
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
void Debugger::snoopVar(wxString varName, bool oneShot)
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
	wxString tempHold, Filename, Linenumber, goBreakpoint;
	wxArrayString tmpArrayString;
	
	int classStatusBackup = 0;
	long tmpLong;

	bool skipThrough = true, 
		 keepParsing = false;

	//step parsing RegEx
	wxRegEx reCase1 = "at (([[:alnum:]]|[[:blank:]]|\\.)+):([[:digit:]]+)";
	wxRegEx reCase2 = "([[:digit:]]+)[[:blank:]]+";

	//go parsing RegEx
	wxRegEx reStart = " command: \"done\"";
	wxRegEx reGoCase = "Breakpoint ([[:digit:]]+)";
	//!!END variables!!//

	status = DEBUG_WAIT;

	//data now has the output...
	tempHold = e.GetOutput();
	data.Add(tempHold);
	
	//data.Add(e.GetOutput());

	//~~DEBUG CODE~~//
	//(*outputScreen)<<"-Class Status: "<<classStatus<<"-\n";
	//(*outputScreen)<<"--Output:\n"<<tempHold<<"\n--end output--\n";
	wxLogDebug("DB output: %s", tempHold);
	tempHold.Empty();

	//What we're doing with output events:
	//1) See if the captured string has a [$] all by itself at the end
	//2) If so, begin parsing.  Set [classStatus = PARSING]
	//3) If not, wait for another output
	if(classStatus != START)
	{
		for(int i = 0; 
		    i < (int)data.GetCount() && skipThrough; 
			i++)
		{
			tempHold = data[i];
			//(*outputScreen)<<"--DEBUG: tmpHold.Last(): "<<tempHold.Last()<<"--\n";
			if(tempHold.Last() == PROMPT_CHAR)
			{
				tempHold.Empty();

				for(int j = 0; j <= i; j++)
				{tempHold << data[j];}

				skipThrough = false;
			}
		}
	}
	else
	{
		skipThrough = false;
	}

	//(*outputScreen)<<"output: "<<tempHold<<"\n";

	//check to see if something good was found.
	if(skipThrough)
	{
		classStatusBackup = classStatus;
		classStatus = WAITING;
	}
	
	//i need to do parsing here & sending mark stuff i guess...
	switch(classStatus)
	{
		case START:			//program start
			if(errorMsg() != "")
			{
				if(reStart.Matches(errorMsg()))
				{
				//	(*outputScreen)<<"\n\nSTART!\n\n";
					status = DEBUG_STOPPED;
					go();
				}
				else
				{
					//(*outputScreen)<<"\n\nreStart does not match error\n\n";
				}
			}

			data.Empty();
			break;

		case E_BREAK:		//enable break
		case D_BREAK:		//disable break
		case K_BREAK:		//kill break
			//nothing to parse...
			data.Empty();
			break;

		case S_BREAK:		//set break
			data.Empty();
			break;

		case GO:
			keepParsing = checkOutputStream(tempHold);

			if(keepParsing)
			{
				if(classStatus == STOP)
				{
					myEvent.SetStatus(ID_DEBUG_EXIT_NORMAL);
					guiPointer->AddPendingEvent(myEvent);
				}

				if(reGoCase.Matches(tempHold))
				{
					goBreakpoint = reGoCase.GetMatch(tempHold, 1);
					//(*outputScreen)<<"\nCaptured GO Breakpoint = "<<goBreakpoint<<"\n\n";

					if(reCase1.Matches(tempHold))
					{
						Filename = reCase1.GetMatch(tempHold, 1);
						Linenumber = reCase1.GetMatch(tempHold, 3);
						status = DEBUG_BREAK;

						Linenumber.ToLong(&tmpLong);
						tmpArrayString = myEvent.GetSourceFilenames();
						tmpArrayString[0] = Filename;

						myEvent.SetLineNumber((int)tmpLong);
						myEvent.SetSourceFilenames(tmpArrayString);
						myEvent.SetStatus(ID_DEBUG_BREAKPOINT);
						guiPointer->AddPendingEvent(myEvent);
						//(*outputScreen)<<"\nCASE1 Matches: fname="<<Filename<<" #="<<Linenumber<<"\n\n";
					}
					else
					{
						//(*outputScreen)<<"Unable to grab filename from breakpoint?!\n";
					}
				}
				else
				{
					error.Printf("program stopped for an unknown reason. ClassStatus %d", classStatus);
					makeGenericError("go parse: ");

					myEvent.SetStatus(ID_DEBUG_EXIT_ERROR);
					guiPointer->AddPendingEvent(myEvent);
					stop(false);
				}
			}
			data.Empty();
			break;

		case JUMP:
		case SOFT_RESET:
		case HARD_RESET:
		case NEW_FILE:		//new file to debug
		case NEW_MODE:		//change in program mode
			flushBuffer();
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
		//	(*outputScreen)<<"--Made it to the STEP case--"<<classStatus<<"\n";
		case STEP_OVER:
		case STEP_OUT:

			//looking for this heirarchy:
			// 1) the word "Program" as this indicates a fatal error
			//  2) the pattern "f_name.ext:ln"
			//   3) a line number
			//case 1, reset statuses and send an error
			//case 2, update the filename and line number vars to send
			//case 3, update the line number and send
			//once one of these items is found, do a check on [topVarIndex > 0]
			//and update the array;

			//kudos to Mark for the RegEx!!
			keepParsing = checkOutputStream(tempHold);

			//begin testing cases
			if(keepParsing)
			{
				if(reCase1.Matches(tempHold))
				{
					Filename = reCase1.GetMatch(tempHold, 1);
					Linenumber = reCase1.GetMatch(tempHold, 3);

					Linenumber.ToLong(&tmpLong);
					tmpArrayString = myEvent.GetSourceFilenames();
					tmpArrayString[0] = Filename;

					myEvent.SetLineNumber((int)tmpLong);
					myEvent.SetSourceFilenames(tmpArrayString);
					myEvent.SetStatus(ID_DEBUG_STEPNEXT);
					guiPointer->AddPendingEvent(myEvent);
					//(*outputScreen)<<"\nCASE 1: file="<<Filename<<" line="<<Linenumber<<"\n\n";
				}
				else if(reCase2.Matches(tempHold))
				{
					Linenumber = reCase2.GetMatch(tempHold, 1);
	
					Linenumber.ToLong(&tmpLong);
					
					myEvent.SetLineNumber((int)tmpLong);
					myEvent.SetStatus(ID_DEBUG_STEPNEXT);
					guiPointer->AddPendingEvent(myEvent);
					//(*outputScreen)<<"\nCASE 2: #="<<Linenumber<<"\n\n";
				}
				else
				{
					//(*outputScreen)<<"Couldn't parse STEP output\n";
					error = "Failed to match STEP output";
					makeGenericError("step parse:");

					myEvent.SetStatus(ID_DEBUG_EXIT_ERROR);
					guiPointer->AddPendingEvent(myEvent);
					stop(false);
				}
			}
			else
			{
				myEvent.SetStatus(ID_DEBUG_EXIT_ERROR);
				guiPointer->AddPendingEvent(myEvent);
				stop(false);
			}

			data.Empty();
			break;

		case CONTINUE:
			break;

		case WAITING:		//i'm waiting...
			classStatus = classStatusBackup;
			break;

		case STOP:
			status = DEBUG_ERROR;
			classStatus = STOP;
			error = "Extreme debug error OR quit-command sent before output finished: received output with ClassStatus=STOP.";
			makeGenericError("See previous error messages (if any)");

			myEvent.SetStatus(ID_DEBUG_EXIT_ERROR);
			guiPointer->AddPendingEvent(myEvent);
			stop(false);

			data.Empty();
			break;

		default:
			status = DEBUG_ERROR;
			classStatus = STOP;
			error = "[classStatus] value not recognized";
			makeGenericError("failure on output parse:");

			myEvent.SetStatus(ID_DEBUG_EXIT_ERROR);
			guiPointer->AddPendingEvent(myEvent);
			stop(false);

			data.Empty();
	}
}

//checkOutputStream(): function for onOutputEvent, checks stream for common
//  stream features... like "Program received" or "Breakpoint..."
bool Debugger::checkOutputStream(wxString stream)
{
	wxRegEx reCase1 = "at (([[:alnum:]]|[[:blank:]]|\\.)+):([[:digit:]]+)";
	wxString thirdCase = stream;

	//(*outputScreen)<<"-thirdCase.Left[8] ->"<<thirdCase.Left(8)<<"\n";

	if(thirdCase.Left(8) == "Program")
	{
		thirdCase.Remove(0,9);
		if(thirdCase.Left(8) == "received")
		{
			int newlineIndex = thirdCase.find_first_of("\n");
			thirdCase.Remove(0, newlineIndex);

			//if we've gotten here, the program has received a message of some type.
			//either way, not good for further parsing.
			if(reCase1.Matches(thirdCase))
			{
				//(*outputScreen)<<"Match for \"Program recieved\".  match 1 = "<<reCase1.GetMatch(thirdCase, 1)<<", match 2 = "<<reCase1.GetMatch(thirdCase, 3)<<"\n";
				error = "Received bad program signal.  RegEx: " + reCase1.GetMatch(thirdCase, 1) + ", " + reCase1.GetMatch(thirdCase, 3);
				makeGenericError("checkOutputStream:  ");
				return(false);
			}
			else
			{
				//(*outputScreen)<<"Match for \"Program received\"; regex failed\n";
				
				error = "Received bad program signal.  No RegEx.";
				makeGenericError("checkOutputStream:  ");
				return(false);
			}
		}
		else if(thirdCase.Left(6) == "exited")
		{
			thirdCase.Remove(0,7);
			if(thirdCase.Matches("normal"))
			{
				status = DEBUG_STOPPED;
				classStatus = STOP;
				return(true);
			}
			else
			{
				error.Printf("Program terminated abnormally.  ClassStatus %d", classStatus);
				makeGenericError("checkOutputStream: ");
				return(false);
			}
		}
		else
		{
			//(*outputScreen)<<"Unknown program signal received; status halted\n";
			
			error.Printf("Unknown program signal. ClassStatus %d", classStatus);
			makeGenericError("checkOutputStream: ");
			return(false);
		}
	}

	return(true);
}

void Debugger::onProcessErrOutEvent(wxProcess2StdErrEvent &e)
{
	status = DEBUG_ERROR;

	//error now has the output
	error = e.GetError();
	addErrorHist("Event-Generated error");

	//(*outputScreen)<<"Generated Error: "<<error<<"\n";
	wxLogDebug("DB error event: %s", error);
	//again, some event stuff here...
}

void Debugger::onProcessTermEvent(wxProcess2EndedEvent &e) 
{
	//the process has been killed (aka GDB quit for some reason)
	if(e.GetPid() == pid) 
	{
		//(*outputScreen)<<"Process ("<<pid<<") Terminated\n";

		status = DEBUG_DEAD;
		classStatus = STOP;
		streamError = NULL;
		streamIn = NULL;
		pid = -2; // a nothing pid
		procLives = false;
		//if(debugProc != NULL){delete debugProc;}
	}
}

//flushBuffer(): it simply erases the data array.
void Debugger::flushBuffer()
{
	data.Empty();
}

//flushPrivateVar(): erases all private data members
void Debugger::flushPrivateVar()
{
	isRemote = false;
	procLives = false;
	fileIsSet = false;
	currFile = "";
	pid = 0;
	currDebugLine = 0;
	numBreakpoints = 0;
	gdbBreakpointNum = 1;
	//breakpointNum.Empty();
	lineToNum.empty();

	commandHistory.Empty();
	histCount = 0;
	error = "";
	errorHist.Empty();
	errorCount = 0;
	command = "";
	returnChar = "";
	data.Empty();
	varNames.Empty();
	varValue.Empty();
	varDispIndex.Empty();
	gdbVarIndex = 0;
	varCount = 0;
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
		//(*outputScreen)<<send<<"\n";
		wxLogDebug("DB send command: %s", send);
	}
	else
	{
		send = send + "<- NOT SENT: PROC DEAD";
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
	commandHistory.Add(addMe);
	histCount++;
}

//getHistoryItem: returns the last history item offset by [offset].
wxString Debugger::getHistoryItem(int offset = 0)
{
	if(offset > histCount)
	{
		offset = histCount;
	}
	return(commandHistory[offset]);
}

//eof