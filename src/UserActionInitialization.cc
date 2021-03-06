//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UserActionInitialization.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "UserActionInitialization.hh"
#include "PrimaryGenerator.hh"
#include "PrimaryGeneratorGPS.hh"
#include "RunAction.hh"

//------------------------------------------------------------------------------
UserActionInitialization::UserActionInitialization()
    : G4VUserActionInitialization() {}

//------------------------------------------------------------------------------
UserActionInitialization::~UserActionInitialization() {}

//------------------------------------------------------------------------------
void UserActionInitialization::Build() const {
  //  SetUserAction(new PrimaryGenerator());
  SetUserAction(new PrimaryGeneratorGPS());
  SetUserAction(new RunAction());
}

//------------------------------------------------------------------------------
void UserActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction());
}

