/*****************************************************************\
*Wrapper Code file
*
*author: Ben Carhart
*date started: 1/11/04
*date finished: 4/17/04 (tentatively)
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
*BSC-04/03/04->So much has changed that it's not funny.  Debug
*              event code has been changed to work with projects.
*BSC-04/15/04->... yeah... not EVEN gonna.  To see what's changed,
*              somehow get your hands on all the previous versions
*              and do a "DIFF".  ^_^
*BSC-04/17/04->While the conceptual design of this class is done,
*              basic modifications & bug fixes will continue.
\*****************************************************************/

#include "cham_db.h"
#include "../common/ProjectInfo.h"
#include <wx/wfstream.h>
#include <wx/RegEx.h>

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(VariableInfoArray);

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//event handling
BEGIN_EVENT_TABLE(Debugger, wxEvtHandler)
	EVT_PROCESS_ENDED(Debugger::onProcessTermEvent)
	EVT_PROCESS_STDOUT(Debugger::onProcessOutputEvent)
	EVT_PROCESS_STDERR(Debugger::onProcessErrOutEvent)
	EVT_DEBUG(Debugger::onDebugEvent)
END_EVENT_TABLE()

//straight up constructor
Debugger::Debugger(Networking* networking, wxEvtHandler* pointer)
{
	flushPrivateVar();
	varCount = 0;
	m_numberOfDebugEvents = 0;

	myEvent = NULL;

	guiPointer = pointer;
	procLives = false;
	status = DEBUG_DEAD;
	classStatus = STOP;
	myConnection = networking;

	//reg-Ex for Variable watching
	//------
	// ints - "$n = val"
	// look for a space first, to make sure we don't grab the GDB print number instead
	varRegExes["int"] = " ([[:digit:]]+)";

	// ints & - "$n = (int &) val"
	//note: is this even needed??
	varRegExes["&"] = ".+ ([[:digit:]]+)";

	// ints * - "$n = val"
	//TODO: Eliminate this
	varRegExes["int*"] = "([[:digit:]]+)";
	
	// double-"$n = val.val"
	varRegExes["double"] = "([[:digit:]]+\\.[[:digit:]]+)";

	// char[]-"$n = val 'char'"
	varRegExes["char"] = "[[:digit:]]+ '(.*+)'";
	
	// char - "$n = "text""
	varRegExes["char[]"] = "\"(.*)\"";//"[[:digit:]]+ '([[:alnum:]]+)'";
	
	// array- "$n = {val, val...}"
	varRegExes["array"] = "({.*})\\r\\n$";
	
	// str#2- "$n = "text"" (same as [char])
	varRegExes["string"] = "\"(.*)\"";

	// obj -  "$n = {varName = val, varName = val, ..} (varName can be of any listed type...)
	varRegExes["class"] = "({.*})\\r\\n$";
}

//destructor
Debugger::~Debugger()
{
	//initial stuff to cleanly exit (sorta)
	stop(false);

	if(myEvent != NULL)
	{
		delete myEvent;
	}

	flushPrivateVar();
	m_varInfo.Clear();
}//end ~Debugger

//onDebugEvent(): catches when Mark sends me something...
void Debugger::onDebugEvent(wxDebugEvent &event)
{
	int eventCommand = event.GetId();

	/* We can only accept certain types of events while stopped/paused.
	 * If we're running and we get an unacceptable event type for the current
	 * state, we put it back on the event queue.  We use a counter with a 
	 * completely arbitrary number (say, 50) to keep us from going into an
	 * infinite loop.  If we've received events 50 times without actually
	 * processing one, then we'll just pitch it and return.  This combination
	 * prevents stuff like getting stuck after repeatedly pressing Step too fast.
	 */
	if(eventCommand != ID_DEBUG_STOP &&
		eventCommand != ID_DEBUG_START && 
		eventCommand != ID_DEBUG_ADD_WATCH &&
		eventCommand != ID_DEBUG_REMOVE_WATCH)
	{
		if(status != DEBUG_WAIT &&
			status != DEBUG_BREAK)
		{
			if(m_numberOfDebugEvents < 50)
			{
				this->AddPendingEvent(event);
			}
			
			m_numberOfDebugEvents++;
			return;
		}
	}
	m_numberOfDebugEvents = 0;

	if(myEvent != NULL)
	{
		delete myEvent;
	}
	myEvent = (wxDebugEvent*)event.Clone();

	//all the variables i need here:
	int eventLineNum;
	//loop vars
	int i = 0, j = 0;
	
	wxArrayString gui_var_names;
	wxString varName, funcName, className;
	
	wxArrayInt breakpointLines;

	FileBreakpointHash breakpointList;
	
	wxString tmp;
	wxString firstFile;
	wxString sendAllBreakpoints;
	
	//grab information
	if( eventCommand == ID_DEBUG_ADD_BREAKPOINT ||
		eventCommand == ID_DEBUG_REMOVE_BREAKPOINT ||
		eventCommand == ID_DEBUG_RUN_TO_LINE)
	{
		firstFile = event.GetSourceFilename();
		eventLineNum = event.GetLineNumber();
	}

	if(eventCommand == ID_DEBUG_START)
	{
		breakpointList = event.GetFileBreakpoints();
	}

	if( eventCommand == ID_DEBUG_ADD_WATCH ||
		eventCommand == ID_DEBUG_REMOVE_WATCH)
	{
		gui_var_names = event.GetVariableNames();
		varName = gui_var_names[0];
		funcName = event.GetFunctionName();
		className = event.GetClassName();
	}

	//begin parsing the information i grabbed
	switch(eventCommand)
	{
	case ID_DEBUG_START:
	{
	
		//start process
		ProjectInfo* proj = event.GetProject();

		wxArrayString srcFiles = proj->GetSources();
		wxString execFile = proj->GetExecutableFileName();
		projectBeingDebugged = proj;

		//"gdb -q" is the default program run.  Might need to be a "global read-only"
		//var for ease of use.
		startProcess(true, event.IsRemote(), execFile, "gdb -q");

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

					lineToNum[tmp].lineNumbers.Add(breakpointLines[j]);
					lineToNum[tmp].gdbNumbers.Add(gdbBreakpointNum);

					numBreakpoints++;
					gdbBreakpointNum++;	
				}
			}//end if
		}

		sendCommand("list"+returnChar);
		sendCommand(sendAllBreakpoints);
		sendCommand("done"+returnChar);	//tag... not really a command
		break;
	}

	case ID_DEBUG_STOP:
	{
		stop(false);
		wxDebugEvent dbg;
		dbg.SetId(ID_DEBUG_EXIT_NORMAL);
		guiPointer->AddPendingEvent(dbg);
		break;
	}

	case ID_DEBUG_STEPNEXT:
		step();
		break;

	case ID_DEBUG_STEPOVER:
		stepOver();
		break;

	case ID_DEBUG_STEPOUT:
		stepOut();
		break;
	case ID_DEBUG_CONTINUE:
		cont();
		break;

	case ID_DEBUG_RUN_TO_LINE:
		runToCursor(firstFile, eventLineNum);
		break;

	case ID_DEBUG_ADD_BREAKPOINT:
		setBreak(firstFile, eventLineNum);
		break;

	case ID_DEBUG_REMOVE_BREAKPOINT:
		killBreak(firstFile, eventLineNum);
		break;

	case ID_DEBUG_ADD_WATCH:
		snoopVar(varName, funcName, className, false);
		break;

	case ID_DEBUG_REMOVE_WATCH:
		removeVar(varName, funcName, className);
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

//setMode(): removed as it's either never used or too complex.

//currMode(): returns current mode
bool Debugger::getMode()
{
	return(isRemote);
}

//for Mark...
bool Debugger::isDebugging()
{
	return((status == DEBUG_RUNNING) || (status == DEBUG_WAIT) || (status == DEBUG_BREAK));
}

bool Debugger::isPaused()
{
	return(status == DEBUG_WAIT || status == DEBUG_BREAK);
}

ProjectInfo* Debugger::getCurrentProject()
{
	return projectBeingDebugged;
}
//--//

//setFile(): set's the current file to be debugged. Never used.

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
		
		return(true);
	}

	//this means the proc is dead so there's nothing to reset
	return(false);
}

//clearError(): does a "soft" reset by just changing the status variable.
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
	
	startProcess(true, newIsRemote, newFname, nExec);
}


void Debugger::startProcess(bool fullRestart, bool mode, wxString fName, wxString execThis)
{
	if(fullRestart)
	{
		//generic initialization
		isRemote = mode;			//false = local
		currFile = fName;
	
		histCount = 0;
	
		numBreakpoints = 0;
		gdbBreakpointNum = 1;
	
		errorCount = 0;
	
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
	
	procLives = true;
	classStatus = NEW_PROC;
	

	streamOut = myConnection->StartCommand(isRemote, execThis, this);

	//begin funky process stuff

	command = execThis + "<-sent";
	updateHistory(command);

	//initial commands to GDB
	
	wxArrayString initString;
	wxString tmp;

	classStatus = START;

	//initialize GDB
	tmp.Printf("set prompt %s%s", PROMPT_CHAR.c_str(), returnChar.c_str());
	initString.Add(tmp);
	initString.Add("set print array off" + returnChar);
	initString.Add("set print pretty off" + returnChar);
	initString.Add("set print address off" + returnChar);
	initString.Add("set confirm off" + returnChar);

	//the 10000 here is an arbitrary number.
	initString.Add("set print repeats 10000" + returnChar);

	//send all initlization commands to GDB
	for(int currInit = 0; currInit < int(initString.GetCount()); currInit++)
	{
		sendCommand(initString.Item(currInit));
	}
		
	if(fileIsSet)
	{
		command = "file " + currFile + returnChar;
		sendCommand(command);
	}//end set-file conditional
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

		//this if{} should never be run...
		if(numBreak() == 0 && gdbBreakpointNum == 1)
		{
			classStatus = START;	//ignore the output
			gdbBreakpointNum = 1;
		}
		
		classStatus = S_BREAK;
		command.Printf("break \"%s:%d\"%s", srcFile, lineNum, returnChar.c_str());	
		sendCommand(command);

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

	//if(found)
	int lineIndex = tmp.Index(lineNum);
	if(lineIndex != wxNOT_FOUND)
	{
		if(andRemove)
		{
			int equivNum = lineToNum[fName].gdbNumbers[lineIndex];
			
			lineToNum[fName].lineNumbers.Remove(lineNum);
			lineToNum[fName].gdbNumbers.Remove(equivNum);

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
	//if not stopped or waiting, then program status = error of some type
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

//runToCursor(): it runs to the cursor.  GDB has a nice one-shot breakpoint
//  called "tbreak" so i set that, and send a "cont()" command.  :)
void Debugger::runToCursor(wxString srcFile, int lineNum)
{
	if(status == DEBUG_WAIT || status == DEBUG_BREAK)
	{
		//GDB automatically sets & kills the breakpoint, but the number is incremented
		gdbBreakpointNum++;

		command.Printf("tbreak \"%s:%d\"%s", srcFile, lineNum, returnChar);
		sendCommand(command);

		cont();
	}

	//if not stopped or waiting, then program status is not good for stepOut
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
		myConnection->ForceKillProcess(streamOut);
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

		if(varCount > 0)
		{
			command = "delete display" + returnChar;
			sendCommand(command);
		}

		if(fileIsSet)
		{
			command = "file" + returnChar;
			sendCommand(command);
		}

		command = "quit" + returnChar;
		sendCommand(command);
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

	flushPrivateVar();

	classStatus = STOP;
	status = DEBUG_DEAD;
	resetStatus();

	procLives = false;

	if(pleaseRestart)
	{	
		startProcess(true, isRemote, currFile, firstExecString);
	}
}

//kill(): questions?
void Debugger::kill()
{
	classStatus = STOP;
	status = DEBUG_DEAD;

	procLives = false;

	myConnection->ForceKillProcess(streamOut);

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
		classStatus = CONTINUE;
		status = DEBUG_RUNNING;
		sendCommand(command);
	}
}
//END STEP FUNCTIONALITY

//VARIABLE FUNCTIONALITY
//----------------------


//snoopVar(): adds, if not in the array, the given variable.  Actual snooping
//  is done in [onOutputEvent]
void Debugger::snoopVar(wxString varName, wxString funcName, wxString className, bool oneShot)
{
	wxString combinedFunction;
	VariableInfo variableStruct;
	bool notFound = true;

	combinedFunction<<className<<"::"<<funcName;

	//Step 1: See if the variable & function/class exists in the structure array
	if(varCount == 0)
	{	
		//we're adding the first item to the array
		variableStruct.name = varName;
		variableStruct.functionName = combinedFunction;
		variableStruct.type = "null";
		variableStruct.value = "0";

		m_varInfo.Add(variableStruct);

		varCount++;
	}
	else
	{
		//check to see if the variable is already in the array
		for(int i = 0; (i < varCount) && (notFound); i++)
		{
			if(m_varInfo[i].name == varName)
			{
				notFound = false;
			}
		}

		if(notFound)
		{
			variableStruct.name = varName;
			variableStruct.functionName = combinedFunction;
			variableStruct.type = "null";
			variableStruct.value = "0";

			m_varInfo.Add(variableStruct);

			varCount++;
		}
	}

	//Step 2: if GDB is running, send a "whatis" command to get the type.
	if( notFound &&
		(status == DEBUG_BREAK || status == DEBUG_WAIT))
	{
		sendWhat();

		classStatus = GET_WHAT;
	}
}

//setVar(): set's a variable to a given value.  never used.

//sendWhat(): sends "whatis name" for every variable that doesn't have a type set.
void Debugger::sendWhat()
{
	if(varCount > 0)
	{
		wxString tmp;
		command.Clear();
		for(int i = 0; i < varCount; i++)
		{
			tmp.Printf("whatis %s%s", m_varInfo[i].name.c_str(), returnChar.c_str());
			command.Append(tmp);
		}

		sendCommand(command);

		classStatus = GET_WHAT;
	}
}

//sendPrint(): sends "print name" for every variable that has a type
//  Also captures the types send from [sendWhat]
//  NOTE: i'm not sure how this function handles "dirty" input
void Debugger::sendPrint(wxString fromGDB)
{
	wxString singleLine;
	wxArrayString fromWatch, ignoreVars;
	bool watchStatus = false;
	int lineBreak = 0, endQuote = 0, fromWatchIndex = 0, ampIdx = -1;

	do
	{
		lineBreak = fromGDB.Find("\n");

		if(lineBreak == -1)
		{
			fromGDB = PROMPT_CHAR;
		}
		else
		{
			singleLine = fromGDB.Mid(0, lineBreak);
			singleLine = singleLine.BeforeFirst('\r');

			fromGDB = fromGDB.AfterFirst('\n');

			if(singleLine.Mid(0,4) == "type")
			{
				lineBreak = 2 + singleLine.Find("= ");
				singleLine = singleLine.Mid(lineBreak);


				ampIdx = singleLine.Find(" &");
				if(ampIdx != -1)
				{
					singleLine = singleLine.Remove(ampIdx);
					singleLine.Append("&");
				}

				ampIdx = singleLine.Find(" *");
				if(ampIdx != -1)
				{
					singleLine = singleLine.Remove(ampIdx);
					singleLine.Append("*");
				}

				fromWatch.Add(singleLine);
			}
			else if(singleLine.Mid(0,9) == "No symbol")
			{
				lineBreak = singleLine.Find("\"");
				singleLine = singleLine.Mid(lineBreak + 1);

				endQuote = singleLine.Find("\"");
				singleLine = singleLine.Mid(0, endQuote);

				ignoreVars.Add(singleLine);
			}

		}
	}while(fromGDB != PROMPT_CHAR);


	if(!fromWatch.IsEmpty())
	{
		for(int i = 0; i < varCount; i++)
		{
			if(ignoreVars.Index(m_varInfo[i].name) == wxNOT_FOUND)
			{
				//current variable is not in the "ignoreVars" list...
				if(fromWatchIndex < (int)fromWatch.GetCount())
				{
					m_varInfo[i].type = fromWatch[fromWatchIndex];
					fromWatchIndex++;
				}
				else
				{
					//if we're down here, then there are more variables than there are
					//printed outputs.  So we by-pass the output in hopes that the
					//next few "whatis" statements come through
					watchStatus = true;
				}
			}//end variable ignore existance check
		}//end for
	}

	if(!watchStatus)
	{
		wxString tmp;
		command.Clear();
		for(int i = 0; i < varCount; i++)
		{
			wxString name = m_varInfo[i].name;

			//special cases:
			// type==string: append "._M_dataplus._M_p" to format the string
			// type==[xxx]*: pre-pend "*" to de-reference the pointer type
			
			if(m_varInfo[i].type == "string")
			{
				name += "._M_dataplus._M_p";
			}
			if(m_varInfo[i].type.Find("*") != -1)
			{	
				name.Prepend("*");
			}

			tmp.Printf("print %s%s", name, returnChar);
			command.Append(tmp);
		}

		sendCommand(command);

		classStatus = GET_PRINT;
	}
}

//removeVar(): removes a variable from the array list.
void Debugger::removeVar(wxString varName, wxString funcName, wxString className)
{
	//some added verbosity
	bool notFound = true,
		 found = false;

	if(varCount != 0)
	{
		for(int i = 0; (i < varCount) && (notFound); i++)
		{
			if(m_varInfo[i].name == varName)
			{
				notFound = false;
				found = true;
			}
		}

		if(found)
		{
			m_varInfo.RemoveAt(i - 1);
			varCount--;
		}
	}//end varCount if
}

//parseOutput(): takes GDB output & parses it for the "print %s" command output
bool Debugger::parsePrintOutput(wxString fromGDB, wxArrayString &varValue)
{
	wxString singleLine, match;
	wxArrayString fromWatch, ignoreVars;
	int lineBreak = 0, endQuote = 0, fromWatchIndex = 0;
	bool parseError = false, stayIn = true;

	for(int i = 0; (i < varCount) && !parseError; i++)
	{
		lineBreak = fromGDB.Find("\n");

		if(lineBreak == -1)
		{
			parseError = true;
		}
		else
		{
			stayIn = true;

			while(stayIn)
			{
				// fromGDB should contain Windows-based line endings, ie, CRLF (\r\n).  
				lineBreak = fromGDB.Find("\n");
				singleLine = fromGDB.Mid(0, lineBreak + 1);

				fromGDB = fromGDB.AfterFirst('\n');

				if(singleLine.IsEmpty())
				{
					stayIn = false;
				}

				if( stayIn && 
					(singleLine.StartsWith("$") ||
					singleLine.StartsWith("No symbol")))
				{
					while(!fromGDB.StartsWith("$") && 
							!fromGDB.StartsWith("No Symbol") &&
							!fromGDB.StartsWith(PROMPT_CHAR) &&
							!fromGDB.IsEmpty())
					{
						lineBreak = fromGDB.Find("\n");
						singleLine += fromGDB.Mid(0, lineBreak + 1);
						fromGDB = fromGDB.AfterFirst('\n');
					}
					stayIn = false;
				}
			}

			if(!singleLine.IsEmpty())
			{
				wxString regexString;
				if(singleLine.Mid(0,9) == "No symbol")
				{
					varValue.Add(singleLine);
					continue;
				}
				else if(m_varInfo[i].type.Find("[") == -1)
				{
					if( m_varInfo[i].type.Find("&") == -1 &&
						m_varInfo[i].type.Find("*") == -1)
					{
						VariableInfo varInfo = m_varInfo[i];
						wxString type = varInfo.type;
						if(varRegExes.find(type) != varRegExes.end())
						{
							regexString = varRegExes[type];
						}
						else
						{
							regexString = varRegExes["class"];
						}
					}
					else
					{
						//we have a pointer dereference or a referenced variable on our hands
						if(m_varInfo[i].type.Find("&") > 0)
						{
							wxString tmp = m_varInfo[i].type.BeforeLast('&');
							regexString = varRegExes[tmp];
						}
						else
						{
							//we're assuming it's a dereferenced pointer
							wxString tmp = m_varInfo[i].type.BeforeLast('*');
							regexString = varRegExes[tmp];
						}
					}//end referenced variable test
				}
				else
				{
					//we have an array of some type...
					if(m_varInfo[i].type.Find("char") != -1)
					{
						regexString = varRegExes["char[]"];
					}
					else
					{
						regexString = varRegExes["array"];
					}
				}//end array-test
				if(regexString == wxEmptyString)
				{
					//continue;
					m_varInfo[i].type = "Unknown";
					varValue.Add(singleLine);
				}
				else
				{
					wxRegEx varParser(regexString, wxRE_ADVANCED);
					bool keepMatching = true;

					while(keepMatching && !parseError)
					{
						if(varParser.Matches(singleLine))
						{
							match = varParser.GetMatch(singleLine, 1);
							varValue.Add(match);
							keepMatching = false;
						}
						else
						{
							int nextMarker = -1;
							wxString endOfVarRegex;

							endOfVarRegex = "\\r\\n" + PROMPT_CHAR;

							wxRegEx reEndOfVar(endOfVarRegex, wxRE_ADVANCED);
							size_t start, len;

							if(reEndOfVar.Matches(fromGDB))
							{
								reEndOfVar.GetMatch(&start, &len);

								nextMarker = (int)start;
								wxString temp = fromGDB.Mid(0, nextMarker - 1);
								singleLine += temp;
							}
							else
							{
								parseError = true;
							}						
						} // end else
					} // end while
				} // end else regex == wxemptystring				
			} // end if !singleline.empty
		} // end else (linebreak)
	}//end for-loop

	if(parseError)
	{
		return(false);
	}
	return(true);
}
//END VARIABLE FUNCTIONALITY

//MISC FUNCTIONS:
//---------------
//These funcions are random public thingies that don't qualify under any other
//heading.  "stop()" and "kill()" are listed as "misc" but i include those in
//step functionality since those affect the running of this class.

//sendCustomCommand(): sends the passed string straight to the process.  Can be
//  used for user input or custom commands.
void Debugger::sendCustomCommand(wxString cust)
{
	if(procLives == true)
	{
		sendCommand(cust);
	}
}

//getProcOutput(): gets back X number of output entries.
//                 -1 for numEntries means get them ALL back.
wxString Debugger::getProcOutput(int numEntries = -1)
{
	wxString tmp = "";
	int outputCount = (int)fullOutput.GetCount();
	int loopStop = numEntries;
	if(outputCount > 0)
	{
		if( numEntries == -1 ||
			numEntries > outputCount)
		{loopStop = outputCount;}
		
		for(int i = 0; i < loopStop; i++)
		{tmp.Append(fullOutput[i]);}
	}

	return(tmp);
}


//PRIVATE FUNCTIONS:
//------------------

//The following 3 are from Dave Czechowski.  KUDOS!

void Debugger::onProcessOutputEvent(ChameleonProcessEvent &e)
{
	//variables to hold stuff for parsing...
	wxString tempHold, goBreakpoint;
	wxArrayString tmpArrayString;
	
	wxArrayString varNames, varValue, varType;

	wxString tempString;
	wxDebugEvent outputEvent;

	
	int classStatusBackup = 0,
		gdbNum = 0;
	long tmpLong;

	bool skipThrough = true, 
		 keepParsing = false;

	//step parsing RegEx
	wxRegEx reCase1 = " at (([[:alnum:]]|[[:blank:]]|\\.|/)+):([[:digit:]]+)";
	// needs the wxRE_ADVANCED in order to work properly, so can't just
	// assign this one as if it were a string
	wxRegEx reCase2("(\\n|^)(\\d+)[ \\t]+", wxRE_ADVANCED);

	//go parsing RegEx
	wxRegEx reStart = " command: \"done\"";
	wxRegEx reGoCase = "Breakpoint ([[:digit:]]+)";

	//breakpoint parsing RegEx
	wxRegEx reSetBreak = "Breakpoint ([[:digit:]]+)";
	//!!END variables!!//

	status = DEBUG_WAIT;

	//data now has the output...
	tempHold = e.GetString();
	data.Add(tempHold);
	fullOutput.Add(tempHold);

	tempString.Printf("\n---------\nDEBUGGER-> Output:\n%s\n---------\n", tempHold);
	wxLogDebug(tempString);
	tempString.Clear();
	

	//i don't think this code is effectual anymore
	/*
	if(classStatus == START)
	{
		skipThrough = false;
	}

	//check to see if something good was found.
	if(skipThrough)
	{
		classStatusBackup = classStatus;
		classStatus = WAITING;
	}
	*/
	
	switch(classStatus)
	{
		case START:			//program start
			//this re-directs to where the output actually is that we want
			if(isRemote)
			{tempString = tempHold;}
			else
			{tempString = errorMsg();}

			if(tempString != "")
			{
				if(reStart.Matches(tempString))
				{
					//the dope command "done" sent to GDB returns an error.
					//this means that all init. commands have gone through,
					//and we can start the program.
					status = DEBUG_STOPPED;
					go();
				}
			}

			data.Empty();
			break;

		case E_BREAK:		//enable break
		case D_BREAK:		//disable break
		case K_BREAK:		//kill break
			//nothing to parse...
			data.Clear();
			break;

		case S_BREAK:		//set break
			if(reSetBreak.Matches(tempHold))
			{
				goBreakpoint = reSetBreak.GetMatch(tempHold, 1);
				
				goBreakpoint.ToLong(&tmpLong);
				gdbNum = (int)tmpLong;
				
				if(gdbNum != (gdbBreakpointNum - 1))
				{
					error.Printf("#s don't match: caught=%d internal=%d", gdbNum, (gdbBreakpointNum - 1));
					makeGenericError("Set_Break parse: ");

					
					outputEvent.SetId(ID_DEBUG_EXIT_ERROR);
					guiPointer->AddPendingEvent(outputEvent);
					stop(false);
				}
			}
			data.Empty();
			break;

		case CONTINUE:
		case GO:
		{
			keepParsing = checkOutputStream(tempHold);

			if(keepParsing)
			{
				if(classStatus == STOP)
				{
					//here the program stopped normally.  That means, for user-
					//generated output, we can just look to the front of the
					//output string to the place "Program exited normally"
					//appears.  Right?  -BSC
					grabUserOutput(tempHold, "Program", 0);

					outputEvent.SetId(ID_DEBUG_EXIT_NORMAL);
					guiPointer->AddPendingEvent(outputEvent);
					stop(false);
					break;
				}

				if(reGoCase.Matches(tempHold))
				{
					goBreakpoint = reGoCase.GetMatch(tempHold, 1);

					//here we have a "breakpoint" sitting somewhere... and if
					//there's anything before it, that's user output.
					grabUserOutput(tempHold, "Breakpoint ", 0);

					if(reCase1.Matches(tempHold))
					{
						//funcName is for adv. variable watching
						//FuncName = reCase1.GetMatch(tempHold, 1);
						Filename = reCase1.GetMatch(tempHold, 1);
						Linenumber = reCase1.GetMatch(tempHold, 3);

						status = DEBUG_BREAK;

						Linenumber.ToLong(&tmpLong);
	
						outputEvent.SetLineNumber((int)tmpLong);
						outputEvent.SetSourceFilename(Filename);
						outputEvent.SetId(ID_DEBUG_BREAKPOINT);
						guiPointer->AddPendingEvent(outputEvent);

						data.Empty();

						sendWhat();
					}
				}
				else
				{
					//Either the program stopped with a new error...
					//OR
					//It's user input.  Put something here.
					error.Printf("program stopped for an unknown reason. ClassStatus %d", classStatus);
					makeGenericError("go parse: ");

					
					outputEvent.SetId(ID_DEBUG_EXIT_ERROR);
					guiPointer->AddPendingEvent(outputEvent);
					stop(false);
					break;
				}
			}
			else
			{
				//here the program has exited badly.  BUT there is still a 
				//"program" there...
				grabUserOutput(tempHold, "Program ", 0);
			}
			data.Empty();
			break;
		}

		case JUMP: //NOT USED IN RELEASE VERSION OF CHAMELEON
			break;

		case SOFT_RESET:
		case HARD_RESET:
		case NEW_FILE:		//new file to debug
		case NEW_MODE:		//change in program mode
			flushBuffer();
			break;

		case NEW_PROC:		//change in process pointer.  Never used...?
			flushBuffer();
			break;

		case VAR_ASSIGN: //NOT USED IN RELEASE VERSION OF CHAMELEON
			break;

		case GET_WHAT:
			sendPrint(tempHold);
			data.Empty();
			break;

		case GET_PRINT:
		{
			//varNames, varValue, varType
			bool dontskip = true;
			dontskip = parsePrintOutput(tempHold, varValue);

			if(dontskip)
			{
				for(int i = 0; i < varCount; i++)
				{
					varNames.Add(m_varInfo[i].name);
					varType.Add(m_varInfo[i].type);
				}

				classStatus = WAITING;

				if(varCount > 0)
				{
					outputEvent.SetVariableNames(varNames);
					outputEvent.SetVariableTypes(varType);
					outputEvent.SetVariableValues(varValue);
					outputEvent.SetId(ID_DEBUG_VARINFO);
					guiPointer->AddPendingEvent(outputEvent);
				}
			}

			data.Empty();
			break;
		}

		case WATCH_VAR:
			break;

 		case STEP:
		case STEP_OVER:
		case STEP_OUT:
		{

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
			bool noNewWhats = true;
			keepParsing = checkOutputStream(tempHold);

			//begin testing cases
			if(keepParsing)
			{
				if(classStatus == STOP)
				{

					outputEvent.SetId(ID_DEBUG_EXIT_NORMAL);
					guiPointer->AddPendingEvent(outputEvent);
					stop(false);
					break;
				}

				if(reCase1.Matches(tempHold))
				{
					Filename = reCase1.GetMatch(tempHold, 1);
					Linenumber = reCase1.GetMatch(tempHold, 3);

					Linenumber.ToLong(&tmpLong);
					
					outputEvent.SetLineNumber((int)tmpLong);
					outputEvent.SetSourceFilename(Filename);
					outputEvent.SetId(ID_DEBUG_BREAKPOINT);
					guiPointer->AddPendingEvent(outputEvent);

					data.Empty();

					sendWhat();

					noNewWhats = false;
				}
				else if(reCase2.Matches(tempHold))
				{
					Linenumber = reCase2.GetMatch(tempHold, 2);
	
					Linenumber.ToLong(&tmpLong);
					
					outputEvent.SetLineNumber((int)tmpLong);
					outputEvent.SetSourceFilename(Filename);
					outputEvent.SetId(ID_DEBUG_BREAKPOINT);
					guiPointer->AddPendingEvent(outputEvent);
				}
				else
				{
					error = "Failed to match STEP output";
					makeGenericError("step parse:");
					
					outputEvent.SetId(ID_DEBUG_EXIT_ERROR);
					guiPointer->AddPendingEvent(outputEvent);
					stop(false);
				}
			}
			else
			{
				outputEvent.SetId(ID_DEBUG_EXIT_ERROR);
				guiPointer->AddPendingEvent(outputEvent);
				stop(false);
			}

			data.Empty();

			if(noNewWhats)
			{
				sendPrint("");
			}
			break;
		}
		case WAITING:		//i'm waiting...
			classStatus = classStatusBackup;
			break;

		case STOP:
			status = DEBUG_ERROR;
			classStatus = STOP;
			error = "Extreme debug error OR quit-command sent before output finished: received output with ClassStatus=STOP.";
			makeGenericError("See previous error messages (if any)");

			
			outputEvent.SetId(ID_DEBUG_EXIT_ERROR);
			guiPointer->AddPendingEvent(outputEvent);
			stop(false);

			data.Empty();
			break;

		default:
			status = DEBUG_ERROR;
			classStatus = STOP;
			error = "[classStatus] value not recognized";
			makeGenericError("failure on output parse:");

			outputEvent.SetId(ID_DEBUG_EXIT_ERROR);
			guiPointer->AddPendingEvent(outputEvent);
			stop(false);

			data.Empty();
	}
}

//grabUserOutput(): searches for a particular string & if there, grabs
//  everything from [offset] to the found index
void Debugger::grabUserOutput(wxString fromProc, wxString searchString, int offset)
{
	int progIdx = fromProc.Find(searchString);
	if( progIdx != offset &&
		progIdx != -1)
	{
		wxLogDebug("\n-->Supposed User Output:\n");
		wxLogDebug(fromProc.Mid(offset, progIdx));
		wxLogDebug("\n");
	}
}

//checkOutputStream(): function for onOutputEvent, checks stream for common
//  stream features... like "Program received" or "Breakpoint..."
bool Debugger::checkOutputStream(wxString stream)
{
	wxRegEx reCase1 = "at (([[:alnum:]]|[[:blank:]]|\\.)+):([[:digit:]]+)";
	wxString thirdCase = stream;

	wxString prog = "Program ";
	int progIndex = thirdCase.Find(prog);

	if(progIndex != -1)
	{
		thirdCase.Remove(0,progIndex + prog.Len());
		if(thirdCase.Left(8) == "received")
		{
			int newlineIndex = thirdCase.find_first_of("\n");
			thirdCase.Remove(0, newlineIndex);

			//if we've gotten here, the program has received a message of some type.
			//either way, not good for further parsing.
			if(reCase1.Matches(thirdCase))
			{
				error = "Received bad program signal.  RegEx: " + reCase1.GetMatch(thirdCase, 1) + ", " + reCase1.GetMatch(thirdCase, 3);
				makeGenericError("checkOutputStream:  ");
				return(false);
			}
			else
			{
				error = "Received bad program signal.  No RegEx.";
				makeGenericError("checkOutputStream:  ");
				return(false);
			}
		}
		else if(thirdCase.Left(6) == "exited")
		{
			thirdCase.Remove(0,7);
			if(thirdCase.Matches("normal*"))
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
			error.Printf("Unknown program signal. ClassStatus %d", classStatus);
			makeGenericError("checkOutputStream: ");
			return(false);
		}
	}

	return(true);
}

void Debugger::onProcessErrOutEvent(ChameleonProcessEvent &e)
{
	status = DEBUG_ERROR;

	//error now has the output
	error = e.GetString();
	addErrorHist("Event-Generated error");
}

void Debugger::onProcessTermEvent(ChameleonProcessEvent &e) 
{
	//the process has been killed (aka GDB quit for some reason)
	status = DEBUG_DEAD;
	classStatus = STOP;
	procLives = false;

	flushBuffer();
	flushPrivateVar();
}

//flushBuffer(): it simply erases the data array.
void Debugger::flushBuffer()
{
	data.Clear();
}

//flushPrivateVar(): erases all private data members
void Debugger::flushPrivateVar()
{
	isRemote = false;
	procLives = false;
	fileIsSet = false;
	currFile = "";
	pid = 0;
	numBreakpoints = 0;
	gdbBreakpointNum = 1;
	lineToNum.empty();

	commandHistory.Empty();
	histCount = 0;
	error = "";
	errorHist.Empty();
	errorCount = 0;
	command = "";
	returnChar = "";
	data.Empty();
}

//sendCommand(): handles sending a command to the stream.
void Debugger::sendCommand(wxString send)
{
	if(procLives)
	{
		streamOut->WriteString(send);
		send = send + "<- sent";
		
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