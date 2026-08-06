/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

// $Id: 539f524339ae53ad098a07cfa3bebd07784d23dd $

#include "PhononActionInitialization.hh"
#include "PhononPrimaryGeneratorAction.hh"
#include "PhononRunAction.hh"
#include "PhononSourceMessenger.hh"
#include "G4CMPStackingAction.hh"

void PhononActionInitialization::Build() const {
  PhononPrimaryGeneratorAction* fPrimGen = new PhononPrimaryGeneratorAction();
  SetUserAction(fPrimGen);

  PhononSourceMessenger* sourceMessenger = new PhononSourceMessenger(fPrimGen);
  
  SetUserAction(new PhononRunAction);
  SetUserAction(new G4CMPStackingAction);
} 
