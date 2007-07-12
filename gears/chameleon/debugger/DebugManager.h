#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/event.h>
#include <wx/txtstrm.h>

#include "cham_db.h"
#include "../common/chameleonprocessevent.h"
#include "../common/debugevent.h"
#include "../network/networking.h"
#include "../common/datastructures.h"

class wxSSH;
class ChameleonProjectManager;
class ChameleonWindow;
class VariableWatchPanel;

class DebugManager : public wxEvtHandler
{
public:
	DebugManager(Debugger* debugger, wxSSH* debugTerminal, ChameleonProjectManager* projectManager,
					ChameleonWindow* mainFrame, VariableWatchPanel* watchPanel);

	void OnDebugCommand(int eventID, ProjectInfo* project, ChameleonEditor* currentEditor);

	void OnDebugEvent(wxDebugEvent &event);

	bool IsDebugging() { return m_debugger->isDebugging(); }
	bool IsDebuggerPaused() { return m_debugger->isPaused(); }

private:
	ChameleonProjectManager* m_projectManager;
	Debugger* m_debugger;
	wxSSH* m_debugTerminal;
	ChameleonWindow* m_mainFrame;
	VariableWatchPanel* m_watchPanel;


	DECLARE_EVENT_TABLE()
};



#endif