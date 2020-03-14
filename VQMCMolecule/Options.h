#pragma once

#define wxNEEDS_DECL_BEFORE_TEMPLATE
#define _MATH_DEFINES_DEFINED

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

#include <wx/fileconf.h>

class Options
{
public:
	Options();

	~Options()
	{
		delete m_fileconfig;
	}

	// avoid double deletion of m_fileconfig at destruction if copied
	Options(const Options& other)
		:
		nrThreads(other.nrThreads),
		nrWalkers(other.nrWalkers),
		basis(other.basis),
		Z1(other.Z1),
		Z2(other.Z2),
		useZ2(other.useZ2),
		distance(other.distance),
		deltat(other.deltat),
		firstStageGradientDescentSteps(other.firstStageGradientDescentSteps),
		firstThermalSteps(other.firstThermalSteps),
		firstStageThermalSteps(other.firstStageThermalSteps),
		firstStageStatsSteps(other.firstStageStatsSteps),
		secondStageGradientDescentSteps(other.secondStageGradientDescentSteps),
		secondStageThermalSteps(other.secondStageThermalSteps),
		secondStageStatsSteps(other.secondStageStatsSteps),
		thirdStageGradientDescentSteps(other.thirdStageGradientDescentSteps),
		thirdStageThermalSteps(other.thirdStageThermalSteps),
		thirdStageStatsSteps(other.thirdStageStatsSteps),
		lastStepThermalSteps(other.lastStepThermalSteps),
		lastStepStatsSteps(other.lastStepStatsSteps),
		gradDescParam(other.gradDescParam),
		beta(other.beta),
		m_fileconfig(nullptr)
	{
	}

	Options& operator=(const Options& other)
	{
		nrThreads = other.nrThreads;
		nrWalkers = other.nrWalkers;

		basis = other.basis;

		Z1 = other.Z1;
		Z2 = other.Z2;
		useZ2 = other.useZ2;
		distance = other.distance;

		deltat = other.deltat;

		firstStageGradientDescentSteps = other.firstStageGradientDescentSteps;

		firstThermalSteps = other.firstThermalSteps;
		firstStageThermalSteps = other.firstStageThermalSteps;
		firstStageStatsSteps = other.firstStageStatsSteps;

		secondStageGradientDescentSteps = other.secondStageGradientDescentSteps;
		secondStageThermalSteps = other.secondStageThermalSteps;
		secondStageStatsSteps = other.secondStageStatsSteps;

		thirdStageGradientDescentSteps = other.thirdStageGradientDescentSteps;
		thirdStageThermalSteps = other.thirdStageThermalSteps;
		thirdStageStatsSteps = other.thirdStageStatsSteps;

		lastStepThermalSteps = other.lastStepThermalSteps;
		lastStepStatsSteps = other.lastStepStatsSteps;

		gradDescParam = other.gradDescParam;
		beta = other.beta;

		m_fileconfig = nullptr;

		return *this;
	}


	void Load();
	void Save();

	int nrThreads;
	int nrWalkers;

	int basis; // 0 STO3G, 1 STO6G and so on, if I'll add more

	int Z1;
	int Z2;
	bool useZ2; // true - diatomic molecule, false - compute only an atom with Z1
	double distance; // distance between atoms if it's a diatomic molecule

	double deltat; // Fokker-Planck time step

	int firstStageGradientDescentSteps;

	int firstThermalSteps;
	int firstStageThermalSteps;
	int firstStageStatsSteps;

	int secondStageGradientDescentSteps;
	int secondStageThermalSteps;
	int secondStageStatsSteps;

	int thirdStageGradientDescentSteps;
	int thirdStageThermalSteps;
	int thirdStageStatsSteps;

	int lastStepThermalSteps;
	int lastStepStatsSteps;

	double gradDescParam;
	double beta;


protected:
	void Open();
	void Close();

	wxFileConfig* m_fileconfig;
};

