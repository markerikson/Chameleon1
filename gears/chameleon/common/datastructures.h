// Datastructures.h - common data structures for the Chameleon project
#ifndef DATASTRUCTURES__H
#define DATASTRUCTURES__H




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


	// major widgets
	ID_NOTEBOOK_ED,
	ID_NOTEBOOK_TERM,
	ID_TAB_ED,
	ID_PANEL_ED,
	ID_ED,
	ID_TELNET,

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
	ID_START,
	ID_STOP,
	ID_PAUSE,
	ID_STEPNEXT,
	ID_STEPOVER,
	ID_STEPOUT,
};

#endif
