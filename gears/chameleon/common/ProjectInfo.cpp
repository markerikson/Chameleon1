#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "ProjectInfo.h"
#include "../editor/editor.h"
#include "debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ProjectInfo::ProjectInfo(bool singleFile /* = true */)
{
	m_isSingleFile = singleFile;
	m_isCompiled = false;
	m_isReadOnly = false;
	//m_relativePaths = true;
	// isRemote will be set by the creator immediately upon instantiation
	// in fact, it could almost go in the constructor...
	m_isRemote = true;
	m_isBeingCompiled = false;

	if(singleFile) {
		//set some defaults
		//m_headerFiles = wxArrayString;
		//m_sourceFiles = wxArrayString;
		//m_libraryFiles = wxArrayString;
		//m_headersEnabled = BoolArray;
		//m_sourcesEnabled = BoolArray;
		//m_librariesEnabled = BoolArray;
		//m_edPointers = EditorPointerArray;
		m_projectBasePath = "";
		m_projectName = "";
		//m_executableName = wxFileName;
	}
}

bool ProjectInfo::FileExistsInProject(wxString filename)
{
	bool fileInProject = false;

	if(m_headerFiles.Index(filename) != -1)
	{
		fileInProject = true;
	}
	if(m_sourceFiles.Index(filename) != -1)
	{
		fileInProject = true;
	}
	if(m_libraryFiles.Index(filename) != -1)
	{
		fileInProject = true;
	}

	return fileInProject;
}

wxArrayString* ProjectInfo::SelectStringArray(FileFilterType filterType)
{
	switch(filterType)
	{
	case FILE_SOURCES:
		return &m_sourceFiles;
	case FILE_HEADERS:
		return &m_headerFiles;
	case FILE_LIBRARIES:
		return &m_libraryFiles;
	}
	// shouldn't ever get here, just eliminating the "not all code paths
	// return a value" warning
	return NULL;
}

BoolArray* ProjectInfo::SelectBoolArray(FileFilterType filterType)
{
	switch(filterType)
	{
	case FILE_SOURCES:
		return &m_sourcesEnabled;
	case FILE_HEADERS:
		return &m_headersEnabled;
	case FILE_LIBRARIES:
		return &m_librariesEnabled;
	}
	// shouldn't ever get here
	return NULL;
}

void ProjectInfo::AddFileToProject(wxString filename, FileFilterType filterType)
{
	if(FileExistsInProject(filename)) {
		wxLogDebug("Tried adding a file that's already in the file.");
		return;
	}

	wxArrayString* filelist = SelectStringArray(filterType);
	filelist->Add(filename);
	BoolArray* enablelist = SelectBoolArray(filterType);
	enablelist->Add(true);

	//if(m_isSingleFile) {
	if(m_projectName == "" && m_sourceFiles.Count() == 1) {
		wxFileName file(filename);
		m_projectName = file.GetName();
		m_projectBasePath = file.GetPath(wxPATH_GET_VOLUME, m_isRemote ? wxPATH_UNIX : wxPATH_DOS);
	}
}

void ProjectInfo::RemoveFileFromProject(wxString filename, FileFilterType filterType)
{
	wxArrayString* filelist = SelectStringArray(filterType);

	int index = filelist->Index(filename);
	filelist->Remove(filename);

	BoolArray* enablelist = SelectBoolArray(filterType);
	enablelist->RemoveAt(index);
}

bool ProjectInfo::FileIncludedInBuild(wxString filename, FileFilterType filterType)
{
	wxArrayString* filelist = SelectStringArray(filterType);
	int index = filelist->Index(filename);
	
	BoolArray* enablelist = SelectBoolArray(filterType);
	return enablelist->Item(index);
}

void ProjectInfo::SetFileBuildInclusion(wxString filename, FileFilterType filterType, bool enable)
{
	wxArrayString* filelist = SelectStringArray(filterType);
	int index = filelist->Index(filename);

	BoolArray* enablelist = SelectBoolArray(filterType);
	(*enablelist)[index] = enable;
}

wxArrayString ProjectInfo::GetSourcesToBuild()
{
	wxArrayString sourcesToBuild;
	
	for(int i = 0; i < (int)m_sourcesEnabled.GetCount(); i++)
	{
		if(m_sourcesEnabled[i])
		{
			sourcesToBuild.Add(m_sourceFiles[i]);
		}
	}
	
	return sourcesToBuild;
}


void ProjectInfo::AddEditor(ChameleonEditor* edit)
{
	m_edPointers.Add(edit);
}

void ProjectInfo::RemoveEditor(ChameleonEditor* edit)
{
	m_edPointers.Remove(edit);
}

void ProjectInfo::MakeReadOnly(bool makeReadOnly)
{
	m_isReadOnly = makeReadOnly;
	for(int i = 0; i < (int)m_edPointers.GetCount(); i++)
	{
		ChameleonEditor* ed = m_edPointers[i];
		ed->SetReadOnly(makeReadOnly);
	}
}

void ProjectInfo::SetBeingCompiled(bool compiling)
{
	m_isBeingCompiled = compiling;
	MakeReadOnly(compiling);
}
