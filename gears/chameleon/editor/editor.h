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
class ProjectInfo;


class ChameleonEditor : public wxStyledTextCtrl
{
public:
	ChameleonEditor(ChameleonWindow* mframe, Options* options, ProjectInfo* project, 
					wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
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
	bool HasBeenSaved();

	void SetCompiled();

	void OnChar(wxStyledTextEvent &event);
	void OnRightClick(wxMouseEvent &event);

	void UpdateSyntaxHighlighting();
	void FocusOnLine(int linenumber);


	wxString GetFileNameAndPath();
	wxString GetFilenameString();
	wxFileName GetFileName(); // capital N
	wxFileName GetExecutableFileName() 	{ return m_executableFilename; }
	wxString GetFilePath();
	wxArrayInt GetBreakpoints();
	ProjectInfo* GetProject(){ return m_project; }


	//void SetFileNameAndPath(wxString path, wxString name, bool fileIsRemote);
	void SetFilename(wxFileName filename, bool fileIsRemote);
	void SetExecutableFilename(wxFileName filename);
	void SetProject(ProjectInfo* project);
	void ResetEditor();
	bool LastSavedRemotely() {	return m_bLastSavedRemotely;}

private:


	void OnEditorModified(wxStyledTextEvent &event);
	void OnAddBreakpoint(wxCommandEvent &event);
	void OnRemoveBreakpoint(wxCommandEvent &event);
	void OnClearBreakpoints(wxCommandEvent &event);
	void OnCompilerEnded(CompilerEvent &event);

	void CreateBreakpointEvent(int linenumber, bool addBreakpoint);
	

	ChameleonWindow* m_mainFrame;
	ChameleonNotebook* m_parentNotebook;	
	Options* m_options;
	ProjectInfo* m_project;

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