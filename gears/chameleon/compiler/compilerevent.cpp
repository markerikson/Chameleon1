#include "compilerevent.h"


DEFINE_EVENT_TYPE(chEVT_COMPILER_START)
DEFINE_EVENT_TYPE(chEVT_COMPILER_PROBLEM)
DEFINE_EVENT_TYPE(chEVT_COMPILER_END)

//////////////////////////////////////////////////////////////////////////////
///  public constructor CompilerEvent
///  <TODO: insert text here>
///
///  @param  eventtype wxEventType  <TODO: insert text here>
///
///  @return void
///
///  @author David Czechowski @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
CompilerEvent::CompilerEvent(wxEventType t)
	: wxEvent(0, t)
{
	//SetEventType(t);
}
