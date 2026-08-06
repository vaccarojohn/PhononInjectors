/***********************************************************************\
 * This software is licensed under the terms of the GNU General Public *
 * License version 3 or later. See G4CMP/LICENSE for the full license. *
\***********************************************************************/

/// \file exoticphysics/phonon/include/PhononDetectorParameters.hh
/// \brief Phonon detector parameters

#ifndef PhononDetectorParameters_h
#define PhononDetectorParameters_h 1

#include "CLHEP/Units/SystemOfUnits.h"

namespace PhononDetectorParameters
{
  //Overall World
  constexpr double pdp_world_length = 4 * CLHEP::cm;
  
  // Silicon wafer
  constexpr double pdp_wafer_width = 7100 * CLHEP::um;
  constexpr double pdp_wafer_length = 7100 * CLHEP::um;
  constexpr double pdp_wafer_thickness = 525 * CLHEP::um;

  // Ta ground plane
  constexpr double pdp_ground_plane_width = 6800 * CLHEP::um;
  constexpr double pdp_ground_plane_length = 6800 * CLHEP::um;
  constexpr double pdp_ground_plane_thickness = 250 * CLHEP::nm;
  constexpr double pdp_ground_plane_xpos = 0;
  constexpr double pdp_ground_plane_ypos = 0;
  constexpr double pdp_ground_plane_zpos = 0.5 * pdp_ground_plane_thickness + 0.5 * pdp_wafer_thickness;

  // Feedline hole
  constexpr double pdp_feedline_hole_width = 44 * CLHEP::um;
  constexpr double pdp_feedline_hole_length = 5400 * CLHEP::um;
  constexpr double pdp_feedline_hole_thickness = 250 * CLHEP::nm;
  constexpr double pdp_feedline_hole_xpos = 0;
  constexpr double pdp_feedline_hole_ypos = 100 * CLHEP::um;

  // Feedline
  constexpr double pdp_feedline_width = 20 * CLHEP::um;
  constexpr double pdp_feedline_length = 5000 * CLHEP::um;
  constexpr double pdp_feedline_thickness = 250 * CLHEP::nm;

  // Qubit holes
  constexpr double pdp_qubit_hole_width = 400 * CLHEP::um;
  constexpr double pdp_qubit_hole_length = 380 * CLHEP::um;
  constexpr double pdp_qubit_hole_thickness = 250 * CLHEP::nm;
  constexpr double pdp_qubit_top_hole_xpos = -0.5 * pdp_feedline_hole_width - 0.5 * pdp_qubit_hole_length;
  constexpr double pdp_qubit_top_hole_ypos = 0.5 * pdp_ground_plane_length - 1980 * CLHEP::um;
  constexpr double pdp_qubit_middle_hole_xpos = 0.5 * pdp_feedline_hole_width + 0.5 * pdp_qubit_hole_length;
  constexpr double pdp_qubit_middle_hole_ypos = 0.5 * pdp_ground_plane_length - 3300 * CLHEP::um;
  constexpr double pdp_qubit_bottom_hole_xpos = -0.5 * pdp_feedline_hole_width - 0.5 * pdp_qubit_hole_length;
  constexpr double pdp_qubit_bottom_hole_ypos = 0.5 * pdp_ground_plane_length - 4620 * CLHEP::um;

  // Qubit
  constexpr double pdp_fin_radius = 132 * CLHEP::um;
  constexpr double pdp_fin_angle = 150 * CLHEP::deg;
  constexpr double pdp_fin_thickness = 250 * CLHEP::nm;
  constexpr double pdp_junction_length = 20 * CLHEP::um;
  constexpr double pdp_junction_width = 2 * CLHEP::um;
  constexpr double pdp_junction_thickness = 250 * CLHEP::nm;
}

#endif
