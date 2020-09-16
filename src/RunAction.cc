#include "RunAction.hh"
#include "Analysis.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"

RunAction::RunAction() : G4UserRunAction() {
  // Get analysis manager
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  G4cout << "Analysis manager using " << analysisManager->GetType() << G4endl;
  analysisManager->SetActivation(true);

  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFileName("detect_result");

#ifdef ANALYSIS_HIST
  // Creating H1 histograms
  G4int id = analysisManager->CreateH1("h1:0", "sum_eDep", 50, 0., 1.5 * MeV);
  analysisManager->SetH1Activation(id, false);
#else
  // Creation of ntuple
  analysisManager->CreateNtuple("MyNtuple", "Edep and TrackLength");
  analysisManager->CreateNtupleDColumn("sum_eDep");
  analysisManager->CreateNtupleDColumn("sum_stepLength");
  analysisManager->FinishNtuple();
#endif
}

RunAction::~RunAction() {
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  delete analysisManager;
}

void RunAction::BeginOfRunAction(const G4Run * /*run*/) {
  // Get analysis manager
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run * /*run*/) {
  //
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();

  // save the file and close
  analysisManager->Write();
  analysisManager->CloseFile();
}
