/////////////////////////////////////////////////////////////////////////////
// Name:        CompilerOutputPanel.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     04/12/04 19:39:08
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "CompilerOutputPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
////@end includes

#include "CompilerOutputPanel.h"
#include "wx/generic/gridctrl.h"
#include "../../compiler/compilerevent.h"
#include "../../common/datastructures.h"
#include "../ChameleonWindow.h"
#include <wx/regex.h>

////@begin XPM images
////@end XPM images

/*!
 * CompilerOutputPanel type definition
 */

IMPLEMENT_CLASS( CompilerOutputPanel, wxPanel )

/*!
 * CompilerOutputPanel event table definition
 */

BEGIN_EVENT_TABLE( CompilerOutputPanel, wxPanel )

////@begin CompilerOutputPanel event table entries
////@end CompilerOutputPanel event table entries
	EVT_COMPILER_START(CompilerOutputPanel::OnCompilerStart)
	EVT_COMPILER_PROBLEM(CompilerOutputPanel::OnCompilerProblem)
	EVT_COMPILER_END(CompilerOutputPanel::OnCompilerEnd)
	EVT_GRID_CELL_LEFT_DCLICK(CompilerOutputPanel::OnGridDoubleClick)

END_EVENT_TABLE()

/*!
 * CompilerOutputPanel constructors
 */

CompilerOutputPanel::CompilerOutputPanel( )
{
}

CompilerOutputPanel::CompilerOutputPanel( wxWindow* parent, ChameleonWindow* mainFrame, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);

	
	SetAdvanced(false);

	wxFont monospacedFont(10, wxMODERN, wxNORMAL, wxNORMAL, false, "Courier New");
	m_textbox->SetFont(monospacedFont);
	m_grid->SetDefaultCellFont(monospacedFont);
	m_grid->SetEditable(false);	
	//m_grid->EnableGridLines(false);
	m_grid->SetGridLineColour(wxColour("black"));
	m_grid->SetColumnWidth(0, 200);
	m_grid->SetColumnWidth(1, 60);
	m_grid->SetColumnWidth(2, 480);
	m_grid->SetColLabelValue(0, "Filename");
	m_grid->SetColLabelValue(1, "Line");
	m_grid->SetColLabelValue(2, "Message");
	m_grid->SetCellHighlightPenWidth(0);

	m_mainFrame = mainFrame;

	ClearOutput();
}

/*!
 * CompilerOutputPanel creator
 */

bool CompilerOutputPanel::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin CompilerOutputPanel member initialisation
    m_sizer = NULL;
    m_textbox = NULL;
    m_grid = NULL;
////@end CompilerOutputPanel member initialisation

////@begin CompilerOutputPanel creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end CompilerOutputPanel creation
    return TRUE;
}

/*!
 * Control creation for CompilerOutputPanel
 */

void CompilerOutputPanel::CreateControls()
{    
////@begin CompilerOutputPanel content construction

    CompilerOutputPanel* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    m_sizer = item2;
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxTextCtrl* item3 = new wxTextCtrl( item1, ID_COMPILERTEXT, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    m_textbox = item3;
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxGrid* item4 = new wxGrid( item1, ID_COMPILERGRID, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER );
    m_grid = item4;
    item4->SetDefaultColSize(80);
    item4->SetDefaultRowSize(20);
    item4->SetColLabelSize(20);
    item4->SetRowLabelSize(0);
    item4->CreateGrid(1, 3, wxGrid::wxGridSelectRows);
    item2->Add(item4, 1, wxGROW|wxALL, 5);

////@end CompilerOutputPanel content construction
}

/*!
 * Should we show tooltips?
 */

bool CompilerOutputPanel::ShowToolTips()
{
    return TRUE;
}

void CompilerOutputPanel::ClearOutput()
{
	int numGridRows = m_grid->GetNumberRows();
	if(numGridRows > 0)
	{
		m_grid->DeleteRows(0, numGridRows);
	}
	
	m_textbox->SetValue(wxEmptyString);
	m_numErrors = 0;
}

void CompilerOutputPanel::SetAdvanced(bool advanced)
{
	m_isAdvanced = advanced;

	m_sizer->Detach(m_grid);
	m_sizer->Detach(m_textbox);

	if(advanced)
	{
		m_textbox->Hide();
		m_sizer->Add(m_grid, 1, wxGROW|wxALL, 5);
		m_grid->Show();
	}
	else
	{
		m_grid->Hide();
		m_sizer->Add(m_textbox, 1, wxGROW|wxALL, 5);
		m_textbox->Show();
	}
	m_sizer->Layout();
}

void CompilerOutputPanel::OnCompilerStart(CompilerEvent& event)
{
	//ClearOutput();

	wxFileName fn = event.GetFile();
	wxString filename = fn.GetFullPath(event.IsRemoteFile() ? wxPATH_UNIX : wxPATH_DOS);

	wxString compileOutput = "Compiling: " + filename;
	if(filename == "Linking") {
		//special case for Linking
		compileOutput = "Linking:";
	}

	//if(m_isAdvanced)
	//{
		int newRowNum = m_grid->GetNumberRows();
		m_grid->AppendRows(1);
		
		// merge all three cells in the new row into a single cell
		m_grid->SetCellSize(newRowNum, 0, 1, 3);

		m_grid->SetCellValue(compileOutput, newRowNum, 0);
	//}
	//else
	//{
		*m_textbox << compileOutput << "\r\n";
	//}
    
}

void CompilerOutputPanel::OnCompilerProblem(CompilerEvent &event)
{
	wxFileName fn = event.GetFile();
	wxString filename = fn.GetFullPath(event.IsRemoteFile() ? wxPATH_UNIX : wxPATH_DOS);

	int linenum = event.GetInt();
	wxString lineString;
	lineString << linenum;
	wxString message = event.GetMessage();
	wxString output = event.GetGCCOutput();	
	wxString outputCopy = output;

	wxRegEx reParseOutput("(?:(?:\\A|\\n)(.+?):(\\d+):(?:\\d+:)?(.+))", wxRE_ADVANCED);

	while(outputCopy != wxEmptyString)
	{
		wxString line = outputCopy.BeforeFirst('\r');
		outputCopy = outputCopy.AfterFirst('\n');

		if(reParseOutput.Matches(line))
		{
			m_numErrors++;
			size_t start = 0;
			size_t length = 0;
			int counter = 1;

			wxString parsedFile = reParseOutput.GetMatch(line, 1);
			wxString parsedLine = reParseOutput.GetMatch(line, 2);
			wxString parsedMessage = reParseOutput.GetMatch(line, 3);

			//if(m_isAdvanced)
			//{
				int newRowNum = m_grid->GetNumberRows();
				m_grid->AppendRows(1);

				m_grid->SetCellValue(parsedFile, newRowNum, 0);
				m_grid->SetCellValue(parsedLine, newRowNum, 1);
				m_grid->SetCellValue(parsedMessage, newRowNum, 2);
				m_grid->SetCellRenderer(newRowNum , 2, new wxGridCellAutoWrapStringRenderer);
			//}
			//else
			//{
			//	*m_textbox << parsedFile << ":" << parsedLine << ":" << parsedMessage << "\r\n";
			//}
		}
	}

	*m_textbox << output;
}

void CompilerOutputPanel::OnCompilerEnd(CompilerEvent &event)
{
	wxFileName fn = event.GetFile();
	wxString filename = fn.GetFullPath(event.IsRemoteFile() ? wxPATH_UNIX : wxPATH_DOS);
	CompileResult cr = event.GetResult();

	wxString compileResult;
	switch(cr)
	{
		case CR_OK:
			compileResult = filename + " compiled successfully.";
			break;
		case CR_ERROR:
		{
			wxString errorResult;
			errorResult.Printf("Compilation failed. %d total errors / warnings.", m_numErrors);
			compileResult = errorResult;
			break;
		}
		case CR_TERMINATED:
			compileResult = "Compilation terminated by user.";
			break;
	}

	//if(m_isAdvanced)
	//{
		int newRowNum = m_grid->GetNumberRows();
		m_grid->AppendRows(1);

		// merge all three cells in the new row into a single cell
		m_grid->SetCellSize(newRowNum, 0, 1, 3);

		m_grid->SetCellValue(compileResult, newRowNum, 0);
	//}
	//else
	//{
		*m_textbox << "\r\n" << compileResult;
	//}
}

void CompilerOutputPanel::OnGridDoubleClick(wxGridEvent &event)
{
	int rownum = event.GetRow();
	int cellSizeRows = 0;
	int cellSizeCols = 0;
	m_grid->GetCellSize(rownum, 0, &cellSizeRows, &cellSizeCols);

	// if this is greater than one, it's a non-problem row
	if(cellSizeCols == 1)
	{
		wxString filename = m_grid->GetCellValue(rownum, 0);
		wxString linestring = m_grid->GetCellValue(rownum, 1);
		long linenum = 0;
		linestring.ToLong(&linenum);

		m_mainFrame->FocusOnLine(filename, (int)linenum, false);
	}
	
}

void CompilerOutputPanel::SetFocus()
{
	if(m_isAdvanced)
	{
		m_grid->SetFocus();
		m_grid->Refresh();
	}
	else
	{
		m_textbox->SetFocus();
		m_textbox->Refresh();
	}
}