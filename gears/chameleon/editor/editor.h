#ifndef EDITOR__H
#define EDITOR_H

#include "stc.h"
#include "../common/datastructures.h"
#include "../common/debug.h"



#include <wx/datetime.h>


class ChameleonWindow;
class ChameleonNotebook;

class ChameleonEditor : public wxStyledTextCtrl
{
public:
	ChameleonEditor(ChameleonWindow* mframe, wxWindow *parent, wxWindowID id, 
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = 0,
					const wxString& name = wxSTCNameStr);
	~ChameleonEditor();


	bool LoadFile ();
	bool LoadFile (const wxString &filename);
	bool SaveFileAs();
	bool SaveFile(const wxString& filename);

	bool Modified();
	void OnSetTabModified(wxStyledTextEvent &event);

	void OnChar(wxStyledTextEvent &event);



	wxString GetFilename () {return m_filename;};
	void SetFilename (const wxString &filename) {m_filename;};

private:

	ChameleonWindow* m_mainFrame;
	ChameleonNotebook* m_parentNotebook;	

	wxString m_filename;
	wxDateTime m_filetime;
	
	
	DECLARE_EVENT_TABLE()
};


//BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)

#endif