#include "Config.hh"

using namespace std;

Config::Config()
{}
Config::~Config()
{}

void Config::Parse(const std::string &config_file)
{
	UI = G4UImanager::GetUIpointer();
	conf = YAML::LoadFile(config_file);

	if(conf["Project"].IsDefined())
	{
		std::cout<<"Config file loaded sucessfully"<<std::endl;
	}
	else
	{
		throw config_file;
	}
}

bool Config::IsLoad()
{
	return conf["Project"].IsDefined();
}

int Config::Run()
{
	//Initialize the singleton HistoManager
	HistoManager::getInstance().book(conf["Global"]["output"].as<std::string>().c_str(),conf["Global"]["savegeo"].as<bool>());
	//choose the Random engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
	if(conf["Global"]["useseed"].as<bool>())
	{
		CLHEP::HepRandom::setTheSeed(conf["Global"]["seed"].as<int>());
	}
	else
	{
		CLHEP::HepRandom::setTheSeed(this->GetTimeNs());
	}
	CLHEP::HepRandom::showEngineStatus();
	std::cout<<"seed: "<<CLHEP::HepRandom::getTheSeed()<<std::endl;

	// Construct the default run manager
	//my Verbose output class
	G4VSteppingVerbose::SetInstance(new SteppingVerbose);
	G4RunManager* runManager = new G4RunManager;


	// set mandatory initialization classes
	//
	DetectorConstruction* detector = new DetectorConstruction(this);
	if(conf["Global"]["savegeo"].as<bool>())
	{
		G4GDMLParser parser;
		if(std::filesystem::exists("vcalo.gdml")){
			std::remove("vcalo.gdml");
		}
		parser.Write("vcalo.gdml",detector->Construct());
	}
	runManager->SetUserInitialization(detector);

	G4VModularPhysicsList* physics = new FTFP_BERT();
	physics->ReplacePhysics(new G4EmStandardPhysics_option4());
	physics->RegisterPhysics(new G4OpticalPhysics());
	runManager->SetUserInitialization(physics);

	//SteppingVerbose* stepV = new SteppingVerbose();

	PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector, this);
	runManager->SetUserAction(primary);

	RunAction* runAction = new RunAction(primary,this);
	runManager->SetUserAction(runAction);

	EventAction* eventAction = new EventAction(this);
	runManager->SetUserAction(eventAction);

	TrackingAction* trackingAction = new TrackingAction(runAction,eventAction,this);
	runManager->SetUserAction(trackingAction);

	SteppingAction* steppingAction = new SteppingAction(detector,eventAction);
	runManager->SetUserAction(steppingAction);

	runManager->SetVerboseLevel(conf["Verbose"]["run"].as<int>());
	G4String command = "/control/execute ";

	UI->ApplyCommand(G4String("/control/verbose ")+G4String(conf["Verbose"]["control"].as<std::string>()));
	UI->ApplyCommand(G4String("/tracking/verbose ")+G4String(conf["Verbose"]["tracking"].as<std::string>()));
	UI->ApplyCommand(G4String("/event/verbose ")+G4String(conf["Verbose"]["event"].as<std::string>()));
	//Initialize G4 kernel
	runManager->Initialize();

	runManager->BeamOn(conf["Global"]["beamon"].as<int>());
	// job termination
	//
	delete runManager;
	//if(access("vcalo.gdml",F_OK) == 0)std::remove("vcalo.gdml");

	return 1;
}

void Config::Print()
{
	ofstream fout("./default.yaml");
	fout<<"#Project information"<<endl;
	fout<<"Project: Vlast-calo"<<endl;
	fout<<"Contact: Zhen Wang < wangz1996@sjtu.edu.cn >"<<endl;
	fout<<"\n"<<endl;
	fout<<"Global:"<<endl;
	fout<<"        useseed: False # default: system time in nano-second"<<endl;
	fout<<"        seed: 1234"<<endl;
	fout<<"\n"<<endl;
	fout<<"        usemac: False # Currently not applicable"<<endl;
	fout<<"        mac: ./vis.mac"<<endl;
	fout<<"\n"<<endl;
	fout<<"        output: ./test.root # Output root file name"<<endl;
	fout<<"        beamon: 100"<<endl;
	fout<<"        savegeo: False"<<endl;
	fout<<"#Construct Calorimeter"<<endl;
	fout<<"ECAL:"<<endl;
	fout<<"        ECALShield: True"<<endl;
	fout<<"        build: True"<<endl;
	fout<<"\n"<<endl;
	fout<<"#Particle source setup"<<endl;
	fout<<"Source:"<<endl;
	fout<<"        particle: mu+"<<endl;
	fout<<"        energy: 100.  # Unit is in GeV"<<endl;
	fout<<"        position: [ 0., 0., -100.]   # Unit is in cm"<<endl;
    fout<<"        direction: [ 0., 0., 1.]"<<endl;
	fout<<"#Verbose"<<endl;
	fout<<"Verbose:"<<endl;
	fout<<"        run: 0"<<endl;
	fout<<"        control: 0"<<endl;
	fout<<"        event: 0"<<endl;
	fout<<"        tracking: 0"<<endl;
	fout.close();
}
