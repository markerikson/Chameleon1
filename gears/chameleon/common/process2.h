///////////////////////////////////////////////////////////
//
// Notes:
//   * wxProcess2 self-destructs. (I delete myself upon process term)
//   * When you first create a wxProcess2, and call wxExecute with it,
//       you need to call .SetPID(long); to set it's Process ID.
//        **This is a temporary solution (I hope)
//   * Don't use this class if you don't have streams enabled.  This
//      class would be pointless otherwise.
//   * "we don't have any way to be notified when any input appears on
//       the stream so we have to poll it" - wxWindows: exec.cpp
//     That is the approach I take, at a constant interval I check for
//       new stdout/stderr.
//
//
// TODO:
//   * What if I don't get a timer??  Similar to :
//      "I'm using the boolean pauseTimer instead of actually starting
//       nd stopping the timer.  Stopping the timer would prevent
//       TimerEvents from constantly happening (aka. more efficient).
//       But, wxWin documentation seems to indicate Windows timers
//       are limited in number, so, if I get a timer allocated to
//       me - keep it!"
//
//
///////////////////////////////////////////////////////////
#ifndef __WXPROCESS2__H__
#define __WXPROCESS2__H__

#include <wx/process.h>
#include <wx/event.h>
#include <wx/timer.h>
#include "../common/debug.h"
#include "process2events.h"

#define POLL_RATE 10 //milliseconds

class wxProcess2 : public wxProcess
{
	public:
		wxProcess2(wxEvtHandler *parent = (wxEvtHandler *) NULL, int id = -1);
		wxProcess2(int flags);
		~wxProcess2();

		virtual void Detach(); // Call this when done with a live process

	private:
		void OnTimerEvent(wxTimerEvent &e);
		void OnTerminate(int pid, int status);
		void PollOutputs();

	protected:
		wxTimer m_timer;

	//I think the following should be moved into wxProcess:
	public:
		void SetPID(long pid);
		long GetPID() { return m_pid; }
	private:
		long m_pid;


	DECLARE_EVENT_TABLE()
};

#endif // __WXPROCESS2__H__