/////////////////////////////////////////////////////////////////////////////
// Name:        stctest.cpp
// Purpose:     sample of using wxStyledTextCtrl
// Author:      Robin Dunn
// Modified by:
// Created:     3-Feb-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "stctest.cpp"
    #pragma interface "stctest.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/wfstream.h>
#include <wx/frame.h>
#include <wx/toolbar.h>
#include <wx/bitmap.h>
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <wx/splitter.h>

#include "new.xpm"
#include "open.xpm"
#include "save.xpm"
#include "build.xpm"
#include "button.xpm"

#include "stc.h"

//----------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

//----------------------------------------------------------------------

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void SaveFile(wxCommandEvent& event, bool saveas);
	void OpenFile(wxCommandEvent& event);
	void Test(wxCommandEvent& event);
	void SaveFileAs(wxCommandEvent& event);

	
private:
	void CheckSize();
	void ResizeSplitter();



    wxStyledTextCtrl* ed;
	wxSplitterWindow* split;
	wxTextCtrl* textbox;

	wxString saveFileName;

    DECLARE_EVENT_TABLE()
};


// IDs for the controls and the menu commands
enum
{
    // menu items
    ID_Quit = 5200,
	ID_New,
	ID_Open,
	ID_Save,
    ID_About,
    ID_ED,
	ID_Copy,
	ID_Cut,
	ID_Paste,
	ID_Compile,
	ID_Test,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU			(ID_Open, MyFrame::OpenFile)
    EVT_MENU            (ID_Quit,  MyFrame::OnQuit)
    EVT_MENU            (ID_About, MyFrame::OnAbout)
	EVT_MENU			(ID_Test, MyFrame::ResizeSplitter)
	EVT_MENU			(ID_Save, MyFrame::SaveFile)
	//
	//EVT_SIZE			(MyFrame::ResizeSplitter)
END_EVENT_TABLE()


IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------
// `Main program' equivalent: the program execution "starts" here

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Chameleon",
                                 wxPoint(5, 5), wxSize(400, 300));

    frame->Show(TRUE);
    return TRUE;
}

//----------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_About;
#endif

	this->SetClientSize(640, 480);


    // create a menu bar
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_About, "&About...\tCtrl-A", "Show about dialog");

	menuFile->Append(ID_New, "&New\tCtrl-N", "Create a new file");
	menuFile->Append(ID_Open, "&Open\tCtrl-O", "Open an existing file");
	menuFile->Append(ID_Save, "&Save\tCtrl-S", "Save the current file");

    menuFile->Append(ID_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

	
	wxToolBar* toolBar = CreateToolBar(wxTB_FLAT | wxTB_TEXT);
	wxBitmap bmNew(new_xpm);
	toolBar->AddTool(ID_New, "New File", bmNew);
	
		
	wxBitmap bmOpen(open_xpm);
	toolBar->AddTool(ID_Open, "Open File", bmOpen);

	wxBitmap bmSave(save_xpm);
	toolBar->AddTool(ID_Save, "Save File", bmSave);

	toolBar->InsertSeparator(3);

	wxBitmap bmBuild(build_xpm);
	toolBar->AddTool(ID_Compile, "Compile", bmBuild);

	toolBar->InsertSeparator(5);

	wxBitmap bmTest(button_xpm);
	toolBar->AddTool(ID_Test, "Test", button_xpm);
	




	toolBar->Realize();

	SetToolBar(toolBar);
	
    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Statusbar text");
#endif // wxUSE_STATUSBAR


	split = new wxSplitterWindow(this, 5205);
	




    //----------------------------------------
    // Setup the editor
    ed = new wxStyledTextCtrl(split, ID_ED);



	textbox = new wxTextCtrl(split, 5206, "", wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_RICH, wxDefaultValidator);
	ed->SetMarginWidth(0, 20);
	ed->SetMarginType(0, 1);
	
	
	wxColour black("black");
	wxColour white("white");
	wxFont f;
	f.SetFaceName("Courier New");
	f.SetFamily(wxMODERN);
	f.SetPointSize(12);
	wxTextAttr ta(white, black, f);
	textbox->SetDefaultStyle(ta);
	textbox->SetBackgroundColour(black);
	
	textbox->SetForegroundColour(white);
	
	
	

	
	
	*textbox << "[s1278644@john ~]$ ";
	textbox->SetStyle(0, textbox->GetValue().Length(), ta);


	split->SplitHorizontally(ed, textbox, 500);
	split->SetMinimumPaneSize(140);

    // Default font
    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);
    ed->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    ed->StyleClearAll();

    ed->StyleSetForeground(0,  wxColour(0x80, 0x80, 0x80));
    ed->StyleSetForeground(1,  wxColour(0x00, 0x7f, 0x00));
    //ed->StyleSetForeground(2,  wxColour(0x00, 0x7f, 0x00));
    ed->StyleSetForeground(3,  wxColour(0x7f, 0x7f, 0x7f));
    ed->StyleSetForeground(4,  wxColour(0x00, 0x7f, 0x7f));
    ed->StyleSetForeground(5,  wxColour(0x00, 0x00, 0x7f));
    ed->StyleSetForeground(6,  wxColour(0x7f, 0x00, 0x7f));
    ed->StyleSetForeground(7,  wxColour(0x7f, 0x00, 0x7f));
    ed->StyleSetForeground(8,  wxColour(0x00, 0x7f, 0x7f));
    ed->StyleSetForeground(9,  wxColour(0x7f, 0x7f, 0x7f));
    ed->StyleSetForeground(10, wxColour(0x00, 0x00, 0x00));
    ed->StyleSetForeground(11, wxColour(0x00, 0x00, 0x00));
    ed->StyleSetBold(5,  TRUE);
    ed->StyleSetBold(10, TRUE);

#ifdef __WXMSW__
    ed->StyleSetSpec(2, "fore:#007f00,bold,face:Arial,size:9");
#else
    ed->StyleSetSpec(2, "fore:#007f00,bold,face:Helvetica,size:9");
#endif

	/*
    // give it some text to play with
    wxFile   file("stctest.cpp");
    wxString st;

    char* buff = st.GetWriteBuf(file.Length());
    file.Read(buff, file.Length());
    st.UngetWriteBuf();

    ed->InsertText(0, st);
	*/
    ed->EmptyUndoBuffer();

    ed->SetLexer(wxSTC_LEX_CPP);
    ed->SetKeyWords(0,
                    "asm auto bool break case catch char class const "
                    "const_cast continue default delete do double "
                    "dynamic_cast else enum explicit export extern "
                    "false float for friend goto if inline int long "
                    "mutable namespace new operator private protected "
                    "public register reinterpret_cast return short signed "
                    "sizeof static static_cast struct switch template this "
                    "throw true try typedef typeid typename union unsigned "
                    "using virtual void volatile wchar_t while");	
}

void MyFrame::OpenFile(wxCommandEvent& WXUNUSED(event))
{
	wxString filetext;
	wxString filename = ::wxFileSelector("Pick a file", "",
		"", "", "", wxOPEN, NULL);
	if(!filename.empty())
	{

		wxFile file(filename);
		wxString st;

		char* buff = st.GetWriteBuf(file.Length());
		file.Read(buff, file.Length());
		st.UngetWriteBuf();

		ed->InsertText(0, st);

		saveFileName = filename;		
	}
}


// event handlers
void MyFrame::Test(wxCommandEvent& WXUNUSED(event))
{
	int i = 0;
}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
	
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Testing wxStyledTextCtrl...\n"));

    wxMessageBox(msg, "About This Test", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::CheckSize()
{
	int x, y;
	this->GetClientSize(&x, &y);
	wxString result;
	result << x << ", " << y;
	::wxMessageBox(result );
	ed->SetScrollWidth(x);
	
}



void MyFrame::SaveFile(wxCommandEvent& WXUNUSED(event), bool saveas)
{

	wxString name = wxEmptyString;

	/*
	if( (saveFileName == wxEmptyString) ||
		(saveas) )
	{
		SaveFileDialog sfd = new SaveFileDialog();
		sfd.Filter = "HTML files (*.html)|*.html|All files (*.*)|*.*";
		DialogResult dr = sfd.ShowDialog();

		if(dr == DialogResult.OK)
		{	
			name  = sfd.FileName;

			if(sfd.FilterIndex == 1)
			{
				int idx = name.LastIndexOf(".");
				string justname = name.Substring(0, idx);
				name = justname + ".html";
			}	
		}
		else
		{
			return;
		}
	}
	else
	{
		name = saveFileName;
	}			

	StreamWriter sw = new StreamWriter(name);
	sw.WriteLine(ed.TextBox.Text);			
	sw.Close();
	ed.TextBox.Modified = false;


*/






	//if(ed->GetModify())
	wxString filetext;
	wxString filename = ::wxFileSelector("Pick a file", "",
		"", "", "", wxSAVE, NULL);
	if(!filename.empty())
	{
		wxFile file(filename);

		file.Write(ed->GetText());
		/*
		wxTextFile file(filename);
		file.Read
		if(!file.Exists())
		{
			file.Create();
		}
		wxString buffertext = ed->GetText();
		
		file.AddLine(buffertext);
		file.Write();
		file.Close();
		*/
	}
}

void MyFrame::SaveFileAs(wxCommandEvent& WXUNUSED(event))
{

}




void MyFrame::ResizeSplitter()
{
	int x, y;
	this->GetClientSize(&x, &y);
	int newsize = (y / 7) * 6;

	

	split->SetSashPosition(newsize);	
	
}



