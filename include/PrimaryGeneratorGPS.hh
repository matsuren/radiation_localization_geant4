//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PrimaryGeneratorGPS.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
class G4Event;
class G4GeneralParticleSource;

//------------------------------------------------------------------------------
class PrimaryGeneratorGPS : public G4VUserPrimaryGeneratorAction
//------------------------------------------------------------------------------
{
public:
  PrimaryGeneratorGPS();
  ~PrimaryGeneratorGPS() override;

public:
  void GeneratePrimaries(G4Event *) override;

private:
  G4GeneralParticleSource *fpParticleGPS;
};
