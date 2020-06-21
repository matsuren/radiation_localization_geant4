#pragma once
#include "G4UImessenger.hh"
#include "Geometry.hh"
#include "globals.hh"
#include <memory>

class Geometry;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class GeometryMessenger : public G4UImessenger {
public:
  explicit GeometryMessenger(Geometry *geometry);
  virtual ~GeometryMessenger();

  // methods
  virtual void SetNewValue(G4UIcommand *command, G4String value) final;

  Geometry *ptr_geometry; ///< Associated class

  // command
  std::unique_ptr<G4UIdirectory> dir_cmd;
  std::unique_ptr<G4UIcmdWith3VectorAndUnit> detector_position_cmd;
  std::unique_ptr<G4UIcmdWithADoubleAndUnit> detector_rotation_cmd;
  std::unique_ptr<G4UIcmdWithoutParameter> update_cmd;
};
