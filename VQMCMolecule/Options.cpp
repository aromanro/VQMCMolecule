#include "Options.h"

#include <wx/stdpaths.h> 

Options::Options()
	: 
	nrThreads(20),
	nrWalkers(100),
	basis(1),
	Z1(1), Z2(1),
	useZ2(true),
	distance(1.4),
	deltat(0.005),
	firstStageGradientDescentSteps(40),
	firstThermalSteps(200000),
	firstStageThermalSteps(2000),
	firstStageStatsSteps(4000),
	secondStageGradientDescentSteps(60),
	secondStageThermalSteps(4000),
	secondStageStatsSteps(8000),
	thirdStageGradientDescentSteps(40),
	thirdStageThermalSteps(16000),
	thirdStageStatsSteps(32000),
	lastStepThermalSteps(32000),
	lastStepStatsSteps(64000),
	gradDescParam(0.3),
	m_fileconfig(nullptr)
{
}


void Options::Open()
{
	if (m_fileconfig) return;

	wxString dir = wxStandardPaths::Get().GetConfigDir() + wxFileName::GetPathSeparator();

	if (!wxFileName::DirExists(dir))
		wxFileName::Mkdir(dir, 0777, wxPATH_MKDIR_FULL);

	wxString iniFilePath = dir + "VQMCMolecule.ini";

	m_fileconfig = new wxFileConfig("VQMCMolecule", wxEmptyString, iniFilePath);

	wxConfigBase::Set(m_fileconfig);
}


void Options::Close()
{
	delete m_fileconfig;
	m_fileconfig = NULL;
	wxConfigBase::Set(NULL);
}

void Options::Load()
{
	Open();
	wxConfigBase* conf = wxConfigBase::Get(false);
	if (conf)
	{
		nrThreads = conf->ReadLong("/nrThreads", 20);
		nrWalkers = conf->ReadLong("/nrWalkers", 100);
		basis = conf->ReadLong("/basis", 1);
		Z1 = conf->ReadLong("/Z1", 1);
		Z2 = conf->ReadLong("/Z2", 1);
		useZ2 = conf->ReadLong("/useZ2", 1) == 1;

		distance = conf->ReadDouble("/distance", 1.4);
		deltat = conf->ReadDouble("/deltat", 0.005);

		firstStageGradientDescentSteps = conf->ReadLong("/firstStageGradientDescentSteps", 40);
		firstThermalSteps = conf->ReadLong("/firstThermalSteps", 200000);
		firstStageThermalSteps = conf->ReadLong("/firstStageThermalSteps", 2000);
		firstStageStatsSteps = conf->ReadLong("/firstStageStatsSteps", 4000);
		secondStageGradientDescentSteps = conf->ReadLong("/secondStageGradientDescentSteps", 60);
		secondStageThermalSteps = conf->ReadLong("/secondStageThermalSteps", 4000);
		secondStageStatsSteps = conf->ReadLong("/secondStageStatsSteps", 8000);
		thirdStageGradientDescentSteps = conf->ReadLong("/thirdStageGradientDescentSteps", 60);
		thirdStageThermalSteps = conf->ReadLong("/thirdStageThermalSteps", 8000);
		thirdStageStatsSteps = conf->ReadLong("/thirdStageStatsSteps", 16000);
		lastStepThermalSteps = conf->ReadLong("/lastStepThermalSteps", 16000);
		lastStepStatsSteps = conf->ReadLong("/lastStepStatsSteps", 64000);

		gradDescParam = conf->ReadDouble("/gradDescParam", 0.3);
	}
	Close();
}

void Options::Save()
{
	Open();
	wxConfigBase* conf = wxConfigBase::Get(false);
	if (conf)
	{
		conf->Write("/nrThreads", static_cast<long int>(nrThreads));
		conf->Write("/nrWalkers", static_cast<long int>(nrWalkers));
		conf->Write("/basis", static_cast<long int>(basis));
		conf->Write("/Z1", static_cast<long int>(Z1));
		conf->Write("/Z2", static_cast<long int>(Z2));
		conf->Write("/useZ2", static_cast<long int>(useZ2 ? 1 : 0));
		
		conf->Write("/distance", distance);
		conf->Write("/deltat", deltat);

		conf->Write("/firstStageGradientDescentSteps", static_cast<long int>(firstStageGradientDescentSteps));
		conf->Write("/firstThermalSteps", static_cast<long int>(firstThermalSteps));
		conf->Write("/firstStageThermalStep", static_cast<long int>(firstStageThermalSteps));
		conf->Write("/firstStageStatsSteps", static_cast<long int>(firstStageStatsSteps));
		conf->Write("/secondStageGradientDescentSteps", static_cast<long int>(secondStageGradientDescentSteps));
		conf->Write("/secondStageThermalSteps", static_cast<long int>(secondStageThermalSteps));
		conf->Write("/secondStageStatsSteps", static_cast<long int>(secondStageStatsSteps));
		conf->Write("/thirdStageGradientDescentSteps", static_cast<long int>(thirdStageGradientDescentSteps));
		conf->Write("/thirdStageThermalSteps", static_cast<long int>(thirdStageThermalSteps));
		conf->Write("/thirdStageStatsSteps", static_cast<long int>(thirdStageStatsSteps));
		conf->Write("/lastStepThermalSteps", static_cast<long int>(lastStepThermalSteps));
		conf->Write("/lastStepStatsSteps", static_cast<long int>(lastStepStatsSteps));

		conf->Write("/gradDescParam", gradDescParam);
	}

	if (m_fileconfig)
		m_fileconfig->Flush();
	Close();
}
