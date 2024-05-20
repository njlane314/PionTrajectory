#include "PionTrajectoryAnalyser.h"
//_________________________________________________________________________________________
ub_pion_trajectory::PionTrajectoryAnalyser::PionTrajectoryAnalyser(fhicl::ParameterSet const& p)
   : EDAnalyzer{p},
   m_config(ConfigManager::Initialise(p)) //,
   //m_dataHandler(std::make_unique<DataHandler>())
{}
//_________________________________________________________________________________________
void ub_pion_trajectory::PionTrajectoryAnalyser::analyze(art::Event const& e)
{
   // The properties of the stopped pions can be used to enhance pion identification
   // Also, measuring "decay-kinks", and the Michel electrons
   // Negative pions that are stopped and absorbed by nuclei give rise to softer nuclear emission
   // Lepton, pion-plus, and pion-minus signatures are different
   // Also, kinematic variables can be calculated to exclude unphysical kinematic combinations
}
//_________________________________________________________________________________________
void ub_pion_trajectory::PionTrajectoryAnalyser::beginJob()
{}
//_________________________________________________________________________________________
void ub_pion_trajectory::PionTrajectoryAnalyser::endJob()
{}
//_________________________________________________________________________________________
void ub_pion_trajectory::PionTrajectoryAnalyser::FinishEvent()
{}	
//_________________________________________________________________________________________
void ub_pion_trajectory::PionTrajectoryAnalyser::beginSubRun(const art::SubRun& sr)
{}
//_________________________________________________________________________________________
void ub_pion_trajectory::PionTrajectoryAnalyser::endSubRun(const art::SubRun& sr)
{}
//_________________________________________________________________________________________
DEFINE_ART_MODULE(ub_pion_trajectory::PionTrajectoryAnalyser)