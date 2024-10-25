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
const std::unordered_map<std::string, G4SurfaceType> DetectorConstruction::surfaceTypeMap = {
    {"dielectric_metal", G4SurfaceType::dielectric_metal},
    {"dielectric_dielectric", G4SurfaceType::dielectric_dielectric},
    {"firsov", G4SurfaceType::firsov},
    {"x_ray", G4SurfaceType::x_ray}
};
const std::unordered_map<std::string, G4OpticalSurfaceModel> DetectorConstruction::surfaceModelMap = {
    {"glisur", G4OpticalSurfaceModel::glisur},
    {"unified", G4OpticalSurfaceModel::unified},
    {"LUT", G4OpticalSurfaceModel::LUT}
};
const std::unordered_map<std::string, G4OpticalSurfaceFinish> DetectorConstruction::surfaceFinishMap = {
    {"polished", G4OpticalSurfaceFinish::polished},
    {"polishedfrontpainted", G4OpticalSurfaceFinish::polishedfrontpainted},
    {"polishedbackpainted", G4OpticalSurfaceFinish::polishedbackpainted},
    {"ground", G4OpticalSurfaceFinish::ground},
    {"groundfrontpainted", G4OpticalSurfaceFinish::groundfrontpainted},
    {"groundbackpainted", G4OpticalSurfaceFinish::groundbackpainted}
};

void DetectorConstruction::defineECALParameter(){
	CsI_XY = 60. * mm;
	CsI_Z = 200. * mm;

	TiO2_XY = 61.2 * mm;
	TiO2_Z = 201.2 * mm;

	Polish_XY = 35. * mm;
	Polish_Z = (TiO2_Z - CsI_Z)/2.;

	APD_XY = 10. *mm;
	APD_Z = 10. *um;
	
	nCryX = 5;
	nCryY = 5;

	Cushion_XY = TiO2_XY;
	CushionInner_XY = 40. *mm;
	Cushion_Z = 1. *mm;

	if(config->conf["ECAL"]["ECALShield"].as<bool>()){
		CryGap = 1.5 * mm;
		Hole_XY = 62. * mm;
	}
	else{
		CryGap = 0. * mm;
		Hole_XY = TiO2_XY;
	}

	
	Hole_Z = TiO2_Z;
	ECALShield_XY = 6*CryGap + 5*Hole_XY; // 6*CryGap + 5*TiO2_XY = 336.;
	ECALShield_Z = TiO2_Z; // TiO2_Z + 2mm carbone fiber


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

	//silicone rubber
	G4Element* elSi = G4NistManager::Instance()->FindOrBuildElement("Si");
	G4Element* elH = G4NistManager::Instance()->FindOrBuildElement("H");
	siliconeRubber = new G4Material("SiliconeRubber", 1.1*g/cm3, 4);
	siliconeRubber->AddElement(elSi, 1); // 硅
    siliconeRubber->AddElement(elO, 2);  // 氧
    siliconeRubber->AddElement(elC, 2);  // 碳
    siliconeRubber->AddElement(elH, 6);  // 氢

	//Optical properties
	//CsI
	for(size_t i=0;i<CsI_NEntries;i++){CsI_RIndex[i] = 1.79;}
	for(size_t i=0;i<CsI_NEntries;i++){CsI_AbsLength[i] = 500.*cm;}
	for(size_t i=0;i<CsI_NEntries;i++){CsI_Rayleigh[i] = config->conf["Parameter"]["CsI"]["Rayleigh"].as<double>()*m;}
	CsIMPT = new G4MaterialPropertiesTable();
	CsIMPT->AddProperty("RINDEX", CsI_PEnergy, CsI_RIndex, CsI_NEntries);//Refractive index
	CsIMPT->AddProperty("ABSLENGTH", CsI_PEnergy, CsI_AbsLength, CsI_NEntries);//Absorption length
	CsIMPT->AddProperty("RAYLEIGH", CsI_PEnergy, CsI_Rayleigh, CsI_NEntries);//Rayleigh scattering length
	// CsIMPT->AddProperty("RAYLEIGH", PhotonEnergy, {500*cm, 500*cm}, nEntries);//Rayleigh scattering length
	CsIMPT->AddProperty("SCINTILLATIONCOMPONENT1", CsI_PEnergy, CsI_SlowComponentIntensity, CsI_NEntries);//Slow component of the scintillation spectrum

	CsIMPT->AddConstProperty("SCINTILLATIONYIELD",65000./MeV); //The number of photons emitted per MeV of deposited energy
	CsIMPT->AddConstProperty("RESOLUTIONSCALE",1.); //The resolution of the scintillation spectrum
	// CsIMPT->AddConstProperty("FASTTIMECONSTANT", 700*ns); //The time constant of the fast component of the scintillation spectrum
	CsIMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1220*ns); //The time constant of the slow component of the scintillation spectrum
	// CsIMPT->AddConstProperty("YIELDRATIO",0.57);//Fast-to-slow component yield ratio
	CsI->SetMaterialPropertiesTable(CsIMPT);

	//TiO2
	TiO2MPT = new G4MaterialPropertiesTable();
	TiO2MPT->AddProperty("RINDEX", TiO2_PEnergy, TiO2_RIndex, TiO2_NEntries);
	for(size_t i=0;i<TiO2_NEntries;i++){TiO2_Reflectivity[i] = config->conf["Parameter"]["TiO2"]["Reflectivity"].as<double>();}
	TiO2MPT->AddProperty("REFLECTIVITY",TiO2_PEnergy, TiO2_Reflectivity, TiO2_NEntries);
	for(size_t i=0;i<TiO2_NEntries;i++){TiO2_AbsLength[i] = config->conf["Parameter"]["TiO2"]["AbsLength"].as<double>()*m;}
	TiO2MPT->AddProperty("ABSLENGTH",TiO2_PEnergy, TiO2_AbsLength, TiO2_NEntries);
	TiO2->SetMaterialPropertiesTable(TiO2MPT);

	//Optical Surfaces (actually I think it should be called TiO2 surface)
	//CsI(Tl) and TiO2
	for(size_t i=0;i<CsI_NEntries;i++){CsI_SpecularLobe[i] = config->conf["Parameter"]["Surface"]["SpecularLobe"].as<double>();}
	for(size_t i=0;i<CsI_NEntries;i++){CsI_SpecularSpike[i] = config->conf["Parameter"]["Surface"]["SpecularSpike"].as<double>();}
	// for(size_t i=0;i<CsI_NEntries;i++){CsI_DiffuseLobe[i] = 0.05;}
	for(size_t i=0;i<CsI_NEntries;i++){CsI_BackScatter[i] = 0.;}
	for(size_t i=0;i<CsI_NEntries;i++){CsI_Reflectivity[i] = config->conf["Parameter"]["Surface"]["Reflectivity"].as<double>();}
	CsISurface = new G4OpticalSurface("CsISurface");
	CsISurface->SetType(surfaceTypeMap.at(config->conf["Parameter"]["Surface"]["Type"].as<std::string>()));
	CsISurface->SetModel(surfaceModelMap.at(config->conf["Parameter"]["Surface"]["Model"].as<std::string>()));
	CsISurface->SetFinish(surfaceFinishMap.at(config->conf["Parameter"]["Surface"]["Finish"].as<std::string>()));
	CsISurface->SetSigmaAlpha(config->conf["Parameter"]["Surface"]["SigmaAlpha"].as<double>());

	CsI_SurfaceMPT = new G4MaterialPropertiesTable();
	CsI_SurfaceMPT->AddProperty("REFLECTIVITY",CsI_PEnergy, CsI_Reflectivity, CsI_NEntries);
	CsI_SurfaceMPT->AddProperty("SPECULARLOBECONSTANT",CsI_PEnergy, CsI_SpecularLobe, CsI_NEntries);
	CsI_SurfaceMPT->AddProperty("SPECULARSPIKECONSTANT",CsI_PEnergy, CsI_SpecularSpike, CsI_NEntries);
	CsI_SurfaceMPT->AddProperty("BACKSCATTERCONSTANT",CsI_PEnergy, CsI_BackScatter, CsI_NEntries);
	CsISurface->SetMaterialPropertiesTable(CsI_SurfaceMPT);
}

G4SubtractionSolid* DetectorConstruction::constructSolidTiO2(){
	auto TiO2_box = new G4Box("TiO2", 0.5*TiO2_XY, 0.5*TiO2_XY, 0.5*TiO2_Z);
	auto CsI_box = new G4Box("CsI", 0.5*CsI_XY, 0.5*CsI_XY, 0.5*CsI_Z);
	auto Polish_box = new G4Box("Polish", 0.5*Polish_XY, 0.5*Polish_XY, 0.5*Polish_Z);

	auto sTiO2 = new G4SubtractionSolid("sTiO2",TiO2_box,CsI_box);
	auto solidTiO2 = new G4SubtractionSolid("solidTiO2",sTiO2,Polish_box,0,G4ThreeVector(0.,0.,0.5*CsI_Z+0.5*Polish_Z));
	return solidTiO2;
}

G4SubtractionSolid* DetectorConstruction::constructSolidSiliconeRubber(){
	auto TiO2_box = new G4Box("TiO2", 0.5*TiO2_XY, 0.5*TiO2_XY, 0.5*TiO2_Z);
	auto SiliconeRubber_box = new G4Box("SiliconeRubber", 0.5*Hole_XY, 0.5*Hole_XY, 0.5*Hole_Z);

	auto solidSiliconeRubber = new G4SubtractionSolid("solidSiliconeRubber",SiliconeRubber_box,TiO2_box,0,G4ThreeVector(0.,0.,0.));
	return solidSiliconeRubber;
}

G4VSolid* DetectorConstruction::constructECALShield(){
    auto ECALShield_box = new G4Box("ECALShield_box", 0.5*ECALShield_XY, 0.5*ECALShield_XY, 0.5*ECALShield_Z);
	auto Hole_box = new G4Box("Hole_box", 0.5*Hole_XY, 0.5*Hole_XY, 0.5*Hole_Z);

	G4VSolid* unionTiO2Solid = nullptr;

	for(int i=0;i<nCryX;i++){
		for(int j=0;j<nCryY;j++){
			if(i==2 && j==2)continue;
			double x_o = -Hole_XY*2. - CryGap*2.;//left lower X position
			double y_o = -Hole_XY*2. - CryGap*2.;//left lower Y position
			double x = x_o + double(i) * ( Hole_XY + CryGap );
			double y = y_o + double(j) * ( Hole_XY + CryGap );
			if(unionTiO2Solid == nullptr){
				unionTiO2Solid = new G4UnionSolid("unionSolid",Hole_box,Hole_box,0,G4ThreeVector(x,y,0.));
			}
			else{
				unionTiO2Solid = new G4UnionSolid("unionSolid",unionTiO2Solid,Hole_box,0,G4ThreeVector(x,y,0.));
			}
		}
	}

	auto solidECALShield = new G4SubtractionSolid("solidECALShield",ECALShield_box,unionTiO2Solid, 0, G4ThreeVector(0.,0.,0.));
	return solidECALShield;
}

G4VSolid* DetectorConstruction::constructCushion(){
	auto Cushion_box = new G4Box("Cushion_box", 0.5*Cushion_XY, 0.5*Cushion_XY, 0.5*Cushion_Z);
	auto CushionInner_box = new G4Box("CushionInner_box", 0.5*CushionInner_XY, 0.5*CushionInner_XY, 0.5*Cushion_Z);

	auto solidCushion = new G4SubtractionSolid("solidCushion",Cushion_box,CushionInner_box,0,G4ThreeVector(0.,0.,0.));
	return solidCushion;
}

void DetectorConstruction::constructECAL()
{
    G4bool checkOverlaps = false;
	std::cout<<"Starting to construct ECAL"<<std::endl;

	// Construct CsI solid;
	auto solidCsI = new G4Box("solidCsI", 0.5*CsI_XY, 0.5*CsI_XY, 0.5*CsI_Z);
	//Construct logical CsI;
	auto logicCsI = new G4LogicalVolume(solidCsI, CsI, "logicCsI");

	//Construct TiO2 reflector;
	auto solidTiO2 = constructSolidTiO2();
	//Construct logical TiO2 reflector;
	auto logicTiO2 = new G4LogicalVolume(solidTiO2, TiO2, "logicTiO2");

	//Construct APD
	auto solidAPD = new G4Box("solidAPD", 0.5*APD_XY, 0.5*APD_XY, 0.5*APD_Z);
	auto logicAPD = new G4LogicalVolume(solidAPD, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), "logicAPD");

	//Construct Carbon Fiber Shield, silicone rubber;
	G4LogicalVolume* logicECALShield = nullptr;
	G4LogicalVolume* logicSiliconeRubber = nullptr;
	G4LogicalVolume* logicCushion = nullptr;
	G4LogicalVolume* logicLid = nullptr;
	if(config->conf["ECAL"]["ECALShield"].as<bool>()){
		//Solid ECAL Shield
		auto solidECALShield = constructECALShield();
		//Logical ECAL Shield
		logicECALShield = new G4LogicalVolume(solidECALShield, carbonFiber, "logicECALShield");
		// logicECALShield->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.549, 0.0, 0.5)));

		//Solid Silicone Rubber
		auto solidSiliconeRubber = constructSolidSiliconeRubber();
		//Logical Silicone Rubber
		logicSiliconeRubber = new G4LogicalVolume(solidSiliconeRubber, siliconeRubber, "logicSiliconeRubber");

		//Cushion
		auto solidCushion = constructCushion();
		logicCushion = new G4LogicalVolume(solidCushion, siliconeRubber, "logicCushion");

		//Lid
		auto Lid_box = new G4Box("Lid_box", 0.5*ECALShield_XY, 0.5*ECALShield_XY, 0.5*mm);
		logicLid = new G4LogicalVolume(Lid_box, carbonFiber, "logicLid");
	}

	//Placement
	for(int i=0;i<nCryX;i++){
		for(int j=0;j<nCryY;j++){
			double x_o = -Hole_XY*2. - CryGap*2.;//left lower X position
			double y_o = -Hole_XY*2. - CryGap*2.;//left lower Y position
			double x = x_o + double(i) * ( Hole_XY + CryGap );
			double y = y_o + double(j) * ( Hole_XY + CryGap );
			int CopyNo = i*5+j;

			// Create unique names for each physical volume
        	G4String CsIName = "physicCsI_" + std::to_string(CopyNo);
        	G4String TiO2Name = "physicTiO2_" + std::to_string(CopyNo);
			G4String APDName = "physicAPD_" + std::to_string(CopyNo);
			// Create physical volumes
        	G4VPhysicalVolume* physicCsI = new G4PVPlacement(0, G4ThreeVector(x, y, 0.), logicCsI, CsIName, logicWorld, false, CopyNo, true);
        	G4VPhysicalVolume* physicTiO2 = new G4PVPlacement(0, G4ThreeVector(x, y, 0.), logicTiO2, TiO2Name, logicWorld, false, CopyNo, true);
			G4VPhysicalVolume* physicAPD = new G4PVPlacement(0, G4ThreeVector(x, y, CsI_Z/2. + 5.*mm), logicAPD, APDName, logicWorld, false, CopyNo, true);
			// Create logical border surface
        	auto logicBorderSurface = new G4LogicalBorderSurface("CsITiO2BorderSurface_" + std::to_string(CopyNo), physicCsI, physicTiO2, CsISurface);
			auto opticalSurface = dynamic_cast<G4OpticalSurface*>(logicBorderSurface->GetSurface(physicCsI, physicTiO2)->GetSurfaceProperty());
  			if(opticalSurface)opticalSurface->DumpInfo();
			new G4PVPlacement(0, G4ThreeVector(x,y,0.5*TiO2_Z+0.5*Cushion_Z), logicCushion, "physicCushion", logicWorld, false, CopyNo, true);
			if(config->conf["ECAL"]["ECALShield"].as<bool>()){
				new G4PVPlacement(0, G4ThreeVector(x,y,0.), logicSiliconeRubber, "physicSiliconeRubber", logicWorld, false, CopyNo, true);
				new G4PVPlacement(0, G4ThreeVector(x,y,-0.5*TiO2_Z-0.5*Cushion_Z), logicCushion, "physicCushion", logicWorld, false, CopyNo, true);
			}
		}
	}
	
	if(config->conf["ECAL"]["ECALShield"].as<bool>()){
		new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicECALShield, "physicECALShield", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(0.,0.,-0.5*TiO2_Z-0.5*Cushion_Z-0.5*mm), logicLid, "physicLid", logicWorld, false, 0, true);
	}

}

