#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/event.h>
#include <wx/textctrl.h>
#include "../gears_chameleon/network/networking.h"
#include "../gears_chameleon/common/datastructures.h"


const char PLINK_LOCATION[] = "plink.exe"; //"c:/windows/plink.exe";
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
		wxTextCtrl* textbox;
		Networking* n;

	public:
		DECLARE_EVENT_TABLE()
};


enum
{
	ID_Quit = 1,
	ID_PLINKCONNECT,
	ID_PC_SEND,
	ID_PC_ISCONNECTED,
	ID_PC_GETOUTPUT,
	ID_PC_GETERRORS,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_Quit, MyFrame::OnQuit)
	EVT_MENU(ID_PLINKCONNECT, MyFrame::OnFunc1)
	EVT_MENU(ID_PC_SEND, MyFrame::OnFunc2)
	EVT_MENU(ID_PC_ISCONNECTED, MyFrame::OnFunc3)
	EVT_MENU(ID_PC_GETOUTPUT, MyFrame::OnFunc4)
	EVT_MENU(ID_PC_GETERRORS, MyFrame::OnFunc5)
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
	wxMenu *menuFile = new wxMenu;

	menuFile->Append( ID_PLINKCONNECT, "Function 1" );
	menuFile->Append( ID_PC_SEND, "Function 2" );
	menuFile->Append( ID_PC_ISCONNECTED, "Function 3" );
	menuFile->Append( ID_PC_GETOUTPUT, "Function 4" );
	menuFile->Append( ID_PC_GETERRORS, "Function 5" );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, "E&xit" );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );

	SetMenuBar( menuBar );

	CreateStatusBar();

	textbox = new wxTextCtrl( this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

// useful for testing both
	n = new Networking();
	n->SetDetails(HOSTNAME, USERNAME, PASSPHRASE);
	n->SetPlinkProg(PLINK_LOCATION);

}

MyFrame::~MyFrame() {
	if(n != NULL) {
		delete n;
	}
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

//======================================================= COMMANDS

//--------------------------------------------- Function 1
void MyFrame::OnFunc1(wxCommandEvent& WXUNUSED(event))
{
	if(n != NULL) {
		*textbox << "Nothing\n";
	}
	else {
		*textbox << "Networking object doesn't exist.\n";
	}

}


//--------------------------------------------- Function 2
void MyFrame::OnFunc2(wxCommandEvent& WXUNUSED(event))
{
	if(n != NULL ) {
		*textbox << "_Networking_DirListing_of_java_\n";

		DirListing d = it->GetDirListing("~/java");
		int i = 0;
		// Files:
		*textbox << "Files: ";
		for(i = 0; i < int(d.fileNames.Count())-1; ) {
			*textbox << d.fileNames[i] << ", ";
			i++;
		}
		if(!d.fileNames.IsEmpty()) {
			*textbox << d.fileNames[i] << "\n";
		}
		else {
			*textbox << "\n";
		}
		//Directories:
		*textbox << "Directories: ";
		for(i = 0; i < int(d.dirNames.Count())-1; ) {
			*textbox << d.dirNames[i] << ", ";
			i++;
		}
		if(!d.dirNames.IsEmpty()) {
			*textbox << d.dirNames[i] << "\n";
		}
		else {
			*textbox << "\n";
		}

	}
	else {
		*textbox << "Networking object doesn't exist.\n";
	}
}


//--------------------------------------------- Function 3
void MyFrame::OnFunc3(wxCommandEvent& WXUNUSED(event))
{
	if(n != NULL ) {
		*textbox << "Nothing\n";
	}
	else {
		*textbox << "Networking object doesn't exist\n";
	}
}


//--------------------------------------------- Function 4
void MyFrame::OnFunc4(wxCommandEvent& WXUNUSED(event))
{
	if(n != NULL) {
		wxString file = n->GetFileContents("djc", "~");
		*textbox << "File:------------------\n";
		*textbox << file;
		*textbox << "\n-----------------------\n";
	}
	else {
		*textbox << "Networking object doesn't exist.\n";
	}
}


//--------------------------------------------- Function 5
void MyFrame::OnFunc5(wxCommandEvent& WXUNUSED(event))
{
	if(n != NULL) {
		*textbox << "Network object exists.\n";
	}
	else {
		*textbox << "Networking object doesn't exist.\n";
	}
}

