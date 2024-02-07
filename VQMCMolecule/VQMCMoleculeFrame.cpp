#include "VQMCMoleculeFrame.h"

#include <string>
#include <thread>

#include "wx/aboutdlg.h"
#include "wx/statline.h"
#include "wx/generic/aboutdlgg.h"

#include "VQMCMoleculeApp.h"
#include "OptionsFrame.h"

#include "VQMCMolecule.h"


#define ID_TIMER   101
#define ID_EXECUTE 102

wxDECLARE_APP(VQMCMoleculeApp);

wxBEGIN_EVENT_TABLE(VQMCMoleculeFrame, wxFrame)
EVT_MENU(wxID_EXIT, VQMCMoleculeFrame::OnExit)
EVT_MENU(wxID_PREFERENCES, VQMCMoleculeFrame::OnOptions)
EVT_MENU(wxID_ABOUT, VQMCMoleculeFrame::OnAbout)
EVT_MENU(ID_EXECUTE, VQMCMoleculeFrame::OnExecute)
EVT_UPDATE_UI(ID_EXECUTE, VQMCMoleculeFrame::OnUpdateExecute)
EVT_TIMER(ID_TIMER, VQMCMoleculeFrame::OnTimer)
wxEND_EVENT_TABLE()


class MyStream : public std::ostream, std::streambuf
{
public:
	MyStream(std::string& bufStr, std::mutex& strMutex) : std::ostream(this), m_bufStr(bufStr), m_strMutex(strMutex) {}

	int overflow(int c) override
	{
		log(c);

		return 0;
	}

private:
	void log(char c)
	{
		std::lock_guard<std::mutex> lock(m_strMutex);
		m_bufStr += c;
	}

	std::string& m_bufStr;
	std::mutex& m_strMutex;
};


class RedirectStream
{
public:
	RedirectStream(std::ostream& old, std::ostream& dst)
		: s(old), backupbuf(old.rdbuf())
	{
		s.rdbuf(dst.rdbuf());
	}

	~RedirectStream()
	{
		s.rdbuf(backupbuf);
	}

private:
	std::ostream& s;
	std::streambuf* const backupbuf;
};





VQMCMoleculeFrame::VQMCMoleculeFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(nullptr, wxID_ANY, title, pos, size), inExecution(false), timer(this, ID_TIMER)
{
	wxMenu* menuFile = new wxMenu;

	menuFile->Append(ID_EXECUTE, "Execute");
	menuFile->Append(wxID_EXIT);

	wxMenu* menuView = new wxMenu;
	menuView->Append(wxID_PREFERENCES);

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuView, "&View");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to VQMCMolecule!");

	richTextCtrl = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL | wxBORDER_NONE /*| wxWANTS_CHARS*/ | wxTE_MULTILINE | wxTE_READONLY);

	wxFont font(14, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	richTextCtrl->SetFont(font);
	richTextCtrl->GetCaret()->Hide();

	Layout();
}


VQMCMoleculeFrame::~VQMCMoleculeFrame()
{
}


void VQMCMoleculeFrame::OnOptions(wxCommandEvent& WXUNUSED(event))
{
	VQMCMoleculeApp& app = wxGetApp();
	OptionsFrame optionsFrame(app.options, "Options", this);

	if (wxID_OK == optionsFrame.ShowModal())
	{
		app.options = optionsFrame.options;
		app.options.Save();
	}
}


void VQMCMoleculeFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void VQMCMoleculeFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;

	info.SetName("VQMCMolecule");

	static const int majorVer = 1;
	static const int minorVer = 0;
	wxString verStr = wxString::Format("%d.%d", majorVer, minorVer);
	info.SetVersion(verStr, wxString::Format("Version %s", verStr));

	info.SetDescription("   Variational Quantum Monte Carlo Molecule Application   ");
	info.SetLicense("GNU GPL v3.0, see LICENSE file for details");

	info.AddDeveloper("Adrian Roman");

	info.SetWebSite("https://github.com/aromanro/VQMCMolecule", "GitHub repository");

	wxAboutBox(info, this);
}

void VQMCMoleculeFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	if (!inExecution)
	{
		timer.Stop();
		if (wxIsBusy()) wxEndBusyCursor();
	}

	{
		std::lock_guard<std::mutex> lock(bufferStrMutex);
		if (!bufferStr.empty())
		{
			richTextCtrl->AppendText(wxString(bufferStr));
			bufferStr.clear();
		}
	}
	richTextCtrl->ScrollIntoView(richTextCtrl->GetLastPosition(), WXK_END);
}

void VQMCMoleculeFrame::OnExecute(wxCommandEvent& WXUNUSED(event))
{
	if (inExecution.exchange(true)) return;

	wxBeginBusyCursor();
	richTextCtrl->Clear();
	timer.Start(100);
	VQMCMoleculeApp& app = wxGetApp();
	Options options = app.options;

	std::thread([this, options]()
		{
			MyStream myStream(bufferStr, bufferStrMutex);
			RedirectStream redirect(std::cout, myStream);

			Chemistry::Basis basis;

			switch (options.basis)
			{
			case 0:
				basis.Load("sto3g.txt");
				break;

			case 1:
			default:
				basis.Load("sto6g.txt");
				break;
			}

			VQMCMolecule::Compute(options, basis);

			inExecution = false;
		}).detach();
}

void VQMCMoleculeFrame::OnUpdateExecute(wxUpdateUIEvent& event)
{
	event.Enable(!inExecution);
}

