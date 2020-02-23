#define wxNEEDS_DECL_BEFORE_TEMPLATE

#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valnum.h>
#include <wx/panel.h>
#include <wx/bookctrl.h>

#include "VQMCMoleculeApp.h"
#include "OptionsFrame.h"
#include "VQMCMoleculeFrame.h"


wxDECLARE_APP(VQMCMoleculeApp);


wxIMPLEMENT_CLASS(OptionsFrame, wxDialog);

wxBEGIN_EVENT_TABLE(OptionsFrame, wxDialog)
EVT_CLOSE(OptionsFrame::OnClose)
wxEND_EVENT_TABLE()

OptionsFrame::OptionsFrame(const Options& opt, const wxString & title, wxWindow* parent)
	: wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(400, 250)) 
{
	options = opt;
	CreateControls();
	Layout();
	Centre();
}

bool OptionsFrame::TransferDataFromWindow()
{
	if (!wxDialog::TransferDataFromWindow()) return false;
	


	return true;
}

void OptionsFrame::OnClose(wxCloseEvent& event)
{
	event.Skip();
}


void OptionsFrame::CreateControls()
{
	// box to contain them all
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	SetSizer(vbox);	

	// box with margin to contain option controls
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	vbox->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL| wxGROW | wxALL, 5);

	// *****************************************************************
	// Controls

	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	boxSizer->Add(box, 0, wxGROW|wxALL, 5);


	// *****************************************************************
	// Validators


	// *****************************************************************
	
	wxStaticLine* line = new wxStaticLine(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	boxSizer->Add(line, 0, wxGROW|wxALL, 5);

	// bottom box with ok & cancel buttons
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	
	wxButton *okButton = new wxButton(this, wxID_OK, "Ok", wxDefaultPosition, wxSize(70, 30));
	wxButton *closeButton = new wxButton(this, wxID_CANCEL, "Cancel", wxDefaultPosition, wxSize(70, 30));

	hbox->Add(okButton, 1);
	hbox->Add(closeButton, 1, wxLEFT, 5);

	vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
}
