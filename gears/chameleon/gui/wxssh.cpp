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

#ifdef __GNUG__
    #pragma implementation "wxssh.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "../common/CommonHeaders.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//#include <ctype.h>

#include <wx/txtstrm.h>
#include "../network/gterm.hpp"
#include "../network/gtelnet.hpp"
#include "../common/process2.h"
#include "../common/process2events.h"
#include "wxterm.h"
#include "wxssh.h"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//wxTerm
BEGIN_EVENT_TABLE(wxSSH, wxTerm)
	EVT_PROCESS2_STDOUT(wxSSH::OnPlinkEvent)
	//EVT_CUSTOM(wxEVT_PROCESS2_STDOUT, wxID_ANY, wxSSH::OnPlinkEvent)
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
		// do something
	}
}

void wxSSH::SendBack(int len, char *data)
{
	wxLogDebug("wxSSH::SendBack called...\n");
	wxLogDebug("wxSSH::SendBack sending: ");
	for(int j = 0; j < len; j++) {
		wxLogDebug("%d ", data[j]);
	}
	//wxLogDebug("\n");
	wxTextOutputStream os(* (m_plink->GetOutputStream()) );
	for(int i = 0; i < len; i++) {
		os.Write8(data[i]);
	}
}

void wxSSH::Connect(const wxString& hostname, unsigned short port)
{
	if(m_connected) {
		Disconnect();
	}

	Reset();
	Refresh();

	// Start the new Process
	m_plink = new wxProcess2(this);
	int pid = wxExecute("plink.exe -pw dayspring danroeber@163.11.160.218", wxEXEC_ASYNC, m_plink);

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
		os.WriteString("exit\r");
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


//void wxSSH::OnPlinkEvent(wxEvent &e)
void wxSSH::OnPlinkEvent(wxProcess2StdOutEvent &e)
{
	unsigned char buf[900]; // <-- fix the constant
	wxString s = e.GetOutput();
	int len = (e.GetOutput()).Length();
	for(int i = 0; i < len; i++) {
		buf[i] = s.GetChar(i);
	}

	ProcessInput(len, buf);
}

