/////////////////////////////////////////////////////////////////////////////
// Name:        RemoteFileDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     01/12/04 16:32:27
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "RemoteFileDialog.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef MSVC6
#include "../../common/fixvsbug.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

////@begin includes
#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/statline.h"
////@end includes

#include "RemoteFileDialog.h"
#include "../ChameleonWindow.h"
#include "../../network/networking.h"
//#include "../../editor/editor.h"

////@begin XPM images
////@end XPM images

#include "cpp.xpm"
#include "c.xpm"
#include "h.xpm"
#include "folder256.xpm"
#include "defaultfile.xpm"

/*!
 * RemoteFileDialog type definition
 */

IMPLEMENT_CLASS( RemoteFileDialog, wxDialog )

/*!
 * RemoteFileDialog event table definition
 */

BEGIN_EVENT_TABLE( RemoteFileDialog, wxDialog )

////@begin RemoteFileDialog event table entries
    EVT_BUTTON( ID_BUTTONUPFOLDER, RemoteFileDialog::OnButtonUpClick )

    EVT_LIST_ITEM_SELECTED( ID_LISTCTRL, RemoteFileDialog::OnItemSelected )
    EVT_LIST_ITEM_ACTIVATED( ID_LISTCTRL, RemoteFileDialog::OnItemActivated )

    EVT_BUTTON( ID_BUTTONOPEN, RemoteFileDialog::OnButtonOpenClick )

    EVT_BUTTON( ID_BUTTONCANCEL, RemoteFileDialog::OnButtonCancelClick )

////@end RemoteFileDialog event table entries

END_EVENT_TABLE()

/*!
 * RemoteFileDialog constructors
 */

RemoteFileDialog::RemoteFileDialog( )
{

}

RemoteFileDialog::RemoteFileDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);

	m_parentFrame = (ChameleonWindow*)parent;

	m_currentPath.AssignDir("~");
	m_openMode = true;
}

/*!
 * RemoteFileDialog creator
 */

bool RemoteFileDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RemoteFileDialog member initialisation
////@end RemoteFileDialog member initialisation

////@begin RemoteFileDialog creation
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end RemoteFileDialog creation

	wxImageList* images = new wxImageList(16, 16);


	wxBitmap folder(folder256_xpm);
	images->Add(folder);

	wxBitmap standardc(c_xpm);
	images->Add(standardc);

	wxBitmap cpp(cpp_xpm);
	images->Add(cpp);

	wxBitmap h(h_xpm);
	images->Add(h);	

	wxBitmap defaultfile(defaultfile_xpm);
	images->Add(defaultfile);

	m_list->AssignImageList(images, wxIMAGE_LIST_SMALL);
	

	
    return TRUE;
}

/*!
 * Control creation for RemoteFileDialog
 */

void RemoteFileDialog::CreateControls()
{    
////@begin RemoteFileDialog content construction

    RemoteFileDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxBoxSizer* item3 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item3, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxStaticText* item4 = new wxStaticText( item1, wxID_STATIC, _("Current path:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item4, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item5 = new wxTextCtrl( item1, ID_PATHBOX, _(""), wxDefaultPosition, wxSize(260, -1), wxTE_READONLY );
    m_pathBox = item5;
    item3->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxButton* item6 = new wxButton( item1, ID_BUTTONUPFOLDER, _("Up one folder level"), wxDefaultPosition, wxSize(100, -1), 0 );
    item3->Add(item6, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxListCtrl* item7 = new wxListCtrl( item1, ID_LISTCTRL, wxDefaultPosition, wxSize(450, 280), wxLC_LIST  );
    m_list = item7;
    item2->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item8, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item9 = new wxBoxSizer(wxHORIZONTAL);
    item8->Add(item9, 0, wxGROW|wxALL, 0);

    wxStaticText* item10 = new wxStaticText( item1, wxID_STATIC, _("File &name:"), wxDefaultPosition, wxSize(90, -1), 0 );
    item9->Add(item10, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxTextCtrl* item11 = new wxTextCtrl( item1, ID_TXTFILENAME, _(""), wxDefaultPosition, wxSize(246, -1), 0 );
    m_txtFilename = item11;
    item9->Add(item11, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticLine* item12 = new wxStaticLine( item1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item9->Add(item12, 0, wxGROW|wxLEFT|wxRIGHT, 10);

    wxButton* item13 = new wxButton( item1, ID_BUTTONOPEN, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonOpen = item13;
    item9->Add(item13, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* item14 = new wxBoxSizer(wxHORIZONTAL);
    item8->Add(item14, 0, wxGROW|wxALL, 0);

    wxStaticText* item15 = new wxStaticText( item1, wxID_STATIC, _("Files of &type:"), wxDefaultPosition, wxSize(90, -1), 0 );
    item14->Add(item15, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxString item16Strings[] = {
        _("C++ files (*.h, *.c, *.cpp)"),
        _("All files (*.*)")
    };
    wxComboBox* item16 = new wxComboBox( item1, ID_COMBOBOX1, _("C++ files (*.h, *.c, *.cpp)"), wxDefaultPosition, wxSize(246, -1), 2, item16Strings, wxCB_READONLY );
    m_comboFiletypes = item16;
    item16->SetStringSelection(_("C++ files (*.h, *.c, *.cpp)"));
    item14->Add(item16, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticLine* item17 = new wxStaticLine( item1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item14->Add(item17, 0, wxGROW|wxLEFT|wxRIGHT, 10);

    wxButton* item18 = new wxButton( item1, ID_BUTTONCANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonCancel = item18;
    item14->Add(item18, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

////@end RemoteFileDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void RemoteFileDialog::OnButtonOpenClick( wxCommandEvent& event )
{
    // Insert custom code here

	ItemActivated();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void RemoteFileDialog::OnButtonCancelClick( wxCommandEvent& event )
{
    // Insert custom code here
    EndModal(wxCANCEL);
}

/*!
 * Should we show tooltips?
 */

bool RemoteFileDialog::ShowToolTips()
{
  return TRUE;
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
 */

void RemoteFileDialog::OnButtonUpClick( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxPathFormat pathFormat = GetCurrentPathFormat();

	int numDirs = m_currentPath.GetDirCount();

	if(numDirs > 0)
	{
		wxString currPath = m_currentPath.GetPath(false, wxPATH_UNIX);

		if(currPath != "~")
		{
			m_currentPath.RemoveDir(numDirs - 1);
		}
	}
	wxString path = m_currentPath.GetPath();
	ShowDirectory(m_currentPath.GetPath(false, wxPATH_UNIX));
}

wxString RemoteFileDialog::GetRemoteFileNameAndPath()
{
	return m_remoteFileNamePath.GetFullPath(wxPATH_UNIX);
}

wxString RemoteFileDialog::GetLocalFileNameAndPath()
{
	return m_localFileNamePath.GetFullPath();
}


void RemoteFileDialog::SetNetworking(Networking* network)
{
	m_network = network;

	//wxString userHome = m_network->GetHomeDirPath();
	//m_parentFrame->CheckNetworkStatus();
	m_currentPath.AssignDir("~");
}

bool RemoteFileDialog::ShowDirectory(wxString dirname)
{
	//wxBeginBusyCursor();
// disables VS's 4018 warning: "signed/unsigned mismatch" relating to the GetCount() comparison
#pragma warning( disable : 4018)

	/*
	if(dirname.Contains("/"))
	{
		m_currentPath.AssignDir(dirname, wxPATH_UNIX);
	}
	else
	{
		m_currentPath.AssignDir(dirname, wxPATH_NATIVE);
	}
	*/

	m_currentPath.AssignDir(dirname);

	wxString displayPath = dirname;

	/*
	if(displayPath.Left(1) != "/")
	{
		displayPath = "/" + displayPath;
	}
	*/

	m_pathBox->SetValue(displayPath);
	

	m_list->ClearAll();

	DirListing dl;	
	dl= m_network->GetDirListing(dirname);

	NetworkCallResult netStatus = m_parentFrame->CheckNetworkStatus();
	if(netStatus == NETCALL_REDO)
	{
		dl = m_network->GetDirListing(dirname);
	}
	else if(netStatus == NETCALL_FAILED)
	{
		return false;
	}


	wxSortedArrayString sortedDirs(dl.dirNames);
	wxSortedArrayString sortedFiles(dl.fileNames);

	m_currentDirs.Clear();
	m_currentFiles.Clear();



	for(int i = 0; i < sortedDirs.GetCount(); i++)
	{
		wxString fullDirName = sortedDirs[i];

		wxString lastDir = fullDirName.AfterLast(wxChar('\\'));
		m_list->InsertItem(m_list->GetItemCount(), lastDir, 0);
		m_currentDirs.Add(lastDir);
		//wxLogDebug("Dir: " + sortedDirs[i]);
	}

	int fileType = m_comboFiletypes->GetSelection();

	wxSortedArrayString cppFiles;
	wxSortedArrayString cFiles;
	wxSortedArrayString hFiles;

	switch(fileType)
	{
		// C++ files
	case 0:


		for(int i = 0; i < sortedFiles.GetCount(); i++)
		{
			wxFileName currentName(sortedFiles[i]);
			wxString ext = currentName.GetExt();
			wxString file = currentName.GetFullName();
			m_currentFiles.Add(file);


			if(ext == "cpp")
			{
				cppFiles.Add(file);
			}
			else if(ext == "c")
			{
				cFiles.Add(file);
			}
			else if(ext == "h")
			{
				hFiles.Add(file);
			}
		}

		for(int i = 0; i < cFiles.GetCount(); i++)
		{
			m_list->InsertItem(m_list->GetItemCount(), cFiles[i], 1);
			//wxLogDebug(".C file: " + cFiles[i]);
		}

		for(int i = 0; i < cppFiles.GetCount(); i++)
		{
			m_list->InsertItem(m_list->GetItemCount(), cppFiles[i], 2);
			//wxLogDebug(".CPP file: " + cppFiles[i]);
		}

		for(int i = 0; i < hFiles.GetCount(); i++)
		{
			m_list->InsertItem(m_list->GetItemCount(), hFiles[i], 3);
			//wxLogDebug(".H file: " + hFiles[i]);
		}
		break;
	case 1:
		for(int i = 0; i < m_currentFiles.GetCount(); i++)
		{
			wxFileName currentName(m_currentFiles[i]);
			wxString ext = currentName.GetExt();
			wxString file = currentName.GetFullName();
			m_currentFiles.Add(file);

			int iconNum = 4; // default file icon

			if(ext == "c")
			{
				iconNum = 1;
			}
			else if(ext == "cpp")
			{
				iconNum = 2;
			}
			else if(ext == "h")
			{
				iconNum = 3;
			}

			m_list->InsertItem(m_list->GetItemCount(), file, iconNum);
			//wxLogDebug("All files: " + file);
		}
		break;
	default:
		wxLogDebug("RemoteFileDialog::LoadTestData: file type dropdown hit default");
		break;
	}

	m_pathBox->SetSelection(0,0);
	m_list->SetFocus();

#pragma warning( default : 4018 )

	//wxEndBusyCursor();
	return true;
}

void RemoteFileDialog::LoadTestData()
{
	ShowDirectory("C:\\Projects\\cs4810\\gears\\chameleon\\gui\\");
}

void RemoteFileDialog::OpenRemoteFile()
{
	wxString remoteFileName = m_txtFilename->GetValue();
	//wxString localFileName = m_network->GetFile(remoteFileName, m_currentPath.GetPath());


	m_remoteFileNamePath.SetName(remoteFileName);
	m_remoteFileNamePath.SetPath(m_currentPath.GetFullPath());

	//m_localFileNamePath.Assign(localFileName);


	EndModal(wxOK);

}

void RemoteFileDialog::SaveRemoteFile()
{
	wxString remoteFileName = m_txtFilename->GetValue();

	wxFileName localfn(wxGetCwd(), remoteFileName);
	wxFile(localfn.GetFullPath(), wxFile::write);

	m_remoteFileNamePath.Assign(m_currentPath.GetPath(false, wxPATH_UNIX), remoteFileName);

	EndModal(wxOK);

	// = m_network->SendFile()
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL
 */

void RemoteFileDialog::OnItemSelected( wxListEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxString itemText = event.GetText();

	//int result = m_currentDirs.Index(itemText);

	//if(result == wxNOT_FOUND)
	//{
		m_txtFilename->SetValue(itemText);
	//}	
}

/*!
 * wxEVT_COMMAND_LIST_ITEM_ACTIVATED event handler for ID_LISTCTRL
 */

void RemoteFileDialog::OnItemActivated( wxListEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxString itemText = event.GetText();

	m_txtFilename->SetValue(itemText);

	ItemActivated();
	
	
}

wxPathFormat RemoteFileDialog::GetCurrentPathFormat()
{
	wxPathFormat pathFormat = wxPATH_UNIX;


	return pathFormat;
}

bool RemoteFileDialog::Prepare(bool open)
{
	m_openMode = open;

	if(m_openMode)
	{
		SetTitle("Open File");
		m_buttonOpen->SetLabel("Open");
	}
	else
	{
		SetTitle("Save File As");
		m_buttonOpen->SetLabel("Save");
	}

	wxPathFormat format = GetCurrentPathFormat();

	wxString path = m_currentPath.GetPath(false, wxPATH_UNIX);
	return ShowDirectory(path);
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONOPEN
 */


void RemoteFileDialog::ItemActivated()
{
	wxString text = m_txtFilename->GetValue();

	int dirResult = m_currentDirs.Index(text);

	if(dirResult == wxNOT_FOUND)
	{		
		int fileResult = m_currentFiles.Index(text);

		if(fileResult == wxNOT_FOUND)
		{
			// trying to open a non-existent file
			if(m_openMode)
			{
				wxString errorMessage = "\"" + text + "\" was not found.  Please enter or select a valid file name.";
				
				wxMessageBox(errorMessage, "Open", wxOK | wxICON_EXCLAMATION);
				return;
			}
			// trying to save a file and it's a new file
			else
			{
				SaveRemoteFile();
			}
		}
		else
		{
			// filename found, open it
			if(m_openMode)
			{
				OpenRemoteFile();
			}
			// file already exists - confirm overwrite
			else
			{
				wxString confirmMessage = text + " already exists.  Do you want to replace it?";
				int result = wxMessageBox(confirmMessage, "Save As", wxYES_NO | wxICON_QUESTION);

				if(result == wxYES)
				{
					OpenRemoteFile();
				}
				else
				{
					return;
				}
			}
		}
	}	
	else
	{
		// call ShowDirectory w/ new directory
		m_currentPath.AppendDir(text);

		ShowDirectory(m_currentPath.GetPath(false, wxPATH_UNIX));
	}
}