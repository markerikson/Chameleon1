#ifndef PROJECTINFO_H
#define PROJECTINFO_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include "datastructures.h"

class ProjectInfo
{
public:
	wxArrayString headerFiles;
	wxArrayString sourceFiles;
	wxArrayString libraryFiles;
	bool relativePaths;
	bool isRemote;
	wxString projectBasePath;
	wxString projectName;

	bool FileExistsInProject(wxString filename);
	void AddFileToProject(wxString filename, FileFilterType fileType);
	void RemoveFileFromProject(wxString filename, FileFilterType fileType);
};



#endif