// Datastructures.h - common data structures for the Chameleon project
#ifndef DATASTRUCTURES__H
#define DATASTRUCTURES__H

#define CHAMELEON_VERSION_NUM " 0.1beta4 "

#include <wx/hashmap.h>
#include <wx/string.h>
#include <wx/dynarray.h>
//#include <wx/treectrl.h>

class wxWindow;
class ChameleonEditor;

enum VariableNames
{
	VN_NUMPAGES,
	VN_CURRENTPAGE,
	VN_CLICKEDTAB,
};

enum MarkerTypes
{
	MARKER_BREAKPOINT,
	MARKER_FOCUSEDLINE,
};


enum WindowID
{
	// menu items
	ID_QUIT = 5200,

	// File menu
	ID_NEW_SOURCE,
	ID_SAVE,
	ID_SAVE_SOURCE_LOCAL,
	ID_SAVE_SOURCE_REMOTE,
	ID_OPEN_SOURCE_LOCAL,
	ID_OPEN_SOURCE_REMOTE,
	ID_CLOSEPAGE,
	ID_CLOSEALL,
	ID_CLOSETAB,
	ID_NEW_PROJECT,
	ID_OPEN_PROJECT_LOCAL,
	ID_OPEN_PROJECT_REMOTE,
	ID_CLOSE_PROJECT,

	ID_PRINT_PAGE,
	ID_PRINT_PREVIEW,
	ID_PRINT_SETUP,


	// major widgets
	ID_NOTEBOOK_ED,
	ID_NOTEBOOK_TERM,
	ID_TERM_CONTAINER,
	ID_TAB_ED,
	ID_PANEL_ED,
	ID_ED,
	ID_TERMINAL,
	ID_SPLITEDITOROUTPUT,
	ID_SPLITPROJECTEDITOR,
	ID_PROJECTTREE,
	ID_OPTIONSDIALOG,
	ID_REMOTEFILEDIALOG,
	ID_COMPILERTEXTBOX,
	ID_VARWATCHPANEL,

	// edit menu items
	ID_UNDO,
	ID_REDO,
	ID_COPY,
	ID_CUT,
	ID_PASTE,
	ID_FIND,
	ID_REPLACE,
	ID_INDENT,
	ID_UNINDENT,
	ID_SELECTALL,
	ID_SELECTLINE,

	// tools menu items
	ID_OPTIONS,
	ID_STARTCONNECT,
	ID_DISCONNECT,

	ID_COMPILE,

	ID_ABOUT,
	ID_HELP,

	ID_TEST,

	ID_PROJECT_ADDFILE,
	ID_PROJECT_REMOVEFILE,
	ID_PROJECT_SETTINGS,

	ID_PROJECT_ADD_SOURCEFILE,
	ID_PROJECT_ADD_HEADERFILE,
	ID_PROJECT_ADD_LIBRARYFILE,
	ID_PROJECT_REMOVE_SOURCEFILE,
	ID_PROJECT_REMOVE_HEADERFILE,
	ID_PROJECT_REMOVE_LIBRARYFILE,

	// debugging command IDs
	ID_DEBUG_IDS_FIRST,
	ID_DEBUG_START = ID_DEBUG_IDS_FIRST,
	ID_DEBUG_CONTINUE,
	ID_DEBUG_STOP,
	ID_DEBUG_STEPNEXT,
	ID_DEBUG_STEPOVER,
	ID_DEBUG_STEPOUT,
	ID_DEBUG_IDS_LAST,


	ID_DEBUG_ADD_BREAKPOINT,
	ID_DEBUG_REMOVE_BREAKPOINT,
	ID_DEBUG_CLEAR_ALL_BREAKPOINTS,
	ID_DEBUG_RUNTOCURSOR,
	ID_DEBUG_ADD_WATCH,
	ID_DEBUG_REMOVE_WATCH,

	//i added these Mark -B
	//ID_DEBUG_WATCHVAR,
	//ID_DEBUG_REMOVE_VAR,

	ID_DEBUG_BREAKPOINT,
	ID_DEBUG_EXIT_NORMAL,
	ID_DEBUG_EXIT_ERROR,
};

// IMPORTANT!!!  Any changes to this enum need to be reflected in the 
//				 PermStrings array in p.cpp.  
enum modules
{
	PERM_FIRST = 0,
	PERM_SYNTAXHIGHLIGHT = PERM_FIRST,
	PERM_AUTOINDENT,
	PERM_DEBUG,
	PERM_TERMINAL,
	PERM_LOCALMODE,
	PERM_PROJECTS,
	PERM_COMPILE,
	PERM_ADVANCEDCOMPILE,
	PERM_TEST,
	PERM_LAST
};

// IMPORTANT!!! Any changes to this enum need to be reflected in the 
//				PermStrings array in p.cpp.

WX_DEFINE_ARRAY(wxWindow*, WindowPointerArray);
WX_DEFINE_ARRAY(ChameleonEditor*, EditorPointerArray);
WX_DEFINE_ARRAY(bool, BoolArray);


typedef struct 
{
	wxArrayInt gdbNumbers;
	wxArrayInt lineNumbers;	
} DebugBreakInfo;

typedef struct 
{
	bool functionHasBeenVisited;
	BoolArray variableDisplayed;
	wxArrayString variableNames;
} FunctionVariablesInfo;

typedef struct  
{
	wxString type;
	wxString name;
	wxString functionName;
	wxString value;
	wxString regexKey;
} VariableInfo;

typedef struct 
{
	wxArrayString dirNames;
	wxArrayString fileNames;
} DirListing;

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

WX_DECLARE_HASH_MAP(wxString,
					DebugBreakInfo,
					wxStringHash,
					wxStringEqual,
					DebugBreakHash);

WX_DECLARE_HASH_MAP(wxString, 
					wxArrayInt,
					wxStringHash,
					wxStringEqual,
					FileBreakpointHash);

WX_DECLARE_HASH_MAP(int,
					VariableInfo,
					wxIntegerHash,
					wxIntegerEqual,
					VariableInfoHash);

WX_DECLARE_HASH_MAP(wxString,
					FunctionVariablesInfo,
					wxStringHash,
					wxStringEqual,
					FunctionVariablesHash);

WX_DECLARE_STRING_HASH_MAP(wxString, 
						   StringStringHashmap);

WX_DECLARE_OBJARRAY(VariableInfo, VariableInfoArray);


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
	FILE_ALLSOURCETYPES,
	FILE_PROJECT,
	FILE_LIBRARIES,
	FILE_SOURCES,
	FILE_HEADERS,
	FILE_ALLFILES,
	FILE_LASTTYPE,
};

#endif
