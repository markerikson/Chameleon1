//////////////////////////////////////
//
//    Based on Derry Bryson's taTelnet, Copyright 2000
//
/////////////////////////////////////
#include "wxssh.h"
#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_EVENT_TABLE(wxSSH, wxTerm)
	EVT_PROCESS_STDOUT(wxSSH::OnPlinkOut)
	EVT_PROCESS_STDERR(wxSSH::OnPlinkErr)
	EVT_PROCESS_ENDED(wxSSH::OnPlinkTerm)
END_EVENT_TABLE()

wxSSH::wxSSH(wxWindow* parent, wxWindowID id, Networking* network, const wxPoint& pos, int width, int height, const wxString& name)
	: wxTerm(parent, id, pos, width, height, name)
{
	m_connected = false;
	m_networking = network;
	m_plinkStdIn = NULL;
	m_inputBuffer = "";
	m_isInESCsequence = false;
	m_startingDebugConnection = false;
}

wxSSH::~wxSSH()
{
	if(m_connected)
	{
		// "No time for pleasantries"
		m_networking->ForceKillProcess(m_plinkStdIn);
		wxUsleep(250);
		wxLogDebug("wxSSH force killed m_plink.");
	}
}

void wxSSH::SendBack(int len, char *data)
{
	if(m_connected) {
		wxString s(data, len); // = "";
		m_plinkStdIn->WriteString(s);
	}
}

void wxSSH::Connect()
{
	m_startingDebugConnection = false;

	if(m_connected) {
		Disconnect(); // might this be a problem because it's Asynchronous?
	}

	Reset();
	set_mode_flag(CURSORINVISIBLE);
	Refresh();

	// Start the new Process
	m_plinkStdIn = m_networking->StartRemoteCommand("bash", this);
	if(m_plinkStdIn != NULL) {
		m_connected = true;
		m_inputBuffer = "";
		m_isInESCsequence = false;
	}
}


wxString wxSSH::ConnectForDebug()
{
	m_startingDebugConnection = true;

	if(m_connected) {
		wxLogDebug("Bad, Bad.  Tried starting as a debug terminal while already connected.");
		Disconnect();
	}

	Reset();
	set_mode_flag(CURSORINVISIBLE);
	Refresh();

	// Start the new Process
	m_plinkStdIn = m_networking->StartRemoteCommand("who am i && sleep 24h || echo \"CHdEBUGGER-CONNECT\"", this);
	if(m_plinkStdIn != NULL) {
		m_connected = true;
		m_inputBuffer = "";
		m_isInESCsequence = false;
	}

	// Synchronous:
	wxRegEx reParseTTY(".+?\\s+(.+?)\\s+\\w{3}\\s+\\d+\\s+\\d+:\\d+\\s", wxRE_ADVANCED);
	while(!reParseTTY.Matches(m_inputBuffer) && 
		!m_inputBuffer.Contains("CHdEBUGGER-CONNECT")) {
		wxSafeYield(); // yes, mark probably won't like this, but darn it's simple, and works
	}

	wxString tty = reParseTTY.GetMatch(m_inputBuffer, 1); // empty string is return if it fails

	m_inputBuffer = "";
	m_startingDebugConnection = false;

	return tty;
}


void wxSSH::Disconnect(bool clearDisplay)
{
	if(m_connected)
	{
		m_networking->ForceKillProcess(m_plinkStdIn);
		
		// Not sure if this is needed or not... if we've got problems on exit, we'll come back here
		//wxUsleep(250);
		m_connected = false;

		if(clearDisplay) {
			GTerm::Reset();
			set_mode_flag(CURSORINVISIBLE);
			GTerm::Update();
			Refresh();
		}
		else {
			set_mode_flag(CURSORINVISIBLE);
		}
	}
}


bool wxSSH::IsConnected(void)
{
	return m_connected;
}

// wxSSH should catch XTerm specific escape sequences (coming from the server)
// The following sequences are useful in this respect:
//    * ESC]0;stringBEL -- Set icon name and window title to string
//    * ESC]1;stringBEL -- Set icon name to string
//    * ESC]2;stringBEL -- Set window title to string
//
//(where ESC is the escape character (\027), and BEL is the bell character (\007).)
//
// Currently the plan is just to surpress them because they aren't relevant to Chameleon
//wxSSH should override the input mechanism, and when any of the xterm escape sequences are issued it should eat them(into a buffer).  If a max STRING length is reached with out getting BEL the buffer can be 're-issued'.
// **Note: if a vt52 printscreen is sent, the screen will be blank until MAX characters are sent
void wxSSH::OnPlinkOut(ChameleonProcessEvent &e)
{
	wxString s = e.GetString();
	int len = s.Length();

	wxString startEscSeq0 = wxString((char)27) + "]0;";
	//wxString startEscSeq1 = wxEmptyString + (char)27 + "1;";
	//wxString startEscSeq2 = wxEmptyString + (char)27 + "2;";

	int start = s.Find(startEscSeq0);
	if(start != -1) {
		int end = s.Find((char)7);
		if(end > start) {
			s.Remove(start, end-start);
			len -= (end-start);
		}
	}

	if(!m_isInESCsequence && !m_startingDebugConnection) { // && !s.Contains((char)27)) {
		ProcessInput(len, (unsigned char*)s.c_str());
	}
	else {
		m_inputBuffer += s;
	}

	return;
}

void wxSSH::OnPlinkErr(ChameleonProcessEvent &e)
{
	wxLogDebug("Terminal-StdError: "+e.GetString());
}

void wxSSH::OnPlinkTerm(ChameleonProcessEvent &e)
{
	wxLogDebug("Terminal's External Process Terminated!");

	m_connected = false;
}
