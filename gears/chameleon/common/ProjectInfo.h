#ifndef PROJECTINFO_H
#define PROJECTINFO_H

#include <wx/string.h>
#include <wx/filename.h>
#include <wx/dynarray.h>
#include "datastructures.h"

class ProjectInfo
{

public:
	ProjectInfo(bool singleFile = true);
	bool FileExistsInProject(wxString filename);
	void AddFileToProject(wxString filename, FileFilterType fileType);
	void RemoveFileFromProject(wxString filename, FileFilterType fileType);

	wxString GetExecutableFileName() { return executableName.GetFullPath(wxPATH_UNIX); }

	//void SetHeaderFiles(wxArrayString headers);
	//void Set
//private:
	wxArrayString headerFiles;
	wxArrayString sourceFiles;
	wxArrayString libraryFiles;

	EditorPointerArray edPointers;

	bool relativePaths;
	bool isRemote;
	bool isSingleFile;
	bool isCompiled;
	bool isBeingCompiled;
	bool isReadOnly;

	wxString projectBasePath;
	wxString projectName;
	wxFileName executableName;

	
};
#endif