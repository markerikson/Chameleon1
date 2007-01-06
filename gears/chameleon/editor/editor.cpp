#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../common/CommonHeaders.h"

#include <wx/datetime.h>

#include "editor.h"
#include "../perms/p.h"
#include "../gui/ChameleonWindow.h"
#include "../gui/ChameleonNotebook.h"
#include "../common/datastructures.h"
#include "../common/Options.h"
#include "../common/DebugEvent.h"
#include "../common/ProjectInfo.h"
#include "../common/debug.h"
#include "../common/fixvsbug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)
	EVT_STC_CHARADDED(-1, ChameleonEditor::OnChar)
	EVT_STC_MODIFIED(-1, ChameleonEditor::OnEditorModified)
	EVT_RIGHT_DOWN		(ChameleonEditor::OnRightClick)
	EVT_MENU			(ID_DEBUG_ADD_BREAKPOINT, ChameleonEditor::OnAddBreakpoint)
	EVT_MENU			(ID_DEBUG_REMOVE_BREAKPOINT, ChameleonEditor::OnRemoveBreakpoint)
	EVT_MENU			(ID_DEBUG_CLEAR_ALL_BREAKPOINTS, ChameleonEditor::OnClearBreakpoints)
	//EVT_COMPILER_END	(ChameleonEditor::OnCompilerEnded)
	EVT_MENU			(ID_DEBUG_RUNTOCURSOR, ChameleonEditor::OnRunToCursor)
END_EVENT_TABLE()

int CompareInts(int n1, int n2)
{
	return n1 - n2;
}




//////////////////////////////////////////////////////////////////////////////
///  public constructor ChameleonEditor
///  General-purpose editor creation and stuff
///
///  @param  mframe  ChameleonWindow * A pointer to the main application frame
///  @param  options Options *         A pointer to the main Options object
///  @param  project ProjectInfo *     A pointer to this editor's project
///  @param  parent  wxWindow *        The editor's parent window
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
ChameleonEditor::ChameleonEditor( ChameleonWindow *mframe,
								 Options* options,
								 ProjectInfo* project,
                                  wxWindow *parent,     wxWindowID id, const
                                      wxPoint & pos /* = wxDefaultPosition */,
                                  const wxSize & size /* = wxDefaultSize */,
                                  long style /* = 0 */, const
                                      wxString & name /* = wxSTCNameStr */ ) :
    wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    m_mainFrame = mframe;
	m_parentNotebook = (ChameleonNotebook*)parent;
	m_options = options;
	m_project = project;
	m_project->AddEditor(this);

	m_bLoadingFile = false;
	m_bLastSavedRemotely = true;
	m_bHasBeenCompiled = false;

	m_fileNameAndPath.Assign(wxEmptyString);

	m_lastRightClick.x = -1;
	m_lastRightClick.y = -1;

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

	m_bNewFile = true;
	UpdateSyntaxHighlighting();
	m_bNewFile = false;

	this->MarkerDefine(MARKER_BREAKPOINT, wxSTC_MARK_CIRCLE);
	this->MarkerSetBackground(MARKER_BREAKPOINT, wxColour("red"));

	//this->MarkerDefine(1, wxSTC_MARK_CIRCLE);
	//this->MarkerSetForeground(1, wxColour("red"));

	this->MarkerDefine(MARKER_FOCUSEDLINE, wxSTC_MARK_SHORTARROW);
	this->MarkerSetBackground(MARKER_FOCUSEDLINE, wxColour("yellow"));

	
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
	m_popupMenu.Append(ID_DEBUG_CLEAR_ALL_BREAKPOINTS, "Clear all breakpoints");

	m_popupMenu.Append(ID_DEBUG_RUNTOCURSOR, "Run to cursor");

	int modmask =	wxSTC_MOD_INSERTTEXT 
					| wxSTC_MOD_DELETETEXT
					//| wxSTC_MOD_CHANGESTYLE
					| wxSTC_PERFORMED_UNDO 
					| wxSTC_PERFORMED_REDO;

	this->SetModEventMask(modmask);

}

//////////////////////////////////////////////////////////////////////////////
///  public destructor ~ChameleonEditor
///  Handles the pseudo-reference counting for the editor's project
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
ChameleonEditor::~ChameleonEditor() 
{
    if(m_project->IsSingleFile())
	{
		delete m_project;
	}
	else
	{
		m_project->RemoveEditor(this);
	}
}



//////////////////////////////////////////////////////////////////////////////
///  public SaveFileLocal
///  Saves the editor's contents with the current filename
///
///  @return bool Whether or not the save succeeded
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonEditor::SaveFileLocal()
{
	return SaveFile(m_fileNameAndPath.GetFullPath());
}

//////////////////////////////////////////////////////////////////////////////
///  public SaveFile
///  Saves the editor's contents with the given filename
///
///  @param  filename const wxString & The filename to save to
///
///  @return bool     Whether or not the save succeeded
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public LoadFileText
///  Loads a file from the given string
///
///  @param  fileContents wxString  The text of the file
///
///  @return bool         Whether or not the load succeeded
///
///  @remarks  This isn't actually used right now... probably ought to be cleaned up
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public Modified
///  Checks whether or not the editor has been modified
///
///  @return bool Whether or not the editor has been modified
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonEditor::Modified () 
{
    // return modified state
	bool modified = GetModify();
	bool readonly = !GetReadOnly();
	bool canundo = CanUndo();
	
	

	bool isModified = modified && readonly && canundo;
    return isModified; 
}

//////////////////////////////////////////////////////////////////////////////
///  public OnChar
///  Handles auto-indentation and such whenever the user enters a character
///
///  @param  event wxStyledTextEvent & The generated event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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
	if(chr == '\n' && m_options->GetPerms()->isEnabled(PERM_AUTOINDENT))
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

//////////////////////////////////////////////////////////////////////////////
///  public HasBeenSaved
///  Checks if the editor has been saved
///
///  @return bool Whether or not the editor has been saved
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public UpdateSyntaxHighlighting
///  Sets up the editor's syntax highlighting
///
///  @return void
///
///  @remarks Currently only called on creation.  If syntax highlighting customization was
///  @remarks allowed, this is where the user's choices would be used
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::UpdateSyntaxHighlighting()
{
	wxString filename = GetFileNameAndPath();

	if( m_options->GetPerms()->isEnabled(PERM_SYNTAXHIGHLIGHT) &&
		( m_bNewFile || (m_project->GetFileType(filename) != FILE_NONSOURCE) || !HasBeenSaved())
		)
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

//////////////////////////////////////////////////////////////////////////////
///  public SetFilename
///  Sets the filename for the editor
///
///  @param  filename     wxFileName  The filename for this editor
///  @param  fileIsRemote bool        Whether this file is remote or local
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::SetFilename(wxFileName filename, bool fileIsRemote)
{
	m_bLastSavedRemotely = fileIsRemote;

	//m_fileNameAndPath.Assign(path, name, fileIsRemote ? wxPATH_UNIX : wxPATH_DOS);

	if(m_project->IsSingleFile())
	{
		wxString oldFileName = m_fileNameAndPath.GetFullPath(m_bLastSavedRemotely ? wxPATH_UNIX : wxPATH_DOS);

		if(m_project->FileExistsInProject(oldFileName))
		{
			FileFilterType oldFilterType;
			wxString oldExtension = m_fileNameAndPath.GetExt();
			if(oldExtension.StartsWith("h"))
			{
				oldFilterType = FILE_HEADERS;
			}
			else if(oldExtension.StartsWith("c"))
			{
				oldFilterType = FILE_SOURCES;
			}
			else if(oldExtension.StartsWith("txt"))
			{
				oldFilterType = FILE_NONSOURCE;
			}
			else
			{
				oldFilterType = FILE_NONSOURCE;
			}

			m_project->RemoveFileFromProject(oldFileName, oldFilterType);
		}

		wxString newFileName = filename.GetFullPath(fileIsRemote ? wxPATH_UNIX : wxPATH_DOS);
		if(!m_project->FileExistsInProject(newFileName))
		{
			FileFilterType newFilterType;
			wxString newExtension = filename.GetExt();
			if(newExtension.StartsWith("h"))
			{
				newFilterType = FILE_HEADERS;
			}
			else if(newExtension.StartsWith("c"))
			{
				newFilterType = FILE_SOURCES;
			}
			else if(newExtension.StartsWith("txt"))
			{
				newFilterType = FILE_NONSOURCE;
			}
			else
			{
				newFilterType = FILE_NONSOURCE;
			}
			m_project->AddFileToProject(newFileName, newFilterType);
		}		

		m_project->SetRemote(fileIsRemote);
	}	

	m_fileNameAndPath = filename;
}

//////////////////////////////////////////////////////////////////////////////
///  public GetFileNameAndPath
///  Gets the full pathname of this file as a string
///
///  @return wxString The full pathname of this file
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString ChameleonEditor::GetFileNameAndPath()
{
	wxString nameAndPath = m_fileNameAndPath.GetFullPath(m_bLastSavedRemotely ? wxPATH_UNIX : wxPATH_DOS);
	return nameAndPath;
}

//////////////////////////////////////////////////////////////////////////////
///  public GetFilenameString
///  Gets the name of this file with no path
///
///  @return wxString The name of this file
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString ChameleonEditor::GetFilenameString()
{
	return m_fileNameAndPath.GetFullName();
}

//////////////////////////////////////////////////////////////////////////////
///  public GetFileName
///  Gets the wxFileName for this file
///
///  @return wxFileName The editor's filename
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxFileName ChameleonEditor::GetFileName() {
	return m_fileNameAndPath;
}

//////////////////////////////////////////////////////////////////////////////
///  public GetFilePath
///  Gets the path for this file
///
///  @return wxString The path for this file
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxString ChameleonEditor::GetFilePath()
{
	return m_fileNameAndPath.GetPath(false, m_bLastSavedRemotely ? wxPATH_UNIX : wxPATH_DOS);
}

//////////////////////////////////////////////////////////////////////////////
///  public ResetEditor
///  Clears out the editor's contents and resets it completely
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::ResetEditor()
{
	ClearAll();
	
	m_fileNameAndPath.Clear();
	m_breakpoints.Clear();

	SetReadOnly(false);
	SetText(wxEmptyString);
	SetSavePoint();
	EmptyUndoBuffer();
	
	MarkerDeleteAll(MARKER_BREAKPOINT);
	MarkerDeleteAll(MARKER_FOCUSEDLINE);

	if(m_project != NULL && m_project->IsSingleFile())
	{
		delete m_project;
	}
	
	m_project = new ProjectInfo();
}

//////////////////////////////////////////////////////////////////////////////
///  public OnRightClick
///  Handles a right-click in the editor
///
///  @param  event wxMouseEvent & The generated mouse event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::OnRightClick(wxMouseEvent &event)
{
	m_lastRightClick = event.GetPosition();
	int charpos = PositionFromPoint(m_lastRightClick);
	int linenum = LineFromPosition(charpos);
	int markerLineBitmask = this->MarkerGet(linenum);

	bool breakpointOnLine = (markerLineBitmask & (1 << MARKER_BREAKPOINT));

	bool breakpointsAllowed = true;
	bool isDebugging = m_mainFrame->IsDebugging();
	
	if(isDebugging)
	{
		breakpointsAllowed = m_mainFrame->IsDebuggerPaused();
	}

	m_popupMenu.Enable(ID_DEBUG_ADD_BREAKPOINT, breakpointsAllowed && !breakpointOnLine);
	m_popupMenu.Enable(ID_DEBUG_REMOVE_BREAKPOINT, breakpointsAllowed && breakpointOnLine);
	m_popupMenu.Enable(ID_DEBUG_RUNTOCURSOR, breakpointsAllowed && isDebugging);

	// returns a copy of a member variable, which would seem sort of pointless, but
	// GetBreakpoints cleans up any stray marker IDs in the list before returning
	// so we have an accurate count of how many breakpoints there are
	wxArrayInt currentBreakpoints = GetBreakpoints();
	bool canClearBreakpoints = currentBreakpoints.GetCount() > 0;

	m_popupMenu.Enable(ID_DEBUG_CLEAR_ALL_BREAKPOINTS, canClearBreakpoints);

	
	PopupMenu(&m_popupMenu, m_lastRightClick);
}

//////////////////////////////////////////////////////////////////////////////
///  private OnEditorModified
///  Updates the editor's project when the editor is modified
///
///  @param  event wxStyledTextEvent & The generated editor event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::OnEditorModified(wxStyledTextEvent &event)
{
	m_project->SetCompiled(false);
}

//////////////////////////////////////////////////////////////////////////////
///  private OnAddBreakpoint
///  Adds a breakpoint to this file
///
///  @param  event wxCommandEvent & The generated menu event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::OnAddBreakpoint(wxCommandEvent &event)
{
	//int charpos = PositionFromPoint(m_lastRightClick);
	int linenum = GetLineForBreakpointOperation(); //LineFromPosition(charpos);

	int markerNum = this->MarkerAdd(linenum, MARKER_BREAKPOINT);
	
	m_breakpoints.Add(markerNum);
	CreateBreakpointEvent(linenum, true);	

	ResetRightClickLocation();
}

//////////////////////////////////////////////////////////////////////////////
///  private OnRemoveBreakpoint
///  Removes a breakpoint from this file
///
///  @param  event wxCommandEvent & The generated menu event
///
///  @return void
///
///  @remarks This doesn't clean out the marker handle that STC gives us, 
///  @remarks since there's no way to check what marker handles are on a given line.
///  @remarks Orphaned marker handles are cleaned up in GetBreakpoints.
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::OnRemoveBreakpoint(wxCommandEvent &event)
{
	int linenum = GetLineForBreakpointOperation();

	// need to remove the marker handle from the array - use
	// LineFromHandle on debug start and clean up then
	this->MarkerDelete(linenum, MARKER_BREAKPOINT);
	CreateBreakpointEvent(linenum, false);

	ResetRightClickLocation();
}

//////////////////////////////////////////////////////////////////////////////
///  private OnClearBreakpoints
///  Clears all breakpoints from this file
///
///  @param  event wxCommandEvent & The generated menu event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::OnClearBreakpoints(wxCommandEvent &event)
{
	// m_breakpoints should have been cleared of any orphaned marker
	// handles during the right-click that led us here
	//int numBreakpoints = m_breakpoints.GetCount();
	int numBreakpoints = GetBreakpoints().GetCount();

	for(int i = 0; i < numBreakpoints; i++)
	{
		int markerHandle = m_breakpoints[i];
		int linenum = this->MarkerLineFromHandle(markerHandle);
		this->MarkerDeleteHandle(markerHandle);
		CreateBreakpointEvent(linenum, false);
	}

	ResetRightClickLocation();
}

//////////////////////////////////////////////////////////////////////////////
///  public GetBreakpoints
///  Gets a list of all breakpoint line numbers.  Also clears out any invalid (removed) breakpoint IDs.
///
///  @return wxArrayInt The line numbers for all the breakpoints in this file
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
wxArrayInt ChameleonEditor::GetBreakpoints()
{
	wxArrayInt linenumbers;
	wxArrayInt invalidBreakpoints;

	int numStoredBreakpoints = m_breakpoints.GetCount();
	for(int i = 0; i < numStoredBreakpoints; i++)
	{
		int markerHandle = m_breakpoints[i];

		int linenum = this->MarkerLineFromHandle(markerHandle);

		if(linenum != -1)
		{
			linenumbers.Add(linenum + 1);
		}
		else
		{
			invalidBreakpoints.Add(markerHandle);
		}
	}

	for(int i = 0; i < (int)invalidBreakpoints.GetCount(); i++)
	{
		m_breakpoints.Remove(invalidBreakpoints[i]);
	}

	linenumbers.Sort((CMPFUNC_wxArraywxArrayInt)CompareInts);
	return linenumbers;
}

//////////////////////////////////////////////////////////////////////////////
///  public HasBeenCompiled
///  Returns the compiled status for this editor's project
///
///  @return bool Whether or not the editor's project has been compiled
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
bool ChameleonEditor::HasBeenCompiled()
{
	return m_project->IsCompiled();
}

//////////////////////////////////////////////////////////////////////////////
///  public SetCompiled
///  Set this editor's project's compiled status
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::SetCompiled()
{
	m_project->SetCompiled(true);
}

//////////////////////////////////////////////////////////////////////////////
///  public FocusOnLine
///  Moves the cursor to the given line number, optionally showing a highlight marker
///
///  @param  linenumber int   The line to go to
///  @param  showMarker bool  [=true] Whether or not to mark the line
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::FocusOnLine(int linenumber, bool showMarker)
{
	GotoLine(linenumber);
	if(showMarker)
	{
		MarkerDeleteAll(MARKER_FOCUSEDLINE);
		MarkerAdd(linenumber, MARKER_FOCUSEDLINE);
	}
}
//////////////////////////////////////////////////////////////////////////////
///  private CreateBreakpointEvent
///  Sets up a debug event when a breakpoint is added or deleted
///
///  @param  linenumber    int   The line number of the toggled breakpoint
///  @param  addBreakpoint bool  Whether the breakpoint is being added or deleted
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::CreateBreakpointEvent(int linenumber, bool addBreakpoint)
{
	wxDebugEvent dbg;
	wxString filename = m_fileNameAndPath.GetFullPath(wxPATH_UNIX);
	//wxArrayString sources;
	//sources.Add(filename);
	dbg.SetSourceFilename(filename);
	// adjust for the zero-based index
	dbg.SetLineNumber(linenumber + 1);
	int type = addBreakpoint ? ID_DEBUG_ADD_BREAKPOINT : ID_DEBUG_REMOVE_BREAKPOINT;
	dbg.SetId(type);
	dbg.SetId(type);
	dbg.SetProject(m_project);
	m_mainFrame->AddPendingEvent(dbg);
}


//////////////////////////////////////////////////////////////////////////////
///  public SetProject
///  Sets the project for this editor
///
///  @param  project ProjectInfo * The new project
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::SetProject(ProjectInfo* project)
{
	if(m_project != NULL && m_project->IsSingleFile())
	{
		delete m_project;
	}

	m_project = project;
	m_project->AddEditor(this);
}

//////////////////////////////////////////////////////////////////////////////
///  private OnRunToCursor
///  Creates a "one-shot" breakpoint and tells the debugger to run to that line
///
///  @param  event wxCommandEvent & The generated menu event
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
void ChameleonEditor::OnRunToCursor(wxCommandEvent &event)
{
	/*
	int charpos = PositionFromPoint(m_lastRightClick);
	int linenum = LineFromPosition(charpos);
	// adjust for Scintilla's internal zero-based line numbering
	linenum++;
	*/

	int linenum = GetLineForBreakpointOperation();
	wxDebugEvent debugEvent;
	debugEvent.SetId(ID_DEBUG_RUNTOCURSOR);	
	debugEvent.SetSourceFilename(GetFilenameString());
	debugEvent.SetLineNumber(linenum);
	m_mainFrame->AddPendingEvent(debugEvent);

	ResetRightClickLocation();
}

int ChameleonEditor::GetLineForBreakpointOperation()
{
	int lineNum = 0;

	if(m_lastRightClick.x < 0 || m_lastRightClick.y < 0)
	{
		lineNum =  GetCurrentLine();		
	}
	else
	{
		int charpos = PositionFromPoint(m_lastRightClick);
		lineNum = LineFromPosition(charpos);
		lineNum++;
	}

	
	return lineNum;
}

void ChameleonEditor::ResetRightClickLocation()
{
	m_lastRightClick.x = -1;
	m_lastRightClick.y = -1;
}