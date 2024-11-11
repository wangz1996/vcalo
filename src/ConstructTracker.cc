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
}

void DetectorConstruction::defineTrackerMaterial(){
	Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
	Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
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
			if(i==5 && j==5)continue;
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
	return solidHex;
}

void DetectorConstruction::constructTracker()
{
	defineTrackerParameter();
	defineTrackerMaterial();
	auto solidHex = constructSolidHex();
	auto logicHex = new G4LogicalVolume(solidHex, Si, "logicHex");
	new G4PVPlacement(0, G4ThreeVector(0, 0, Conv_PosZ + ConvTiO2_Z*0.5 + Hex_Z*0.5), logicHex, "physHex", logicWorld, false, 0);
}

