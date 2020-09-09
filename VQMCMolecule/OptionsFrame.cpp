#define wxNEEDS_DECL_BEFORE_TEMPLATE

#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valnum.h>
#include <wx/panel.h>
#include <wx/bookctrl.h>

#include "VQMCMoleculeApp.h"
#include "OptionsFrame.h"
#include "VQMCMoleculeFrame.h"

#include "ChemUtils.h"

#define GRADIENT_DESCENT_FIRST_STAGE_STEPS_ID 201
#define GRADIENT_DESCENT_SECOND_STAGE_STEPS_ID 202
#define GRADIENT_DESCENT_THIRD_STAGE_STEPS_ID 203

#define FIRST_THERMAL_STEPS_ID 204

#define FIRST_STAGE_THERMAL_STEPS_ID 205
#define FIRST_STAGE_STATS_STEPS_ID 206
#define SECOND_STAGE_THERMAL_STEPS_ID 207
#define SECOND_STAGE_STATS_STEPS_ID 208
#define THIRD_STAGE_THERMAL_STEPS_ID 209
#define THIRD_STAGE_STATS_STEPS_ID 210

#define LAST_STEP_THERMAL_STEPS_ID 211
#define LAST_STEP_STATS_STEPS_ID 212

#define NR_THREADS_ID 213
#define NR_WALKERS_ID 214
#define BASIS_ID      215
#define DELTAT_ID     216
#define GRAD_PARAM_ID 217
#define BETA_ID       218

#define FIRST_ATOM_ID 219
#define SECOND_ATOM_ID 220
#define USE_Z2_ID      221
#define DISTANCE_ID    222

#define CYCLES_REFRESH_ID 223


wxDECLARE_APP(VQMCMoleculeApp);


wxIMPLEMENT_CLASS(OptionsFrame, wxPropertySheetDialog);

wxBEGIN_EVENT_TABLE(OptionsFrame, wxPropertySheetDialog)
EVT_CLOSE(OptionsFrame::OnClose)
EVT_CHECKBOX(USE_Z2_ID, OptionsFrame::OnUseZ2)
EVT_RADIOBOX(BASIS_ID, OptionsFrame::OnBasisChoose)
wxEND_EVENT_TABLE()

OptionsFrame::OptionsFrame(const Options& opt, const wxString & title, wxWindow* parent)
	: sel1(0), sel2(0)
{
	basisSTO3G.Load("sto3g.txt");
	basisSTO6G.Load("sto6g.txt");

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
	
	Chemistry::Basis* basisPtr = GetBasisPtr();

	int s = 0;
	for (const auto& atom : basisPtr->atoms)
	{
		if (sel1 == s)
			options.Z1 = atom.Z;

		if (sel2 == s)
			options.Z2 = atom.Z;

		++s;
	}


	if (options.useZ2 && options.distance < 0.01)
	{
		wxMessageBox("Please enter a bigger distance between atoms", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.firstStageGradientDescentSteps + options.secondStageGradientDescentSteps + options.thirdStageGradientDescentSteps < 1)
	{
		wxMessageBox("You need to have at least one gradient descent step", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.nrThreads < 1)
	{
		wxMessageBox("At least one computing thread is needed", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.nrWalkers < 1)
	{
		wxMessageBox("At least one walker is needed", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.beta < 0.3)
	{
		wxMessageBox("Beta should be bigger than 0.3", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.gradDescParam < 0.1)
	{
		wxMessageBox("The gradient descent parameter should be bigger than 0.1", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.deltat > 0.3 || options.deltat < 0.001)
	{
		wxMessageBox("Please enter a value for delta t smaller than 0.3 and bigger than 0.001", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.firstThermalSteps < 100000)
	{
		wxMessageBox("First thermal steps should be over 100000", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.lastStepStatsSteps < 30000)
	{
		wxMessageBox("Last statistics steps should be over 30000", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	if (options.cyclesRefresh < 1)
	{
		wxMessageBox("Recompute Slater inverse steps should not be 0", "Validation", wxOK | wxICON_INFORMATION, this);

		return false;
	}

	return true;
}

void OptionsFrame::OnClose(wxCloseEvent& event)
{
	event.Skip();
}


std::vector<wxString> OptionsFrame::GetAtoms()
{
	std::vector<wxString> strings;

	Chemistry::Basis* basisPtr = GetBasisPtr();

	int s = 0;
	for (const auto& atom : basisPtr->atoms)
	{
		strings.push_back(Chemistry::ChemUtils::GetAtomNameForZ(atom.Z));

		if (atom.Z == options.Z1)
			sel1 = s;

		if (atom.Z == options.Z2)
			sel2 = s;

		++s;
	}

	return strings;
}


wxPanel* OptionsFrame::CreateMoleculeSettingsPage(wxBookCtrlBase* parent)
{
	wxPanel* panel = new wxPanel(parent, wxID_ANY | wxGROW);


	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* item0 = new wxBoxSizer(wxVERTICAL);

	// ***********************************************************************************

	wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);

	// add controls
	
	wxStaticText* label = new wxStaticText(panel, wxID_STATIC, "First atom:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	std::vector<wxString> strings = GetAtoms();

	wxChoice* atom1Choice = new wxChoice(panel, FIRST_ATOM_ID, wxDefaultPosition, wxSize(100, -1), strings.size(), strings.data(), 0);

	atom1Choice->SetSelection(sel1);
	itemSizer->Add(atom1Choice, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	label = new wxStaticText(panel, wxID_STATIC, "Second atom:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxChoice* atom2Choice = new wxChoice(panel, SECOND_ATOM_ID, wxDefaultPosition, wxSize(100, -1), strings.size(), strings.data(), 0);

	atom2Choice->SetSelection(sel2);
	itemSizer->Add(atom2Choice, 1, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	// and so on...

	itemSizer = new wxBoxSizer(wxHORIZONTAL);

	wxCheckBox* checkBox = new wxCheckBox(panel, USE_Z2_ID, "Compute molecule (unchecked means computing only the atom selected in the first combo)");
	itemSizer->Add(checkBox, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL);

	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	itemSizer = new wxBoxSizer(wxHORIZONTAL);

	label = new wxStaticText(panel, wxID_STATIC, "Distance:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString str = wxString::Format(wxT("%g"), options.distance);
	wxTextCtrl* distanceCtrl = new wxTextCtrl(panel, DISTANCE_ID, str, wxDefaultPosition, wxSize(100, -1), 0);
	itemSizer->Add(distanceCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	if (!options.useZ2)
	{
		distanceCtrl->Enable(false);
		atom2Choice->Enable(false);
	}


	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);


	// *************************
	// Validators

	atom1Choice->SetValidator(wxGenericValidator(&sel1));
	atom2Choice->SetValidator(wxGenericValidator(&sel2));

	checkBox->SetValidator(wxGenericValidator(&options.useZ2));

	wxFloatingPointValidator<double> v1(&options.distance, wxNUM_VAL_DEFAULT);
	v1.SetRange(0, 10.);
	v1.SetPrecision(3);
	distanceCtrl->SetValidator(v1);

	// *********************************

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

	wxStaticText* label = new wxStaticText(panel, wxID_STATIC, "Nr threads:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString str = wxString::Format(wxT("%d"), options.nrThreads);
	wxTextCtrl* nrThreadsCtrl = new wxTextCtrl(panel, NR_THREADS_ID, str, wxDefaultPosition, wxSize(80, -1), 0);
	itemSizer->Add(nrThreadsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	label = new wxStaticText(panel, wxID_STATIC, "Nr walkers:", wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.nrWalkers);
	wxTextCtrl* nrWalkersCtrl = new wxTextCtrl(panel, NR_WALKERS_ID, str, wxDefaultPosition, wxSize(80, -1), 0);
	itemSizer->Add(nrWalkersCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	// and so on...

	itemSizer = new wxBoxSizer(wxHORIZONTAL);

	label = new wxStaticText(panel, wxID_STATIC, "Gradient descent:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%g"), options.gradDescParam);
	wxTextCtrl* gradCtrl = new wxTextCtrl(panel, GRAD_PARAM_ID, str, wxDefaultPosition, wxSize(100, -1), 0);
	itemSizer->Add(gradCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	label = new wxStaticText(panel, wxID_STATIC, "Delta t:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%g"), options.deltat);
	wxTextCtrl* deltatCtrl = new wxTextCtrl(panel, DELTAT_ID, str, wxDefaultPosition, wxSize(100, -1), 0);
	itemSizer->Add(deltatCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	itemSizer = new wxBoxSizer(wxHORIZONTAL);

	label = new wxStaticText(panel, wxID_STATIC, "Beta (Jastrow):", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%g"), options.beta);
	wxTextCtrl* betaCtrl = new wxTextCtrl(panel, BETA_ID, str, wxDefaultPosition, wxSize(100, -1), 0);
	itemSizer->Add(betaCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	label = new wxStaticText(panel, wxID_STATIC, "Recompute Slater inv steps:", wxDefaultPosition, wxSize(200, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.cyclesRefresh);
	wxTextCtrl* cyclesRefresgCtrl = new wxTextCtrl(panel, CYCLES_REFRESH_ID, str, wxDefaultPosition, wxSize(100, -1), 0);
	itemSizer->Add(cyclesRefresgCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	itemSizer = new wxBoxSizer(wxHORIZONTAL);

	wxArrayString choices;
	choices.Add("STO3G");
	choices.Add("STO6G");
	
	wxRadioBox* m_radioBox = new wxRadioBox(panel, BASIS_ID, "Basis", wxDefaultPosition, wxDefaultSize, choices, 2, wxRA_VERTICAL);
	itemSizer->Add(m_radioBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	item0->Add(itemSizer, 0, wxGROW | wxALL, 0);

	// *************************
	// Validators


	wxIntegerValidator<int> val1(&options.nrThreads, wxNUM_VAL_DEFAULT);
	val1.SetRange(0, 256);
	nrThreadsCtrl->SetValidator(val1);

	wxIntegerValidator<int> val2(&options.nrWalkers, wxNUM_VAL_DEFAULT);
	val2.SetRange(0, 1000);
	nrWalkersCtrl->SetValidator(val2);


	wxFloatingPointValidator<double> v1(&options.gradDescParam, wxNUM_VAL_DEFAULT);
	v1.SetRange(0, 10.);
	v1.SetPrecision(2);
	gradCtrl->SetValidator(v1);

	wxFloatingPointValidator<double> v2(&options.deltat, wxNUM_VAL_DEFAULT);
	v2.SetRange(0, 1.);
	v2.SetPrecision(3);
	deltatCtrl->SetValidator(v2);

	wxFloatingPointValidator<double> v3(&options.beta, wxNUM_VAL_DEFAULT);
	v3.SetRange(0, 10.);
	v3.SetPrecision(2);
	betaCtrl->SetValidator(v3);

	wxIntegerValidator<int> val4(&options.cyclesRefresh, wxNUM_VAL_DEFAULT);
	val4.SetRange(0, 1000000);
	cyclesRefresgCtrl->SetValidator(val4);

	m_radioBox->SetValidator(wxGenericValidator(&options.basis));

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

	wxStaticBoxSizer* itemBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Gradient Descent Steps");

	// add controls

	wxStaticText* label = new wxStaticText(panel, wxID_STATIC, "First stage:", wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString str = wxString::Format(wxT("%d"), options.firstStageGradientDescentSteps);
	wxTextCtrl* firstStageGradStepsCtrl = new wxTextCtrl(panel, GRADIENT_DESCENT_FIRST_STAGE_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(firstStageGradStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	label = new wxStaticText(panel, wxID_STATIC, "Second stage:", wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.secondStageGradientDescentSteps);
	wxTextCtrl* secondStageGradStepsCtrl = new wxTextCtrl(panel, GRADIENT_DESCENT_SECOND_STAGE_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(secondStageGradStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	label = new wxStaticText(panel, wxID_STATIC, "Third stage:", wxDefaultPosition, wxSize(80, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.thirdStageGradientDescentSteps);
	wxTextCtrl* thirdStageGradStepsCtrl = new wxTextCtrl(panel, GRADIENT_DESCENT_THIRD_STAGE_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(thirdStageGradStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	item0->Add(itemBoxSizer, 0, wxALL | wxGROW, 0);

	// and so on...

	wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);
	label = new wxStaticText(panel, wxID_STATIC, "First equilibration steps:", wxDefaultPosition, wxSize(150, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.firstThermalSteps);
	wxTextCtrl* firstThermalCtrl = new wxTextCtrl(panel, FIRST_THERMAL_STEPS_ID, str, wxDefaultPosition, wxSize(100, -1), 0);
	itemSizer->Add(firstThermalCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);



	itemBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "First stage");

	label = new wxStaticText(panel, wxID_STATIC, "Equilibration steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.firstStageThermalSteps);
	wxTextCtrl* firstStageThermalStepsCtrl = new wxTextCtrl(panel, FIRST_STAGE_THERMAL_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(firstStageThermalStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	label = new wxStaticText(panel, wxID_STATIC, "Statistics steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.firstStageStatsSteps);
	wxTextCtrl* firstStageStatsStepsCtrl = new wxTextCtrl(panel, FIRST_STAGE_STATS_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(firstStageStatsStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	item0->Add(itemBoxSizer, 0, wxALL | wxGROW, 0);


	itemBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Second stage");


	label = new wxStaticText(panel, wxID_STATIC, "Equilibration steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.secondStageThermalSteps);
	wxTextCtrl* secondStageThermalStepsCtrl = new wxTextCtrl(panel, SECOND_STAGE_THERMAL_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(secondStageThermalStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	label = new wxStaticText(panel, wxID_STATIC, "Statistics steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.secondStageStatsSteps);
	wxTextCtrl* secondStageStatsStepsCtrl = new wxTextCtrl(panel, SECOND_STAGE_STATS_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(secondStageStatsStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);


	item0->Add(itemBoxSizer, 0, wxALL | wxGROW, 0);


	itemBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Third stage");

	label = new wxStaticText(panel, wxID_STATIC, "Equilibration steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.thirdStageThermalSteps);
	wxTextCtrl* thirdStageThermalStepsCtrl = new wxTextCtrl(panel, THIRD_STAGE_THERMAL_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(thirdStageThermalStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	label = new wxStaticText(panel, wxID_STATIC, "Statistics steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.thirdStageStatsSteps);
	wxTextCtrl* thirdStageStatsStepsCtrl = new wxTextCtrl(panel, THIRD_STAGE_STATS_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(thirdStageStatsStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	item0->Add(itemBoxSizer, 0, wxALL | wxGROW, 0);


	itemBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Last step");

	label = new wxStaticText(panel, wxID_STATIC, "Equilibration steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.lastStepThermalSteps);
	wxTextCtrl* lastStepThermalStepsCtrl = new wxTextCtrl(panel, LAST_STEP_THERMAL_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(lastStepThermalStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	label = new wxStaticText(panel, wxID_STATIC, "Statistics steps:", wxDefaultPosition, wxSize(100, -1), wxALIGN_RIGHT);
	itemBoxSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

	str = wxString::Format(wxT("%d"), options.lastStepStatsSteps);
	wxTextCtrl* lastStepStatsStepsCtrl = new wxTextCtrl(panel, LAST_STEP_STATS_STEPS_ID, str, wxDefaultPosition, wxSize(60, -1), 0);
	itemBoxSizer->Add(lastStepStatsStepsCtrl, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	item0->Add(itemBoxSizer, 0, wxALL | wxGROW, 0);


	itemSizer = new wxBoxSizer(wxHORIZONTAL);

	label = new wxStaticText(panel, wxID_STATIC, "Equilibration and statistics steps get multiplied by the number of electrons", wxDefaultPosition, wxSize(450, -1), wxALIGN_RIGHT);
	itemSizer->Add(label, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL | wxGROW, 5);

	item0->Add(itemSizer, 0, wxALL | wxGROW, 0);

	// *************************
	// Validators


	wxIntegerValidator<int> val1(&options.firstStageGradientDescentSteps, wxNUM_VAL_DEFAULT);
	val1.SetRange(0, 100);
	firstStageGradStepsCtrl->SetValidator(val1);

	wxIntegerValidator<int> val2(&options.secondStageGradientDescentSteps, wxNUM_VAL_DEFAULT);
	val2.SetRange(0, 100);
	secondStageGradStepsCtrl->SetValidator(val2);

	wxIntegerValidator<int> val3(&options.thirdStageGradientDescentSteps, wxNUM_VAL_DEFAULT);
	val3.SetRange(0, 100);
	thirdStageGradStepsCtrl->SetValidator(val3);


	wxIntegerValidator<int> val4(&options.firstThermalSteps, wxNUM_VAL_DEFAULT);
	val4.SetRange(0, 1000000000);
	firstThermalCtrl->SetValidator(val4);

	
	wxIntegerValidator<int> val5(&options.firstStageThermalSteps, wxNUM_VAL_DEFAULT);
	val5.SetRange(0, 1000000000);
	firstStageThermalStepsCtrl->SetValidator(val5);

	wxIntegerValidator<int> val6(&options.secondStageThermalSteps, wxNUM_VAL_DEFAULT);
	val6.SetRange(0, 1000000000);
	secondStageThermalStepsCtrl->SetValidator(val6);

	wxIntegerValidator<int> val7(&options.thirdStageThermalSteps, wxNUM_VAL_DEFAULT);
	val7.SetRange(0, 1000000000);
	thirdStageThermalStepsCtrl->SetValidator(val7);


	wxIntegerValidator<int> val8(&options.firstStageStatsSteps, wxNUM_VAL_DEFAULT);
	val8.SetRange(0, 1000000000);
	firstStageStatsStepsCtrl->SetValidator(val8);

	wxIntegerValidator<int> val9(&options.secondStageStatsSteps, wxNUM_VAL_DEFAULT);
	val9.SetRange(0, 1000000000);
	secondStageStatsStepsCtrl->SetValidator(val9);

	wxIntegerValidator<int> val10(&options.thirdStageStatsSteps, wxNUM_VAL_DEFAULT);
	val10.SetRange(0, 1000000000);
	thirdStageStatsStepsCtrl->SetValidator(val10);

	wxIntegerValidator<int> val11(&options.lastStepThermalSteps, wxNUM_VAL_DEFAULT);
	val11.SetRange(0, 1000000000);
	lastStepThermalStepsCtrl->SetValidator(val11);

	wxIntegerValidator<int> val12(&options.lastStepStatsSteps, wxNUM_VAL_DEFAULT);
	val12.SetRange(0, 1000000000);
	lastStepStatsStepsCtrl->SetValidator(val12);

	// **************************

	topSizer->Add(item0, 0, wxALL | wxGROW, 5);
	panel->SetSizerAndFit(topSizer);


	return panel;
}

void OptionsFrame::OnBasisChoose(wxCommandEvent& /*event*/)
{
	TransferDataFromWindow();

	std::vector<wxString> atoms = GetAtoms();

	wxChoice* atomChoice = (wxChoice*)FindWindow(FIRST_ATOM_ID);
	atomChoice->Set(atoms.size(), atoms.data());
	atomChoice->SetSelection(sel1);

	atomChoice = (wxChoice*)FindWindow(SECOND_ATOM_ID);
	atomChoice->Set(atoms.size(), atoms.data());
	atomChoice->SetSelection(sel2);
}

void OptionsFrame::OnUseZ2(wxCommandEvent& /*event*/)
{
	TransferDataFromWindow();

	wxChoice* atomChoice = (wxChoice*)FindWindow(SECOND_ATOM_ID);
	wxTextCtrl* distanceCtrl = (wxTextCtrl*)FindWindow(DISTANCE_ID);
	atomChoice->Enable(options.useZ2);
	distanceCtrl->Enable(options.useZ2);
}