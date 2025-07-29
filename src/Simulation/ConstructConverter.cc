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
    
}
G4SubtractionSolid* DetectorConstruction::constructSolidConvTiO2(){
	auto ConvTiO2_box = new G4Box("ConvTiO2", 0.5*ConvTiO2_XY, 0.5*ConvTiO2_XY, 0.5*ConvTiO2_Z);
    auto ConvAir_box = new G4Box("ConvAir", 0.5*ConvAir_XY, 0.5*ConvAir_XY, 0.5*ConvAir_Z);
	auto Conv_box = new G4Box("Conv", 0.5*Conv_XY, 0.5*Conv_XY, 0.5*Conv_Z);
	auto ConvPolish_box = new G4Box("Polish", 0.5*ConvPolish_XY, 0.5*ConvPolish_XY, 0.5*ConvPolish_Z);

    G4RotationMatrix* rotationy = new G4RotationMatrix();
    rotationy->rotateY(90 * deg);  // 90 degrees around Y-axis
    G4RotationMatrix* rotationx = new G4RotationMatrix();
    rotationx->rotateX(90 * deg);  // 90 degrees around Y-axis

    // Step1: Construct a TiO2 shell
    auto sConvTiO2 = new G4SubtractionSolid("sConvTiO2", ConvTiO2_box, ConvAir_box);

    // Step2: Subtract one polish from the shell
    auto solidConvTiO2_step1 = new G4SubtractionSolid("solidConvTiO2_step1", sConvTiO2, ConvPolish_box, rotationy,
    G4ThreeVector(0.5 * ConvAir_XY + 0.5 * ConvPolish_Z, 0., 0.));

    // Step3: Subtract the other polish from the shell
    auto solidConvTiO2 = new G4SubtractionSolid("solidConvTiO2", solidConvTiO2_step1, ConvPolish_box, rotationx,
    G4ThreeVector(0., 0.5 * ConvAir_XY + 0.5 * ConvPolish_Z, 0.));

    // 返回最终的固体
    return solidConvTiO2;
}

G4SubtractionSolid* DetectorConstruction::constructSolidConvSS(){
    auto ConvSS_box = new G4Box("ConvSS", 0.5*ConvSS_XY, 0.5*ConvSS_XY, 0.5*ConvSS_Z);
    auto ConvTiO2_box = new G4Box("ConvTiO2", 0.5*ConvTiO2_XY, 0.5*ConvTiO2_XY, 0.5*ConvTiO2_Z);
    G4SubtractionSolid* solidConvSS = nullptr;
    for(auto x: {-0.5*mm - ConvTiO2_XY*0.5 , 0.5*mm + ConvTiO2_XY*0.5}){
        for(auto y: {-0.5*mm - ConvTiO2_XY*0.5 , 0.5*mm + ConvTiO2_XY*0.5}){
            if(solidConvSS == nullptr){
                solidConvSS = new G4SubtractionSolid("solidConvSS", ConvSS_box, ConvTiO2_box, 0, G4ThreeVector(x, y, 0.));
            }
            else {
                solidConvSS = new G4SubtractionSolid("solidConvSS", solidConvSS, ConvTiO2_box, 0, G4ThreeVector(x, y, 0.));
            }
        }
    }
    return solidConvSS;
}

void DetectorConstruction::constructConverter()
{
    G4bool checkOverlaps = true;
	std::cout<<"Starting to construct Converter"<<std::endl;

	// Construct CsI solid;
	auto solidConv = new G4Box("solidConv", 0.5*Conv_XY, 0.5*Conv_XY, 0.5*Conv_Z);
	//Construct logical CsI;
	auto logicConv = new G4LogicalVolume(solidConv, CsI, "logicConv");
	//Construct TiO2 reflector;
	auto solidConvTiO2 = constructSolidConvTiO2();
	//Construct logical TiO2 reflector;
	auto logicConvTiO2 = new G4LogicalVolume(solidConvTiO2, TiO2, "logicConvTiO2");

	//Construct APD
	auto solidAPD = new G4Box("solidConvAPD", 0.5*APD_XY, 0.5*APD_XY, 0.5*APD_Z);
	auto logicConvAPD = new G4LogicalVolume(solidAPD, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), "logicConvAPD");

    //Construct Converter Support Structure
    auto solidConvSS = constructSolidConvSS();
    auto logicConvSS = new G4LogicalVolume(solidConvSS, carbonFiber, "logicConvSS");

    //Converter Lid
    auto solidConvLid = new G4Box("solidConvLid", 0.5*Hex_XY, 0.5*Hex_XY, 0.5*mm);
    auto logicConvLid = new G4LogicalVolume(solidConvLid, carbonFiber, "logicConvLid");


    //Placement
    new G4PVPlacement(0, G4ThreeVector(0., 0., Conv_PosZ), logicConvSS, "physicConvSS", logicWorld, false, 0, checkOverlaps);
    G4RotationMatrix* rotationy = new G4RotationMatrix();
    rotationy->rotateY(90 * deg);  // 90 degrees around Y-axis
    G4RotationMatrix* rotationx = new G4RotationMatrix();
    rotationx->rotateX(90 * deg);  // 90 degrees around Y-axis
    G4RotationMatrix* rotationz = new G4RotationMatrix();
    rotationz->rotateZ(90 * deg);  // 90 degrees around Z-axis
    int tmp_index=0;
    for(auto x: {-0.5*mm - ConvTiO2_XY*0.5 , 0.5*mm + ConvTiO2_XY*0.5}){
        for(auto y: {-0.5*mm - ConvTiO2_XY*0.5 , 0.5*mm + ConvTiO2_XY*0.5}){
            auto physicConv = new G4PVPlacement(0, G4ThreeVector(x, y, Conv_PosZ), logicConv, "physicConv"+G4String(std::to_string(tmp_index)), logicWorld, false, 0, checkOverlaps);
            G4RotationMatrix* rz=new G4RotationMatrix(); rz->rotateZ(tmp_index*90.*deg);
            auto physicConvTiO2 = new G4PVPlacement(rz, G4ThreeVector(x, y, Conv_PosZ), logicConvTiO2, "physicConvTiO2"+G4String(std::to_string(tmp_index)), logicWorld, false, 0, checkOverlaps);
            G4float APD_X = x < 0. ? x - 0.5*Conv_XY - 3.*mm - 0.5*APD_Z : x + 0.5*Conv_XY + 3.*mm + 0.5*APD_Z;
            G4float APD_Y = y < 0. ? y - 0.5*Conv_XY - 3.*mm - 0.5*APD_Z : y + 0.5*Conv_XY + 3.*mm + 0.5*APD_Z;
            auto physicConvAPD1 = new G4PVPlacement(rotationy, G4ThreeVector(APD_X, y, Conv_PosZ), logicConvAPD, "physicConvAPD1"+G4String(std::to_string(tmp_index)), logicWorld, false, 0, checkOverlaps);
            auto physicConvAPD2 = new G4PVPlacement(rotationx, G4ThreeVector(x, APD_Y, Conv_PosZ), logicConvAPD, "physicConvAPD2"+G4String(std::to_string(tmp_index)), logicWorld, false, 0, checkOverlaps);
            tmp_index++;
        }
    }
	//Placement
    // G4String ConvName = "physicConv";
    // G4String ConvTiO2Name = "physicConvTiO2";
    // G4String APDName = "physicConvAPD";
    // G4VPhysicalVolume* physicConv = new G4PVPlacement(0, G4ThreeVector(0., 0., Conv_PosZ), logicConv, ConvName, logicWorld, false, 0, checkOverlaps);
    // G4VPhysicalVolume* physicConvTiO2 = new G4PVPlacement(0, G4ThreeVector(0., 0., Conv_PosZ), logicConvTiO2, ConvTiO2Name, logicWorld, false, 0, checkOverlaps);
    // G4RotationMatrix* rotationy = new G4RotationMatrix();
    // rotationy->rotateY(90 * deg);  // 90 degrees around Y-axis
    // G4RotationMatrix* rotationx = new G4RotationMatrix();
    // rotationx->rotateX(90 * deg);  // 90 degrees around Y-axis
    // G4VPhysicalVolume* physicConvAPD1 = new G4PVPlacement(rotationy, G4ThreeVector(0.5*Conv_XY + 5.*mm + 0.5*APD_Z, 0.,Conv_PosZ), logicConvAPD, APDName+G4String("1"), logicWorld, false, 0, checkOverlaps);
    // G4VPhysicalVolume* physicConvAPD2 = new G4PVPlacement(rotationx, G4ThreeVector(0., 0.5*Conv_XY + 5.*mm + 0.5*APD_Z,Conv_PosZ), logicConvAPD, APDName+G4String("2"), logicWorld, false, 0, checkOverlaps);
    // if(config->conf["Global"]["optical"].as<bool>()){
    //     // Create logical border surface
    //     auto logicBorderSurface = new G4LogicalBorderSurface("ConvTiO2BorderSurface", physicConv, physicConvTiO2, CsISurface);
    //     auto opticalSurface = dynamic_cast<G4OpticalSurface*>(logicBorderSurface->GetSurface(physicConv, physicConvTiO2)->GetSurfaceProperty());
    //     if(opticalSurface)opticalSurface->DumpInfo();
    //     // Create physical border surface between TiO2 and world
	// 	new G4LogicalBorderSurface("ConvTiO2GalacticBorderSurface", physicConvTiO2, physiWorld, TiO2_Galactic_Surface);
	// 	// Create physical border surface between CsI and world
	// 	new G4LogicalBorderSurface("ConvCsIGalacticBorderSurface", physicConv, physiWorld, CsI_Galactic_Surface);
    //     // Create physical border surface between APD and world
    //     new G4LogicalSkinSurface("ConvAPDGalacticSkinSurface", logicConvAPD, APD_Galactic_Surface);
    // }
    //Converter Lid Placement
    new G4PVPlacement(0, G4ThreeVector(0., 0., Conv_PosZ-0.5*ConvTiO2_Z-0.5*mm), logicConvLid, "ConvLid", logicWorld, false, 0, checkOverlaps);
    std::cout<<"Converter constructed at : "<<Conv_PosZ<<std::endl;
}

