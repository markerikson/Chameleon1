#ifndef OPTIONS_H
#define OPTIONS_H

#include <wx/wx.h>
#include <wx/validate.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

class Permission;

// copied from stc.h
// PrintColourMode - force black text on white background for printing.
#define wxSTC_PRINT_BLACKONWHITE 2

// PrintColourMode - text stays coloured, but all background is forced to be white for printing.
#define wxSTC_PRINT_COLOURONWHITE 3


class Options
{
	friend class OptionsDialog;
	friend class wxValidator;
	friend class wxTextValidator;
	friend class wxGenericValidator;
	public:
		Options();
		~Options();

		// Modifiers:
		bool SetPscpApp(wxString path_and_prog);
		bool SetPlinkApp(wxString path_and_prog);
		bool SetMingwPath(wxString path);
		bool SetUsername(wxString user);
		bool SetHostname(wxString host);
		bool SetPassphrase(wxString pass);
		bool SetRemoteCompileOut(wxString path_and_file);
		bool SetLocalCompileOut(wxString path_and_file);
		void SetPrintStyle(int style);
		void SetShowToolbarText(bool useText);
		void SetTerminalHistorySize(int size);

		// Accessors:  (inlined)
		wxString GetPscpApp() { return m_pscpProg; }
		wxString GetPlinkApp() { return m_plinkProg; }
		wxString GetMingwPath() { return m_mingwPath; }
		wxString GetUsername() { return m_username; }
		wxString GetHostname() { return m_hostname; }
		wxString GetPassphrase() { return m_password; }
		wxString GetRemoteCompileOut() { return m_remoteCompileOut; }
		wxString GetLocalCompileOut() { return m_localCompileOut; }
		Permission* GetPerms() { return m_perms; }
		int GetPrintStyle() { return m_printStyle; }
		bool GetShowToolbarText() { return m_showToolbarText; }
		int GetTerminalHistorySize() { return m_terminalSize; }

	private:
		wxString m_pscpProg;
		wxString m_plinkProg;
		wxString m_mingwPath; // path only? (may be a good idea)

		wxString m_username;
		wxString m_hostname;
		wxString m_password;

		wxString m_remoteCompileOut;
		wxString m_localCompileOut;

		int m_printStyle;
		int m_terminalSize;

		bool m_showToolbarText;

		Permission* m_perms;
};



#endif