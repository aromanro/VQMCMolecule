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


wxIMPLEMENT_CLASS(OptionsFrame, wxPropertySheetDialog);

wxBEGIN_EVENT_TABLE(OptionsFrame, wxPropertySheetDialog)
EVT_CLOSE(OptionsFrame::OnClose)
wxEND_EVENT_TABLE()

OptionsFrame::OptionsFrame(const Options& opt, const wxString & title, wxWindow* parent)
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

	options = opt;
	Create(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | (int)wxPlatform::IfNot(wxOS_WINDOWS_CE, wxRESIZE_BORDER));

	CreateButtons(wxOK | wxCANCEL);

	wxBookCtrlBase* notebook = GetBookCtrl();


	wxPanel* settingsPage = CreateMoleculeSettingsPage(notebook);
	notebook->AddPage(settingsPage, "Molecule", true);

	settingsPage = CreateComputationSettingsPage(notebook);
	notebook->AddPage(settingsPage, "Computation");

	settingsPage = CreateStepsSettingsPage(notebook);
	notebook->AddPage(settingsPage, "Steps");

	LayoutDialog();

	Centre();
}

bool OptionsFrame::TransferDataFromWindow()
{
	if (!wxPropertySheetDialog::TransferDataFromWindow()) return false;
	


	return true;
}

void OptionsFrame::OnClose(wxCloseEvent& event)
{
	event.Skip();
}



wxPanel* OptionsFrame::CreateMoleculeSettingsPage(wxBookCtrlBase* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY | wxGROW);


	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* item0 = new wxBoxSizer(wxVERTICAL);

	// ***********************************************************************************

	wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);

	// add controls
	
	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	// and so on...

	// *************************

	topSizer->Add(item0, 0, wxALL | wxGROW, 5);
	panel->SetSizerAndFit(topSizer);


	return panel;
}


wxPanel* OptionsFrame::CreateComputationSettingsPage(wxBookCtrlBase* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY | wxGROW);


	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* item0 = new wxBoxSizer(wxVERTICAL);

	// ***********************************************************************************

	wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);

	// add controls

	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	// and so on...


	// *************************

	topSizer->Add(item0, 0, wxALL | wxGROW, 5);
	panel->SetSizerAndFit(topSizer);


	return panel;
}


wxPanel* OptionsFrame::CreateStepsSettingsPage(wxBookCtrlBase* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY | wxGROW);


	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* item0 = new wxBoxSizer(wxVERTICAL);

	// ***********************************************************************************

	wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);

	// add controls

	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	// and so on...


	// *************************

	topSizer->Add(item0, 0, wxALL | wxGROW, 5);
	panel->SetSizerAndFit(topSizer);


	return panel;
}

