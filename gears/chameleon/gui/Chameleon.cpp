#include "ChameleonWindow.h"

//----------------------------------------------------------------------
// `Main program' equivalent: the program execution "starts" here

bool MyApp::OnInit()
{
    ChameleonWindow *frame = new ChameleonWindow("Chameleon",
                                 wxPoint(5, 5), wxSize(400, 300));

    frame->Show(TRUE);
    return TRUE;
}

//----------------------------------------------------------------------

// frame constructor
ChameleonWindow::ChameleonWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_About;
#endif

	this->SetClientSize(640, 480);


    // create a menu bar
    wxMenu* menuFile = new wxMenu("", wxMENU_TEAROFF);

	menuFile->Append(ID_NEW, "&New\tCtrl-N", "Create a new file");
	menuFile->Append(ID_OPEN, "&Open\tCtrl-O", "Open an existing file");
	menuFile->Append(ID_SAVE, "&Save\tCtrl-S", "Save the current file");
	menuFile->Append(ID_QUIT, "E&xit\tAlt-X", "Quit this program");

	wxMenu* menuEdit = new wxMenu();

	menuEdit->Append(ID_UNDO, "&Undo\tCtrl-Z");
	menuEdit->Append(ID_REDO, "&Redo\t\trl-Y");



    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_ABOUT, "&About...\tCtrl-A", "Show about dialog");



    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
    menuBar->Append(helpMenu, "&Help");

	
	wxToolBar* toolBar = CreateToolBar(wxTB_FLAT | wxTB_TEXT);
	wxBitmap bmNew(new_xpm);
	toolBar->AddTool(ID_NEW, "New File", bmNew);
	
		
	wxBitmap bmOpen(open_xpm);
	toolBar->AddTool(ID_OPEN, "Open File", bmOpen);

	wxBitmap bmSave(save_xpm);
	toolBar->AddTool(ID_SAVE, "Save File", bmSave);

	toolBar->InsertSeparator(3);

	wxBitmap bmBuild(build_xpm);
	toolBar->AddTool(ID_COMPILE, "Compile", bmBuild);

	toolBar->InsertSeparator(5);

	wxBitmap bmTest(button_xpm);
	toolBar->AddTool(ID_TEST, "Test", button_xpm);
	




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
    ed = new ChameleonEditor(split, ID_ED);//wxStyledTextCtrl(split, ID_ED);
	uih = new UpdateUIHandler(this);

	ed->PushEventHandler(uih);

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

void ChameleonWindow::OpenFile(wxCommandEvent& WXUNUSED(event))
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
void ChameleonWindow::Test(wxCommandEvent& WXUNUSED(event))
{
	//ed->Undo();
//	wxEventType et = event.GetEventType();
	
}


void ChameleonWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);	
}

void ChameleonWindow::OnUndo(wxCommandEvent &event)
{
	ed->Undo();
}

void ChameleonWindow::OnRedo(wxCommandEvent &event)
{
	ed->Redo();
}

void ChameleonWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("Testing wxStyledTextCtrl...\n"));

    wxMessageBox(msg, "About This Test", wxOK | wxICON_INFORMATION, this);
}

void ChameleonWindow::CheckSize()
{
	int x, y;
	this->GetClientSize(&x, &y);
	wxString result;
	result << x << ", " << y;
	::wxMessageBox(result );
	ed->SetScrollWidth(x);
	
}

void ChameleonWindow::OnUpdateSave(wxUpdateUIEvent &event)
{
	event.Enable(ed->GetModify());
}


void ChameleonWindow::OnSave(wxCommandEvent& WXUNUSED(event))
{
	::wxMessageBox("OnSave");
	SaveFileLocal(false);
}

void ChameleonWindow::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	::wxMessageBox("OnSaveAs");
	SaveFileLocal(true);
}


void ChameleonWindow::SaveFileLocal(bool saveas)
{

	wxString name = wxEmptyString;

	
	if( (saveFileName == wxEmptyString) ||
		(saveas) )
	{
/*
		SaveFileDialog sfd = new SaveFileDialog();
		sfd.Filter = "HTML files (*.html)|*.html|All files (*.*)|*.*";
		DialogResult dr = sfd.ShowDialog();
*/
		wxString filetext;
		wxString filetypes = "C++ files (*.cpp)|*.cpp|Header files (*.h)|*.h|All files (*.*)|*.*";
		//wxString filename = ::wxFileSelector("Pick a file", "",
		//	"", "", "", wxSAVE, NULL);
		wxFileDialog fd(NULL, "Save As", wxEmptyString, 
						wxEmptyString, filetypes, wxSAVE);

		int dialogResult = fd.ShowModal();
		//ed->On

		if(dialogResult == wxID_OK)
		{			
			wxString filename = fd.GetFilename();
			int filteridx = fd.GetFilterIndex();

			wxString message = "Filter index: ";
			message << filteridx;
			
			::wxMessageBox(message);
/*
			switch(filteridx)
			{
				case 0:
					::wxMessageBox("Filter index: 0");
					break;
				case 1:
					::wxMessageBox()
			}
		

			wxFile file();

			file.Write(ed->GetText());
			file.Close();
*/	

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

/*
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






	}
	

}


void ChameleonWindow::ResizeSplitter()
{
	/*
	int x, y;
	this->GetClientSize(&x, &y);
	int newsize = (y / 7) * 6;

	

	split->SetSashPosition(newsize);	
	*/
	
}



