#ifndef EDITOR__H
#define EDITOR_H


#include "stc.h"
#include <wx/menu.h>
#include <wx/dynarray.h>
//#include <wx/wx.h>

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


	//bool LoadFile ();
	bool LoadLocalFile (const wxString &filename);
	bool LoadFileText(wxString fileContents);
	//bool SaveFileAs();
	bool SaveFile(const wxString& filename);
	bool SaveFileLocal();

	bool Modified();

	//void SetTabUnmodified();

	void OnChar(wxStyledTextEvent &event);
	void OnRightClick(wxMouseEvent &event);

	void UpdateSyntaxHighlighting();

	//wxString GetRemoteFileName() { return m_remoteFileName.GetFullName();}
	//wxString GetRemotePath() { return m_remoteFileName.GetPath(false, wxPATH_UNIX);}
	//wxString GetRemoteFileNameAndPath() { return m_remoteFileName.GetFullPath(wxPATH_UNIX);}
	//void SetRemoteFileNameAndPath(wxString path, wxString name);
	//void SetLocalFileNameAndPath(wxString path, wxString name);



	//wxString GetFilename () {return m_simpleFileName;};
	//void SetFilename (const wxString &filename) {m_simpleFileName =  filename;};

	wxString GetFileNameAndPath();
	wxString GetFilename();
	wxString GetFilePath();
	void SetFileNameAndPath(wxString path, wxString name, bool fileIsRemote);
	bool HasBeenSaved();
	void ResetEditor();
	bool LastSavedRemotely() 
	{
		return m_bLastSavedRemotely;
	}

private:


	//void OnSetTabModified(wxStyledTextEvent &event);
	

	ChameleonWindow* m_mainFrame;
	ChameleonNotebook* m_parentNotebook;	

	wxFileName m_fileNameAndPath;
	//wxFileName m_remoteFileName;
	//wxFileName m_localFileName;
	wxString m_simpleFileName;
	wxDateTime m_filetime;

	wxArrayInt m_breakpoints;

	bool m_bLoadingFile;
	bool m_bLastSavedRemotely;
	
	wxMenu m_popupMenu;
	
	DECLARE_EVENT_TABLE()
};


//BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)

#endif