#ifndef PROJECTINFO_H
#define PROJECTINFO_H
#include <wx/dynarray.h>

class ProjectInfo
{
public:
	wxArrayString headerFiles;
	wxArrayString sourceFiles;
	wxArrayString libraryFiles;
	bool relativePaths;
	bool remoteProject;
	wxString projectBasePath;
};


#endif