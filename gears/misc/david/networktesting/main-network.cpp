#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include "../gears_chameleon/network/plinkconnect.h"
#include "../gears_chameleon/common/chameleonprocessevent.h"
//#include "../gears_chameleon/network/networking.h"
#include "../gears_chameleon/common/datastructures.h"
#include "../gears_chameleon/common/debug.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif


const char PLINK_LOCATION[] = "c:\\plink.exe";
const char HOSTNAME[] = "163.11.160.218";
const char USERNAME[] = "danroeber";
const char PASSPHRASE[] = "dayspring";



//=================================================  GUI things
class MyApp: public wxApp
{
	virtual bool OnInit();
};

class MyFrame: public wxFrame
{
	public:
		MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
		~MyFrame();
		void OnQuit(wxCommandEvent& event);
		//===================================== COMMANDS
		void OnFunc1(wxCommandEvent& event);
		void OnFunc2(wxCommandEvent& event);
		void OnFunc3(wxCommandEvent &event);
		void OnFunc4(wxCommandEvent &event);
		void OnFunc5(wxCommandEvent &event);
		//=====================================
	private:
		//===================================== EVENTS
		void OnTerm(ChameleonProcessEvent& event);
		void OnOutput(ChameleonProcessEvent& event);
		void OnError(ChameleonProcessEvent& event);
		//=====================================

		wxTextCtrl* textbox;
		wxLogWindow* logWindow;

		//===================================== INTERNALS
		PlinkConnect* p;
		//=====================================

	
		DECLARE_EVENT_TABLE()
};


enum
{
	ID_Quit = 1,
	ID_FUNC1,
	ID_FUNC2,
	ID_FUNC3,
	ID_FUNC4,
	ID_FUNC5,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_Quit, MyFrame::OnQuit)
	EVT_MENU(ID_FUNC1, MyFrame::OnFunc1)
	EVT_MENU(ID_FUNC2, MyFrame::OnFunc2)
	EVT_MENU(ID_FUNC3, MyFrame::OnFunc3)
	EVT_MENU(ID_FUNC4, MyFrame::OnFunc4)
	EVT_MENU(ID_FUNC5, MyFrame::OnFunc5)

	EVT_PROCESS_ENDED(MyFrame::OnTerm)
	EVT_PROCESS_STDOUT(MyFrame::OnOutput)
	EVT_PROCESS_STDERR(MyFrame::OnError)
END_EVENT_TABLE()


//===========================================================
//      IMPLEMENTATION
//===========================================================
IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame( "wxguiproc", wxPoint(50,50), wxSize(450,340) );
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
} 

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	// DEBUG WINDOW:
	logWindow = new wxLogWindow(this, "Debug messages");
	wxLog::SetActiveTarget(logWindow);

	wxMenu *menuFile = new wxMenu;

	menuFile->Append( ID_FUNC1, "isConnected" );
	menuFile->Append( ID_FUNC2, "send command" );
	menuFile->Append( ID_FUNC3, "delete PlinkConnect*" );
	menuFile->Append( ID_FUNC4, "Function 4" );
	menuFile->Append( ID_FUNC5, "Poll Tick" );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, "E&xit" );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );

	SetMenuBar( menuBar );

	//CreateStatusBar();

	textbox = new wxTextCtrl( this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	///////////////////////////////////
	p = new PlinkConnect(PLINK_LOCATION, HOSTNAME, USERNAME, PASSPHRASE);
	///////////////////////////////////
}

MyFrame::~MyFrame() {
	///////////////////////////////////
	if(p != NULL) {
		delete p;
	}
	///////////////////////////////////
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

//======================================================= COMMANDS

//--------------------------------------------- Function 1
void MyFrame::OnFunc1(wxCommandEvent& WXUNUSED(event))
{
	if(p != NULL) {
		if(p->getIsConnected()) {
			*textbox << "PlinkConnect is connected.\n";
		}
		else {
			*textbox << "PlinkConnect is not connected.\n";
			*textbox << "Message: \"" + p->getMessage() + "\"\n";
		}
	}
	else {
		*textbox << "Object doesn't exist.\n";
	}

}


//--------------------------------------------- Function 2
void MyFrame::OnFunc2(wxCommandEvent& WXUNUSED(event))
{
	if(p->getIsConnected()) {
		*textbox << "Sending \'ls\'.\n";
		/*wxTextInputStream* pstdin =*/ p->executeCommand("ls --color=never -A -1", this);
	}
	else {
		*textbox << "Can't send command.  Plink Connect is not connected.\n";
	}

}


//--------------------------------------------- Function 3
void MyFrame::OnFunc3(wxCommandEvent& WXUNUSED(event))
{
	if(p != NULL) {
		*textbox << "Deleting PlinkConnect.\n";
		delete p;
		p = new PlinkConnect(PLINK_LOCATION, HOSTNAME, USERNAME, PASSPHRASE);
	}
	else {
		*textbox << "Object doesn't exist.\n";
	}

}


//--------------------------------------------- Function 4
void MyFrame::OnFunc4(wxCommandEvent& WXUNUSED(event))
{
	if(p->getIsConnected()) {
		*textbox << "Sending \'ls\'.\n";
		/*wxTextInputStream* pstdin =*/ p->executeSyncCommand("ls --color=never -A -1");
	}
	else {
		*textbox << "Can't send command.  Plink Connect is not connected.\n";
	}

}


//--------------------------------------------- Function 5
void MyFrame::OnFunc5(wxCommandEvent& WXUNUSED(event))
{
	if(p != NULL) {
		*textbox << "Polling\n";
		p->PollTick();
	}
	else {
		*textbox << "Object doesn't exist.\n";
	}

}


//--------------------------------------------- Events:
void MyFrame::OnTerm(ChameleonProcessEvent& e)
{
	*textbox << "Process Termed\n";
	return;
}


void MyFrame::OnOutput(ChameleonProcessEvent& e)
{
	*textbox << "Output: \"" + e.GetString() + "\"\n";
	return;
}


void MyFrame::OnError(ChameleonProcessEvent& e)
{
	*textbox << "Error: \"" + e.GetString() + "\"\n";
	return;
}