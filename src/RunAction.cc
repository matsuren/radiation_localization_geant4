#include "RunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

RunAction::RunAction() : G4UserRunAction() {
  // Get analysis manager
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  G4cout << "Analysis manager using " << analysisManager->GetType() << G4endl;
  analysisManager->SetActivation(true);

  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFileName("detect_result");

  // Creating H1 histograms
  G4int id = analysisManager->CreateH1("h1:0", "sum_eDep", 50, 0., 1.5 * MeV);
  analysisManager->SetH1Activation(id, false);
  id = analysisManager->CreateH1("h1:1","sum_stepLength", 50, 0., 5.*mm);
  analysisManager->SetH1Activation(id, false);
}

RunAction::~RunAction() { delete G4AnalysisManager::Instance(); }

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
