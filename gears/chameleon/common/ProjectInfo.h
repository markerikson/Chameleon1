#ifndef PROJECTINFO_H
#define PROJECTINFO_H

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/dynarray.h>
#include <wx/treectrl.h>
#include "datastructures.h"

class ChameleonEditor;

class ProjectInfo
{

public:
	ProjectInfo(bool singleFile = true);

	bool FileExistsInProject(wxString filename);
	void AddFileToProject(wxString filename, FileFilterType fileType);
	void RemoveFileFromProject(wxString filename, FileFilterType fileType);

	bool FileIncludedInBuild(wxString filename, FileFilterType filterType);
	void SetFileBuildInclusion(wxString filename, FileFilterType filterType, bool enable);

	void AddEditor(ChameleonEditor* edit);
	void RemoveEditor(ChameleonEditor* edit);

	wxArrayString GetSourcesToBuild();

	wxArrayString GetHeaders() { return m_headerFiles; }
	wxArrayString GetSources() { return m_sourceFiles; }
	wxArrayString GetLibraries() { return m_libraryFiles; }
	EditorPointerArray GetEditors() { return m_edPointers; }
	bool IsRemote() { return m_isRemote; }
	bool IsSingleFile() { return m_isSingleFile; }
	bool IsCompiled() { return m_isCompiled; }
	bool IsBeingCompiled() { return m_isBeingCompiled; }
	bool IsReadOnly() { return m_isReadOnly; }
	wxString GetExecutableFileName() { return m_executableName.GetFullPath(wxPATH_UNIX); }
	wxString GetProjectName() { return m_projectName; }
	wxString GetProjectBasePath() { return m_projectBasePath; }

	void SetHeaders(wxArrayString headers) {m_headerFiles = headers; }
	void SetSources(wxArrayString sources) {m_sourceFiles = sources; }
	void SetLibraries(wxArrayString libraries) {m_libraryFiles = libraries; }
	void SetRemote(bool remote) {m_isRemote = remote; }
	void SetSingleFile(bool singleFile) { m_isSingleFile = singleFile; }
	void SetCompiled(bool compiled) { m_isCompiled = compiled; }
	void SetBeingCompiled(bool compiling);
	void SetProjectPath(wxString path) {m_projectBasePath = path; }
	void SetProjectName(wxString projname) {m_projectName = projname; }
	void SetExecutableName(wxFileName filename) {m_executableName = filename; }
private:

	wxArrayString* SelectStringArray(FileFilterType filterType);
	BoolArray* SelectBoolArray(FileFilterType filterType);
	void MakeReadOnly(bool makeReadOnly);
	
	wxArrayString m_headerFiles;
	wxArrayString m_sourceFiles;
	wxArrayString m_libraryFiles;

	BoolArray m_headersEnabled;
	BoolArray m_sourcesEnabled;
	BoolArray m_librariesEnabled;

	EditorPointerArray m_edPointers;

	//bool m_relativePaths;
	bool m_isRemote;
	bool m_isSingleFile;
	bool m_isCompiled;
	bool m_isBeingCompiled;
	bool m_isReadOnly;

	wxString m_projectBasePath;
	wxString m_projectName;
	wxFileName m_executableName;

	
};
#endif