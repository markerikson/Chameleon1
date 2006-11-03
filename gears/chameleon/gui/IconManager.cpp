#include "IconManager.h"

#include <wx/icon.h>
#include <wx/iconloc.h>
#include <wx/mimetype.h>
#include <wx/bitmap.h>

#include "dialogs/defaultfile.xpm"
#include "dialogs/closedfolder16x1632bpp.xpm"
#include "dialogs/openfolder16x1632bpp.xpm"


IconManager::IconManager()
{
	m_images = new wxImageList(16, 16);

	m_iconExtensionMapping["cpj"] = m_images->GetImageCount();
	m_iconExtensionMapping["DEFAULTFILEEXTENSION"] = m_images->GetImageCount();
	wxBitmap defaultfile(defaultfile_xpm);
	m_images->Add(defaultfile);

	wxBitmap closedfolder(closedfolder16x1632bpp_xpm);
	m_iconExtensionMapping["FOLDERCLOSED"] = m_images->GetImageCount();
	m_images->Add(closedfolder);

	wxBitmap openfolder(openfolder16x1632bpp_xpm);
	m_iconExtensionMapping["FOLDEROPEN"] = m_images->GetImageCount();
	m_images->Add(openfolder);

	AddIconToList("c");
	AddIconToList("cpp");
	AddIconToList("h");

	m_iconExtensionMapping["hpp"] = GetIconIndex("h");
	AddIconToList("lib");

	CreateDisabledIcon("c");
	CreateDisabledIcon("cpp");
	CreateDisabledIcon("h");

	AddIconToList("txt");

}

IconManager::~IconManager()
{
	delete m_images;
}

bool IconManager::AddIconToList(wxString iconInfo)
{
	// wxTheMimeTypesManager is a wxWidgets-created global instance
	wxFileType* fileType = wxTheMimeTypesManager->GetFileTypeFromExtension(iconInfo);

	if(fileType == NULL)
	{
		return false;
	}

	wxIconLocation iconLocation;

	bool result = false;
	if(fileType->GetIcon(&iconLocation))
	{
		wxIcon fileIcon(iconLocation);
		
		fileIcon.SetHeight(16);
		fileIcon.SetWidth(16);
		fileIcon.SetDepth(32);

		int newIconIndex = m_images->GetImageCount();
		m_images->Add(fileIcon);

		m_iconExtensionMapping[iconInfo] = newIconIndex;

		result = true;
	}	

	delete fileType;
	return result;
}

int IconManager::GetIconIndex(wxString iconInfo)
{
	int currentExtensionIconNumber;

	if(iconInfo == wxEmptyString)
	{
		return m_iconExtensionMapping["DEFAULTFILEEXTENSION"];
	}

	if(m_iconExtensionMapping.find(iconInfo) != m_iconExtensionMapping.end())
	{
		currentExtensionIconNumber = m_iconExtensionMapping[iconInfo];
	}
	else
	{
		if(AddIconToList(iconInfo))
		{
			currentExtensionIconNumber = m_iconExtensionMapping[iconInfo];
		}
		else
		{
			currentExtensionIconNumber = m_iconExtensionMapping["DEFAULTFILEEXTENSION"];
		}
	}

	return currentExtensionIconNumber;
}


void IconManager::CreateDisabledIcon(wxString iconInfo)
{
	int iconIndex = GetIconIndex(iconInfo);
	wxIcon icon = m_images->GetIcon(iconIndex);
	wxBitmap iconBitmap;
	iconBitmap.CopyFromIcon(icon);
	
	wxMemoryDC dc;
	wxPen pen(wxColour("navy"), 2);
	dc.SetPen(pen);

	dc.SelectObject(iconBitmap);
	dc.DrawLine(0, 0, 15, 15);
	dc.DrawLine(15, 0, 0, 15);
	dc.SelectObject(wxNullBitmap);

	// TODO: copying back to an icon may be unnecessary.
	wxIcon disabledIcon;
	disabledIcon.CopyFromBitmap(iconBitmap);

	wxString disabledIconName = iconInfo + "_disabled";
	m_iconExtensionMapping[disabledIconName] = m_images->GetImageCount();
	m_images->Add(disabledIcon);
}

wxImageList* IconManager::GetImageList()
{
	return m_images;
}