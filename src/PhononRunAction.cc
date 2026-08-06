/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

// $Id$
// File:  PhononRunAction.cc
//
// Description:	Run action to initialize G4 analysis manager.
//
// 20170816  John Vaccaro


#include "PhononRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"


PhononRunAction::PhononRunAction()
: G4UserRunAction()
{
  // Create analysis manager for setting up outputs
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "For analysis manager, using " << analysisManager->GetType() << G4endl;

  // Create directories 
  // analysisManager->SetHistoDirectoryName("histograms");
  // analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  // analysisManager->SetNtupleMerging(true); // Should be uncommented for multi-threading (but requires ROOT output)

  // Creating ntuple
  analysisManager->CreateNtuple("Phonon", "EventInformation");
  analysisManager->CreateNtupleIColumn("eventID");
  analysisManager->CreateNtupleIColumn("trackID");
  analysisManager->CreateNtupleDColumn("edep_eV");
  analysisManager->CreateNtupleDColumn("startPosition_x");
  analysisManager->CreateNtupleDColumn("startPosition_y");
  analysisManager->CreateNtupleDColumn("startPosition_z");
  analysisManager->CreateNtupleDColumn("finalPosition_x");
  analysisManager->CreateNtupleDColumn("finalPosition_y");
  analysisManager->CreateNtupleDColumn("finalPosition_z");
  analysisManager->CreateNtupleDColumn("globalTime");
  analysisManager->CreateNtupleSColumn("particleName");
  analysisManager->CreateNtupleDColumn("particleEnergy");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhononRunAction::~PhononRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhononRunAction::BeginOfRunAction(const G4Run* run)
{
  //  std::cout << "---> Beginning of run action." << std::endl;
  
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // ID the analysis manager and Open an output file
  //
  auto analysisManager = G4AnalysisManager::Instance();
  // G4String fileName = "OLAFOutput-" + std::to_string(run->GetRunID()) + ".csv";
  // analysisManager->OpenFile(fileName);
  analysisManager->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhononRunAction::EndOfRunAction(const G4Run* run)
{
  //  std::cout << "---> End of run action." << std::endl;

  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // save the analysis manager's ntuple
  //
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}
