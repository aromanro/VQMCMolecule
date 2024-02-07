#pragma once

#define wxNEEDS_DECL_BEFORE_TEMPLATE

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <wx/propdlg.h>


#include "Options.h"
#include "Basis.h"

class OptionsFrame : public wxPropertySheetDialog
{
	wxDECLARE_CLASS(OptionsFrame);
public:
	OptionsFrame(const Options& opt, const wxString& title, wxWindow* parent = nullptr);

	Options options;


	Chemistry::Basis basisSTO3G;
	Chemistry::Basis basisSTO6G;

	Chemistry::Basis* GetBasisPtr()
	{
		Chemistry::Basis* basisPtr = &basisSTO6G; // some default

		if (0 == options.basis)
		{
			basisPtr = &basisSTO3G;
		}
		else
		{
			basisPtr = &basisSTO6G;
		}


		return basisPtr;
	}

	int sel1;
	int sel2;
	
protected:
	wxPanel* CreateMoleculeSettingsPage(wxBookCtrlBase* parent);
	wxPanel* CreateComputationSettingsPage(wxBookCtrlBase* parent);
	wxPanel* CreateStepsSettingsPage(wxBookCtrlBase* parent);

	std::vector<wxString> GetAtoms();

	bool TransferDataFromWindow() override;
	bool CheckOptions();

	void OnClose(wxCloseEvent& event);
	void OnUseZ2(wxCommandEvent& event);
	void OnBasisChoose(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};