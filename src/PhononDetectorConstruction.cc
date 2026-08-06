/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/src/PhononDetectorConstruction.cc \brief
/// Implementation of the PhononDetectorConstruction class
//
// $Id: a2016d29cc7d1e75482bfc623a533d20b60390da $
//
// 20140321  Drop passing placement transform to G4LatticePhysical
// 20211207  Replace G4Logical*Surface with G4CMP-specific versions.
// 20220809  [ For M. Hui ] -- Add frequency dependent surface properties.
// 20221006  Remove unused features; add phonon sensor pad with use of
//		G4CMPPhononElectrode to demonstrate KaplanQP.
// 20251116  G4CMP-539 -- Use UpdateMPT wrapper function to set properties.
// 20251117  G4CMP-541 -- For G4 v11, replace ::Invisible w/::GetInvisible()

#include "PhononDetectorConstruction.hh"
#include "PhononSensitivity.hh"
#include "G4CMPLogicalBorderSurface.hh"
#include "G4CMPPhononElectrode.hh"
#include "G4CMPSurfaceProperty.hh"
#include "G4CMPUtils.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4GeometryManager.hh"
#include "G4LatticeLogical.hh"
#include "G4LatticeManager.hh"
#include "G4LatticePhysical.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4MultiUnion.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "PhononDetectorParameters.hh"

using namespace PhononDetectorParameters;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

PhononDetectorConstruction::PhononDetectorConstruction()
  : fVacuum(0), fAluminum(0), fTantalum(0), fSilicon(0),
    fWorldPhys(0), groundPlaneSurface(0), detectionSurface(0),
    vacuumSurface(0), electrodeSensitivity(0), fConstructed(false) {;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

PhononDetectorConstruction::~PhononDetectorConstruction() {
  delete groundPlaneSurface;
  delete detectionSurface;
  delete vacuumSurface;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4VPhysicalVolume* PhononDetectorConstruction::Construct()
{
  if (fConstructed) {
    if (!G4RunManager::IfGeometryHasBeenDestroyed()) {
      // Run manager hasn't cleaned volume stores. This code shouldn't execute
      G4GeometryManager::GetInstance()->OpenGeometry();
      G4PhysicalVolumeStore::GetInstance()->Clean();
      G4LogicalVolumeStore::GetInstance()->Clean();
      G4SolidStore::GetInstance()->Clean();
    }
    // Have to completely remove all lattices to avoid warning on reconstruction
    G4LatticeManager::GetLatticeManager()->Reset();
    // Clear all LogicalSurfaces
    // NOTE: No need to redefine the G4CMPSurfaceProperties
    G4CMPLogicalBorderSurface::CleanSurfaceTable();
  }

  DefineMaterials();
  SetupGeometry();
  fConstructed = true;

  return fWorldPhys;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PhononDetectorConstruction::DefineMaterials()
{ 
  G4NistManager* nistManager = G4NistManager::Instance();

  fVacuum  = new G4Material("vacuum",
					                  1.,
					                  1.008*CLHEP::g/CLHEP::mole,
					                  1.0e-25*CLHEP::g/CLHEP::cm3,
					                  kStateGas,
					                  0.01*CLHEP::kelvin,
					                  3.0e-18*pascal); // to be corrected

  fAluminum = nistManager->FindOrBuildMaterial("G4_Al");
  fTantalum = nistManager->FindOrBuildMaterial("G4_Ta");
  fSilicon = nistManager->FindOrBuildMaterial("G4_Si");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PhononDetectorConstruction::SetupGeometry()
{
  //     
  // World
  //
  G4VSolid* worldSolid = new G4Box("World", pdp_world_length / 2.0, pdp_world_length / 2.0, pdp_world_length / 2.0);
  G4LogicalVolume* worldLogical =
    new G4LogicalVolume(worldSolid,fVacuum,"World");
  worldLogical->SetUserLimits(new G4UserLimits(10*mm, DBL_MAX, DBL_MAX, 0, 0));
  fWorldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "World", 0,
                                 false, 0);
  
  //                               
  // Silicon wafer - this is the volume in which we will propagate phonons
  //  
  G4VSolid* fSiliconWafer = new G4Box("fSiliconWafer", pdp_wafer_width / 2.0, pdp_wafer_length / 2.0, pdp_wafer_thickness / 2.0);
  G4LogicalVolume* fSiliconLogical =
    new G4LogicalVolume(fSiliconWafer, fSilicon, "fSiliconLogical");
  G4VPhysicalVolume* fSiliconPhysical =
    new G4PVPlacement(0, G4ThreeVector(), fSiliconLogical, "fSiliconPhysical",
                      worldLogical, false, 0);

  //
  // Ta ground plane
  //

  G4VSolid* fTaGroundPlane = new G4Box("fTaGroundPlane", pdp_ground_plane_width / 2.0, pdp_ground_plane_length / 2.0, pdp_ground_plane_thickness / 2.0);
  G4LogicalVolume* fTaGroundPlaneLogical =
    new G4LogicalVolume(fTaGroundPlane, fTantalum, "fTaGroundPlaneLogical");
  G4VPhysicalVolume* fTaGroundPlanePhysical =
    new G4PVPlacement(0, G4ThreeVector(pdp_ground_plane_xpos, pdp_ground_plane_ypos, pdp_ground_plane_zpos), fTaGroundPlaneLogical, "fTaGroundPlanePhysical",
                      worldLogical, false, 0);

  //
  // Feedline hole
  //
  
  G4VSolid* fFeedlineHole = new G4Box("fFeedlineHole", pdp_feedline_hole_width / 2.0, pdp_feedline_hole_length / 2.0, pdp_feedline_hole_thickness / 2.0);
  G4LogicalVolume* fFeedlineHoleLogical =
    new G4LogicalVolume(fFeedlineHole, fVacuum, "fFeedlineHoleLogical");
  G4VPhysicalVolume* fFeedlineHolePhysical =
    new G4PVPlacement(0, G4ThreeVector(pdp_feedline_hole_xpos, pdp_feedline_hole_ypos, 0), fFeedlineHoleLogical, "fFeedlineHolePhysical",
                      fTaGroundPlaneLogical, false, 0);

  //
  // Feedline
  //
  
  G4VSolid* fFeedline = new G4Box("fFeedline", pdp_feedline_width / 2.0, pdp_feedline_length / 2.0, pdp_feedline_thickness / 2.0);
  G4LogicalVolume* fFeedlineLogical =
    new G4LogicalVolume(fFeedline, fTantalum, "fFeedlineLogical");
  G4VPhysicalVolume* fFeedlinePhysical =
    new G4PVPlacement(0, G4ThreeVector(), fFeedlineLogical, "fFeedlinePhysical",
                      fFeedlineHoleLogical, false, 0);

  //
  // Qubit holes
  //
  
  G4VSolid* fTopQubitHole = new G4Box("fTopQubitHole", pdp_qubit_hole_width / 2.0, pdp_qubit_hole_length / 2.0, pdp_qubit_hole_thickness / 2.0);
  G4LogicalVolume* fTopQubitHoleLogical =
    new G4LogicalVolume(fTopQubitHole, fVacuum, "fTopQubitHoleLogical");
  G4VPhysicalVolume* fTopQubitHolePhysical =
    new G4PVPlacement(0, G4ThreeVector(pdp_qubit_top_hole_xpos, pdp_qubit_top_hole_ypos, 0), fTopQubitHoleLogical, "fTopQubitHolePhysical",
                      fTaGroundPlaneLogical, false, 0);

  G4VSolid* fMiddleQubitHole = new G4Box("fMiddleQubitHole", pdp_qubit_hole_width / 2.0, pdp_qubit_hole_length / 2.0, pdp_qubit_hole_thickness / 2.0);
  G4LogicalVolume* fMiddleQubitHoleLogical =
    new G4LogicalVolume(fMiddleQubitHole, fVacuum, "fMiddleQubitHoleLogical");
  G4VPhysicalVolume* fMiddleQubitHolePhysical =
    new G4PVPlacement(0, G4ThreeVector(pdp_qubit_middle_hole_xpos, pdp_qubit_middle_hole_ypos, 0), fMiddleQubitHoleLogical, "fMiddleQubitHolePhysical",
                      fTaGroundPlaneLogical, false, 0);

  G4VSolid* fBottomQubitHole = new G4Box("fBottomQubitHole", pdp_qubit_hole_width / 2.0, pdp_qubit_hole_length / 2.0, pdp_qubit_hole_thickness / 2.0);
  G4LogicalVolume* fBottomQubitHoleLogical =
    new G4LogicalVolume(fBottomQubitHole, fVacuum, "fBottomQubitHoleLogical");
  G4VPhysicalVolume* fBottomQubitHolePhysical =
    new G4PVPlacement(0, G4ThreeVector(pdp_qubit_bottom_hole_xpos, pdp_qubit_bottom_hole_ypos, 0), fBottomQubitHoleLogical, "fBottomQubitHolePhysical",
                      fTaGroundPlaneLogical, false, 0);

  //
  // Qubit fins + junctions
  //
  G4RotationMatrix rotm = G4RotationMatrix();

  G4VSolid* finRight = new G4Tubs("finRight", 0., pdp_fin_radius, pdp_fin_thickness / 2.0,
                                  -pdp_fin_angle / 2.0, pdp_fin_angle);
  G4VSolid* finLeft = new G4Tubs("finLeft",  0., pdp_fin_radius, pdp_fin_thickness / 2.0,
                                  180.*deg - pdp_fin_angle / 2.0, pdp_fin_angle);
  G4VSolid* junction = new G4Box("junction", pdp_junction_length / 2.0, pdp_junction_width / 2.0,
                                  pdp_junction_thickness / 2.0);

  G4MultiUnion* fQubit = new G4MultiUnion("fQubit");
  fQubit->AddNode(*finRight, G4Transform3D(rotm, G4ThreeVector(pdp_junction_length / 2.0, 0, 0)));
  fQubit->AddNode(*finLeft, G4Transform3D(rotm, G4ThreeVector(-pdp_junction_length / 2.0, 0, 0)));
  fQubit->AddNode(*junction, G4Transform3D(rotm, G4ThreeVector()));
  fQubit->Voxelize();

  G4LogicalVolume* fQubitLogical = new G4LogicalVolume(fQubit, fAluminum, "fQubitLogical");

  G4VPhysicalVolume* fTopQubitPhysical = new G4PVPlacement(0, G4ThreeVector(), fQubitLogical,
                                                            "fTopQubitPhysical", fTopQubitHoleLogical, false, 0);
  G4VPhysicalVolume* fMiddleQubitPhysical = new G4PVPlacement(0, G4ThreeVector(), fQubitLogical,
                                                              "fMiddleQubitPhysical", fMiddleQubitHoleLogical, false, 0);
  G4VPhysicalVolume* fBottomQubitPhysical = new G4PVPlacement(0, G4ThreeVector(), fQubitLogical,
                                                              "fBottomQubitPhysical", fBottomQubitHoleLogical, false, 0);
  //
  //Silicon lattice information
  //

  // G4LatticeManager gives physics processes access to lattices by volume
  G4LatticeManager* LM = G4LatticeManager::GetLatticeManager();
  G4LatticeLogical* SiLogical = LM->LoadLattice(fSilicon, "Si");

  // G4LatticePhysical assigns G4LatticeLogical a physical orientation
  G4LatticePhysical* SiPhysical = new G4LatticePhysical(SiLogical);
  SiPhysical->SetMillerOrientation(1,0,0);
  LM->RegisterLattice(fSiliconPhysical, SiPhysical);

  // NOTE:  Above registration can also be done in single step:
  // G4LatticlePhysical* GePhysical = LM->LoadLattice(GePhys, "Ge");


  //
  // detector -- Note : "sensitive detector" is attached to Germanium crystal
  //
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if (!electrodeSensitivity)
    electrodeSensitivity = new PhononSensitivity("PhononElectrode");
  SDman->AddNewDetector(electrodeSensitivity);
  fSiliconLogical->SetSensitiveDetector(electrodeSensitivity);

  const G4double GHz = 1e9 * hertz;

  //the following coefficients and cutoff values are not well-motivated
  //the code below is used only to demonstrate how to set these values.
  const std::vector<G4double> anhCoeffs = {0, 0, 0, 0, 0, 1.51e-14};
  const std::vector<G4double> diffCoeffs = {5.88e-2, 7.83e-4, -2.47e-6, 1.71e-8, -2.98e-11};
  const std::vector<G4double> specCoeffs = {0,928, -2.03e-4, -3.21e-6, 3.1e-9, 2.9e-13};
  const G4double anhCutoff = 520., reflCutoff = 350.;   // Units external

  //
  // surface between Al and Ge determines phonon reflection/absorption
  //
  if (!fConstructed) {
    groundPlaneSurface = new G4CMPSurfaceProperty("GroundPlane", 1.0, 0.0, 0.0, 0.0, 0.25, 1.0, 1.0, 0.0);
    groundPlaneSurface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs, diffCoeffs, specCoeffs, GHz, GHz, GHz);

    detectionSurface = new G4CMPSurfaceProperty("Detector", 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0);
    detectionSurface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs, diffCoeffs, specCoeffs, GHz, GHz, GHz);

    vacuumSurface = new G4CMPSurfaceProperty("Vacuum", 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0);
    vacuumSurface->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs, diffCoeffs, specCoeffs, GHz, GHz, GHz);
    AttachPhononSensor(detectionSurface);

    /*const G4double GHz = 1e9 * hertz;

    //the following coefficients and cutoff values are not well-motivated
    //the code below is used only to demonstrate how to set these values.
    const std::vector<G4double> anhCoeffs = {0, 0, 0, 0, 0, 1.51e-14};
    const std::vector<G4double> diffCoeffs =
      {5.88e-2, 7.83e-4, -2.47e-6, 1.71e-8, -2.98e-11};
    const std::vector<G4double> specCoeffs =
      {0,928, -2.03e-4, -3.21e-6, 3.1e-9, 2.9e-13};

    const G4double anhCutoff = 520., reflCutoff = 350.;   // Units external

    topSurfProp = new G4CMPSurfaceProperty("TopAlSurf", 1.0, 0.0, 0.0, 0.0,
					  	        0.3, 1.0, 0.0, 0.0);
    topSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
					 diffCoeffs, specCoeffs, GHz, GHz, GHz);
    AttachPhononSensor(topSurfProp);

    botSurfProp = new G4CMPSurfaceProperty("BotAlSurf", 1.0, 0.0, 0.0, 0.0,
					   	        0.3, 1.0, 0.0, 0.0);
    botSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
					 diffCoeffs, specCoeffs, GHz, GHz, GHz);
    AttachPhononSensor(botSurfProp);

    wallSurfProp = new G4CMPSurfaceProperty("WallSurf", 0.0, 1.0, 0.0, 0.0,
					    	          0.0, 1.0, 0.0, 0.0);
    wallSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
					  diffCoeffs, specCoeffs, GHz, GHz,GHz);*/

  }

  //
  // Separate surfaces for sensors vs. bare sidewall
  //
  new G4CMPLogicalBorderSurface("Vacuum", fSiliconPhysical, fWorldPhys, vacuumSurface);
  new G4CMPLogicalBorderSurface("FeedlineHole", fSiliconPhysical, fFeedlineHolePhysical, vacuumSurface);
  new G4CMPLogicalBorderSurface("TopQubitHole", fSiliconPhysical, fTopQubitHolePhysical, vacuumSurface);
  new G4CMPLogicalBorderSurface("MiddleQubitHole", fSiliconPhysical, fMiddleQubitHolePhysical, vacuumSurface);
  new G4CMPLogicalBorderSurface("BottomQubitHole", fSiliconPhysical, fBottomQubitHolePhysical, vacuumSurface);

  new G4CMPLogicalBorderSurface("GroundPlane", fSiliconPhysical, fTaGroundPlanePhysical, groundPlaneSurface);
  new G4CMPLogicalBorderSurface("Feedline", fSiliconPhysical, fFeedlinePhysical, groundPlaneSurface);
  new G4CMPLogicalBorderSurface("TopQubit", fSiliconPhysical, fTopQubitPhysical, detectionSurface);
  new G4CMPLogicalBorderSurface("MiddleQubit", fSiliconPhysical, fMiddleQubitPhysical, detectionSurface);
  new G4CMPLogicalBorderSurface("BottomQubit", fSiliconPhysical, fBottomQubitPhysical, detectionSurface);

  //                                        
  // Visualization attributes
  //
  worldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());
  G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  fSiliconLogical->SetVisAttributes(simpleBoxVisAtt);
  // fAluminumLogical->SetVisAttributes(simpleBoxVisAtt);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Attach material properties and electrode/sensor handler to surface

void PhononDetectorConstruction::
AttachPhononSensor(G4CMPSurfaceProperty *surfProp) {
  if (!surfProp) return;		// No surface, nothing to do

  // Specify properties of aluminum sensor, same on both detector faces
  // See G4CMPPhononElectrode.hh or README.md for property keys

  // Properties must be added to existing surface-property table
  auto sensorProp = surfProp->GetPhononMaterialPropertiesTablePointer();
  G4CMP::UpdateMPT(sensorProp, "filmAbsorption", 0.20);    // True sensor area
  G4CMP::UpdateMPT(sensorProp, "filmThickness", 600.*nm);
  G4CMP::UpdateMPT(sensorProp, "gapEnergy", 173.715e-6*eV);
  G4CMP::UpdateMPT(sensorProp, "lowQPLimit", 3.);
  G4CMP::UpdateMPT(sensorProp, "phononLifetime", 242.*ps);
  G4CMP::UpdateMPT(sensorProp, "phononLifetimeSlope", 0.29);
  G4CMP::UpdateMPT(sensorProp, "vSound", 3.26*km/s);
  G4CMP::UpdateMPT(sensorProp, "subgapAbsorption", 0.1);

  // Attach electrode object to handle KaplanQP interface
  surfProp->SetPhononElectrode(new G4CMPPhononElectrode);
}

