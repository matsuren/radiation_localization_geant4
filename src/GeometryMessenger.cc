#include "GeometryMessenger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"
#include <memory>

//------------------------------------------------------------------------------
GeometryMessenger::GeometryMessenger(Geometry *geometry)
    : ptr_geometry(geometry)
//------------------------------------------------------------------------------
{
  dir_cmd = std::make_unique<G4UIdirectory>("/my_detector/", this);
  dir_cmd->SetGuidance("My custom commands for changing detector pose");

  detector_position_cmd = std::make_unique<G4UIcmdWith3VectorAndUnit>(
      "/my_detector/position", this);
  detector_position_cmd->SetGuidance("Insert X Y and Z dimensions for the "
                                     "position of the center of the detector"
                                     " respect to that of the \"World\"");
  detector_position_cmd->SetParameterName("PositionAlongX", "PositionAlongY",
                                          "PositionAlongZ", false);
  detector_position_cmd->SetDefaultUnit("cm");
  detector_position_cmd->SetUnitCandidates("mm cm m");
  detector_position_cmd->AvailableForStates(G4State_Idle);

  detector_rotation_cmd = std::make_unique<G4UIcmdWithADoubleAndUnit>(
      "/my_detector/rotationY", this);
  detector_rotation_cmd->SetDefaultUnit("deg");
  detector_rotation_cmd->SetDefaultValue(0.);
  detector_rotation_cmd->SetGuidance("rotation around Y [deg]");

  update_cmd =
      std::make_unique<G4UIcmdWithoutParameter>("/my_detector/update", this);
  update_cmd->SetGuidance("Update calorimeter geometry.");
  update_cmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  update_cmd->SetGuidance("if you changed geometrical value(s).");
  update_cmd->AvailableForStates(G4State_Idle);
}

//------------------------------------------------------------------------------
GeometryMessenger::~GeometryMessenger() {}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void GeometryMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
//------------------------------------------------------------------------------
{
  if (command == detector_position_cmd.get()) {
    G4ThreeVector size = detector_position_cmd->GetNew3VectorValue(newValue);
    ptr_geometry->SetDetectorPosition(size.getX(), size.getY(), size.getZ());
  } else if (command == detector_rotation_cmd.get()) {
    auto rot_y = detector_rotation_cmd->GetNewDoubleValue(newValue);
    ptr_geometry->SetDetectorRot(0, rot_y, 0);
  } else if (command == update_cmd.get()) {
    ptr_geometry->UpdateGeometry();
  }
}
