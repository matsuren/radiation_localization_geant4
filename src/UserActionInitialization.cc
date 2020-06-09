//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// UserActionInitialization.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "UserActionInitialization.hh"
#include "PrimaryGenerator.hh"

//------------------------------------------------------------------------------
UserActionInitialization::UserActionInitialization()
    : G4VUserActionInitialization()
{}

//------------------------------------------------------------------------------
UserActionInitialization::~UserActionInitialization()
{}

//------------------------------------------------------------------------------
void UserActionInitialization::Build() const
{
  SetUserAction(new PrimaryGenerator());
}
