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
}

wxSSH::~wxSSH()
{
	if(m_connected)
	{
		// "No time for pleasantries"
		m_networking->ForceKillProcess(m_plinkStdIn);
		wxUsleep(250);
		wxLogDebug("wxSSH tried to kill m_plink.");
	}
}

void wxSSH::SendBack(int len, char *data)
{
	if(m_connected) {
		wxString s = "";
		for(int i = 0; i < len; i++) {
			//wxLogDebug("%d ", data[i]);
			s += data[i];
		}
		m_plinkStdIn->WriteString(s);
	}
}

void wxSSH::Connect(wxString hostname, wxString username, wxString passphrase)
{
	if(m_connected) {
		Disconnect(); // might this be a problem because it's Asynchronous?  Is wxProcess2 this independant?
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


void wxSSH::Disconnect()
{
	if(m_connected)
	{
		m_networking->ForceKillProcess(m_plinkStdIn);
		m_connected = false;

		GTerm::Reset();
		set_mode_flag(CURSORINVISIBLE);
		GTerm::Update();
		Refresh();
	}
}


bool wxSSH::IsConnected(void)
{
	return m_connected;
}

// wxSSH will catch XTerm specific escape sequences (coming from the server)
// The following sequences are useful in this respect:
//    * ESC]0;stringBEL -- Set icon name and window title to string
//    * ESC]1;stringBEL -- Set icon name to string
//    * ESC]2;stringBEL -- Set window title to string
//
//(where ESC is the escape character (\033), and BEL is the bell character (\007).)
//
// Currently the plan is just to surpress them because they aren't relevant to Chameleon
//wxSSH should override the input mechanism, and when any of the xterm escape sequences are issued it should eat them(into a buffer).  If a max STRING length is reached with out getting BEL the buffer can be 're-issued'.
//**Note: if a vt52 printscreen is sent, the screen will be blank until MAX characters are sent
void wxSSH::OnPlinkOut(ChameleonProcessEvent &e)
{
	wxString s = e.GetString();
	int len = s.Length();

	if(!m_isInESCsequence && !s.Contains((char)27)) {
		// This test is provided to 'short-circuit' the logic below
		ProcessInput(len, (unsigned char*)s.c_str());
		return; // multiple return statements are confusing (sorry)
	}


	ProcessInput(len, (unsigned char*)s.c_str());
	return;
/*
	int escPos = s.Find((char)27);
	if(escPos != -1) {	//if contains ESC
		if(m_isInESCsequence) {
			m_inputBuffer += s.Remove(0,escPos);

			// determine the fate of the buffer

			ProcessInput(s.Length(), (unsigned char*)s.c_str());
		}
		else { // begining
			ProcessInput(escPos, (unsigned char*)s.Remove(0,escPos).c_str());
		}

	}
	else {
		//continue adding to the buffer
	}
*/
}

void wxSSH::OnPlinkErr(ChameleonProcessEvent &e)
{
	//wxString s = e.GetString();
	//int len = s.Length();
	//ProcessInput(len, (unsigned char*)s.c_str()); // dump errors to the terminal window

	wxLogDebug("Terminal-StdError: "+e.GetString());
}

void wxSSH::OnPlinkTerm(ChameleonProcessEvent &e)
{
	wxLogDebug("Terminal's Plink Terminated!");

	m_connected = false;
}
