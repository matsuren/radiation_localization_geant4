//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#include "G4VUserDetectorConstruction.hh"
#include <G4MultiFunctionalDetector.hh>
#include <memory>
class G4VPhysicalVolume;
class GeometryMessenger;
class SensitiveVolume;

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

  // detector pose
  G4double pos_X_detector; // X-location LogV detector
  G4double pos_Y_detector; // Y-location LogV detector
  G4double pos_Z_detector; // Z-location LogV detector
  G4double rot_Y_detector; // Y-rotation LogV detector

  std::unique_ptr<GeometryMessenger> messenger;
  static G4ThreadLocal SensitiveVolume *ptr_SV;

  // logical volume for sensitive detector
  static G4ThreadLocal G4LogicalVolume *logVol_detector;

public:
  void SetDetectorPosition(G4double x, G4double y, G4double z);
  void SetDetectorRot(G4double x, G4double y, G4double z);
  void UpdateGeometry();
  void SetDetectorVerboseLevel(G4int lv);
};
