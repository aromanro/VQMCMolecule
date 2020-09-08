#include <iostream>
#include <fstream>
#include <chrono>
#include <future>

#include "Basis.h"
#include "Molecule.h"
#include "VQMC.h"
#include "VQMCMolecule.h"
#include "ChemUtils.h"


void VQMCMolecule::Compute(const Options& opt, const Chemistry::Basis& basisSTOXG)
{    
    Systems::AtomWithShells atom1;
    Systems::AtomWithShells atom2;
    for (auto& atom : basisSTOXG.atoms)
    {
        if (atom.Z == opt.Z1)
        {
            atom1 = atom;
            if (!opt.useZ2) break;
        }
        
        if (atom.Z == opt.Z2)
            atom2 = atom;
    }

    Systems::Molecule molecule;

    molecule.atoms.push_back(atom1);
    
    std::cout << "Starting " << Chemistry::ChemUtils::GetAtomNameForZ(atom1.Z);

    if (opt.useZ2)
    {
        atom2.position.X = opt.distance;
        molecule.atoms.push_back(atom2);

        if (atom2.Z != atom1.Z)
            std::cout << Chemistry::ChemUtils::GetAtomNameForZ(atom2.Z);
        else
            std::cout << "2";
    }

    std::cout << " computations, using ";
    
    switch (opt.basis)
    {
    case 0:
        std::cout << "STO3G";
        break;
    case 1:
        std::cout << "STO6G";
        break;
    case 2:
        std::cout << "3-21G";
        break;
    case 3:
        std::cout << "6-21G";
        break;
    case 4:
        std::cout << "6-31G";
        break;
    case 5:
        std::cout << "6-31G*";
        break;
    case 6:
        std::cout << "6-31+G**";
        break;
    }
        
    std::cout << std::endl;

    molecule.Init();

    const int nrWalkers = opt.nrWalkers;

    std::vector<VQMC> vqmcWalkers(nrWalkers);
    
    Random initialSeed;
    for (unsigned int i = 0; i < static_cast<unsigned int>(nrWalkers); ++i)
    {
        vqmcWalkers[i].SetDeltat(opt.deltat);
        vqmcWalkers[i].Init(molecule, static_cast<int>(initialSeed.getZeroOne() * 1E5));
    }


    const unsigned int NrE = molecule.alphaElectrons + molecule.betaElectrons;
    
    int thermalSteps = opt.firstThermalSteps * NrE;
    int cycleSteps = opt.firstStageStatsSteps * NrE;

    double beta = opt.beta;

    const double cnst = opt.gradDescParam; // for gradient descent

    const int nrThreads = opt.nrThreads > 0 ? opt.nrThreads : 1;

    std::vector<std::future<std::tuple<double, double, double>>> tasks(nrThreads);
    std::launch launchType = nrThreads == 1 ? std::launch::deferred : std::launch::async;

    auto t1 = std::chrono::high_resolution_clock::now();
    
    int step = static_cast<int>(ceil(static_cast<double>(nrWalkers) / nrThreads));
    if (step < 1) step = 1;
    
    int nrGradientDescentSteps = opt.firstStageGradientDescentSteps + opt.secondStageGradientDescentSteps + opt.thirdStageGradientDescentSteps;
    int secondStageLimit = opt.firstStageGradientDescentSteps + opt.secondStageGradientDescentSteps;

    const double nuclearRepulsionEnergy = molecule.NuclearRepulsionEnergy();
    const int cyclesRefresh = opt.cyclesRefresh;

    for (int i = 0; i < nrGradientDescentSteps; ++i)
    {

        int nextPos;

        int startPos = 0;
        for (int t = 0; t < nrThreads; ++t, startPos = nextPos)
        {
            if (t == nrThreads - 1) nextPos = nrWalkers;
            else nextPos = startPos + step;

            if (nextPos > nrWalkers) nextPos = nrWalkers;

            tasks[t] = std::async(launchType, [&vqmcWalkers, startPos, nextPos, thermalSteps, cycleSteps, beta, cyclesRefresh]()->std::tuple<double, double, double>
                {
                    double E = 0;
                    double E2 = 0;
                    double dEdB = 0;
                    // loop over walkers
                    for (int w = startPos; w < nextPos; ++w)
                    {
                        double Ep;
                        double E2p;
                        double dEdBp;
                        std::tie(Ep, E2p, dEdBp) = vqmcWalkers[w].SamplingFokkerPlanck(thermalSteps, cycleSteps, beta, cyclesRefresh);
                        E += Ep;
                        E2 += E2p;
                        dEdB += dEdBp;
                    }

                    return std::make_tuple(E, E2, dEdB);
                }
            );
        }

        double E = 0;
        double E2 = 0;
        double dEdB = 0;
        for (auto& task : tasks)
        {
            double Ep;
            double E2p;
            double dEdBp;
            std::tie(Ep, E2p, dEdBp) = task.get();
            E += Ep;
            E2 += E2p;
            dEdB += dEdBp;
        }
        E /= nrWalkers;
        E2 /= nrWalkers;
        dEdB /= nrWalkers;

        const unsigned int moves = NrE * cycleSteps;

        if (0 == i)
            thermalSteps = opt.firstStageThermalSteps * NrE;

        std::cout << "Step: " << i + 1 << "\tEnergy: " << E + nuclearRepulsionEnergy << "\tError estimation: " << sqrt(abs(E2 - E * E) / moves) << "\tdE/db= " << dEdB << "\tBeta: " << beta << std::endl;

        if (i == opt.firstStageGradientDescentSteps)
        {
            thermalSteps = opt.secondStageThermalSteps * NrE;
            cycleSteps = opt.secondStageStatsSteps * NrE;
        }
        else if (i == secondStageLimit)
        {
            thermalSteps = opt.thirdStageThermalSteps * NrE;
            cycleSteps = opt.thirdStageStatsSteps * NrE;
        }
        else if (i == nrGradientDescentSteps - 2) // will enter last cycle
        {
            thermalSteps = opt.lastStepThermalSteps * NrE;
            cycleSteps = opt.lastStepStatsSteps * NrE;
        }

        beta -= cnst * dEdB; // gradient descent

        if (beta < 0.3) beta = 0.3; // don't allow it go very low
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    std::cout << "Duration: " << duration << " seconds" << std::endl;
}

