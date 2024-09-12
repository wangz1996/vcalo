#ifndef CONFIG_HH
#define CONFIG_HH
#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "SteppingVerbose.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "TrackingAction.hh"
#include "QGSP_BERT.hh"
#include "G4GDMLParser.hh"
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4SystemOfUnits.hh"
#include <CLHEP/Units/SystemOfUnits.h>

#include "G4VisExecutive.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalPhoton.hh"
#include "G4UIExecutive.hh"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include "yaml-cpp/yaml.h"
#include <filesystem>

class Config
{
public:
	Config();
	~Config();
	virtual void Print();
    virtual void Parse(const std::string &config_file);
    virtual int Run();
	bool IsLoad();
	YAML::Node conf;
private:
	G4UImanager* UI;
	G4long GetTimeNs()
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME,&ts);
		return ts.tv_sec*1000000000+ts.tv_nsec;
	}

};

#endif
