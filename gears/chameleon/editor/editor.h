#ifndef EDITOR__H
#define EDITOR_H


#include "stc.h"
#include <wx/menu.h>
#include <wx/dynarray.h>
#include "../common/datastructures.h"
#include "../compiler/compilerevent.h"
//#include <wx/wx.h>

class ChameleonWindow;
class ChameleonNotebook;
class wxFileName;
class Options;


class ChameleonEditor : public wxStyledTextCtrl
{
public:
	ChameleonEditor(ChameleonWindow* mframe, Options* options, wxWindow *parent, wxWindowID id, 
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
	bool HasBeenCompiled();

	void SetCompiled();

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
	wxString GetFilenameString();
	wxFileName GetFileName(); // capital N
	wxFileName GetExecutableFileName() 	{ return m_executableFilename; }
	wxString GetFilePath();
	wxArrayInt GetBreakpoints();
	//void SetFileNameAndPath(wxString path, wxString name, bool fileIsRemote);
	void SetFilename(wxFileName filename, bool fileIsRemote);
	void SetExecutableFilename(wxFileName filename){ m_executableFilename = filename; }
	bool HasBeenSaved();
	void ResetEditor();
	bool LastSavedRemotely() {	return m_bLastSavedRemotely;}

private:


	void OnEditorModified(wxStyledTextEvent &event);
	void OnAddBreakpoint(wxCommandEvent &event);
	void OnRemoveBreakpoint(wxCommandEvent &event);
	void OnCompilerEnded(wxCompilerEndedEvent &event);
	

	ChameleonWindow* m_mainFrame;
	ChameleonNotebook* m_parentNotebook;	
	Options* m_options;

	wxFileName m_fileNameAndPath;
	wxFileName m_executableFilename;
	//wxFileName m_remoteFileName;
	//wxFileName m_localFileName;
	wxString m_simpleFileName;
	wxDateTime m_filetime;

	wxArrayInt m_breakpoints;
	wxPoint m_lastRightClick;

	bool m_bLoadingFile;
	bool m_bLastSavedRemotely;
	bool m_bHasBeenCompiled;
	
	wxMenu m_popupMenu;
	
	DECLARE_EVENT_TABLE()
};


//BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)

#endif