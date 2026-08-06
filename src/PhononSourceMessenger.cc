#include "PhononSourceMessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"

class PhononPrimaryGeneratorAction;

PhononSourceMessenger::PhononSourceMessenger(PhononPrimaryGeneratorAction* fPrimGen)
: fPrimaryGenerator(fPrimGen), sourceDirectory(0), awgCmd(0), numPhononsCmd(0), pulseCmd(0)
{
    sourceDirectory = new G4UIdirectory("/phonon/source/");
    sourceDirectory->SetGuidance("Phonon source parameters.");

    awgCmd = new G4UIcmdWithABool("/phonon/source/awg", this);
    awgCmd->SetGuidance("Turn AWG mode on/off.");

    numPhononsCmd = new G4UIcmdWithAnInteger("/phonon/source/numPhonons", this);
    numPhononsCmd->SetGuidance("Set the number of phonons to be generated per event.");

    pulseCmd = new G4UIcmdWithAString("/phonon/source/setPulseFilename", this);
    pulseCmd->SetGuidance("Set the AWG filename with the LTspice pulse output.");
}

PhononSourceMessenger::~PhononSourceMessenger()
{
    delete awgCmd;
    delete numPhononsCmd;
    delete pulseCmd;
    delete sourceDirectory;
}

void PhononSourceMessenger::SetNewValue(G4UIcommand* command, G4String value)
{
    if (command == awgCmd) {
        fPrimaryGenerator->SetAWGMode(awgCmd->GetNewBoolValue(value));
    } else if (command == numPhononsCmd) {
        fPrimaryGenerator->SetNumPhonons(numPhononsCmd->GetNewIntValue(value));
    } else if (command == pulseCmd) {
        fPrimaryGenerator->SetPulseFilename(value);
    }
}

G4String PhononSourceMessenger::GetCurrentValue(G4UIcommand* command)
{
    G4String cv;

    if (command == awgCmd) {
        cv = awgCmd->ConvertToString(fPrimaryGenerator->GetAWGMode());
    } else if (command == numPhononsCmd) {
        cv = numPhononsCmd->ConvertToString(fPrimaryGenerator->GetNumPhonons());
    } else if (command == pulseCmd) {
        cv = fPrimaryGenerator->GetPulseFilename();
    }

    return cv;
}