// Datastructures.h - common data structures for the Chameleon project
#ifndef DATASTRUCTURES__H
#define DATASTRUCTURES__H

#define CHAMELEON_VERSION_NUM " 0.1beta3 "

#include <wx/hashmap.h>
#include <wx/string.h>
#include <wx/dynarray.h>
//#include <wx/treectrl.h>

class wxWindow;

enum VariableNames
{
	VN_NUMPAGES,
	VN_CURRENTPAGE,
	VN_CLICKEDTAB,
};


enum WindowID
{
	// menu items
	ID_QUIT = 5200,

	// File menu
	ID_NEW,
	ID_ABOUT,
	ID_OPEN,
	ID_SAVE,
	ID_SAVE_AS,
	ID_PAGECLOSE,
	ID_CLOSETAB,
	ID_OPEN_SOURCE_LOCAL,
	ID_OPEN_SOURCE_REMOTE,
	ID_OPEN_PROJECT_LOCAL,
	ID_OPEN_PROJECT_REMOTE,


	// major widgets
	ID_NOTEBOOK_ED,
	ID_NOTEBOOK_TERM,
	ID_TAB_ED,
	ID_PANEL_ED,
	ID_ED,
	ID_TELNET,
	ID_SPLITEDITOROUTPUT,
	ID_SPLITPROJECTEDITOR,
	ID_PROJECTTREE,
	ID_OPTIONSDIALOG,
	ID_REMOTEFILEDIALOG,

	// edit menu items
	ID_UNDO,
	ID_REDO,
	ID_COPY,
	ID_CUT,
	ID_PASTE,

	// tools menu items
	ID_OPTIONS,


	ID_COMPILE,

	ID_STARTCONNECT,

	ID_TEST,

	ID_PROJECT_ADDFILE,
	ID_PROJECT_REMOVEFILE,

	ID_PROJECT_ADD_SOURCEFILE,
	ID_PROJECT_ADD_HEADERFILE,
	ID_PROJECT_ADD_LIBRARYFILE,
	ID_PROJECT_REMOVE_SOURCEFILE,
	ID_PROJECT_REMOVE_HEADERFILE,
	ID_PROJECT_REMOVE_LIBRARYFILE,

	// debugging IDs
	ID_DEBUG_IDS_FIRST,
	ID_START = ID_DEBUG_IDS_FIRST,
	ID_STOP,
	ID_PAUSE,
	ID_STEPNEXT,
	ID_STEPOVER,
	ID_STEPOUT,
	ID_DEBUG_IDS_LAST,
};

enum modules
{
	PERM_FIRST = 0,
	PERM_SYNTAXHIGHLIGHT = PERM_FIRST,
	PERM_AUTOCOMPLETE,
	PERM_AUTOINDENT,
	PERM_DEBUG,
	PERM_TERMINAL,
	PERM_REMOTELOCAL,
	PERM_TELNETTEST,
	PERM_PROJECTS,
	PERM_COMPILE,
	PERM_TEST,
	PERM_LAST
};


#ifdef PERM___H
wxString GlobalPermStrings[] = {"Syntax highlighting", 
								"Name completion",
								"Auto-indentation",
								"Debugging",
								"Terminal",
								"Local mode",
								"Telnet testing",
								"Projects",
								"Compilation",
								"Test permission"};

char AuthCodeLookupTable[] = {'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
								'U', 'V', 'W', 'X', 'Y', 'Z', 'A', 'B', 'C', 'D', 'E', 'F'};
#endif
#endif



WX_DECLARE_HASH_MAP( int,
					int,
					wxIntegerHash,
					wxIntegerEqual,
					IntIntHashmap );

WX_DECLARE_HASH_MAP(wxString, 
					int,
					wxStringHash,
					wxStringEqual,
					StringIntHashmap);


WX_DEFINE_ARRAY(wxWindow*, WindowPointerArray);



typedef struct 
{
	wxArrayString dirNames;
	wxArrayString fileNames;
} DirListing;

enum NetworkStatus
{
	NET_GOOD = 0,
	NET_UNKNOWN_HOST,  // host finger print not in cache
	NET_CONN_REFUSED,  // host exists, but wouldn't allow a connection
	NET_AUTH_FAILED,   // user+pass did not work on host
	NET_READ_ERROR,
	NET_WRITE_ERROR,
	NET_BAD_PLINK_PROG,
	NET_BAD_PSCP_PROG,
	NET_ERROR_MESSAGE, // default/catch-all
};

enum NetworkCallResult
{
	NETCALL_WORKED,
	NETCALL_FAILED,
	NETCALL_REDO,
};

enum FileFilterType
{
	FILE_ALLSOURCES,
	FILE_PROJECT,
	FILE_LIBRARY,
	FILE_SOURCEONLY,
	FILE_HEADERONLY,
	FILE_ALLFILES,
	FILE_LASTTYPE,
};

/*
class FileNameTreeData : public wxTreeItemData
{
public:
	wxString filename;
};
*/

#endif
