//////////////////////////////////////
//
//    Based on Derry Bryson's taTelnet, Copyright 2000
//
/////////////////////////////////////
#ifndef __WXSSH__H__
#define __WXSSH__H__

#include "../common/CommonHeaders.h"

#include <wx/txtstrm.h>
#include "../network/networking.h"
#include "../network/gterm.hpp"
#include "../network/gtelnet.hpp"  // <------------
#include "../common/debug.h"
#include "../common/chameleonprocessevent.h"
#include "../common/Options.h"
#include "wxterm.h"

#define MAX_INPUT_BUF_LEN 50


class wxSSH : public wxTerm
{
	public:
		wxSSH(wxWindow* parent, wxWindowID id,
				Networking* network,
				const wxPoint& pos = wxDefaultPosition,
				int width = 80, int height = 25,
				const wxString& name = "wxSSH");
		virtual ~wxSSH();

		void OnPlinkOut(ChameleonProcessEvent& event);
		void OnPlinkErr(ChameleonProcessEvent& event);
		void OnPlinkTerm(ChameleonProcessEvent& event);

		virtual void SendBack(int len, char *data);

		virtual void Connect(wxString hostname, wxString username, wxString passphrase);
		virtual void Disconnect(void);
		virtual bool IsConnected(void);

	private:
		bool m_connected;
		wxTextOutputStream* m_plinkStdIn;
		Networking* m_networking;
		//wxString m_host;
		//wxString m_user;
		long m_plinkPid;
		wxString m_inputBuffer;
		bool m_isInESCsequence;

	
	DECLARE_EVENT_TABLE()
};

#endif /* __WXSSH__H__ */
