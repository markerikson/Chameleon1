// Datastructures.h - common data structures for the Chameleon project
#ifndef DATASTRUCTURES__H
#define DATASTRUCTURES__H

#include <wx/hashmap.h>
#include <wx/string.h>


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
	ID_PAGECLOSE,
	ID_CLOSETAB,
	ID_OPEN_REMOTE,
	ID_SAVE_REMOTE,


	// major widgets
	ID_NOTEBOOK_ED,
	ID_NOTEBOOK_TERM,
	ID_TAB_ED,
	ID_PANEL_ED,
	ID_ED,
	ID_TELNET,
	ID_SPLITTER,
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


	// temporary - IDs for working with multiple documents
	ID_CREATEDOC,
	ID_SETDOC, 
	ID_GETDOC,
	ID_RELEASEDOC,

	ID_COMPILE,

	ID_STARTCONNECT,


	ID_TEST,

	// debugging IDs
	ID_DEBUG_IDS_FIRST,
	ID_START = ID_DEBUG_IDS_FIRST,
	ID_STOP,
	ID_PAUSE,
	ID_STEPNEXT,
	ID_STEPOVER,
	ID_STEPOUT,
	ID_DEBUG_IDS_LAST,

	ID_OPTIONS_GRID = 10005,
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
	PERM_LAST
};


#ifdef PERM___H
wxString GlobalPermStrings[] = {"Syntax Highlighting", 
								"Name completion",
								"Auto-indentation",
								"Debugging",
								"Terminal",
								"Remote/Local modes"};

#endif

WX_DECLARE_HASH_MAP( int,
					int,
					wxIntegerHash,
					wxIntegerEqual,
					IntIntHashmap );

typedef struct 
{
	wxArrayString dirNames;
	wxArrayString fileNames;
} DirListing;


#endif
