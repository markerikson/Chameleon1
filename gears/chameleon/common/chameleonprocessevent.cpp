#include "chameleonprocessevent.h"
#include "../common/debug.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

DEFINE_EVENT_TYPE(chEVT_PROCESS_ENDED)
DEFINE_EVENT_TYPE(chEVT_PROCESS_STDOUT)
DEFINE_EVENT_TYPE(chEVT_PROCESS_STDERR)

//////////////////////////////////////////////////////////////////////////////
///  public constructor ChameleonProcessEvent
///  <TODO: insert text here>
///
///  @param  eventtype wxEventType  <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
ChameleonProcessEvent::ChameleonProcessEvent(wxEventType eventtype)
	: wxEvent(0, eventtype)
{
	//
}
