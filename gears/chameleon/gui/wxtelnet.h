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



#ifndef INCLUDE_WXTELNET
#define INCLUDE_WXTELNET

#include "wxterm.h"
#include <wx/socket.h>

#ifdef __GNUG__
#pragma interface
#endif

#define wxEVT_COMMAND_TELNET_CONNECT           (wxEVT_COMMAND_TERM_NEXT + 1)
#define wxEVT_COMMAND_TELNET_DISCONNECT        (wxEVT_COMMAND_TERM_NEXT + 2)

#define EVT_TELNET_CONNECT(id, fn) { wxEVT_COMMAND_TELNET_CONNECT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &fn, (wxObject *)NULL },
#define EVT_TELNET_DISCONNECT(id, fn) { wxEVT_COMMAND_TELNET_DISCONNECT, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) &fn, (wxObject *)NULL },



class wxTelnet : public wxTerm
{
  int
    m_state,
    m_connected;

  wxSocketClient
    *m_sock;

public:
  wxTelnet(wxWindow* parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
          int width = 80, int height = 25,
          const wxString& name = "wxTelnet");

  virtual ~wxTelnet();

  void OnSocketEvent(wxSocketEvent& event);

  virtual void SendBack(int len, char *data);

  virtual void Connect(const wxString& hostname, unsigned short port);
  virtual void Disconnect(void);
  virtual bool IsConnected(void);

private:
  DECLARE_EVENT_TABLE()
};

#endif /* INCLUDE_WXTELNET */