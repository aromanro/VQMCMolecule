#include "VQMCMoleculeApp.h"
#include "VQMCMoleculeFrame.h"


wxIMPLEMENT_APP(VQMCMoleculeApp);

bool VQMCMoleculeApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	options.Load();

	frame = new VQMCMoleculeFrame("VQMCMolecule", wxPoint(50, 50), wxSize(1024, 800));
	frame->Show(true);

	return true;
}
