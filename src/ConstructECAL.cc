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
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::defineECALParameter(){
	CsI_XY = 60. * mm;
	CsI_Z = 200. * mm;

	TiO2_XY = 61.2 * mm;
	TiO2_Z = 201.2 * mm;

	Polish_XY = 35. * mm;
	Polish_Z = (TiO2_Z - CsI_Z)/2.;
	
	nCryX = 5;
	nCryY = 5;

	CryGap = 5*mm; // 5mm gap between crystals
	ECALShield_XY = 336. * mm; // 6*CryGap + 5*TiO2_XY = 336.;
	ECALShield_Z = 201.2 * mm; // <TiO2_Z
}

void DetectorConstruction::defineECALMaterial(){
    // vacuum  
    Vacuum =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

	//CsI
	CsI = 
		G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");

	//TiO2
	TiO2 = 
		G4NistManager::Instance()->FindOrBuildMaterial("G4_TITANIUM_DIOXIDE");

	//ECAL Shield
	G4Element* elC = G4NistManager::Instance()->FindOrBuildElement("C");
	G4Element* elO = G4NistManager::Instance()->FindOrBuildElement("O");
	carbonFiber = new G4Material("carbonFiber", 1.75*g/cm3, 2);
	carbonFiber->AddElement(elC, 0.9); 
	carbonFiber->AddElement(elO, 0.1);

}

G4SubtractionSolid* DetectorConstruction::constructSolidTiO2(){
	auto TiO2_box = new G4Box("TiO2", 0.5*TiO2_XY, 0.5*TiO2_XY, 0.5*TiO2_Z);
	auto CsI_box = new G4Box("CsI", 0.5*CsI_XY, 0.5*CsI_XY, 0.5*CsI_Z);
	auto Polish_box = new G4Box("Polish", 0.5*Polish_XY, 0.5*Polish_XY, 0.5*Polish_Z);

	auto sTiO2 = new G4SubtractionSolid("sTiO2",TiO2_box,CsI_box);
	auto solidTiO2 = new G4SubtractionSolid("solidTiO2",sTiO2,Polish_box,0,G4ThreeVector(0.,0.,0.5*CsI_Z+0.5*Polish_Z));
	return solidTiO2;
}

G4VSolid* DetectorConstruction::constructECALShield(){
    auto ECALShield_box = new G4Box("ECALShield_box", 0.5*ECALShield_XY, 0.5*ECALShield_XY, 0.5*ECALShield_Z);
	auto ES_TiO2_box = new G4Box("ES_TiO2_box", 0.5*TiO2_XY, 0.5*TiO2_XY, 0.5*TiO2_Z);

	G4VSolid* unionTiO2Solid = nullptr;

	for(int i=0;i<nCryX;i++){
		for(int j=0;j<nCryY;j++){
			if(i==2 && j==2)continue;
			double x_o = -TiO2_XY*2. - CryGap*2.;//left lower X position
			double y_o = -TiO2_XY*2. - CryGap*2.;//left lower Y position
			double x = x_o + double(i) * ( TiO2_XY + CryGap );
			double y = y_o + double(j) * ( TiO2_XY + CryGap );
			if(unionTiO2Solid == nullptr){
				unionTiO2Solid = new G4UnionSolid("unionSolid",ES_TiO2_box,ES_TiO2_box,0,G4ThreeVector(x,y,0.));
			}
			else{
				unionTiO2Solid = new G4UnionSolid("unionSolid",unionTiO2Solid,ES_TiO2_box,0,G4ThreeVector(x,y,0.));
			}
		}
	}

	auto solidECALShield = new G4SubtractionSolid("solidECALShield",ECALShield_box,unionTiO2Solid);
	return solidECALShield;
}

void DetectorConstruction::ConstructECAL()
{
    G4bool checkOverlaps = false;
	std::cout<<"Starting to construct ECAL"<<std::endl;
    //
    // define materials and parameters
    defineECALParameter();
    defineECALMaterial();

	// Construct CsI solid;
	auto solidCsI = new G4Box("solidCsI", 0.5*CsI_XY, 0.5*CsI_XY, 0.5*CsI_Z);
	//Construct logical CsI;
	auto logicCsI = new G4LogicalVolume(solidCsI, CsI, "logicCsI");
	//Construct TiO2 reflector;
	auto solidTiO2 = constructSolidTiO2();
	//Construct logical TiO2 reflector;
	auto logicTiO2 = new G4LogicalVolume(solidTiO2, TiO2, "logicTiO2");

	//Construct Carbon Fiber Shield;
	G4LogicalVolume* logicECALShield = nullptr;
	if(config->conf["ECAL"]["ECALShield"].as<bool>()){
		//Solid ECAL Shield
		auto solidECALShield = constructECALShield();
		//Logical ECAL Shield
		logicECALShield = new G4LogicalVolume(solidECALShield, carbonFiber, "logicECALShield");
	}

	//Placement
	for(int i=0;i<nCryX;i++){
		for(int j=0;j<nCryY;j++){
			double x_o = -TiO2_XY*2. - CryGap*2.;//left lower X position
			double y_o = -TiO2_XY*2. - CryGap*2.;//left lower Y position
			double x = x_o + double(i) * ( TiO2_XY + CryGap );
			double y = y_o + double(j) * ( TiO2_XY + CryGap );
			int CopyNo = i*5+j;
			new G4PVPlacement(0, G4ThreeVector(x,y,0.), logicCsI, "physicCsI", logicWorld, false, CopyNo, true);
			new G4PVPlacement(0, G4ThreeVector(x,y,0.), logicTiO2, "physicTiO2", logicWorld, false, CopyNo, true);
		}
	}
	new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicECALShield, "physicECALShield", logicWorld, false, 0, true);

}

