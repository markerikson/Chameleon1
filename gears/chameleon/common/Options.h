#ifndef OPTIONS_H
#define OPTIONS_H

#include <wx/wx.h>
#include <wx/validate.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

class Permission;


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

	private:
		wxString m_pscpProg;
		wxString m_plinkProg;
		wxString m_mingwPath; // path only? (may be a good idea)

		wxString m_username;
		wxString m_hostname;
		wxString m_password;

		wxString m_remoteCompileOut;
		wxString m_localCompileOut;

		Permission* m_perms;
};



#endif