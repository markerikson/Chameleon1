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
#include "../network/networking.h"

BEGIN_EVENT_TABLE(wxSSH, wxTerm)
	EVT_PROCESS2_STDOUT(wxSSH::OnPlinkOut)
	EVT_PROCESS2_STDERR(wxSSH::OnPlinkErr)
	EVT_PROCESS2_ENDED(wxSSH::OnPlinkTerm)
END_EVENT_TABLE()

wxSSH::wxSSH(wxWindow* parent, wxWindowID id, const wxPoint& pos, int width, int height, const wxString& name)
	: wxTerm(parent, id, pos, width, height, name)
{
  m_connected = false;
  m_plink = NULL;
  m_host = "";
  m_user = "";
  m_plinkPid = -2;
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
	wxString cmd = m_networking->GetPlinkProg();
	if(passphrase != "") {
		cmd += " -pw "+passphrase;
	}
	if(username != "") {
		cmd += " " + username + "@" + hostname;
	}
	else
	{
		cmd += " " + hostname;
	}
	
	m_plink = new wxProcess2(this);
	m_plinkPid = wxExecute(cmd, wxEXEC_ASYNC, m_plink);

	if(m_plinkPid == 0) {
		//Command could not be executed
		wxLogDebug("Could not start process.");
		delete m_plink;
	}
	else if (m_plinkPid == -1) {
		// BAD ERROR!  User ought to upgrade their operating system
		// User has DDE running under windows (OLE deprecated this)
		wxLogDebug("Could not start process.");
		delete m_plink;
	}
	else { // Process is Live!
		m_host = hostname;
		m_user = username;
		m_connected = true;
		m_plink->SetPID(m_plinkPid); // Temporary solution
		wxLogDebug("Process started successfully!");
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


void wxSSH::OnPlinkOut(wxProcess2StdOutEvent &e)
{
	wxString s = e.GetOutput();
	int len = s.Length();
	ProcessInput(len, (unsigned char*)s.c_str());
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

void wxSSH::SetNetworking(Networking* networking)
{
	m_networking = networking;
}