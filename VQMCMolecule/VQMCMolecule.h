#pragma once

#include "Options.h"

class VQMCMolecule
{
public:
	static void Compute(const Options& opt, const Chemistry::Basis& basisSTOXG);
};

