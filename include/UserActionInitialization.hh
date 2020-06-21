//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UserActionInitialization.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

#include "G4VUserActionInitialization.hh"

//------------------------------------------------------------------------------
class UserActionInitialization : public G4VUserActionInitialization
//------------------------------------------------------------------------------
{
public:
  UserActionInitialization();
  ~UserActionInitialization() override;

  void Build() const override;
};
