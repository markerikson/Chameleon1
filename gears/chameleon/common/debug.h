#ifndef DEBUG__H
#define DEBUG__H

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif

#define DEBUGLOG(x) wxString logstring; logstring << __FILE__ << ":" << __LINE__ << ": " << x; ::wxLogDebug(logstring);

#endif