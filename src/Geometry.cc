//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "SensitiveVolume.hh"
#include <G4SDManager.hh>

//------------------------------------------------------------------------------
Geometry::Geometry() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
Geometry::~Geometry() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
G4VPhysicalVolume *Geometry::Construct()
//------------------------------------------------------------------------------
{
  return ConstructDetector();
}

//------------------------------------------------------------------------------
G4VPhysicalVolume *Geometry::ConstructDetector()
//------------------------------------------------------------------------------
{
  // Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();

  // Define 'World Volume'
  // Define the shape of solid
  G4double leng_X_World = 1.0 * m; // X-full-length of world
  G4double leng_Y_World = 1.0 * m; // Y-full-length of world
  G4double leng_Z_World = 1.0 * m; // Z-full-length of world
  G4Box *solid_World = new G4Box("Solid_World", leng_X_World / 2.0,
                                 leng_Y_World / 2.0, leng_Z_World / 2.0);

  // Define logical volume
  G4Material *materi_World = materi_Man->FindOrBuildMaterial("G4_AIR");
  G4LogicalVolume *logVol_World =
      new G4LogicalVolume(solid_World, materi_World, "LogVol_World");
  //   logVol_World->SetVisAttributes (G4VisAttributes::Invisible);

  // Placement of logical volume
  G4int copyNum_World = 0; // Set ID number of world
  G4PVPlacement *physVol_World = new G4PVPlacement(
      G4Transform3D(), "PhysVol_World", logVol_World, 0, false, copyNum_World);

  // Define Detector
  // Define the shape of solid
  G4double leng_X_detector = 5.0 * cm;
  G4double leng_Y_detector = 5.0 * cm;
  G4double leng_Z_detector = 5.0 * cm;
  G4Box *solid_detector =
      new G4Box("Solid_detector", leng_X_detector / 2.0, leng_Y_detector / 2.0,
                leng_Z_detector / 2.0);

  // Define material
  enum DetectorMat { BGO, CeBr3 };
  const DetectorMat materi_detector_selector = CeBr3;
  G4Material *materi_detector = nullptr;
  switch (materi_detector_selector) {
  case BGO:
    materi_detector = materi_Man->FindOrBuildMaterial("G4_BGO");
    break;
  case CeBr3:
    G4Element *Br = new G4Element("Bromium", "Br", 35., 79.904 * g / mole);
    G4Element *Ce = new G4Element("Cerium", "Ce", 58., 140.116 * g / mole);
    G4Material *CeBr3 = new G4Material("CeBr3", 5.1 * g / cm3, 2);
    CeBr3->AddElement(Ce, 1);
    CeBr3->AddElement(Br, 3);
    materi_detector = CeBr3;
    break;
  }

  // Define logical volume
  G4LogicalVolume *logVol_detector = new G4LogicalVolume(
      solid_detector, materi_detector, "LogVol_detector", 0, 0, 0);

  // Placement of logical volume
  G4double pos_X_LogV = 0.0 * cm; // X-location LogV
  G4double pos_Y_LogV = 0.0 * cm; // Y-location LogV
  G4double pos_Z_LogV = 0.0 * cm; // Z-location LogV
  G4ThreeVector threeVect_LogV =
      G4ThreeVector(pos_X_LogV, pos_Y_LogV, pos_Z_LogV);
  G4RotationMatrix rotMtrx_LogV = G4RotationMatrix();
  G4Transform3D trans3D_LogV = G4Transform3D(rotMtrx_LogV, threeVect_LogV);

  G4int copyNum_LogV = 1000; // Set ID number of LogV
  new G4PVPlacement(trans3D_LogV, "PhysVol_detector", logVol_detector,
                    physVol_World, false, copyNum_LogV);


    // Sensitive volume
    auto aSV = new SensitiveVolume("SensitiveVolume");
  logVol_detector->SetSensitiveDetector(aSV);         // Add sensitivity to the logical volume
    auto SDman = G4SDManager::GetSDMpointer();
    SDman->AddNewDetector(aSV);

  // Return the physical world
  return physVol_World;
}
