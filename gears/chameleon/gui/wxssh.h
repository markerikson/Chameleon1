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



#ifndef __WXSSH__H__
#define __WXSSH__H__

#include "../common/CommonHeaders.h"

#include <wx/txtstrm.h>
#include "../network/gterm.hpp"
#include "../network/gtelnet.hpp"  // <------------
#include "../common/debug.h"
#include "../common/process2.h"
#include "../common/process2events.h"
#include "wxterm.h"
#include "wxssh.h"


class wxSSH : public wxTerm
{
	public:
		wxSSH(wxWindow* parent, wxWindowID id,
				const wxPoint& pos = wxDefaultPosition,
				int width = 80, int height = 25,
				const wxString& name = "wxSSH");
		virtual ~wxSSH();

		void OnPlinkEvent(wxProcess2StdOutEvent& event);

		virtual void SendBack(int len, char *data);

		virtual void Connect(wxString hostname, wxString username, wxString passphrase);
		virtual void Disconnect(void);
		virtual bool IsConnected(void);

	private:
		bool m_connected;
		wxProcess2* m_plink;

	
	DECLARE_EVENT_TABLE()
};

#endif /* __WXSSH__H__ */
