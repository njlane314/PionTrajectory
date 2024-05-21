#include "PionTrajectoryAnalyser.h"
//_________________________________________________________________________________________
ubpiontraj::PionTrajectoryAnalyser::PionTrajectoryAnalyser(fhicl::ParameterSet const& p) : 
   EDAnalyzer{p}
{
   m_config = ConfigManager::GetInstance(p);
   m_dataHandler = DataHandler::GetInstance();
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::analyze(art::Event const& e)
{
   // The properties of the stopped pions can be used to enhance pion identification
   // Also, measuring "decay-kinks", and the Michel electrons
   // Negative pions that are stopped and absorbed by nuclei give rise to softer nuclear emission
   // Lepton, pion-plus, and pion-minus signatures are different
   // Also, kinematic variables can be calculated to exclude unphysical kinematic combinations
}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::beginJob()
{}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::endJob()
{}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::FinishEvent()
{}	
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::beginSubRun(const art::SubRun& sr)
{}
//_________________________________________________________________________________________
void ubpiontraj::PionTrajectoryAnalyser::endSubRun(const art::SubRun& sr)
{}
//_________________________________________________________________________________________
DEFINE_ART_MODULE(ubpiontraj::PionTrajectoryAnalyser)