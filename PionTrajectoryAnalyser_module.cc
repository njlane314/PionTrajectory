#include "PionTrajectoryAnalyser.h"
//_________________________________________________________________________________________
ubpiontraj::PionTrajectoryAnalyser::PionTrajectoryAnalyser(fhicl::ParameterSet const& p) : 
   EDAnalyzer{p},
   m_SimLabel(p.get<std::string>("sim_label", "largeant")),
   m_Debug(p.get<bool>("debug", false))
{}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::beginJob()
{
   art::ServiceHandle<art::TFileService> tfs;

   m_TrajTree = tfs->make<TTree>("TrajTree", "Trajectory Tree");
   m_TrajTree->Branch("traj_n", &m_traj_n);
   m_TrajTree->Branch("traj_x", &m_traj_x);
   m_TrajTree->Branch("traj_y", &m_traj_y);
   m_TrajTree->Branch("traj_z", &m_traj_z);
   m_TrajTree->Branch("traj_px", &m_traj_px);
   m_TrajTree->Branch("traj_py", &m_traj_py);
   m_TrajTree->Branch("traj_pz", &m_traj_pz);
   m_TrajTree->Branch("traj_e", &m_traj_e);
   m_TrajTree->Branch("traj_proc", &m_traj_proc);

   if (m_Debug) {
       std::cout << ">>> [PionTrajectoryAnalyser] Trees for Trajectory, Scatter, and Final State have been created and branches set up." << std::endl;
   }
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::endJob()
{
    if (m_Debug) {
        std::cout << ">>> [PionTrajectoryAnalyser] endJob called. Trees should be written to the output file." << std::endl;
    }
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::analyze(art::Event const& e)
{
   if (m_Debug) {
      std::cout << "///----------------------------------------" << std::endl;
      std::cout << ">>> [PionTrajectoryAnalyser] Analyzing event: " << e.id() << std::endl;
   }

   PionSimulationAnalyser* piSimAna = new PionSimulationAnalyser(e, m_SimLabel, m_Debug);
   piSimAna->AnalyseEvent(e);

   ubpiontraj::Trajectory trajectory = piSimAna->GetTrajectory();

   m_traj_n = 0;
   m_traj_x.clear();
   m_traj_y.clear();
   m_traj_z.clear();
   m_traj_px.clear();
   m_traj_py.clear();
   m_traj_pz.clear();
   m_traj_e.clear();
   m_traj_proc.clear();

   std::vector<TLorentzVector> positions = trajectory.GetPositions();
   std::vector<TLorentzVector> momenta = trajectory.GetMomenta();
   std::vector<double> energies = trajectory.GetEnergies();
   std::vector<std::string> processes = trajectory.GetProcesses();

   m_traj_n = positions.size();

   if (m_Debug) {
      std::cout << ">>> [PionTrajectoryAnalyser] Processing trajectory with " << m_traj_n << " points." << std::endl;
   }

   for (int i = 0; i < m_traj_n; i++) {
      m_traj_x.push_back(positions[i].X());
      m_traj_y.push_back(positions[i].Y());
      m_traj_z.push_back(positions[i].Z());
      m_traj_px.push_back(momenta[i].Px());
      m_traj_py.push_back(momenta[i].Py());
      m_traj_pz.push_back(momenta[i].Pz());
      m_traj_e.push_back(energies[i]);
      m_traj_proc.push_back(processes[i]);
   }

   if (m_Debug) {
       std::cout << ">>> [PionTrajectoryAnalyser] Filling trees for the event..." << std::endl;
   }

   m_TrajTree->Fill();

   if (m_Debug) {
       std::cout << ">>> [PionTrajectoryAnalyser] Trees filled!" << std::endl;
   }

   delete piSimAna;
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::beginSubRun(const art::SubRun& sr)
{
    if (m_Debug) {
        std::cout << ">>> [PionTrajectoryAnalyser] beginSubRun called for subrun: " << sr.id() << std::endl;
    }
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::endSubRun(const art::SubRun& sr)
{
    if (m_Debug) {
        std::cout << ">>> [PionTrajectoryAnalyser] endSubRun called for subrun: " << sr.id() << std::endl;
    }
}
//_________________________________________________________________________________________
DEFINE_ART_MODULE(ubpiontraj::PionTrajectoryAnalyser)