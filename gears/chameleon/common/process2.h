///////////////////////////////////////////////////////////
//
// Note:
//   * wxProcess2 self-destructs. I delete myself upon process term
//   * Don't use this class if you don't have streams enabled.  This
//      class would be pointless otherwise.
//   * "we don't have any way to be notified when any input appears on
//       the stream so we have to poll it" - wxWindows: exec.cpp
//     That is the approach I take, at a constant interval I check for
//       new stdout/stderr.
//
//
// TODO:
//   * Set the pid of the event.
//   * What if I don't get a timer??  Similar to :
//      "I'm using the boolean pauseTimer instead of actually starting
//       nd stopping the timer.  Stopping the timer would prevent
//       TimerEvents from constantly happening (aka. more efficient).
//       But, wxWin documentation seems to indicate Windows timers
//       are limited in number, so, if I get a timer allocated to
//       me - keep it!"
//
///////////////////////////////////////////////////////////
#ifndef __WXPROCESS2__H__
#define __WXPROCESS2__H__

#include <wx/process.h>
#include <wx/event.h>
#include <wx/timer.h>
#include "process2events.h"

#define POLL_RATE 10 //milliseconds

class wxProcess2 : public wxProcess
{
	public:
		wxProcess2(wxEvtHandler *parent = (wxEvtHandler *) NULL, int id = -1);
		wxProcess2(int flags);
		~wxProcess2();

	private:
		void OnTimerEvent(wxTimerEvent &e);
		void OnTerminate(int pid, int status);
		void PollOutputs();

	protected:
		wxTimer m_timer;


	DECLARE_EVENT_TABLE()
};

#endif // __WXPROCESS2__H__