#include "../common/CommonHeaders.h"

#include "stc.h"
#include "editor.h"
#include "../gui/ChameleonWindow.h"
#include "../gui/ChameleonNotebook.h"
#include "../common/datastructures.h"
#include "../common/debug.h"


#include <wx/datetime.h>

#ifdef _DEBUG

    #define new DEBUG_NEW

#endif


BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)
	EVT_STC_CHARADDED(-1, ChameleonEditor::OnChar)
	EVT_STC_MODIFIED(-1, ChameleonEditor::OnEditorModified)
	EVT_RIGHT_DOWN		(ChameleonEditor::OnRightClick)
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

	m_bLoadingFile = false;
	m_bLastSavedRemotely = true;

	m_fileNameAndPath.Assign(wxEmptyString);

    this->SetTabWidth(4);

    this->SetMarginWidth(0, 40);

    this->SetMarginType(0, wxSTC_MARGIN_NUMBER);

	this->SetMarginWidth(1, 16);
	this->SetMarginType(1, wxSTC_MARGIN_SYMBOL);

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

	UpdateSyntaxHighlighting();

	this->MarkerDefine(0, wxSTC_MARK_CIRCLE);
	this->MarkerSetBackground(0, wxColour("red"));

	this->MarkerDefine(1, wxSTC_MARK_CIRCLE);
	this->MarkerSetForeground(1, wxColour("red"));

	this->MarkerDefine(2, wxSTC_MARK_SHORTARROW);
	this->MarkerSetBackground(2, wxColour("yellow"));

	
	/*
	for(int i = 0; i <= wxSTC_MARK_PLUS; i++)
	{
		this->MarkerDefine(i, i);
		this->MarkerAdd(i, i);
	}
	*/

	this->UsePopUp(false);
	
	m_popupMenu.Append(ID_CUT, "Cut");
	m_popupMenu.Append(ID_COPY, "Copy");
	m_popupMenu.Append(ID_PASTE, "Paste");
	m_popupMenu.AppendSeparator();

	m_popupMenu.Append(ID_DEBUG_ADD_BREAKPOINT, "Add a breakpoint");
	m_popupMenu.Append(ID_DEBUG_REMOVE_BREAKPOINT, "Remove this breakpoint");

	m_popupMenu.Append(ID_DEBUG_RUNTOCURSOR, "Run to cursor");

	int modmask = wxSTC_MOD_DELETETEXT | wxSTC_MOD_INSERTTEXT;

	this->SetModEventMask(modmask);

}

ChameleonEditor::~ChameleonEditor() 
{
    //this->PopEventHandler(true);
}



bool ChameleonEditor::SaveFileLocal()
{
	return SaveFile(m_fileNameAndPath.GetFullPath());
}

bool ChameleonEditor::SaveFile( const wxString & filename )
{

    // return if no change
    if(!Modified() && filename.IsEmpty()) { return true; }

	wxFileName fn(filename);

    // save edit in file and clear undo
    if(!filename.IsEmpty()) 
	{ 
		m_simpleFileName = fn.GetFullName(); 
	}

    wxFile file (filename, wxFile::write);

    if(!file.IsOpened()) { return false; }

    wxString buf = GetText();

    bool okay = file.Write(buf);

    file.Close();

    if(!okay) { return false; }

    EmptyUndoBuffer();
    SetSavePoint();

    m_filetime = fn.GetModificationTime();

    return true;
}

bool ChameleonEditor::LoadFileText(wxString fileContents)
{
	if(fileContents.Length() > 0)
	{
		m_bLoadingFile = true;
		ClearAll();
		InsertText(0, fileContents);
	}

    EmptyUndoBuffer();

    // determine and set EOL mode
    int eolMode = -1;

    bool eolMix = false;

    wxString eolName;

    if( fileContents.Contains("\r\n") )
    {
        eolMode = wxSTC_EOL_CRLF;

        eolName = _("CR+LF (Windows)");
    }
    else if( fileContents.Contains("\r") )
    {
        if(eolMode != -1) { eolMix = true; }
        else
        {
            eolMode = wxSTC_EOL_CR;

            eolName = _("CR (Macintosh)");
        }
    }
    else if( fileContents.Contains("\n") )
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

			// set staus bar text
            // g_statustext->Clear();
            //g_statustext->Append(_("Converted line endings to "));
            //g_statustext->Append(eolName);
        }

        SetEOLMode(eolMode);
    }

	m_bLoadingFile = false;

    return true;
}

bool ChameleonEditor::Modified () 
{
    // return modified state
	bool modified = GetModify();
	bool readonly = !GetReadOnly();
	bool canundo = CanUndo();
	
	

	bool returnModify = modified && readonly && canundo;
    return returnModify; 
}

// called every time a character is entered.  currently not actually doing much.
void ChameleonEditor::OnChar( wxStyledTextEvent &event )
{

	const wxChar chr = event.GetKey();
	const int currentLine = GetCurrentLine();
	const int tabWidth = GetTabWidth();
	const int eolMode = GetEOLMode();

	// TODO Block indent/dedent isn't workng yet.  OnChar not being called?

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

bool ChameleonEditor::HasBeenSaved()
{
	bool result = m_fileNameAndPath.GetFullPath() != wxEmptyString;
	return result;
}

/*
void ChameleonEditor::SetRemoteFileNameAndPath(wxString path, wxString name)
{
	m_remoteFileName.Assign(path, name, wxPATH_UNIX);
}

void ChameleonEditor::SetLocalFileNameAndPath(wxString path, wxString name)
{
	m_localFileName.Assign(path, name, wxPATH_DOS);
}
*/

void ChameleonEditor::UpdateSyntaxHighlighting()
{
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
		this->Colourise(0, -1);
	}
	else
	{
		this->SetLexer(wxSTC_LEX_NULL);
		this->ClearDocumentStyle();
	}
}

//void ChameleonEditor::SetFileNameAndPath(wxString path, wxString name, bool fileIsRemote)
void ChameleonEditor::SetFilename(wxFileName filename, bool fileIsRemote)
{
	m_bLastSavedRemotely = fileIsRemote;

	//m_fileNameAndPath.Assign(path, name, fileIsRemote ? wxPATH_UNIX : wxPATH_DOS);
	m_fileNameAndPath = filename;
}

wxString ChameleonEditor::GetFileNameAndPath()
{
	wxString nameAndPath = m_fileNameAndPath.GetFullPath(m_bLastSavedRemotely ? wxPATH_UNIX : wxPATH_DOS);
	return nameAndPath;
}

wxString ChameleonEditor::GetFilenameString()
{
	return m_fileNameAndPath.GetFullName();
}

wxFileName ChameleonEditor::GetFileName() {
	return m_fileNameAndPath;
}

wxString ChameleonEditor::GetFilePath()
{
	return m_fileNameAndPath.GetPath(false, m_bLastSavedRemotely ? wxPATH_UNIX : wxPATH_DOS);
}

void ChameleonEditor::ResetEditor()
{
	ClearAll();
	SetSavePoint();
	EmptyUndoBuffer();
	m_fileNameAndPath.Clear();
}

void ChameleonEditor::OnRightClick(wxMouseEvent &event)
{

	bool breakpointsAllowed = !m_mainFrame->IsDebugging();

	m_popupMenu.Enable(ID_DEBUG_ADD_BREAKPOINT, breakpointsAllowed);
	m_popupMenu.Enable(ID_DEBUG_REMOVE_BREAKPOINT, breakpointsAllowed);
	m_popupMenu.Enable(ID_DEBUG_RUNTOCURSOR, breakpointsAllowed);

	PopupMenu(&m_popupMenu, event.GetPosition());
}

void ChameleonEditor::OnEditorModified(wxStyledTextEvent &event)
{
	int modType = event.GetModificationType();

	wxString debugMessage;
	wxString message;

	switch(modType)
	{
		case wxSTC_MOD_INSERTTEXT:
			message = "Insert mod.";
			break;
		case wxSTC_MOD_DELETETEXT:
			message = "Delete mod.";
			break;
	}


	
}