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
#include <G4PVReplica.hh>
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

  ////////////////////////////////////////////////////////
  /// Define World Volume
  ////////////////////////////////////////////////////////
  // Define the shape of solid
  G4double leng_X_World = 0.7 * m; // X-full-length of world
  G4double leng_Y_World = 0.7 * m; // Y-full-length of world
  G4double leng_Z_World = 0.7 * m; // Z-full-length of world
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
  /// Define Lead box
  ////////////////////////////////////////////////////////
  G4LogicalVolume *logVol_lead_inside = nullptr;
  if (false) {
    // Define 'lead box'
    // Define the shape of solid
    G4double leng_X_lead = 16.0 * cm; // X-full-length of water box
    G4double leng_Y_lead = 16.0 * cm; // Y-full-length of water box
    G4double leng_Z_lead = 16.0 * cm; // Z-full-length of water box
    auto solid_lead = new G4Box("Solid_lead", leng_X_lead / 2.0,
                                leng_Y_lead / 2.0, leng_Z_lead / 2.0);

    // Define logical volume
    auto logVol_lead = new G4LogicalVolume(
        solid_lead, materi_Man->FindOrBuildMaterial("G4_Pb"), "LogVol_lead", 0,
        0, 0);
    G4double pos_X_lead = 0.0 * cm; // X-location LogV
    G4double pos_Y_lead = 0.0 * cm; // Y-location LogV
    G4double pos_Z_lead = 0.0 * cm; // Z-location LogV
    auto threeVect_lead = G4ThreeVector(pos_X_lead, pos_Y_lead, pos_Z_lead);
    auto rotMtrx_lead = G4RotationMatrix();
    auto trans3D_lead = G4Transform3D(rotMtrx_lead, threeVect_lead);
    copyNum_LogV = 900; // Set ID number of LogV
    new G4PVPlacement(trans3D_lead, "PhysVol_lead", logVol_lead, physVol_World,
                      false, copyNum_LogV, true);

    // lead inside
    auto solid_lead_inside = new G4Box(
        "Solid_lead_inside", (leng_X_lead - 6.0 * cm) / 2.0,
        (leng_Y_lead - 6.0 * cm) / 2.0, (leng_Z_lead - 6.0 * cm) / 2.0);
    logVol_lead_inside = new G4LogicalVolume(
        solid_lead_inside, materi_Man->FindOrBuildMaterial("G4_AIR"),
        "LogVol_lead_inside", 0, 0, 0);
    copyNum_LogV = 901; // Set ID number of LogV
    new G4PVPlacement(trans3D_lead, logVol_lead_inside, "PhysVol_lead_inside",
                      logVol_lead, false, copyNum_LogV, true);
  }

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
  G4LogicalVolume *logVol_detector = new G4LogicalVolume(
      solid_detector, materi_detector, "LogVol_detector", 0, 0, 0);

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
  G4double pos_X_LogV = 0.0 * cm; // X-location LogV
  G4double pos_Y_LogV = 0.0 * cm; // Y-location LogV
  G4double pos_Z_LogV = 0.0 * cm; // Z-location LogV
  G4ThreeVector threeVect_LogV =
      G4ThreeVector(pos_X_LogV, pos_Y_LogV, pos_Z_LogV);
  G4RotationMatrix rotMtrx_LogV = G4RotationMatrix();
  G4Transform3D trans3D_LogV = G4Transform3D(rotMtrx_LogV, threeVect_LogV);

  copyNum_LogV = 1000; // Set ID number of LogV
  new G4PVPlacement(trans3D_LogV, logVol_detector, "PhysVol_detector",
                    logVol_container, false, copyNum_LogV, true);
  copyNum_LogV = 999; // Set ID number of LogV

  if (logVol_lead_inside) {
    // add inside lead box
    new G4PVPlacement(trans3D_LogV, logVol_container, "PhysVol_container",
                      logVol_lead_inside, false, copyNum_LogV, true);
  } else {
    new G4PVPlacement(trans3D_LogV, "PhysVol_container", logVol_container,
                      physVol_World, false, copyNum_LogV, true);
  }

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
  logVol_PixEnvG->SetVisAttributes(G4VisAttributes::Invisible);

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
  // logVol_PixEnvL->SetVisAttributes ( G4VisAttributes::Invisible );

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
  auto logVol_PixElmt =
      new G4LogicalVolume(solid_PixElmt, materi_PixElmt, "LogVol_PixElmt");
  //  logVol_PixElmt->SetVisAttributes(G4VisAttributes::Invisible);

  // Placement of pixel elements to the local envelop using Replica
  new G4PVReplica("PhysVol_PixElmt", logVol_PixElmt, logVol_PixEnvL, kXAxis,
                  nDiv_X, leng_X_PixElmt);

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

  ////////////////////////////////////////////////////////
  /// Sensitive volume
  ////////////////////////////////////////////////////////
  auto aSV = new SensitiveVolume("SensitiveVolume");
  logVol_detector->SetSensitiveDetector(aSV); // Add sensitivity
  logVol_PixElmt->SetSensitiveDetector(aSV);  // Add sensitivity

  auto SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector(aSV);

  // Return the physical world
  return physVol_World;
}
