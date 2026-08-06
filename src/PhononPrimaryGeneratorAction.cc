/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/src/PhononPrimaryGeneratorAction.cc
/// \brief Implementation of the PhononPrimaryGeneratorAction class
//
// $Id: e75f788b103aef810361fad30f75077829192c13 $
//
// 20140519  Allow the user to specify phonon type by name in macro; if
//	     "geantino" is set, use random generator to select.

#include "PhononPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4Geantino.hh"
#include "G4ParticleGun.hh"
#include "G4RandomDirection.hh"
#include "G4PhononTransFast.hh"
#include "G4PhononTransSlow.hh"
#include "G4PhononLong.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <fstream>
#include <sstream>

using namespace std;

PhononPrimaryGeneratorAction::PhononPrimaryGeneratorAction()
: fAWGMode(false), fPulseNumPhonons(1), fPulseFilename(""), fPulseMaximum(0) {
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);   

  fParticleGun->SetParticlePosition(G4ThreeVector(-1.4, -1.4, 0.2624));
  fParticleGun->SetParticleEnergy(0.004*eV);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


PhononPrimaryGeneratorAction::~PhononPrimaryGeneratorAction() {
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PhononPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  for (int i = 0; i < fPulseNumPhonons; i++) {
    G4double selector = G4UniformRand();
    if (selector<0.53539) {
      fParticleGun->SetParticleDefinition(G4PhononTransSlow::Definition()); 
    } else if (selector<0.90217) {
      fParticleGun->SetParticleDefinition(G4PhononTransFast::Definition());
    } else {
      fParticleGun->SetParticleDefinition(G4PhononLong::Definition());
    }

    fParticleGun->SetParticleMomentumDirection(G4RandomDirection());

    if (fAWGMode) {
      if (fPulseTimes.size() == 0) {
        G4cout << "Error: AWG mode is enabled but no file is loaded!" << G4endl;
      } else {
        fParticleGun->SetParticleTime(SampleTiming());
      }
    }

    fParticleGun->GeneratePrimaryVertex(anEvent);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PhononPrimaryGeneratorAction::SetAWGMode(G4bool awgMode) {
  fAWGMode = awgMode;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PhononPrimaryGeneratorAction::SetNumPhonons(G4int numPhonons) {
  fPulseNumPhonons = numPhonons;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PhononPrimaryGeneratorAction::SetPulseFilename(G4String filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    G4cout << "Error: could not open pulse file!" << G4endl;
    return;
  }

  std::string line;
  std::getline(file, line); // Ignore first line

  fPulseTimes.clear();
  fPulseMagnitudes.clear();
  fPulseMaximum = 0;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string numstr;

    int i = 0;
    while (std::getline(ss, numstr, '\t')) {
      G4double num = std::stod(numstr);
      if (i % 3 == 0) {
        fPulseTimes.push_back(num);
      } else if (i % 3 == 2) {
        fPulseMagnitudes.push_back(num);
        
        if (num > fPulseMaximum) {
          fPulseMaximum = num;
        }
      }
      i++;
    }
  }

  file.close();
  fPulseFilename = filename;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4bool PhononPrimaryGeneratorAction::GetAWGMode() {
  return fAWGMode;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4int PhononPrimaryGeneratorAction::GetNumPhonons() {
  return fPulseNumPhonons;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4String PhononPrimaryGeneratorAction::GetPulseFilename() {
  return fPulseFilename;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double PhononPrimaryGeneratorAction::SampleTiming() {
  int size = fPulseTimes.size();

  G4int choice = 0;
  G4double mag = 0;
  do {
    choice = (G4int)(G4UniformRand() * size);
    mag = (G4int)(G4UniformRand() * fPulseMaximum);
  } while (mag >= fPulseMagnitudes[choice]);

  //G4cout << "sampled at: " << fPulseTimes[choice] << "s" << G4endl;
  return fPulseTimes[choice];
}
