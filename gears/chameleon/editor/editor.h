#include "stc.h"
#include "../common/datastructures.h"
#include "../common/debug.h"

#include <wx/datetime.h>


class ChameleonWindow;

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
	bool SaveFile();
	bool SaveFile(const wxString& filename);

	bool Modified();

	void OnChar(wxKeyEvent& evt);

	wxString GetFilename () {return m_filename;};
	void SetFilename (const wxString &filename) {m_filename = filename;};

private:

	ChameleonWindow* m_mainFrame;
	

	wxString m_filename;
	wxDateTime m_filetime;
	//DECLARE_EVENT_TABLE()
};


//BEGIN_EVENT_TABLE(ChameleonEditor, wxStyledTextCtrl)
	