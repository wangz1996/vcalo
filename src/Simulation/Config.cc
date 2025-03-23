#include "Config.hh"

using std::ofstream, std::cout, std::endl, std::ifstream;

Config::Config()
{
}
Config::~Config()
{
}

void Config::Parse(const std::string &config_file)
{
	UI = G4UImanager::GetUIpointer();
	conf = YAML::LoadFile(config_file);

	if (conf["Project"].IsDefined())
	{
		std::cout << "Config file loaded sucessfully" << std::endl;
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
	// Initialize the singleton HistoManager
	HistoManager::getInstance().book(conf["Global"]["output"].as<std::string>().c_str(), conf["Global"]["savegeo"].as<bool>());
	HistoManager::getInstance().bindConfig(this);
	// choose the Random engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
	if (conf["Global"]["useseed"].as<bool>())
	{
		CLHEP::HepRandom::setTheSeed(conf["Global"]["seed"].as<int>());
	}
	else
	{
		CLHEP::HepRandom::setTheSeed(this->GetTimeNs());
	}
	CLHEP::HepRandom::showEngineStatus();
	std::cout << "seed: " << CLHEP::HepRandom::getTheSeed() << std::endl;

	// Construct the default run manager
	// my Verbose output class
	G4VSteppingVerbose::SetInstance(new SteppingVerbose);
	G4RunManager *runManager = new G4RunManager;

	// set mandatory initialization classes
	//
	DetectorConstruction *detector = new DetectorConstruction(this);
	if (conf["Global"]["savegeo"].as<bool>())
	{
		G4GDMLParser parser;
		if (std::filesystem::exists("vcalo.gdml"))
		{
			std::remove("vcalo.gdml");
		}
		parser.Write("vcalo.gdml", detector->Construct());
	}
	runManager->SetUserInitialization(detector);

	G4VModularPhysicsList *physics = new QGSP_BERT();

	// StepLimiter 可能影响 radioactiveDecay，建议只对特定粒子生效
	G4StepLimiterPhysics *stepLimitPhys = new G4StepLimiterPhysics();
	stepLimitPhys->SetApplyToAll(false); // 只对非放射性粒子生效
	physics->RegisterPhysics(stepLimitPhys);

	// 可选：如果涉及光学过程
	if (conf["Global"]["optical"].as<bool>())
	{
		physics->RegisterPhysics(new G4OpticalPhysics());
	}

	if (conf["Source"]["isradio"].as<bool>())
	{
		physics->RegisterPhysics(new G4DecayPhysics());
		physics->RegisterPhysics(new G4RadioactiveDecayPhysics()); // 自动包含 G4RadioactiveDecay

		// 确保离子物理过程被正确加载
		physics->RegisterPhysics(new G4IonPhysics());

		// Biasing 需要在所有物理过程注册之后
		G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
		biasingPhysics->Bias("radioactiveDecay");
		physics->RegisterPhysics(biasingPhysics);
	}

	// 初始化 PhysicsList
	runManager->SetUserInitialization(physics);

	// SteppingVerbose* stepV = new SteppingVerbose();

	PrimaryGeneratorAction *primary = new PrimaryGeneratorAction(detector, this);
	runManager->SetUserAction(primary);

	RunAction *runAction = new RunAction(primary, this);
	runManager->SetUserAction(runAction);

	EventAction *eventAction = new EventAction(this);
	runManager->SetUserAction(eventAction);

	TrackingAction *trackingAction = new TrackingAction(runAction, eventAction, this);
	runManager->SetUserAction(trackingAction);

	SteppingAction *steppingAction = new SteppingAction(detector, eventAction, this);
	runManager->SetUserAction(steppingAction);

	runManager->SetVerboseLevel(conf["Verbose"]["run"].as<int>());
	G4String command = "/control/execute ";

	UI->ApplyCommand(G4String("/control/verbose ") + G4String(conf["Verbose"]["control"].as<std::string>()));
	UI->ApplyCommand(G4String("/tracking/verbose ") + G4String(conf["Verbose"]["tracking"].as<std::string>()));
	UI->ApplyCommand(G4String("/event/verbose ") + G4String(conf["Verbose"]["event"].as<std::string>()));
	

	G4VisManager *visManager = new G4VisExecutive();
	visManager->Initialize();
	if (conf["Global"]["vis"].as<bool>())
	{
		G4UIExecutive *uie = new G4UIExecutive(1, nullptr, "Qt");
		SetupVisualization();
		uie->SessionStart();
		// delete uie;
	}
	// Initialize G4 kernel
	runManager->Initialize();
	if (conf["Global"]["usemac"].as<bool>())
	{
		if (!ifstream(conf["Global"]["mac"].as<std::string>()).good())
		{
			std::cout << "Macro file not found!" << std::endl;
			throw conf["Global"]["mac"].as<std::string>().c_str();
		}
		std::cout << "Executing macro file..." << std::endl;
		UI->ExecuteMacroFile(conf["Global"]["mac"].as<std::string>().c_str());
	}
	else
	{
		if(conf["Source"]["isradio"].as<bool>()){
		UI->ApplyCommand("/process/had/rdm/thresholdForVeryLongDecayTime 1.0e+60 year");
	}
		runManager->BeamOn(conf["Global"]["beamon"].as<int>());
	}
	// job termination
	//
	delete runManager;
	// if(access("vcalo.gdml",F_OK) == 0)std::remove("vcalo.gdml");

	return 1;
}

void Config::SetupVisualization()
{
	UI->ApplyCommand("/vis/viewer/set/autoRefresh false");
	UI->ApplyCommand("/vis/open OGL");
	UI->ApplyCommand("/vis/drawVolume");
	UI->ApplyCommand("/vis/viewer/set/viewpointTheta 45");
	UI->ApplyCommand("/vis/viewer/set/viewpointPhi 45");
	UI->ApplyCommand("/vis/viewer/set/style wireframe");
	UI->ApplyCommand("/vis/viewer/set/background white");
	UI->ApplyCommand("/vis/scene/add/trajectories smooth");
	UI->ApplyCommand("/vis/scene/add/hits");
	UI->ApplyCommand("/vis/scene/add/axes 0 0 1 0.2");
	UI->ApplyCommand("/vis/scene/add/axes 1 0 0 0.2");
	UI->ApplyCommand("/vis/scene/add/axes 0 1 0 0.2");
	UI->ApplyCommand("/vis/scene/add/axes 0 0 0 0.2");
	UI->ApplyCommand("/vis/scene/add/text 0 0 0 0.1 \"Vlast-CALO\" red");
	UI->ApplyCommand("/vis/scene/add/text 0 0 0 0.1 \"Zhen Wang\" red");
	UI->ApplyCommand("/vis/scene/add/text 0 0 0 0.1 \"wangz1996@sjtu.edu.cn\" red");
	return;
}

void Config::Print()
{
	ofstream fout("./default.yaml");
	fout << "# Project information" << endl;
	fout << "Project: Vlast-CALO" << endl;
	fout << "Contact: Zhen Wang < wangz1996@sjtu.edu.cn >" << endl;
	fout << "\n"
		 << endl;

	fout << "Global:" << endl;
	fout << "    vis: False" << endl;
	fout << "    useseed: False # default: system time in nano-second" << endl;
	fout << "    seed: 100" << endl;
	fout << "    usemac: False # Currently not applicable" << endl;
	fout << "    mac: ./primaryproton.mac" << endl;
	fout << "    usespec: False # Use spectrum for incident energy" << endl;
	fout << "    specfile: ./spec.root" << endl;
	fout << "    spechist: hspec" << endl;
	fout << "    optical: False" << endl;
	fout << "    #output: ./priproton.root # Output root file name" << endl;
	fout << "    #output: ./secproton.root # Output root file name" << endl;
	fout << "    output: ./test.root # Output root file name" << endl;
	fout << "    beamon: 20" << endl;
	fout << "    savegeo: False" << endl;
	fout << "    savetrack: 1" << endl;
	fout << "    trackEnergy_threshold: 2.0" << endl;
	fout << "\n"
		 << endl;

	fout << "Parameter:" << endl;
	fout << "    CsI:" << endl;
	fout << "        # Rayleigh in meter" << endl;
	fout << "        Rayleigh: 0.6" << endl;
	fout << "    TiO2:" << endl;
	fout << "        # Reflectivity (0~1)" << endl;
	fout << "        Reflectivity: 0.8" << endl;
	fout << "        # Absorption Length in meter" << endl;
	fout << "        AbsLength: 1.0" << endl;
	fout << "    Surface:" << endl;
	fout << "        # Specular reflection parameter (0~1)" << endl;
	fout << "        SpecularLobe: 0.8" << endl;
	fout << "        # Ideal specular reflection parameter (0~1)" << endl;
	fout << "        SpecularSpike: 0.2" << endl;
	fout << "        # Reflectivity (0~1)" << endl;
	fout << "        Reflectivity: 0.98" << endl;
	fout << "        #dielectric_dielectric dielectric_metal" << endl;
	fout << "        Type: dielectric_dielectric" << endl;
	fout << "        #glisur unified LUT dichroic" << endl;
	fout << "        Model: unified" << endl;
	fout << "        #polished polishedfrontpainted polishedbackpainted ground groundbackpainted polishedtioair groundtioair" << endl;
	fout << "        Finish: ground" << endl;
	fout << "        # Roughness of the surface (0~1)" << endl;
	fout << "        SigmaAlpha: 0.1" << endl;
	fout << "\n"
		 << endl;

	fout << "#Construct Tracker" << endl;
	fout << "Tracker:" << endl;
	fout << "    build: True" << endl;
	fout << "\n"
		 << endl;
	
	fout << "#Construct ACD" << endl;
	fout << "ACD:" << endl;
	fout << "    build: True" << endl;
	fout << "\n"
		 << endl;

	fout << "#Construct Converter" << endl;
	fout << "Converter:" << endl;
	fout << "    build: True" << endl;
	fout << "    Crystal-Nonuniformity: 0.05" << endl;
	fout << "    light-yield-effect: True" << endl;
	fout << "    E-Noise: True" << endl;
	fout << "\n"
		 << endl;

	fout << "#Construct Calorimeter" << endl;
	fout << "ECAL:" << endl;
	fout << "    nCryX: 5" << endl;
	fout << "    nCryY: 5" << endl;
	fout << "    ECALShield: True" << endl;
	fout << "    build: True" << endl;
	fout << "    Crystal-Nonuniformity: 0.02" << endl;
	fout << "    light-yield-effect: True" << endl;
	fout << "    light-yield: 1500" << endl;
	fout << "    E-Noise: True" << endl;
	fout << "    APD-Ionisation: True" << endl;
	fout << "    APD-distance: 5" << endl;
	fout << "    Digi: True" << endl;
	fout << "\n"
		 << endl;

	fout << "#Particle source setup" << endl;
	fout << "Source:" << endl;
	fout << "    isradio: False" << endl;
	fout << "    radio:" << endl;
	fout << "        Z: 27" << endl;
	fout << "        A: 60" << endl;
	fout << "    type: Point" << endl;
	fout << "    size: 15.0 # For Plane type source only (cm)" << endl;
	fout << "    angtype: planar" << endl;
	fout << "    particle: gamma" << endl;
	fout << "    energy: 100 # Unit is in MeV" << endl;
	fout << "    position: [0.0, 0.0, -289.0] # Unit is in mm #Converter upper plane -287.5 mm" << endl;
	fout << "    direction: [0.0, 0.0, 1.0]" << endl;
	fout << "\n"
		 << endl;

	fout << "#Verbose" << endl;
	fout << "Verbose:" << endl;
	fout << "    run: 0" << endl;
	fout << "    control: 0" << endl;
	fout << "    event: 0" << endl;
	fout << "    tracking: 0" << endl;

	fout.close();
}
