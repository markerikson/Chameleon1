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
	EVT_COMPILER_PROBLEM(CompilerOutputPanel::OnCompilerEnd)
	EVT_COMPILER_END(CompilerOutputPanel::OnCompilerEnd)

END_EVENT_TABLE()

/*!
 * CompilerOutputPanel constructors
 */

CompilerOutputPanel::CompilerOutputPanel( )
{
}

CompilerOutputPanel::CompilerOutputPanel( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);

	SetAdvanced(false);
	m_grid->SetEditable(false);
	m_grid->EnableGridLines(false);
	m_grid->SetColumnWidth(0, 200);
	m_grid->SetColumnWidth(2, 500);
	m_grid->SetColLabelValue(0, "Filename");
	m_grid->SetColLabelValue(1, "Line");
	m_grid->SetColLabelValue(2, "Message");
}

/*!
 * CompilerOutputPanel creator
 */

bool CompilerOutputPanel::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin CompilerOutputPanel member initialisation
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
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxTextCtrl* item3 = new wxTextCtrl( item1, ID_COMPILERTEXT, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    m_textbox = item3;
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxGrid* item4 = new wxGrid( item1, ID_COMPILERGRID, wxDefaultPosition, wxSize(200, 150), wxSUNKEN_BORDER );
    m_grid = item4;
    item4->SetDefaultColSize(50);
    item4->SetDefaultRowSize(25);
    item4->SetColLabelSize(25);
    item4->SetRowLabelSize(0);
    item4->CreateGrid(5, 3, wxGrid::wxGridSelectRows);
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
	if(advanced)
	{
		m_textbox->Hide();
		m_grid->Show();
	}
	else
	{
		m_grid->Hide();
		m_textbox->Show();
	}
}

void CompilerOutputPanel::OnCompilerStart(CompilerEvent& event)
{
	ClearOutput();

	wxFileName fn = event.GetFile();
	wxString filename = fn.GetFullPath(event.IsRemoteFile() ? wxPATH_UNIX : wxPATH_DOS);

	wxString compileOutput = "Compiling: " + filename;
	if(m_isAdvanced)
	{
		int newRowNum = m_grid->GetNumberRows();
		m_grid->AppendRows(1);
		
		// merge all three cells in the new row into a single cell
		m_grid->SetCellSize(newRowNum, 0, 1, 3);

		m_grid->SetCellValue(compileOutput, newRowNum, 0);
	}
	else
	{
		m_textbox->AppendText(compileOutput + "\r\n");
	}
    
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

	if(m_isAdvanced)
	{
		int newRowNum = m_grid->GetNumberRows();
		m_grid->AppendRows(1);

		m_grid->SetCellValue(filename, newRowNum, 0);
		m_grid->SetCellValue(lineString, newRowNum, 1);
		m_grid->SetCellValue(message, newRowNum, 2);
		m_grid->SetCellRenderer(newRowNum , 2, new wxGridCellAutoWrapStringRenderer);
	}
	else
	{
		*m_textbox << output << "\r\n";
	}

	m_numErrors++;

}

void CompilerOutputPanel::OnCompilerEnd(CompilerEvent &event)
{
	wxFileName fn = event.GetFile();
	wxString filename = fn.GetFullPath(event.IsRemoteFile() ? wxPATH_UNIX : wxPATH_DOS);
	CompileResult cr = event.GetResult();

	wxString compileResult = filename + ": ";
	switch(cr)
	{
		case CR_OK:
			compileResult += "successful compilation.";
			break;
		case CR_ERROR:
		{
		
			wxString errorResult;
			errorResult.Printf("compilation failed. %d total errors.", m_numErrors);
			compileResult += errorResult;
			break;
		}
		case CR_TERMINATED:
			compileResult += "compilation terminated by user.";
			break;
	}

	if(m_isAdvanced)
	{
		int newRowNum = m_grid->GetNumberRows();
		m_grid->AppendRows(1);

		// merge all three cells in the new row into a single cell
		m_grid->SetCellSize(newRowNum, 0, 1, 3);

		m_grid->SetCellValue(compileResult, newRowNum, 0);
	}
	else
	{
		m_textbox->AppendText(compileResult + "\r\n");
	}
}