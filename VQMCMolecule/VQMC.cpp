#include "VQMC.h"

#include "MathUtils.h"

#include <iostream>
#include <fstream>


void VQMC::Init(const Systems::Molecule& mol, int seed)
{
    random.InitSeed(seed);

    molecule = mol;

    wavefunction.Init(molecule, random);
}


std::tuple<double, double, double> VQMC::SamplingFokkerPlanck(int thermalSteps, int cycles, double beta, int cyclesRefreshOpt)
{    
    const unsigned int Ne = molecule.alphaElectrons + molecule.betaElectrons;
   
    double E = 0;
    double E2 = 0;

    double logDerivBeta = 0;
    double logDerivBetaE = 0;
    double o2dt = 1. / (2. * dt);

    //long long int accepted = 0;

    const long long int totalCycles = static_cast<long long int>(thermalSteps) + cycles;
    const int cyclesRefresh = (cyclesRefreshOpt > 0 && cyclesRefreshOpt > static_cast<int>(Ne)) ? static_cast<int>(cyclesRefreshOpt / Ne) : 1;

    for (long long int cycle = 0; cycle < totalCycles; ++cycle)
    {
        // recompute the Slater inverse, in case it accumulates errors
        if (cycle % cyclesRefresh == 0) 
            wavefunction.ComputeSlaterInv();

        // one particle move at a time!
        for (unsigned int p = 0; p < Ne; ++p)
        {
            const Vector3D oldHalfF(wavefunction.HalfQuantumForce(wavefunction.currentParticles[p], p, beta));

            // drift and diffusion
            const Vector3D newPos(wavefunction.currentParticles[p] + oldHalfF * dt + random.getRandomGaussianVector() * sdt);

            const double SlaterRatio = wavefunction.SlaterRatio(newPos, p);
            const Vector3D newHalfF(wavefunction.HalfQuantumForce(newPos, p, beta, SlaterRatio));

            // Metropolis test
            const Vector3D difR(newPos - wavefunction.currentParticles[p]);
            const Vector3D RoldRnew(difR - oldHalfF * dt);
            const Vector3D RnewRold(-difR - newHalfF * dt);
                
            const double JastrowRatio = wavefunction.JastrowRatio(newPos, p, beta);
            const double WavefunctionRatio = JastrowRatio * SlaterRatio;
             
            if (const double ratio = WavefunctionRatio * WavefunctionRatio * exp((RoldRnew * RoldRnew - RnewRold * RnewRold) * o2dt); ratio >= 1 || random.getZeroOne() <= ratio)
            {
                // accept move
                wavefunction.currentParticles[p] = newPos;
                wavefunction.UpdateInverses(newPos, p, SlaterRatio);

                //++accepted; // for statistical purposes
            }
            // else reject move

            if (cycle > thermalSteps)
            {
                const double localE = wavefunction.LocalEnergy(beta);
                const double ldb = wavefunction.log_deriv_beta_wf(beta);

                logDerivBeta += ldb;
                logDerivBetaE += ldb * localE;

                E += localE;
                E2 += localE * localE;
            }
        }
    }

    //std::cout << "Acceptance ratio: " << static_cast<double>(accepted) / (totalCycles * static_cast<double>(Ne)) << std::endl;

    const double dv = Ne * static_cast<double>(cycles);
    const double odv = 1. / dv;

    E *= odv;
    E2 *= odv;
    logDerivBeta *= odv;
    logDerivBetaE *= odv;

    return std::make_tuple(E, E2, 2. * (logDerivBetaE - E * logDerivBeta));
}

