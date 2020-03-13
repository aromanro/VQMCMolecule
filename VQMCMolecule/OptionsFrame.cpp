#define wxNEEDS_DECL_BEFORE_TEMPLATE

#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valnum.h>
#include <wx/panel.h>
#include <wx/bookctrl.h>

#include "VQMCMoleculeApp.h"
#include "OptionsFrame.h"
#include "VQMCMoleculeFrame.h"

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
	deltatCtrl->SetValidator(v3);

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
	firstStageGradStepsCtrl->SetValidator(val2);

	wxIntegerValidator<int> val3(&options.thirdStageGradientDescentSteps, wxNUM_VAL_DEFAULT);
	val3.SetRange(0, 100);
	firstStageGradStepsCtrl->SetValidator(val3);


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

