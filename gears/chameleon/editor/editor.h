#ifndef EDITOR__H
#define EDITOR_H

#include "stc.h"
#include "../common/datastructures.h"
#include "../common/debug.h"



#include <wx/datetime.h>


class ChameleonWindow;
class ChameleonNotebook;

class ChameleonEditor : public wxStyledTextCtrl
{
public:
	ChameleonEditor(ChameleonWindow* mframe, wxWindow *parent, wxWindowID id, 
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = 0,
					const wxString& name = wxSTCNameStr);
	~ChameleonEditor();


	bool LoadFile ();
	bool LoadLocalFile (const wxString &filename);
	bool LoadFileText(wxString fileContents);
	bool SaveFileAs();
	bool SaveFile(const wxString& filename);
	bool SaveFile();

	bool Modified();

	void SetTabUnmodified();

	void OnChar(wxStyledTextEvent &event);

	void UpdateSyntaxHighlighting();

	wxString GetRemoteFileName() { return m_remoteFileName.GetFullName();}
	wxString GetRemotePath() { return m_remoteFileName.GetPath(false, wxPATH_UNIX);}
	wxString GetRemoteFileNameAndPath() { return m_remoteFileName.GetFullPath(wxPATH_UNIX);}
	void SetRemoteFileNameAndPath(wxString path, wxString name);
	void SetLocalFileNameAndPath(wxString path, wxString name);



	wxString GetFilename () {return m_simpleFileName;};
	void SetFilename (const wxString &filename) {m_simpleFileName =  filename;};
	bool HasBeenSaved();

private:


	void OnSetTabModified(wxStyledTextEvent &event);
	

	ChameleonWindow* m_mainFrame;
	ChameleonNotebook* m_parentNotebook;	

	wxFileName m_remoteFileName;
	wxFileName m_localFileName;
	wxString m_simpleFileName;
	wxDateTime m_filetime;

	bool m_bLoadingFile;
	
	
	DECLARE_EVENT_TABLE()
};


//BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)

#endif