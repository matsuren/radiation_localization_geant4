//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

#include "G4VUserDetectorConstruction.hh"
class G4VPhysicalVolume;

//------------------------------------------------------------------------------
class Geometry : public G4VUserDetectorConstruction
//------------------------------------------------------------------------------
{
public:
  Geometry();
  ~Geometry() override;

  G4VPhysicalVolume *Construct() override;
  void ConstructSDandField() override;

private:
  G4VPhysicalVolume *ConstructWorld();
  G4LogicalVolume *ConstructDetector();
  G4LogicalVolume *ConstructPixelDetector();
};
