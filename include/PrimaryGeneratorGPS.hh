//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PrimaryGeneratorGPS.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef PrimaryGeneratorGPS_h
#define PrimaryGeneratorGPS_h 1

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
#endif
