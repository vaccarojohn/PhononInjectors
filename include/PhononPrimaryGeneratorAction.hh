/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/include/PhononPrimaryGeneratorAction.hh
/// \brief Definition of the PhononPrimaryGeneratorAction class
//
// $Id: ecbf57649dfaeb88e0fac25491bf8fb68c9308ec $
//

#ifndef PhononPrimaryGeneratorAction_h
#define PhononPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include <vector>


class G4ParticleGun;
class G4Event;

class PhononPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PhononPrimaryGeneratorAction();    
  virtual ~PhononPrimaryGeneratorAction();

  void SetAWGMode(G4bool awgMode);
  void SetNumPhonons(G4int numPhonons);
  void SetPulseFilename(G4String filename);
  G4bool GetAWGMode();
  G4int GetNumPhonons();
  G4String GetPulseFilename();
  G4int TimeToIndex(G4double time);

  public:
    virtual void GeneratePrimaries(G4Event*);

  private:
    G4double SampleTiming();

    G4ParticleGun*                fParticleGun;
    G4bool                        fAWGMode;
    G4int                         fPulseNumPhonons;
    G4String                      fPulseFilename;
    std::vector<G4double>         fPulseTimes;
    std::vector<G4double>         fPulseMagnitudes;
    G4double                      fPulseMaximum;
};

#endif
