//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geant4 Application: Tutorial course for Hep/Medicine Users
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"
#include "UserActionInitialization.hh"
#include <chrono>

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
//#include "Shielding.hh"
#include "FTFP_BERT.hh"

//-------------------------------------------------------------------------------
int main(int argc, char **argv)
//-------------------------------------------------------------------------------
{
// Construct the default run manager
#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  G4int nThreads = G4Threading::G4GetNumberOfCores();
  // Use all core - 2
  runManager->SetNumberOfThreads(nThreads - 2);
#else
  auto runManager = new G4RunManager;
#endif

  // Set up mandatory user initialization: Geometry
  runManager->SetUserInitialization(new Geometry);

  // Set up mandatory user initialization: Physics-List
  //  runManager->SetUserInitialization(new Shielding);
  runManager->SetUserInitialization(new FTFP_BERT);

  // Set up user initialization: User Actions
  runManager->SetUserInitialization(new UserActionInitialization);

  // Initialize G4 kernel
  runManager->Initialize();

  // Create visualization environment
  auto visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get UI (User Interface) manager
  G4UImanager *uiManager = G4UImanager::GetUIpointer();

  // Switch batch or interactive mode
  bool interactive_mode = argc == 1;
  //  bool interactive_mode = false;
  if (interactive_mode) { // Interactive mode - no command argument
    auto uiExec = new G4UIExecutive(argc, argv);
    // Enable analysis
    uiManager->ApplyCommand("/analysis/h1/set 0 1 0.65 0.67 MeV");
    uiManager->ApplyCommand("/analysis/setActivation 0");

    // Initialize
    uiManager->ApplyCommand("/control/execute init_setup.mac");

    // Start GUI
    uiExec->SessionStart();
    delete uiExec;
  } else { // Batch mode
    const std::string run_cmd = "/run/beamOn 10000";
    const double y_height = 5.0;
    auto setPosCmd = [&y_height](double x, double z) {
      return "/my_detector/position " + std::to_string(x) + " " +
             std::to_string(y_height) + " " + std::to_string(z) + " cm";
    };
    auto changeFnameCmd = [](int id) {
      std::ostringstream out;
      out << std::internal << std::setfill('0') << std::setw(3) << id;
      return "/analysis/setFileName detector_loc_" + out.str();
    };

    // Enable analysis
    uiManager->ApplyCommand("/analysis/setFileName detector_loc_000");
    uiManager->ApplyCommand("/analysis/h1/set 0 1 0.65 0.67 MeV");
    uiManager->ApplyCommand("/analysis/setActivation 0");

    // Initialize
    uiManager->ApplyCommand("/control/execute init_setup.mac");
    uiManager->ApplyCommand("/tracking/verbose 0");

    // Run
    // Change detector position
    //    uiManager->ApplyCommand("/analysis/setFileName detector_loc_001");
    //    uiManager->ApplyCommand("/my_detector/position 0 5 0 cm");
    //    uiManager->ApplyCommand("/my_detector/update");
    //    uiManager->ApplyCommand(run_cmd);
    std::ofstream pos_file;
    pos_file.open("detector_position.txt");
    int file_id = 0;
    const int MAX_I = 11;
    const int MAX_J = 11;
    const double factor = 2; // id to cm

    // Start time
    auto time_s = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < MAX_J; ++j) {
      for (int i = 0; i < MAX_I; ++i) {
        const double x_pos = factor * (i - (MAX_I - 1) / 2.0);
        const double z_pos = factor * (j - (MAX_J - 1) / 2.0);
        uiManager->ApplyCommand(changeFnameCmd(file_id));
        uiManager->ApplyCommand(setPosCmd(x_pos, z_pos));
        uiManager->ApplyCommand("/my_detector/update");
        uiManager->ApplyCommand(run_cmd);
        file_id++;
        // output position
        pos_file << x_pos << ", " << y_height << ", " << z_pos << "\n";
      }
    }
    pos_file.close();

    // Elapsed time
    auto elps = std::chrono::high_resolution_clock::now() - time_s;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elps);
    G4cout << "//================================================//\n"
           << "Total " << file_id
           << " run. Elapsed for one run: " << ms.count() / 1000.0 / file_id
           << "[s]\n"
           << "//================================================//\n";
  }

  // Job termination
  delete visManager;
  delete runManager;

  return 0;
}