//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SensitiveVolume.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "SensitiveVolume.hh"
#include "Analysis.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"

//------------------------------------------------------------------------------
SensitiveVolume::SensitiveVolume(G4String name) : G4VSensitiveDetector(name) {}
//------------------------------------------------------------------------------
SensitiveVolume::~SensitiveVolume() {}
//------------------------------------------------------------------------------
void SensitiveVolume::Initialize(G4HCofThisEvent * /* HCE */) {

  // get event id
  auto event = G4EventManager::GetEventManager()->GetConstCurrentEvent();
  event_id = event->GetEventID();
  sum_eDep = 0.;
  if (verboseLevel > 0) {
    G4cout << "== Initialize sum_eDeps. Event id:" << event_id << G4endl;
  }
}
//------------------------------------------------------------------------------
void SensitiveVolume::EndOfEvent(G4HCofThisEvent *) {
  //
  if (verboseLevel > 0) {
    G4cout << " sum_eDep = " << sum_eDep / keV << " keV" << G4endl;
  }
  //" sum_stepLength = " << sum_stepLength << G4endl;

  // save
  if (sum_eDep / eV > 1.0 * eV) {
    // save results
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillH1(0, sum_eDep);
  }
}

//------------------------------------------------------------------------------
G4bool SensitiveVolume::ProcessHits(G4Step *aStep, G4TouchableHistory *) {

  G4StepPoint *pre = aStep->GetPreStepPoint();
  G4int copyNo = pre->GetPhysicalVolume()->GetCopyNo();
  // get total energy deposit in this step "edep"  and  accumulate it in
  // "sum_eDep"
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.0)
    return true;

  // get name
  auto physvol_name = pre->GetTouchable()->GetVolume()->GetName();
    //    G4cout << "verboseLevel: " << verboseLevel << G4endl;
    if (verboseLevel > 0) {
      G4cout << " ## --- ProcessHits --- ## " << edep / keV << " keV" << G4endl;
    }
    if (copyNo == 1000) {
      sum_eDep = sum_eDep + edep;
    }

  return true;
}
