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
    #pragma implementation "wxtelnet.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/socket.h>

#include <ctype.h>

#include "../network/gterm.hpp"
#include "../network/gtelnet.hpp"
#include "wxterm.h"
#include "wxtelnet.h"


#define ID_SOCKET        1000

BEGIN_EVENT_TABLE(wxTelnet, wxTerm)
    EVT_SOCKET(ID_SOCKET, wxTelnet::OnSocketEvent)
END_EVENT_TABLE()

wxTelnet::wxTelnet(wxWindow* parent, wxWindowID id,
               const wxPoint& pos,
               int width, int height,
               const wxString& name) :
  wxTerm(parent, id, pos, width, height, name)
{
  m_sock = 0;
  m_connected = 0;
}

wxTelnet::~wxTelnet()
{
  if(m_sock)
  {
    m_sock->Close();
    delete m_sock;
  }
}

void
wxTelnet::OnSocketEvent(wxSocketEvent& event)
{
#define BUFSIZE        900
  int
    done = 0,
    bytesRead;

  unsigned char
    buf[BUFSIZE];

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT :
//printf("got read event...\n");
#if 0
      if(m_connected)
      {
#endif
        while(!done)
        {
//printf("reading %d bytes...\n", BUFSIZE);
          m_sock->Read((char *)buf, BUFSIZE);
#if 0
          if(m_sock->Error());
{
//fprintf(stderr, "wxTelnet::OnSocketEvent got error %d!\n", m_sock->LastError());
            done = 1;
}
#else
          if((bytesRead = m_sock->LastCount()) != BUFSIZE)
            done = 1;
//printf("bytesRead = %d\n", bytesRead);
#endif
//fprintf(stderr, "wxTelnet::OnSocketEvent got %d\n", buf[0]);

          ProcessInput(bytesRead, buf);
        }
#if 0
      }
	  else
	    wxLogMessage("got input event before connected!");
#endif
    break;

    case wxSOCKET_LOST :
    {
//printf("got lost event...\n");
      m_connected = 0;
      wxCommandEvent e(wxEVT_COMMAND_TELNET_DISCONNECT, GetId());
      e.SetEventObject(this);
      GetParent()->GetEventHandler()->ProcessEvent(e);
      m_sock->Close();
      delete m_sock;
      m_sock = 0;
    }
    break;

    case wxSOCKET_CONNECTION :
    {
//printf("got connection event...\n");
      m_connected = 1;
//      SendWill(0);
//      SendDo(0);
      wxCommandEvent e(wxEVT_COMMAND_TELNET_CONNECT, GetId());
      e.SetEventObject(this);
      GetParent()->GetEventHandler()->ProcessEvent(e);
    }
    break;

    default :
//printf("got unknown event...\n");
    break;
  }
}

void
wxTelnet::SendBack(int len, char *data)
{
//printf("wxTelnet::SendBack called...\n");
//printf("wxTelnet::SendBack sending: ");
//for(int i = 0; i < len; i++)
//  printf("%d ", data[i]);
//printf("\n");
  if(m_sock)
    m_sock->Write((char *)data, len);
}

void
wxTelnet::Connect(const wxString& hostname, unsigned short port)
{
  wxIPV4address
    addr;

  if(m_connected)
    Disconnect();

  Reset();
  Refresh();
  addr.Hostname(hostname);
  addr.Service(port);

  m_sock = new wxSocketClient();
  m_sock->SetFlags(wxSOCKET_NOWAIT);
  m_sock->SetEventHandler(*this, ID_SOCKET);
  m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
  m_sock->Notify(TRUE);

  m_sock->Connect(addr, FALSE);
}

void
wxTelnet::Disconnect(void)
{
  if(m_sock)
  {
    m_sock->Close();
    delete m_sock;
    m_sock = 0;
  }
  m_connected = 0;

  Reset();
  Refresh();
}

bool
wxTelnet::IsConnected(void)
{
  if(m_sock)
    return m_sock->IsConnected();
  return FALSE;
}
