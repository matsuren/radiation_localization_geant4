//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "GeometryMessenger.hh"
#include "SensitiveVolume.hh"
#include <G4PVReplica.hh>
#include <G4SDManager.hh>

//------------------------------------------------------------------------------
Geometry::Geometry()
    : pos_X_detector(0.0 * cm), pos_Y_detector(4.0 * cm),
      pos_Z_detector(0.0 * cm), rot_Y_detector(0.0 * deg), ptr_SV(nullptr),
      logVol_detector(nullptr), logVol_PixElmt(nullptr)
//------------------------------------------------------------------------------
{
  messenger = std::make_unique<GeometryMessenger>(this);
}
//------------------------------------------------------------------------------
Geometry::~Geometry() = default;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
G4VPhysicalVolume *Geometry::Construct()
//------------------------------------------------------------------------------
{
  return ConstructWorld();
}

//------------------------------------------------------------------------------
G4VPhysicalVolume *Geometry::ConstructWorld()
//------------------------------------------------------------------------------
{
  G4cout << " === Geometry::ConstructWorld() =====" << G4endl;
  // Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();

  ////////////////////////////////////////////////////////
  /// Define World Volume
  ////////////////////////////////////////////////////////
  // Define the shape of solid
  G4double leng_X_World = 2.0 * m; // X-full-length of world
  G4double leng_Y_World = 2.0 * m; // Y-full-length of world
  G4double leng_Z_World = 2.0 * m; // Z-full-length of world
  G4Box *solid_World = new G4Box("Solid_World", leng_X_World / 2.0,
                                 leng_Y_World / 2.0, leng_Z_World / 2.0);

  // Define logical volume
  G4Material *materi_World = materi_Man->FindOrBuildMaterial("G4_AIR");
  G4LogicalVolume *logVol_World =
      new G4LogicalVolume(solid_World, materi_World, "LogVol_World");
  logVol_World->SetVisAttributes(G4VisAttributes::Invisible);

  // Placement of logical volume
  G4int copyNum_World = 0; // Set ID number of world
  G4PVPlacement *physVol_World = new G4PVPlacement(
      G4Transform3D(), "PhysVol_World", logVol_World, 0, false, copyNum_World);

  G4int copyNum_LogV = 1;
  ////////////////////////////////////////////////////////
  /// Concrete ground
  ////////////////////////////////////////////////////////
  // Define the shape of solid
  G4double leng_X_ground = 2.0 * m; // X-full-length of world
  G4double leng_Y_ground = 0.5 * m; // Y-full-length of world
  G4double leng_Z_ground = 2.0 * m; // Z-full-length of world
  G4Box *solid_ground = new G4Box("Solid_ground", leng_X_ground / 2.0,
                                  leng_Y_ground / 2.0, leng_Z_ground / 2.0);

  // Define logical volume
  G4Material *materi_ground = materi_Man->FindOrBuildMaterial("G4_CONCRETE");
  G4LogicalVolume *logVol_ground =
      new G4LogicalVolume(solid_ground, materi_ground, "LogVol_ground");
  auto color_green = G4Colour(0.0, 1.0, 0.0, 0.4);
  G4VisAttributes *attribute = new G4VisAttributes(color_green);
  attribute->SetForceSolid(true);
  logVol_ground->SetVisAttributes(attribute);

  // Placement of logical volume
  copyNum_LogV = 1;
  auto trans3D_ground = G4Transform3D(
      G4RotationMatrix(), G4ThreeVector(0, -leng_Y_ground / 2 - 1 * mm, 0));
  new G4PVPlacement(trans3D_ground, "PhysVol_ground", logVol_ground,
                    physVol_World, false, copyNum_LogV, true);

  ////////////////////////////////////////////////////////
  /// Construct Detector
  ////////////////////////////////////////////////////////
  auto logVol_container = ConstructDetector();
  // Placement of logical volume
  G4ThreeVector threeVect_LogV =
      G4ThreeVector(pos_X_detector, pos_Y_detector, pos_Z_detector);
  G4RotationMatrix rotMtrx_LogV = G4RotationMatrix();
  rotMtrx_LogV.rotateY(rot_Y_detector);
  G4Transform3D trans3D_LogV = G4Transform3D(rotMtrx_LogV, threeVect_LogV);
  copyNum_LogV = 999; // Set ID number of LogV

  new G4PVPlacement(trans3D_LogV, "PhysVol_container", logVol_container,
                    physVol_World, false, copyNum_LogV, true);

  ////////////////////////////////////////////////////////
  /// Pixel Detector
  ////////////////////////////////////////////////////////
  auto logVol_PixEnvG = ConstructPixelDetector();
  // Placement of the 'Pixel Detector' to the world: Put the 'global envelop'
  G4double pos_X_LogV_PixEnvG = 0.0 * cm;  // X-location LogV_PixEnvG
  G4double pos_Y_LogV_PixEnvG = 1.0 * cm;  // Y-location LogV_PixEnvG
  G4double pos_Z_LogV_PixEnvG = -4.0 * cm; // Z-location LogV_PixEnvG
  auto threeVect_LogV_PixEnvG =
      G4ThreeVector(pos_X_LogV_PixEnvG, pos_Y_LogV_PixEnvG, pos_Z_LogV_PixEnvG);
  auto rotMtrx_LogV_PixEnvG = G4RotationMatrix();
  rotMtrx_LogV_PixEnvG.rotateZ(20);
  auto trans3D_LogV_PixEnvG =
      G4Transform3D(rotMtrx_LogV_PixEnvG, threeVect_LogV_PixEnvG);

  //  G4int copyNum_LogV_PixEnvG = 2000; // Set ID number of LogV_PixEnvG
  //  new G4PVPlacement(trans3D_LogV_PixEnvG, "PhysVol_PixEnvG", logVol_PixEnvG,
  //                    physVol_World, false, copyNum_LogV_PixEnvG);

  // Return the physical world
  return physVol_World;
}
//------------------------------------------------------------------------------
G4LogicalVolume *Geometry::ConstructDetector()
//------------------------------------------------------------------------------
{
  // Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();
  ////////////////////////////////////////////////////////
  /// Define Detector
  ////////////////////////////////////////////////////////
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
  logVol_detector = new G4LogicalVolume(solid_detector, materi_detector,
                                        "LogVol_detector", 0, 0, 0);

  // Define aluminum container to block alpha and beta
  // Define the shape of solid
  auto solid_container = new G4Box(
      "Solid_container", (leng_X_detector + 2.0 * mm) / 2.0,
      (leng_Y_detector + 2.0 * mm) / 2.0, (leng_Z_detector + 2.0 * mm) / 2.0);
  // Define logical volume (Container is Aluminun)
  G4Material *materi_WaterCont = materi_Man->FindOrBuildMaterial("G4_Al");
  auto logVol_container = new G4LogicalVolume(solid_container, materi_WaterCont,
                                              "LogVol_container", 0, 0, 0);
  // Placement of logical volume
  G4ThreeVector pos = G4ThreeVector(0.0 * cm, 0.0 * cm, 0.0 * cm);
  G4RotationMatrix rot = G4RotationMatrix();
  G4Transform3D trans3D_LogV = G4Transform3D(rot, pos);

  G4int copyNum_LogV = 1000; // Set ID number of LogV
  new G4PVPlacement(trans3D_LogV, logVol_detector, "PhysVol_detector",
                    logVol_container, false, copyNum_LogV, true);

  return logVol_container;
}

//------------------------------------------------------------------------------
G4LogicalVolume *Geometry::ConstructPixelDetector()
//------------------------------------------------------------------------------
{
  // Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();
  ////////////////////////////////////////////////////////
  /// Define Pixel Detector
  ////////////////////////////////////////////////////////

  // Define 'Pixel Detector' - Global Envelop
  // Define the shape of the global envelop
  G4double leng_X_PixEnvG = 16.8 * mm; // X-full-length of pixel: global envelop
  G4double leng_Y_PixEnvG = 20.0 * mm; // Y-full-length of pixel: global envelop
  G4double leng_Z_PixEnvG = 3 * mm;    // Z-full-length of pixel: global envelop
  auto solid_PixEnvG = new G4Box("Solid_PixEnvG", leng_X_PixEnvG / 2.0,
                                 leng_Y_PixEnvG / 2.0, leng_Z_PixEnvG / 2.0);

  // Define logical volume of the global envelop
  G4Material *materi_PixEnvG = materi_Man->FindOrBuildMaterial("G4_AIR");
  auto logVol_PixEnvG =
      new G4LogicalVolume(solid_PixEnvG, materi_PixEnvG, "LogVol_PixEnvG");
  //  logVol_PixEnvG->SetVisAttributes(G4VisAttributes::Invisible);

  // Define 'Pixel Detector' - Local Envelop (divided the global envelop in
  // Y-direction) Define the shape of the local envelop
  G4int nDiv_Y = 5; // Number of divisions in Y-direction
  G4double leng_X_PixEnvL =
      leng_X_PixEnvG; // X-full-length of pixel: local envelop
  G4double leng_Y_PixEnvL =
      leng_Y_PixEnvG / nDiv_Y; // Y-full-length of pixel: local envelop
  G4double leng_Z_PixEnvL =
      leng_Z_PixEnvG; // Z-full-length of pixel: local envelop
  auto solid_PixEnvL = new G4Box("Solid_PixEnvL", leng_X_PixEnvL / 2.0,
                                 leng_Y_PixEnvL / 2.0, leng_Z_PixEnvL / 2.0);

  // Define logical volume of the local envelop
  G4Material *materi_PixEnvL = materi_Man->FindOrBuildMaterial("G4_AIR");
  auto logVol_PixEnvL =
      new G4LogicalVolume(solid_PixEnvL, materi_PixEnvL, "LogVol_PixEnvL");
  logVol_PixEnvL->SetVisAttributes(G4VisAttributes::Invisible);

  // Placement of the local envelop to the global envelop using Replica
  new G4PVReplica("PhysVol_PixEnvL", logVol_PixEnvL, logVol_PixEnvG, kYAxis,
                  nDiv_Y, leng_Y_PixEnvL);

  // Define 'Pixel Detector' - Pixel Element (divided the local envelop in
  // X-direction) Define the shape of the pixel element
  G4int nDiv_X = 5; // Number of divisions in Y-direction
  G4double leng_X_PixElmt =
      leng_X_PixEnvG / nDiv_X; // X-full-length of pixel: pixel element
  G4double leng_Y_PixElmt =
      leng_Y_PixEnvG / nDiv_Y; // Y-full-length of pixel: pixel element
  G4double leng_Z_PixElmt =
      leng_Z_PixEnvG; // Z-full-length of pixel: pixel element
  auto solid_PixElmt = new G4Box("Solid_PixElmt", leng_X_PixElmt / 2.0,
                                 leng_Y_PixElmt / 2.0, leng_Z_PixElmt / 2.0);

  // Define logical volume of the pixel element
  G4Material *materi_PixElmt =
      materi_Man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  logVol_PixElmt =
      new G4LogicalVolume(solid_PixElmt, materi_PixElmt, "LogVol_PixElmt");
  // Add color
  auto color = G4Colour(1.0, 0.0, 0.0, 0.1);
  G4VisAttributes *attribute = new G4VisAttributes(color);
  attribute->SetForceSolid(true);
  logVol_PixElmt->SetVisAttributes(attribute);
  //  logVol_PixElmt->SetVisAttributes(G4VisAttributes::Invisible);

  // Placement of pixel elements to the local envelop using Replica
  new G4PVReplica("PhysVol_PixElmt", logVol_PixElmt, logVol_PixEnvL, kXAxis,
                  nDiv_X, leng_X_PixElmt);

  return logVol_PixEnvG;
}
//------------------------------------------------------------------------------
void Geometry::ConstructSDandField()
//------------------------------------------------------------------------------
{
  ////////////////////////////////////////////////////////
  /// Sensitive volume
  /// https://twiki.cern.ch/twiki/bin/view/Geant4/QuickMigrationGuideForGeant4V10
  ////////////////////////////////////////////////////////
  if (ptr_SV == nullptr) {
    ptr_SV = new SensitiveVolume("SensitiveVolume");
    ptr_SV->SetVerboseLevel(0);
    G4SDManager::GetSDMpointer()->AddNewDetector(ptr_SV);
  }
  // Add sensitivity
  if (logVol_detector)
    SetSensitiveDetector(logVol_detector, ptr_SV);
  if (logVol_PixElmt)
    SetSensitiveDetector(logVol_PixElmt, ptr_SV);
}

//------------------------------------------------------------------------------
void Geometry::SetDetectorPosition(G4double x, G4double y, G4double z)
//------------------------------------------------------------------------------
{
  // detector pose
  pos_X_detector = x; // X-location LogV detector
  pos_Y_detector = y; // Y-location LogV detector
  pos_Z_detector = z; // Z-location LogV detector
}

//------------------------------------------------------------------------------
void Geometry::SetDetectorRot(G4double x, G4double y, G4double z)
//------------------------------------------------------------------------------
{
  rot_Y_detector = y; // Y-rotation LogV detector
  x;
  z;
}

//------------------------------------------------------------------------------
void Geometry::UpdateGeometry()
//------------------------------------------------------------------------------
{
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

//------------------------------------------------------------------------------
void Geometry::SetDetectorVerboseLevel(G4int lv)
//------------------------------------------------------------------------------
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(lv);
}
