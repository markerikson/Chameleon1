#include "../gui/ChameleonWindow.h"

#include "../common/debug.h"

#ifdef _DEBUG

    #define new DEBUG_NEW

#endif


BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)
	EVT_STC_CHARADDED(-1, ChameleonEditor::OnChar)
	EVT_STC_CHANGE(-1, ChameleonEditor::OnSetTabModified)
END_EVENT_TABLE()

ChameleonEditor::ChameleonEditor( ChameleonWindow *mframe,
                                  wxWindow *parent,     wxWindowID id, const
                                      wxPoint & pos /* = wxDefaultPosition */,
                                  const wxSize & size /* = wxDefaultSize */,
                                  long style /* = 0 */, const
                                      wxString & name /* = wxSTCNameStr */ ) :
    wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    m_mainFrame = mframe;
	m_parentNotebook = (ChameleonNotebook*)parent;

    this->SetTabWidth(4);

    this->SetMarginWidth(0, 40);

    this->SetMarginType(0, 1);

    wxFont font(10, wxMODERN, wxNORMAL, wxNORMAL);

    this->StyleSetFont(wxSTC_STYLE_DEFAULT, font);

    this->StyleClearAll();

    // Editor style setup

    // whitespace
    //this->StyleSetForeground(0, wxColour(0x80, 0x80, 0x80));

    // comment
    this->StyleSetForeground(1, wxColour(0x00, 0x7f, 0x00));

    // line comment
    this->StyleSetForeground(2, wxColour(0x00, 0x7f, 0x00));

    // numbers
    this->StyleSetForeground(4, wxColour("Red"));

    // keywords
    this->StyleSetForeground(5, wxColour("Blue"));

    this->StyleSetBold(5, TRUE);

    // double-quoted strings
    this->StyleSetForeground(6, wxColour(0x00, 0x80, 0x80));

    // single-quoted strings
    this->StyleSetForeground(7, wxColour(0x00, 0x80, 0x80));

    // preprocessor
    this->StyleSetForeground(9, wxColour("Purple"));

    // operators
    this->StyleSetForeground(10, wxColour(0x00, 0x00, 0x00));

    this->StyleSetForeground(11, wxColour(0x00, 0x00, 0x00));

    this->StyleSetBold(10, TRUE);

	this->SetSelBackground(true, wxColour(49, 106, 197));
	this->SetSelForeground(true, wxColour("white"));

    this->EmptyUndoBuffer();

    if( m_mainFrame->IsEnabled(PERM_SYNTAXHIGHLIGHT) )
    {
        this->SetLexer(wxSTC_LEX_CPP);

        this->SetKeyWords(0, "asm auto bool break case catch char class const "
            "const_cast continue default delete do double "
            "dynamic_cast else enum explicit export extern "
            "false float for friend goto if inline int long "
            "mutable namespace new operator private protected "
            "public register reinterpret_cast return short signed "
            "sizeof static static_cast struct switch template this "
            "throw true try typedef typeid typename union unsigned "
            "using virtual void volatile wchar_t while");
    }
	else
	{
		this->SetLexer(wxSTC_LEX_CONTAINER);
	}
}

ChameleonEditor::~ChameleonEditor() {
    //this->PopEventHandler(true);
    }

bool ChameleonEditor::SaveFileAs()
{

    // return if no change
    if( !Modified() )
        return true;

    // get filname
    if( !m_filename )
    {
        wxFileDialog
            dlg (this, _T("Save file"), _T(""), _T(""), _T("Any file (*)|*"),
        wxSAVE | wxOVERWRITE_PROMPT);

        if(dlg.ShowModal() != wxID_OK) { return false; }

        m_filename = dlg.GetPath();
    }

    // save file
    return SaveFile(m_filename);
}

bool ChameleonEditor::SaveFile( const wxString & filename )
{

    // return if no change
    if(!Modified() && filename.IsEmpty()) { return true; }

    // save edit in file and clear undo
    if(!filename.IsEmpty()) { m_filename = filename; }

    wxFile file (m_filename, wxFile::write);

    if(!file.IsOpened()) { return false; }

    wxString buf = GetText();

    bool okay = file.Write(buf);

    file.Close();

    if(!okay) { return false; }

    EmptyUndoBuffer();
    SetSavePoint();



	wxFileName fn;
	fn.Assign(m_filename);

    m_filetime = fn.GetModificationTime();

	int currentTab = m_parentNotebook->GetSelection();
	
	m_parentNotebook->SetPageText(currentTab, fn.GetFullName());
	m_parentNotebook->Refresh();

    return true;
}

bool ChameleonEditor::LoadFile ()
{

    // get filname
    if( !m_filename )
    {
        wxFileDialog dlg (this, _T("Open file"), _T(""), _T(""),
        _T("Any file (*)|*"), wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR);

        if( dlg.ShowModal() != wxID_OK )
            return false;

        m_filename = dlg.GetPath();
    }

    // load file
    return LoadFile(m_filename);
}

bool ChameleonEditor::LoadFile( const wxString & filename )
{
    wxString buf;

    // load file in edit and clear undo
    if( !filename.IsEmpty() )
	{
        m_filename = filename;
	}

    wxFile file (m_filename);

    if( !file.IsOpened() )
	{
        return false;

	}

    ClearAll();

    long lng = file.Length();

    if( lng > 0 )
    {
        char *pBuf = buf.GetWriteBuf(lng);

        file.Read(pBuf, lng);

        buf.UngetWriteBuf();

        InsertText(0, buf);
    }

    EmptyUndoBuffer();

    file.Close();

    m_filetime = wxFileName(m_filename).GetModificationTime();

    // determine and set EOL mode
    int eolMode = -1;

    bool eolMix = false;

    wxString eolName;

    if( buf.Contains("\r\n") )
    {
        eolMode = wxSTC_EOL_CRLF;

        eolName = _("CR+LF (Windows)");
    }
    else if( buf.Contains("\r") )
    {
        if(eolMode != -1) { eolMix = true; }
        else
        {
            eolMode = wxSTC_EOL_CR;

            eolName = _("CR (Macintosh)");
        }
    }
    else if( buf.Contains("\n") )
    {
        if(eolMode != -1) { eolMix = true; }
        else
        {
            eolMode = wxSTC_EOL_LF;

            eolName = _("LF (Unix)");
        }
    }

    if( eolMode != -1 )
    {
        if( eolMix && wxMessageBox(_("Convert all line endings to ")
            + eolName + _("?"), _("Line endings"), wxYES_NO | wxICON_QUESTION)
            == wxYES )
        {
            ConvertEOLs(eolMode);

           // g_statustext->Clear();

            //g_statustext->Append(_("Converted line endings to "));

            //g_statustext->Append(eolName);
        }

        SetEOLMode(eolMode);
    }

    // determine lexer language
    //wxFileName fname (m_filename);

    //InitializePrefs(DeterminePrefs(fname.GetFullName()));

	//int currentTab = m_parentNotebook->GetSelection();

	//m_parentNotebook->SetPageText(currentTab, wxFileName(m_filename).GetFullName());
	//m_parentNotebook->Refresh();



    return true;
}

bool ChameleonEditor::Modified () 
{
    // return modified state
    return (GetModify() && !GetReadOnly()); 
}

void ChameleonEditor::OnSetTabModified(wxStyledTextEvent &event)
{
	
	bool isModified = this->Modified();

	if(isModified)
	{
		int tabNum = m_parentNotebook->GetSelection();
		wxString title = m_parentNotebook->GetPageText(tabNum);
		
		if(!title.Contains("*"))
		{
			title += "*";
			m_parentNotebook->SetPageText(tabNum, title);
			//this->Refresh();
			m_parentNotebook->Refresh();
		}

	}
	
	

	//wxMessageBox("Modified");


}

void ChameleonEditor::OnChar( wxStyledTextEvent &event )
{

	const wxChar chr = event.GetKey();
	const int currentLine = GetCurrentLine();
	const int tabWidth = GetTabWidth();
	const int eolMode = GetEOLMode();


    if( chr == WXK_TAB )
    {
        event.Skip(true);

        int startLine = LineFromPosition(GetSelectionStart());

        int endline = LineFromPosition(GetSelectionEnd());

        int newStartPos = PositionFromLine(startLine);

        int newEndPos = PositionFromLine(endline) + LineLength(endline);

        this->SetSelection(newStartPos, newEndPos);

        bool doIndent = event.GetShift();

        int indentWidth = this->GetIndent();

        for( int i = startLine; i <= endline; i++ )
        {
            int lineIndent = this->GetLineIndentation(i);

            if(doIndent)
                { this->SetLineIndentation(i, lineIndent + indentWidth); }
            else
            {
                this->SetLineIndentation(i, lineIndent - indentWidth);
            }
        }
    }

	//if (((eolMode == CRLF || eolMode == LF) && chr == '\n')
	//	|| (eolMode == CR && chr == '\r'))
	if(chr == '\n' && m_mainFrame->IsEnabled(PERM_AUTOINDENT))
	{
		int previousLineInd = 0;

		if (currentLine > 0)
		{
			previousLineInd = GetLineIndentation(currentLine - 1);
		}

		if (previousLineInd == 0)
		{
			return;
		}

		SetLineIndentation(currentLine, previousLineInd);

		// If tabs are being used then change previousLineInd to tab sizes
		if (GetUseTabs())
		{
			previousLineInd /= GetTabWidth();
		}

		GotoPos(PositionFromLine(currentLine) + previousLineInd);
	}

	return;
}