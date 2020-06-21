//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PrimaryGeneratorGPS.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "PrimaryGeneratorGPS.hh"
#include "G4GeneralParticleSource.hh"

//------------------------------------------------------------------------------
PrimaryGeneratorGPS::PrimaryGeneratorGPS()
    : fpParticleGPS{nullptr}
//------------------------------------------------------------------------------
{
  fpParticleGPS = new G4GeneralParticleSource();
}

//------------------------------------------------------------------------------
PrimaryGeneratorGPS::~PrimaryGeneratorGPS()
//------------------------------------------------------------------------------
{
  delete fpParticleGPS;
}

//------------------------------------------------------------------------------
void PrimaryGeneratorGPS::GeneratePrimaries(G4Event *anEvent)
//------------------------------------------------------------------------------
{
  fpParticleGPS->GeneratePrimaryVertex(anEvent);
}