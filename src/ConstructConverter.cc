//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file radioactivedecay/rdecay01/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
// $Id: DetectorConstruction.cc 78307 2013-12-11 10:55:57Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "SteppingAction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::defineConvParameter(){
    Conv_XY = 15.*cm;
    Conv_Z = 1.5*cm;
    ConvTiO2_XY = 15.12*cm;
    ConvTiO2_Z = 1.62*cm;
    ConvPolish_XY = 1.5*cm;
    ConvPolish_Z = 0.06*cm;
}
G4SubtractionSolid* DetectorConstruction::constructSolidConvTiO2(){
	auto ConvTiO2_box = new G4Box("ConvTiO2", 0.5*ConvTiO2_XY, 0.5*ConvTiO2_XY, 0.5*ConvTiO2_Z);
	auto Conv_box = new G4Box("Conv", 0.5*Conv_XY, 0.5*Conv_XY, 0.5*Conv_Z);
	auto ConvPolish_box = new G4Box("Polish", 0.5*ConvPolish_XY, 0.5*ConvPolish_XY, 0.5*ConvPolish_Z);

    G4RotationMatrix* rotationy = new G4RotationMatrix();
    rotationy->rotateY(90 * deg);  // 90 degrees around Y-axis
    G4RotationMatrix* rotationx = new G4RotationMatrix();
    rotationx->rotateX(90 * deg);  // 90 degrees around Y-axis

	// auto sConvTiO2 = new G4SubtractionSolid("sConvTiO2",ConvTiO2_box,Conv_box);
	// auto solidConvTiO2 = new G4SubtractionSolid("solidConvTiO2",sConvTiO2,Polish_box,rotationy,G4ThreeVector(0.5*Conv_XY+0.5*ConvPolish_XY,0.,0.));
    // solidConvTiO2 = new G4SubtractionSolid("solidConvTiO2",solidConvTiO2,Polish_box,rotationx,G4ThreeVector(0.,0.5*Conv_XY+0.5*ConvPolish_XY,0.));
	// return solidConvTiO2;

    // Step1: Construct a TiO2 shell
    auto sConvTiO2 = new G4SubtractionSolid("sConvTiO2", ConvTiO2_box, Conv_box);

    // Step2: Subtract one polish from the shell
    auto solidConvTiO2_step1 = new G4SubtractionSolid("solidConvTiO2_step1", sConvTiO2, ConvPolish_box, rotationy,
    G4ThreeVector(0.5 * Conv_XY + 0.5 * ConvPolish_Z, 0., 0.));

    // Step3: Subtract the other polish from the shell
    auto solidConvTiO2 = new G4SubtractionSolid("solidConvTiO2", solidConvTiO2_step1, ConvPolish_box, rotationx,
    G4ThreeVector(0., 0.5 * Conv_XY + 0.5 * ConvPolish_Z, 0.));

    // 返回最终的固体
    return solidConvTiO2;

}

void DetectorConstruction::constructConverter()
{
    G4bool checkOverlaps = false;
	std::cout<<"Starting to construct Converter"<<std::endl;
    defineConvParameter();

	// Construct CsI solid;
	auto solidConv = new G4Box("solidConv", 0.5*Conv_XY, 0.5*Conv_XY, 0.5*Conv_Z);
	//Construct logical CsI;
	auto logicConv = new G4LogicalVolume(solidConv, CsI, "logicConv");

	//Construct TiO2 reflector;
	auto solidConvTiO2 = constructSolidConvTiO2();
	//Construct logical TiO2 reflector;
	auto logicConvTiO2 = new G4LogicalVolume(solidConvTiO2, TiO2, "logicConvTiO2");

	// //Construct APD
	// auto solidAPD = new G4Box("solidAPD", 0.5*APD_XY, 0.5*APD_XY, 0.5*APD_Z);
	// auto logicAPD = new G4LogicalVolume(solidAPD, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), "logicAPD");

	//Placement
    G4String ConvName = "physicConv";
    G4String ConvTiO2Name = "physicConvTiO2";
    G4VPhysicalVolume* physicConv = new G4PVPlacement(0, G4ThreeVector(0., 0., -CsI_Z), logicConv, ConvName, logicWorld, false, 0, checkOverlaps);
    G4VPhysicalVolume* physicConvTiO2 = new G4PVPlacement(0, G4ThreeVector(0., 0., -CsI_Z), logicConvTiO2, ConvTiO2Name, logicWorld, false, 0, checkOverlaps);
    // Create logical border surface
    auto logicBorderSurface = new G4LogicalBorderSurface("ConvTiO2BorderSurface", physicConv, physicConvTiO2, CsISurface);
    auto opticalSurface = dynamic_cast<G4OpticalSurface*>(logicBorderSurface->GetSurface(physicConv, physicConvTiO2)->GetSurfaceProperty());
  	if(opticalSurface)opticalSurface->DumpInfo();


}

