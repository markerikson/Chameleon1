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
	EVT_PROCESS2_STDOUT(wxSSH::OnPlinkEvent)
END_EVENT_TABLE()

wxSSH::wxSSH(wxWindow* parent, wxWindowID id, const wxPoint& pos, int width, int height, const wxString& name)
	: wxTerm(parent, id, pos, width, height, name)
{
  m_connected = false;
  m_plink = NULL;
}

wxSSH::~wxSSH()
{
	if(m_plink)
	{
		// Rudimentary exiting
		wxTextOutputStream os(* (m_plink->GetOutputStream()) );
		os.WriteString("\rexit\r"); // this may be tacking on an extra \r or \n
	}
}

void wxSSH::SendBack(int len, char *data)
{
	if(m_connected) {
		wxLogDebug("wxSSH::SendBack called...\n");
		wxLogDebug("wxSSH::SendBack sending: ");
		for(int j = 0; j < len; j++) {
			wxLogDebug("%d ", data[j]);
		}
		//wxLogDebug("\n");
		wxTextOutputStream os(* (m_plink->GetOutputStream()) );
		wxString s = "";
		for(int i = 0; i < len; i++) {
			s += data[i];
		}
		os.WriteString(s); // this may be tacking on an extra \r or \n
	}
}

void wxSSH::Connect(wxString hostname, wxString username, wxString passphrase)
{
	if(m_connected) {
		Disconnect();
	}

	Reset();
	Refresh();

	// Start the new Process
	wxString cmd = "plink.exe -pw "+passphrase+" "+username+"@"+hostname;
	m_plink = new wxProcess2(this);
	int pid = wxExecute(cmd, wxEXEC_ASYNC, m_plink);

	if(pid == 0) {
		//Command could not be executed
		wxLogDebug("Could not start process.");
		delete m_plink;
	}
	else if (pid == -1) {
		// BAD ERROR!  User ought to upgrade their operating system
		// User has DDE running under windows (OLE deprecated this)
		wxLogDebug("Could not start process.");
		delete m_plink;
	}
	else { // Process is Live!
		m_connected = true;
		wxLogDebug("Process started successfully!");
	}
}


void wxSSH::Disconnect()
{
	if(m_plink)
	{
		wxTextOutputStream os(* (m_plink->GetOutputStream()) );
		os.WriteString("\rexit\r");
//		delete m_plink;
	}
	m_connected = false;

	Reset();
	Refresh();
}


bool wxSSH::IsConnected(void)
{
	return m_connected;
}


void wxSSH::OnPlinkEvent(wxProcess2StdOutEvent &e)
{
	// ProcessInput needs a const unsigned char* -- so create one
	unsigned char buf[9000]; // <-- fix the constant
	wxString s = e.GetOutput();
	int len = s.Length();
	for(int i = 0; i < len; i++) {
		buf[i] = s.GetChar(i);
	}

	ProcessInput(len, buf);
}

