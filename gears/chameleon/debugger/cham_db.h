/*****************************************************************\
*Wrapper Header file
*
*author: Ben Carhart
*date started: 1/11/04
*date finished:
*
*description: Code for the implementation of a wrapper for GDB
*revisions:
*
\*****************************************************************/

#ifndef CHAMELEON_DEBUGER_H
#define CHAMELEON_DEBUGER_H

#include <wx/string.h>
//#include <wx/process.h>
#include <wx/dynarray.h>
#include <wx/event.h>
#include <wx/txtstrm.h>

#include "../common/process2.h"
#include "../common/process2events.h"
#include "../common/debugevent.h"

//dave code
#include <wx/textctrl.h>

//global declarations
//const int MAX_HIST_ITEMS = 50;
const wxString PROMPT_CHAR = "%";

//keywords for parsing output
enum DEBUG_KEYS {
	START = -1,				//program start
	CONTINUE = 134242,
	E_BREAK,				//enable break
	D_BREAK,				//disable break
	K_BREAK,				//kill break
	S_BREAK,				//set break
	GO,
	JUMP,
	SOFT_RESET,
	HARD_RESET,
	NEW_FILE,				//new file to debug
	NEW_MODE,				//change in program mode
	NEW_PROC,				//change in process pointer
	VAR_ASSIGN,
	WATCH_VAR,
	STEP,
	STEP_OVER,
	STEP_OUT,
	STOP,
	WAITING,	//134260
};

//status of program
enum DEBUG_STATUS {
	DEBUG_RUNNING = 135401,	//program running / command executing
	DEBUG_STOPPED,			//nothing happening right now
	DEBUG_WAIT,				//waiting for instruction
	DEBUG_BREAK,			//program paused (because of breakpoint)
	DEBUG_ERROR,			//problem with debug
	DEBUG_NEED_FILE,		//waiting for a file
	DEBUG_DEAD,				//there AIN'T no debug!
};

class Debugger : public wxEvtHandler
{
	public:
		//defaults
		Debugger(wxTextCtrl* outBox, wxEvtHandler* pointer);
		~Debugger();					//destructor

		void onDebugEvent(wxDebugEvent &event);

		//status modifiers
		void setMode(bool mode);		//switch between LOCAL and REMOTE mode
		bool getMode();					//get current mode
		
		bool setFile(wxString fName);	//sets a file for debugging
		wxString getFile();				//returns current file up for debugging

		int genericStatus();			//returns current debug status
		int programStatus();			//returns EXACT class status

		bool isDebugging();				//returns whether i'm running or not
		bool isPaused();				//returns status == DEBUG_WAIT more or less

		wxString errorMsg();			//returns error message, if any
		bool resetStatus();				//hard reset * USE CAUTIOUSLY *
		void clearError();				//just resets the status variable
		
		void setProcess(bool newIsRemote, wxString newFname, wxString nExec);//sets the current process pointer

		//breakpoint management
		void setBreak(wxString srcFile, int lineNum);		//set breakpoint
		void disableBreak(wxString srcFile, int lineNum);	//disable breakpoint
		void enableBreak(wxString srcFile, int lineNum);	//enable a breakpoint
		void killBreak(wxString srcFile, int lineNum);		//delete breakpoint
		int numBreak();										//return the # of breakpoints

		//step management
		void step();
		void stepOver();				//execute function lines (don't go into)
		void stepOut();					//execute current stack frame to finish
		void jump(int lineNum);			//go to line # and run
		void go();						//run program
		void cont();					//continue debugging
		//CODE IN A RUN-TO-CURSOR function

		//variable management
		void snoopVar(wxString varName, bool oneShot);
		void setVar(wxString varName, wxString newValue);
		void removeVar(wxString varName);

		//misc management
		void stop(bool pleaseRestart);	//kills process & reloads it (?)
		void kill();					//simple?
		//int getCurrLine();				//returns current line number on debug
		//use STEPI ^

	private:
		void startProcess(bool fullRestart, bool mode, wxString fName, wxString execThis, wxTextCtrl* outBox);

		//misc functions i thought would be handy
		void updateHistory(wxString addMe);	//so I don't have to do it manualy
		wxString getHistoryItem(int offset);//ditto
		void flushBuffer();					//flushes the input-data array
		void flushPrivateVar();				//flushes all private variables

		int findBreakpoint(wxString fName, int lineNum, bool andRemove = false);
		bool checkOutputStream(wxString stream);	//true = okay to parse further
 
		void sendCommand(wxString send);	//sends command & updates history
		wxString getResult(int debugFlag);	//gets returned output

		void makeGenericError(wxString comment);
		void addErrorHist(wxString comment);//updates error history

		//ASYNC Event Triggered Functions
		void onProcessOutputEvent(wxProcess2StdOutEvent &e);
		void onProcessErrOutEvent(wxProcess2StdErrEvent &e);
		//void onProcessTermEvent(wxProcessEvent &e);
		void onProcessTermEvent(wxProcess2EndedEvent &e);

		//private elements [grouped according to function]
		bool isRemote;				//stores current mode: false = local
		bool procLives;				//stores status of async process
		bool fileIsSet;				//stores whether a file is loaded
		wxString currFile;			//stores current filename for debugging

		int status;					//status of debugger
		int classStatus;			//holds the last function called
		long pid;					//process ID of GDB (if running)
		int currDebugLine;			//holds line number program is on
		
		int gdbBreakpointNum;		//keeps track of GDB's #ing system
//		int deadBreakpoints;		//counts deleted breakpoints
		int numBreakpoints;			//keeps track of actual number of live ones
//		wxArrayInt breakpointNum;	//hold breakpoint line #'s
		DebugBreakHash lineToNum;
		
		wxArrayString commandHistory;	//stores command history
		int histCount;				//points to current top command
		
		wxString error;				//holds error messages
		wxArrayString errorHist;	//holds ALL error messages ^_^
		int errorCount;				//counts the errors
		
		wxString command;			//holds current / last GDB command
		wxString returnChar;		//contains the \n or \r for each command
		wxArrayString data;			//holds data from GDB

		wxArrayString varNames;		//holds variables being watched
		wxArrayString varValue;		//holds variable values
		wxArrayInt varDispIndex;	//holds variable "display#" assigned by GDB
		int gdbVarIndex;			//holds next GDB display #
		int varCount;				//holds array position to insert next var
		
		wxString firstExecString;	//holds the first executed string
		wxProcess2 *debugProc;		//holds pointer to process pipe
		wxInputStream *streamIn;		//GDB to me
		wxInputStream *streamError;		//problems

		//dave code
		wxTextCtrl* outputScreen;

		//GUI connectivity
		wxEvtHandler* guiPointer;

		DECLARE_EVENT_TABLE()
};

#endif