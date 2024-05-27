#include "Include/PionSimulationAnalyser.h"
#include <iostream>
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
PionSimulationAnalyser::PionSimulationAnalyser(art::Event const& event, std::string simlabel, bool debug) :
   m_SimLabel(simlabel),
   m_Debug(debug)
{
   if (m_Debug) {
       std::cout << ">>> [PionSimulationAnalyser] Initializing with simulation label: " << simlabel << std::endl;
   }

   if (!event.getByLabel(m_SimLabel, m_SimHandle)) {
       if (m_Debug) {
           std::cerr << ">>> [PionSimulationAnalyser] Error: Could not get simulation handle for label: " << m_SimLabel << std::endl;
       }
       return;
   }

   art::fill_ptr_vector(m_SimParticles, m_SimHandle);

   for (const art::Ptr<simb::MCParticle>& initialparticle : m_SimParticles) {
      m_SimParticleMap.insert(std::make_pair(initialparticle->TrackId(), initialparticle));
   }

   if (m_Debug) {
       std::cout << ">>> [PionSimulationAnalyser] Simulation particles and initialparticle map initialized. Total particles: " << m_SimParticles.size() << std::endl;
   }
}
//_________________________________________________________________________________________
PionSimulationAnalyser::~PionSimulationAnalyser() 
{}
//_________________________________________________________________________________________
void PionSimulationAnalyser::AnalyseEvent(art::Event const& event)
{  
   std::cout << "Analysing event..." << std::endl;
   art::Ptr<simb::MCParticle> initialparticle;

   bool pionfound = false;
   for (art::Ptr<simb::MCParticle>& particle : m_SimParticles) {
      if (isChargedPion(particle->PdgCode()) && particle->Process() == "primary") {
         initialparticle = particle;
         pionfound = true;
         break;
      }
   }

   if (!pionfound) {
       std::cerr << ">>> [PionSimulationAnalyser] No primary charged pion found." << std::endl;
       return;
   }

   art::Ptr<simb::MCParticle> scatteredparticle;
   FollowScatters(initialparticle, scatteredparticle);

   std::cout << "Finished analysing event!" << std::endl;
}
//_________________________________________________________________________________________
void PionSimulationAnalyser::FollowScatters(art::Ptr<simb::MCParticle> particle, art::Ptr<simb::MCParticle> scatteredparticle)
{
   m_Trajectory.AddSimParticle(particle);
   const simb::MCTrajectory traj = particle->Trajectory();
   AnalyseTrajectory(traj); 

   scatteredparticle = particle;

   art::Ptr<simb::MCParticle> finalparticle;
   std::vector<art::Ptr<simb::MCParticle>> daughters = FindDaughters(particle);
   for(auto& daughter : daughters){
      if(daughter->PdgCode() == scatteredparticle->PdgCode()){
         std::cout << daughter->Process() << std::endl;
         finalparticle = daughter;
         break;
      }
   }

   if(finalparticle.isNull()){
      return;
   }

   FollowScatters(finalparticle, scatteredparticle);
}
//_________________________________________________________________________________________
void ubpiontraj::PionSimulationAnalyser::AnalyseTrajectory(const simb::MCTrajectory traj)
{
   for (size_t i = 0; i < traj.size(); ++i) {
      m_Trajectory.AddPoint(traj.Position(i), traj.Momentum(i), "");
   }

   std::vector<std::pair<size_t, unsigned char>> processmap = traj.TrajectoryProcesses();

   for (const auto& process : processmap) {
      size_t stepIndex = process.first;
      unsigned char processKey = process.second;
      std::string processName = traj.KeyToProcess(processKey);
      m_Trajectory.SetProcess(stepIndex, processName);
   }
}
//_________________________________________________________________________________________
std::vector<art::Ptr<simb::MCParticle>> PionSimulationAnalyser::FindDaughters(const art::Ptr<simb::MCParticle> initialparticle)
{
   std::vector<art::Ptr<simb::MCParticle>> daughters;

   for (int i = 0; i < initialparticle->NumberDaughters(); i++) {
      if (m_SimParticleMap.find(initialparticle->Daughter(i)) == m_SimParticleMap.end()) continue;

      daughters.push_back(m_SimParticleMap[initialparticle->Daughter(i)]);
   }

   if (m_Debug) {
       std::cout << ">>> [PionSimulationAnalyser] Found " << daughters.size() << " daughters for initialparticle with TrackId: " << initialparticle->TrackId() << std::endl;
   }

   return daughters;
}
//_________________________________________________________________________________________
}