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
#include "upfolder.xpm"
#include "refresh.xpm"

/*!
 * RemoteFileDialog type definition
 */

IMPLEMENT_CLASS( RemoteFileDialog, wxDialog )

/*!
 * RemoteFileDialog event table definition
 */

BEGIN_EVENT_TABLE( RemoteFileDialog, wxDialog )

////@begin RemoteFileDialog event table entries
    EVT_LIST_ITEM_SELECTED( ID_LISTCTRL, RemoteFileDialog::OnItemSelected )
    EVT_LIST_ITEM_ACTIVATED( ID_LISTCTRL, RemoteFileDialog::OnItemActivated )

    EVT_BUTTON( ID_BUTTONOPEN, RemoteFileDialog::OnButtonOpenClick )

    EVT_COMBOBOX( ID_COMBOBOX1, RemoteFileDialog::OnCombobox1Selected )

    EVT_BUTTON( ID_BUTTONCANCEL, RemoteFileDialog::OnButtonCancelClick )

////@end RemoteFileDialog event table entries

	EVT_MENU(ID_UPFOLDER, RemoteFileDialog::OnButtonUpFolder)
	EVT_MENU(ID_REFRESHFOLDER, RemoteFileDialog::OnButtonRefresh)
	EVT_TEXT_ENTER(ID_TXTFILENAME, RemoteFileDialog::OnEnter)
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

	m_currentFilterIndex = 0;
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

	// put together the file icons for C++ file types

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

	m_toolbar = new wxToolBar(this, ID_DIALOGTOOLBAR, wxDefaultPosition, wxDefaultSize, 
								wxTB_HORIZONTAL | wxTB_FLAT | wxNO_BORDER | wxTB_NODIVIDER);

	m_sizToolbar->Add(m_toolbar, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

	m_toolbar->SetToolBitmapSize(wxSize(16, 16));


    wxBitmap upfolder(upfolder_xpm);
	//m_butUpFolder->SetBitmapLabel(upfolder);
	m_toolbar->AddTool(ID_UPFOLDER, upfolder, "Go up a folder");

	

	wxBitmap refresh(refresh_xpm);
	//m_butRefresh->SetBitmapLabel(refresh);
	m_toolbar->AddTool(ID_REFRESHFOLDER, refresh, "Refresh this folder");

	m_toolbar->Realize();

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
    m_sizToolbar = item3;
    item2->Add(item3, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    wxStaticText* item4 = new wxStaticText( item1, wxID_STATIC, _("Current path:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item4, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item5 = new wxTextCtrl( item1, ID_PATHBOX, _(""), wxDefaultPosition, wxSize(300, -1), wxTE_READONLY );
    m_pathBox = item5;
    item3->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxListCtrl* item6 = new wxListCtrl( item1, ID_LISTCTRL, wxDefaultPosition, wxSize(440, 280), wxLC_LIST  );
    m_list = item6;
    item2->Add(item6, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item7, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxHORIZONTAL);
    item7->Add(item8, 0, wxGROW|wxALL, 0);

    wxStaticText* item9 = new wxStaticText( item1, wxID_STATIC, _("File &name:"), wxDefaultPosition, wxSize(70, -1), 0 );
    item8->Add(item9, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxTextCtrl* item10 = new wxTextCtrl( item1, ID_TXTFILENAME, _(""), wxDefaultPosition, wxSize(246, -1), wxTE_PROCESS_ENTER );
    m_txtFilename = item10;
    item8->Add(item10, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticLine* item11 = new wxStaticLine( item1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item8->Add(item11, 0, wxGROW|wxLEFT|wxRIGHT, 10);

    wxButton* item12 = new wxButton( item1, ID_BUTTONOPEN, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonOpen = item12;
    item8->Add(item12, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* item13 = new wxBoxSizer(wxHORIZONTAL);
    item7->Add(item13, 0, wxGROW|wxALL, 0);

    wxStaticText* item14 = new wxStaticText( item1, wxID_STATIC, _("Files of &type:"), wxDefaultPosition, wxSize(70, -1), 0 );
    item13->Add(item14, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxString item15Strings[] = {
        _("C++ files (*.h, *.c, *.cpp)"),
        _("All files (*.*)")
    };
    wxComboBox* item15 = new wxComboBox( item1, ID_COMBOBOX1, _("C++ files (*.h, *.c, *.cpp)"), wxDefaultPosition, wxSize(246, -1), 2, item15Strings, wxCB_READONLY );
    m_comboFiletypes = item15;
    item15->SetStringSelection(_("C++ files (*.h, *.c, *.cpp)"));
    item13->Add(item15, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticLine* item16 = new wxStaticLine( item1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item13->Add(item16, 0, wxGROW|wxLEFT|wxRIGHT, 10);

    wxButton* item17 = new wxButton( item1, ID_BUTTONCANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_buttonCancel = item17;
    item13->Add(item17, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

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

void RemoteFileDialog::OnButtonUpFolder( wxCommandEvent& event )
{
    event.Skip();

	wxPathFormat pathFormat = GetCurrentPathFormat();

	int numDirs = m_currentPath.GetDirCount();

	if(numDirs > 0)
	{
		wxString currPath = m_currentPath.GetPath(false, wxPATH_UNIX);
 
		// if we're not at a user's home directory, get rid of the last
		// directory in the path
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
	m_currentPath.AssignDir("~");
}

bool RemoteFileDialog::ShowDirectory(wxString dirname, bool refresh, bool showHidden)
{

	m_currentPath.AssignDir(dirname);
	m_pathBox->SetValue(dirname);	

	m_list->ClearAll();

	DirListing dl;	
	//wxBeginBusyCursor();
	dl= m_network->GetDirListing(dirname, refresh, showHidden);
	//wxEndBusyCursor();

	NetworkCallResult netStatus = m_parentFrame->CheckNetworkStatus();
	if(netStatus == NETCALL_REDO)
	{
		wxBeginBusyCursor();
		dl = m_network->GetDirListing(dirname);
		wxEndBusyCursor();
	}
	else if(netStatus == NETCALL_FAILED)
	{
		return false;
	}

	m_currentDirListing = dl;

	FillListView();

	return true;
}

void RemoteFileDialog::FillListView()
{
	// disables VS's 4018 warning: "signed/unsigned mismatch" relating to the GetCount() comparison
#pragma warning( disable : 4018)

	m_list->ClearAll();

	wxSortedArrayString sortedDirs(m_currentDirListing.dirNames);
	wxSortedArrayString sortedFiles(m_currentDirListing.fileNames);

	m_currentDirs.Clear();
	m_currentFiles.Clear();

	// should be getting just the name of the directory itself from Networking, so I 
	// can insert it directly into the listview and give it the folder icon
	for(int i = 0; i < sortedDirs.GetCount(); i++)
	{
		m_list->InsertItem(m_list->GetItemCount(), sortedDirs[i], 0);
		m_currentDirs.Add(sortedDirs[i]);
	}

	//int fileType = m_comboFiletypes->GetSelection();

	wxSortedArrayString cppFiles;
	wxSortedArrayString cFiles;
	wxSortedArrayString hFiles;

	// the user's filter selection ("C++ files", "All Files", etc)
	switch(m_currentFilterIndex)
	{
		// C++ files
	case 0:
		// go through ALL files, find the C++ files, and sort them according to extension
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

		// insert .C files
		for(int i = 0; i < cFiles.GetCount(); i++)
		{
			m_list->InsertItem(m_list->GetItemCount(), cFiles[i], 1);
			//wxLogDebug(".C file: " + cFiles[i]);
		}

		// insert .CPP files
		for(int i = 0; i < cppFiles.GetCount(); i++)
		{
			m_list->InsertItem(m_list->GetItemCount(), cppFiles[i], 2);
			//wxLogDebug(".CPP file: " + cppFiles[i]);
		}

		// insert .H files
		for(int i = 0; i < hFiles.GetCount(); i++)
		{
			m_list->InsertItem(m_list->GetItemCount(), hFiles[i], 3);
			//wxLogDebug(".H file: " + hFiles[i]);
		}
		break;

	case 1:
		// just show ALL files, changing the icon if necessary
		for(int i = 0; i < sortedFiles.GetCount(); i++)
		{
			wxFileName currentName(sortedFiles[i]);
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

	// turn the size_t warning back on
#pragma warning( default : 4018 )
}

void RemoteFileDialog::LoadTestData()
{
	ShowDirectory("C:\\Projects\\cs4810\\gears\\chameleon\\gui\\");
}

// save the information for opening a file
void RemoteFileDialog::OpenRemoteFile()
{
	wxString remoteFileName = m_txtFilename->GetValue();
	m_remoteFileNamePath.Assign(m_currentPath.GetPath(false, wxPATH_UNIX), remoteFileName);

	EndModal(wxOK);

}

// save the information for saving a file
void RemoteFileDialog::SaveRemoteFile()
{
	wxString remoteFileName = m_txtFilename->GetValue();

	if(m_currentFilterIndex == 0)
	{
		wxFileName fn;

		fn.SetFullName(remoteFileName);
		wxString ext = fn.GetExt();

		if(ext == wxEmptyString)
		{
			fn.SetExt("cpp");
			remoteFileName = fn.GetFullName();
		}
		
	}
	m_remoteFileNamePath.Assign(m_currentPath.GetPath(false, wxPATH_UNIX), remoteFileName);

	EndModal(wxOK);
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL
 */

void RemoteFileDialog::OnItemSelected( wxListEvent& event )
{
    // Insert custom code here
    event.Skip();

	wxString itemText = event.GetText();
	m_txtFilename->SetValue(itemText);
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

// Sets up the dialog for use as either an open or a save dialog
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
	m_txtFilename->SetValue(wxEmptyString);
	return ShowDirectory(path);
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTONOPEN
 */

// user double-clicked a list item OR hit the open button
void RemoteFileDialog::ItemActivated()
{
	wxString text = m_txtFilename->GetValue();

	int dirResult = m_currentDirListing.dirNames.Index(text); //m_currentDirs.Index(text);

	// the item name isn't a directory
	if(dirResult == wxNOT_FOUND)
	{		
		int fileResult = m_currentDirListing.fileNames.Index(text);//m_currentFiles.Index(text);

		// and it's not a file either
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
}/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX1
 */

void RemoteFileDialog::OnCombobox1Selected( wxCommandEvent& event )
{
    // Insert custom code here
    event.Skip();

	int selectionIndex = m_comboFiletypes->GetSelection();

	if(selectionIndex != m_currentFilterIndex)
	{
		m_currentFilterIndex = selectionIndex;
		FillListView();
	}
}

void RemoteFileDialog::OnButtonRefresh(wxCommandEvent &event)
{
	wxString path = m_currentPath.GetPath(false, wxPATH_UNIX);
	ShowDirectory(path, true);
}

void RemoteFileDialog::OnEnter(wxCommandEvent &event)
{
	ItemActivated();
}