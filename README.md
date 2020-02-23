# VQMCMolecule
Variational Quantum Monte Carlo for a molecule, using Fokker-Planck/Langevin approach


WARNING! Still work in progress!

It requires quite a lot of computation power, by default now it will start for 20 threads setting, so be sure if you try it out to have enough computing power.

It works quite well for atoms, better than Hartree-Fock with the same basis set and with STO3G even better than Hartree-Fock with STO6G.
For molecules, works for diatomic ones (for the other ones, quite badly for now, for reasons I'll detail later, there are hints in the code comments).

The refered formulae in the code are here: https://github.com/CompPhysics/ComputationalPhysics2/tree/gh-pages/doc/Literature in Lectures2015.pdf.
