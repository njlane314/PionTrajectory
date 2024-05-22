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

   m_ScatTree = tfs->make<TTree>("ScatTree", "Scatter Tree");
   m_ScatTree->Branch("scat_elas", &m_scat_elas);
   m_ScatTree->Branch("scat_inelas", &m_scat_inelas);
   m_ScatTree->Branch("scat_pfls", &m_scat_pfls);
   m_ScatTree->Branch("scat_thta", &m_scat_thta);

   m_FinSttTree = tfs->make<TTree>("FinalStateTree", "Final State Tree");
   m_FinSttTree->Branch("fstt_typ", &m_fstt_typ);
   m_FinSttTree->Branch("fstt_nprd", &m_fstt_nprd);
   m_FinSttTree->Branch("fstt_prdpdg", &m_fstt_prdpdg);
   
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
       std::cout << ">>> [PionTrajectoryAnalyser] Analyzing event: " << e.id() << std::endl;
   }

   PionSimulationAnalyser* piSimAna = new PionSimulationAnalyser(e, m_SimLabel, m_Debug);
   piSimAna->AnalyseEvent(e);

   std::vector<simb::MCTrajectory> trajectories = piSimAna->GetTrajectories();
   std::vector<ubpiontraj::Scatter> scatters = piSimAna->GetScatters();
   ubpiontraj::FinalState finalstate = piSimAna->GetFinalState();

   delete piSimAna;

   m_traj_n.clear(); 
   m_traj_x.clear();
   m_traj_y.clear();
   m_traj_z.clear();
   m_traj_px.clear();
   m_traj_py.clear();
   m_traj_pz.clear();
   m_traj_e.clear();

   m_scat_elas.clear();
   m_scat_inelas.clear();
   m_scat_pfls.clear();
   m_scat_thta.clear();

   m_fstt_typ = FinalState::None;
   m_fstt_nprd = 0;
   m_fstt_prdpdg.clear();

   for (simb::MCTrajectory traj : trajectories) {
      std::vector<double> traj_x, traj_y, traj_z, traj_px, traj_py, traj_pz, traj_e;
      int traj_n = traj.size(); 
      if (m_Debug) {
          std::cout << ">>> [PionTrajectoryAnalyser] Processing trajectory with " << traj_n << " points." << std::endl;
      }

      for (int i = 0; i < traj_n; i++) {
         traj_x.push_back(traj.X(i));
         traj_y.push_back(traj.Y(i));
         traj_z.push_back(traj.Z(i));
         traj_px.push_back(traj.Px(i));
         traj_py.push_back(traj.Py(i));
         traj_pz.push_back(traj.Pz(i));
         traj_e.push_back(traj.E(i));
      }

      m_traj_n.push_back(traj_n);
      m_traj_x.push_back(traj_x);
      m_traj_y.push_back(traj_y);
      m_traj_z.push_back(traj_z);
      m_traj_px.push_back(traj_px);
      m_traj_py.push_back(traj_py);
      m_traj_pz.push_back(traj_pz);
      m_traj_e.push_back(traj_e);

      if (m_Debug) {
          std::cout << ">>> [PionTrajectoryAnalyser] Trajectory filled with " << traj_n << " points." << std::endl;
      }
   }

   for (Scatter scat : scatters) {
      m_scat_elas.push_back(scat.isElastic());
      m_scat_inelas.push_back(scat.isInelastic());

      m_scat_pfls.push_back(scat.MomentumFractionLoss());
      m_scat_thta.push_back(scat.CosTheta());

      if (m_Debug) {
          std::cout << ">>> [PionTrajectoryAnalyser] Scatter filled: "
                    << "Elastic: " << scat.isElastic()
                    << ", Inelastic: " << scat.isInelastic()
                    << ", MomentumFractionLoss: " << scat.MomentumFractionLoss()
                    << ", CosTheta: " << scat.CosTheta() << std::endl;
      }
   }

   int type;
   switch (finalstate.getType()) {
      case FinalState::DecayToMuon:
         type = 1;
         break;
      case FinalState::InelasticAbsorption:
         type = 2;
         break;
      case FinalState::NeutralPionChargeExchange:
         type = 3;
         break;
      case FinalState::Other: 
         type = 0;
         break;
      case FinalState::None:
      default:
         type = -1;
         break;
   }
    
   m_fstt_typ = type;
   m_fstt_nprd = finalstate.getProducts().size();

   std::vector<int> prod_pdgs;
   for (const auto& product : finalstate.getProducts()) {
      m_fstt_prdpdg.push_back(product->PdgCode());
      if (m_Debug) {
          std::cout << ">>> [PionTrajectoryAnalyser] Final state product PDG: " << product->PdgCode() << std::endl;
      }
   }
   
   if (m_Debug) {
       std::cout << ">>> [PionTrajectoryAnalyser] Final state type: " << type << ", Number of products: " << m_fstt_nprd << std::endl;
   }
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::FinishEvent()
{
   if (m_Debug) {
       std::cout << ">>> [PionTrajectoryAnalyser] Filling trees for the event." << std::endl;
   }
   m_TrajTree->Fill();
   m_ScatTree->Fill();
   m_FinSttTree->Fill();
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