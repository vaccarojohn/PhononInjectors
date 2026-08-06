#ifndef PhononSourceMessenger_h
#define PhononSourceMessenger_h 1

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithAString;

#include "PhononPrimaryGeneratorAction.hh"
#include "G4UImessenger.hh"
#include "globals.hh"

class PhononSourceMessenger : public G4UImessenger
{
  public:
    PhononSourceMessenger(PhononPrimaryGeneratorAction* fPrimGen);
    ~PhononSourceMessenger();

  public:
    void SetNewValue(G4UIcommand* command, G4String newValues);
    G4String GetCurrentValue(G4UIcommand* command);

  private:
    PhononPrimaryGeneratorAction* fPrimaryGenerator;
    G4UIdirectory*        sourceDirectory;
    G4UIcmdWithABool*     awgCmd;
    G4UIcmdWithAnInteger* numPhononsCmd;
    G4UIcmdWithAString*   pulseCmd;
};

#endif
