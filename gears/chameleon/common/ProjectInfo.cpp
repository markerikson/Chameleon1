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

void ProjectInfo::AddFileToProject(wxString filename, FileFilterType fileType)
{
	switch(fileType)
	{
	case FILE_SOURCES:
		m_sourceFiles.Add(filename);
		break;
	case FILE_HEADERS:
		m_headerFiles.Add(filename);
		break;
	case FILE_LIBRARIES:
		m_libraryFiles.Add(filename);
		break;
	default:
		wxLogDebug("Invalid file type in ProjectInfo::AddFile.  Filename: %s, type: %d", filename.c_str(), fileType);
		break;
	}
}

void ProjectInfo::RemoveFileFromProject(wxString filename, FileFilterType fileType)
{
	switch(fileType)
	{
	case FILE_SOURCES:
		m_sourceFiles.Remove(filename);
		break;
	case FILE_HEADERS:
		m_headerFiles.Remove(filename);
		break;
	case FILE_LIBRARIES:
		m_libraryFiles.Remove(filename);
		break;
	default:
		wxLogDebug("Invalid file type in ProjectInfo::RemoveFile.  Filename: %s, type: %d", filename.c_str(), fileType);
		break;
	}
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