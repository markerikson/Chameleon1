/*
    taTelnet - A cross-platform telnet program.
    Copyright (c) 2000 Derry Bryson.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact Information:

       Technology Associates, Inc.
       Attn:  Derry Bryson
       959 W. 5th Street
       Reno, NV  89503
       USA

       derry@techass.com
*/

#include "wxssh.h"

BEGIN_EVENT_TABLE(wxSSH, wxTerm)
	EVT_PROCESS2_STDOUT(wxSSH::OnPlinkOut)
	EVT_PROCESS2_STDERR(wxSSH::OnPlinkErr)
	EVT_PROCESS2_ENDED(wxSSH::OnPlinkTerm)
END_EVENT_TABLE()

wxSSH::wxSSH(wxWindow* parent, wxWindowID id, Networking* network, const wxPoint& pos, int width, int height, const wxString& name)
	: wxTerm(parent, id, pos, width, height, name)
{
	m_connected = false;
	m_networking = network;
	m_plink = NULL;
	m_plinkPid = -2;
	m_inputBuffer = "";
	m_isInESCsequence = false;
}

wxSSH::~wxSSH()
{
	if(m_connected)
	{
		// "No time for pleasantries"
		m_plink->Detach();
		int result = wxProcess::Kill(m_plinkPid, wxSIGKILL);
		wxLogDebug("wxSSH tried to kill m_plink (wxKill Result: %d)", result);
		//delete m_plink; <-- wxProcess2 self-destructs
	}
}

void wxSSH::SendBack(int len, char *data)
{
	if(m_connected) {
		wxTextOutputStream os(* (m_plink->GetOutputStream()) );
		wxString s = "";
		for(int i = 0; i < len; i++) {
			//wxLogDebug("%d ", data[i]);
			s += data[i];
		}
		os.WriteString(s);
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
	m_plink = m_networking->GetPlinkProcess(this);
	if(m_plink != NULL) {
		m_connected = true;
		m_inputBuffer = "";
		m_isInESCsequence = false;
	}
}


void wxSSH::Disconnect()
{
	if(m_connected)
	{
		// Rudimentary exiting
		wxTextOutputStream os(* (m_plink->GetOutputStream()) );
		os.WriteString("\r^c\rexit\r"); // this may be tacking on an extra \r or \n
		//os.WriteString("\rexit\r");
	}
	//m_connected = false; // done when the processterm event is caught
	//m_plinkPid = -2; // ditto

	Reset();
	set_mode_flag(CURSORINVISIBLE);
	GTerm::Update();
	Refresh();
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
void wxSSH::OnPlinkOut(wxProcess2StdOutEvent &e)
{
	wxString s = e.GetOutput();
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

void wxSSH::OnPlinkErr(wxProcess2StdErrEvent &e)
{
	wxString s = e.GetError();
	int len = s.Length();
	ProcessInput(len, (unsigned char*)s.c_str()); // dump errors to the terminal window

	wxLogDebug("Terminal-Error: "+e.GetError());
}

void wxSSH::OnPlinkTerm(wxProcess2EndedEvent &e)
{
	wxLogDebug("Plink Terminated!");
	m_connected = false;
	m_plinkPid = -2;
	//delete m_plink; <--- wxProcess2 self-destructs
}
