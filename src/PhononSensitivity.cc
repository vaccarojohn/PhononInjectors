/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

#include "PhononSensitivity.hh"
#include "G4CMPElectrodeHit.hh"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4PhononLong.hh"
#include "G4PhononTransFast.hh"
#include "G4PhononTransSlow.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "PhononConfigManager.hh"
#include <fstream>


PhononSensitivity::PhononSensitivity(G4String name) :
  G4CMPElectrodeSensitivity(name) {
}

/* Move is disabled for now because old versions of GCC can't move ofstream
PhononSensitivity::PhononSensitivity(PhononSensitivity&& in) :
  G4CMPElectrodeSensitivity(std::move(in)),
  output(std::move(in.output)),
  fileName(std::move(in.fileName)) {
}

PhononSensitivity& PhononSensitivity::operator=(PhononSensitivity&& in) {
  // Move all base mebers
  G4CMPElectrodeSensitivity::operator=(std::move(in));

  // Our members
  output.close();
  output = std::move(in.output);
  fileName = in.fileName;

  return *this;
}
*/

PhononSensitivity::~PhononSensitivity() {
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}

void PhononSensitivity::EndOfEvent(G4HCofThisEvent* HCE) {
  G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(hitsCollection);
  auto* hitCol = static_cast<G4CMPElectrodeHitsCollection*>(HCE->GetHC(HCID));
  std::vector<G4CMPElectrodeHit*>* hitVec = hitCol->GetVector();

  G4RunManager* runMan = G4RunManager::GetRunManager();
  auto analysisManager = G4AnalysisManager::Instance();

  if( runMan->GetCurrentEvent()->GetEventID() % 100 == 0 ){
    std::cout << "--> Finished simulating " << runMan->GetCurrentEvent()->GetEventID() << " events." << std::endl;
  }

  for (G4CMPElectrodeHit* hit : *hitVec) {
    analysisManager->FillNtupleIColumn(0, runMan->GetCurrentEvent()->GetEventID());
    analysisManager->FillNtupleIColumn(1, hit->GetTrackID());
    analysisManager->FillNtupleDColumn(2, hit->GetEnergyDeposit()/eV);
    analysisManager->FillNtupleDColumn(3, hit->GetStartPosition().getX()/um);
    analysisManager->FillNtupleDColumn(4, hit->GetStartPosition().getY()/um);
    analysisManager->FillNtupleDColumn(5, hit->GetStartPosition().getZ()/um);
    analysisManager->FillNtupleDColumn(6, hit->GetFinalPosition().getX()/um);
    analysisManager->FillNtupleDColumn(7, hit->GetFinalPosition().getY()/um);
    analysisManager->FillNtupleDColumn(8, hit->GetFinalPosition().getZ()/um);
    analysisManager->FillNtupleDColumn(9, hit->GetFinalTime()/ns);
    analysisManager->FillNtupleSColumn(10, hit->GetParticleName());
    analysisManager->FillNtupleDColumn(11, hit->GetStartEnergy()/eV);
    analysisManager->AddNtupleRow();
  }

  volumeNames.clear();
}

G4bool PhononSensitivity::IsHit(const G4Step* step,
                                const G4TouchableHistory*) const {
  /* Phonons tracks are sometimes killed at the boundary in order to spawn new
   * phonon tracks. These tracks that are killed deposit no energy and should
   * not be picked up as hits.
   */
  const G4Track* track = step->GetTrack();
  const G4StepPoint* postStepPoint = step->GetPostStepPoint();
  const G4ParticleDefinition* particle = track->GetDefinition();

  G4VPhysicalVolume* pv = step->GetPostStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume*po = step->GetPreStepPoint()->GetPhysicalVolume();

  G4bool correctParticle = particle == G4PhononLong::Definition() ||
                           particle == G4PhononTransFast::Definition() ||
                           particle == G4PhononTransSlow::Definition();

  G4bool correctStatus = step->GetTrack()->GetTrackStatus() == fStopAndKill &&
                         postStepPoint->GetStepStatus() == fGeomBoundary &&
                         ((po->GetName()!="fTopQubitPhysical" && pv->GetName()=="fTopQubitPhysical") ||
                          (po->GetName()!="fMiddleQubitPhysical" && pv->GetName()=="fMiddleQubitPhysical") ||
                          (po->GetName()!="fBottomQubitPhysical" && pv->GetName()=="fBottomQubitPhysical")) &&
                         step->GetNonIonizingEnergyDeposit() > 0.;

  return correctParticle & correctStatus;
}
