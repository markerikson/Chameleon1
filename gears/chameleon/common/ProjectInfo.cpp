#include "ProjectInfo.h"
#include "debug.h"

bool ProjectInfo::FileExistsInProject(wxString filename)
{
	bool fileInProject = false;

	if(headerFiles.Index(filename) != -1)
	{
		fileInProject = true;
	}
	if(sourceFiles.Index(filename) != -1)
	{
		fileInProject = true;
	}
	if(sourceFiles.Index(filename) != -1)
	{
		fileInProject = true;
	}

	return fileInProject;
}

void ProjectInfo::AddFileToProject(wxString filename, FileFilterType fileType)
{
	switch(fileType)
	{
	case FILE_SOURCEONLY:
		sourceFiles.Add(filename);
		break;
	case FILE_HEADERONLY:
		headerFiles.Add(filename);
		break;
	case FILE_LIBRARY:
		libraryFiles.Add(filename);
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
	case FILE_SOURCEONLY:
		sourceFiles.Remove(filename);
		break;
	case FILE_HEADERONLY:
		headerFiles.Remove(filename);
		break;
	case FILE_LIBRARY:
		libraryFiles.Remove(filename);
		break;
	default:
		wxLogDebug("Invalid file type in ProjectInfo::RemoveFile.  Filename: %s, type: %d", filename.c_str(), fileType);
		break;
	}
}