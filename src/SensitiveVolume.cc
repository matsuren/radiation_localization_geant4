//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SensitiveVolume.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "SensitiveVolume.hh"
#include "Analysis.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"

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
  sum_stepLength = 0.;
  G4cout << "== Initialize sum_eDeps. Event id:" << event_id << G4endl;
}
//------------------------------------------------------------------------------
void SensitiveVolume::EndOfEvent(G4HCofThisEvent *) {
  //
  G4cout << " sum_eDep = " << sum_eDep / keV << " keV" << G4endl;
  //" sum_stepLength = " << sum_stepLength << G4endl;

  // save
  if (sum_eDep / eV > 1.0 * eV) {
    // save results
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

#ifdef ANALYSIS_HIST
    analysisManager->FillH1(0, sum_eDep);
    analysisManager->FillH1(1, sum_stepLength);
#else
    analysisManager->FillNtupleDColumn(0, sum_eDep);
    analysisManager->FillNtupleDColumn(1, sum_stepLength);
    analysisManager->AddNtupleRow();
#endif
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
  if (physvol_name == "PhysVol_PixElmt" && edep != 0.0) {
    G4int replicaNo0 = pre->GetTouchableHandle()->GetReplicaNumber(0);
    G4int replicaNo1 = pre->GetTouchableHandle()->GetReplicaNumber(1);
    G4cout << "Pixel hit location: " << replicaNo0 << ", " << replicaNo1
           << " Energy: " << edep / keV << " keV" << G4endl;
  } else {
    G4cout << " ## --- ProcessHits --- ## " << edep / keV << " keV" << G4endl;
    // get step length and accumulate it
    G4double stepLength = 0.;
    if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
      stepLength = aStep->GetStepLength();
    }
    if (copyNo == 1000) {
      sum_eDep = sum_eDep + edep;
      sum_stepLength = sum_stepLength + stepLength;
    }
  }

  return true;
}
