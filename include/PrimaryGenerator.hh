//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PrimaryGenerator.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef PrimaryGenerator_h
#define PrimaryGenerator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
class G4Event;
class G4ParticleGun;

//------------------------------------------------------------------------------
class PrimaryGenerator : public G4VUserPrimaryGeneratorAction
//------------------------------------------------------------------------------
{
public:
  PrimaryGenerator();
  ~PrimaryGenerator() override;

public:
  void GeneratePrimaries(G4Event *) override;

private:
  G4ParticleGun *fpParticleGun;
};
#endif
