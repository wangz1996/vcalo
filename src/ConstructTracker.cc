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
#include "G4LogicalSkinSurface.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "ConstructTracker.hh"
#include "G4Polyhedra.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::defineTrackerParameter(){
	Hex_XY = 287. * mm;
	Hex_Z = 23. * mm;
	HexCell_XY = 25. * mm;
	HexCell_Z = Hex_Z;
	Tray_XY = Hex_XY;
	Tray_Z = Hex_Z + 2.*mm;
	Tracker_XY = 285.*mm;
	Tracker_Z = 0.35*mm;
	//Conv_PosZ = -270.5; ConvTiO2_Z=16.4;  Upper surface = -262.3 mm;
	TrackerPosZ = std::vector<G4double>{25.175,30.135,55.835,60.795,86.495,91.455};
	for(auto &i:TrackerPosZ)i-=262.3;
	TrayPosZ = std::vector<G4double>{12.5,43.16,73.82,104.48};
	for(auto &i:TrayPosZ)i-=262.3;
}

void DetectorConstruction::defineTrackerMaterial(){
	Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
	Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

	Kapton = new G4Material("Kapton", 1.42*g/cm3, 4);
	Kapton->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 0.69113);
	Kapton->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 0.02636);
	Kapton->AddElement(G4NistManager::Instance()->FindOrBuildElement("N"), 0.07327);
	Kapton->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 0.20924);
}
G4VSolid* DetectorConstruction::constructSolidHex(){
	G4int nX = Hex_XY/(HexCell_XY);
	G4int nY = Hex_XY/(HexCell_XY);

	auto solidHexMother = new G4Box("solidHex", 0.5*Hex_XY, 0.5*Hex_XY, 0.5*Hex_Z);
	// G4Box * solidHexCell = nullptr;
	auto solidHexCell = new G4Box("solidHexCell", 0.5*HexCell_XY, 0.5*HexCell_XY, 0.5*Hex_Z);

	G4VSolid* unionHexCellSolid = nullptr;

	for(int i=0;i<nX;i++){
		for(int j=0;j<nY;j++){
			// if(i==5 && j==5)continue;
			double x_o = -0.5*Hex_XY + HexCell_XY*0.5 + 1.*mm;//left lower X position
			double y_o = -0.5*Hex_XY + HexCell_XY*0.5 + 1.*mm;//left lower Y position
			double x = x_o + double(i) * ( HexCell_XY + 1.*mm );
			double y = y_o + double(j) * ( HexCell_XY + 1.*mm );
			if(unionHexCellSolid == nullptr){
				unionHexCellSolid = new G4UnionSolid("unionSolid",solidHexCell,solidHexCell,0,G4ThreeVector(x,y,0.));
			}
			else{
				unionHexCellSolid = new G4UnionSolid("unionSolid",unionHexCellSolid,solidHexCell,0,G4ThreeVector(x,y,0.));
			}
		}
	}
	auto solidHex = new G4SubtractionSolid("solidHex",solidHexMother,unionHexCellSolid, 0, G4ThreeVector(0.,0.,0.));

	auto solidHexLid = new G4Box("solidHexLid", 0.5*Hex_XY, 0.5*Hex_XY, 0.5*mm);
	auto solidHexCarbonFiber = new G4UnionSolid("solidHexCarbonFiber",solidHex,solidHexLid,0,G4ThreeVector(0.,0.,0.5*Hex_Z+0.5*mm));
	auto solidHexCarbonFiber2 = new G4UnionSolid("solidHexCarbonFiber2",solidHexCarbonFiber,solidHexLid,0,G4ThreeVector(0.,0.,-0.5*Hex_Z-0.5*mm));

	return solidHexCarbonFiber2;
}

void DetectorConstruction::constructTracker()
{
	defineTrackerParameter();
	defineTrackerMaterial();
	//Hex and Tray
	auto solidHex = constructSolidHex();
	auto logicHex = new G4LogicalVolume(solidHex, Si, "logicHex");

	//tracker
	auto solidTracker = new G4Box("solidTracker", 0.5*Tracker_XY, 0.5*Tracker_XY, 0.5*Tracker_Z);
	auto logicTracker = new G4LogicalVolume(solidTracker, Si, "logicTracker");
	//Kapton
	auto solidKapton = new G4Box("solidKapton", 0.5*Tracker_XY, 0.5*Tracker_XY, 0.5*Tracker_Z);
	auto logicKapton = new G4LogicalVolume(solidKapton, Kapton, "logicKapton");

	//Placement
	//Tray 0
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrayPosZ.at(0)), logicHex, "physHex", logicWorld, false,0, true);
	//X Tracker + Kapton
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(0)), logicTracker, "physTracker_0", logicWorld, false,0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(0) + 0.5*Tracker_Z + 0.5*Tracker_Z), logicKapton, "physKapton_0", logicWorld, false,0, true);

	//Y Tracker + Kapton
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(1)), logicTracker, "physTracker_1", logicWorld, false,0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(1) + 0.5*Tracker_Z + 0.5*Tracker_Z), logicKapton, "physKapton_1", logicWorld, false,0, true);

	//Tray 1
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrayPosZ.at(1)), logicHex, "physHex", logicWorld, false,0, true);

	//X Tracker + Kapton
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(2)), logicTracker, "physTracker_2", logicWorld, false,0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(2) + 0.5*Tracker_Z + 0.5*Tracker_Z), logicKapton, "physKapton_2", logicWorld, false,0, true);

	//Y Tracker + Kapton
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(3)), logicTracker, "physTracker_3", logicWorld, false,0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(3) + 0.5*Tracker_Z + 0.5*Tracker_Z), logicKapton, "physKapton_3", logicWorld, false,0, true);

	//Tray 2
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrayPosZ.at(2)), logicHex, "physHex", logicWorld, false,0, true);

	//X Tracker + Kapton
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(4)), logicTracker, "physTracker_4", logicWorld, false,0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(4) + 0.5*Tracker_Z + 0.5*Tracker_Z), logicKapton, "physKapton_4", logicWorld, false,0, true);

	//Y Tracker + Kapton
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(5)), logicTracker, "physTracker_5", logicWorld, false,0, true);
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrackerPosZ.at(5) + 0.5*Tracker_Z + 0.5*Tracker_Z), logicKapton, "physKapton_5", logicWorld, false,0, true);

	//Tray 3
	new G4PVPlacement(0, G4ThreeVector(0, 0, TrayPosZ.at(3)), logicHex, "physHex", logicWorld, false,0, true);

}

