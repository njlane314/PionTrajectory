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

   for (const art::Ptr<simb::MCParticle>& particle : m_SimParticles) {
      m_SimParticleMap.insert(std::make_pair(particle->TrackId(), particle));
   }

   if (m_Debug) {
       std::cout << ">>> [PionSimulationAnalyser] Simulation particles and particle map initialized. Total particles: " << m_SimParticles.size() << std::endl;
   }
}
//_________________________________________________________________________________________
PionSimulationAnalyser::~PionSimulationAnalyser() 
{}
//_________________________________________________________________________________________
void PionSimulationAnalyser::AnalyseEvent(art::Event const& event)
{  
   if (m_Debug) {
       std::cout << "///----------------------------------------" << std::endl;
       std::cout << ">>> [PionSimulationAnalyser] Analyzing event: " << event.id() << std::endl;
   }

   std::vector<art::Ptr<simb::MCParticle>> pionParticles;

   for (const art::Ptr<simb::MCParticle>& particle : m_SimParticles) {
      if (isChargedPion(particle->PdgCode()) && particle->Mother() != 0) {
         pionParticles.push_back(particle);
      }
   }

   if(m_Debug) {
      std::cout << ">>> [PionSimulationAnalyser] Primary pions found: " << pionParticles.size() << std::endl;
   }

   if(pionParticles.size() == 0) {
      std::cout << ">>> [PionSimulationAnalyser] Exiting AnalyseEvent function." << std::endl;
      std::cout << "///----------------------------------------" << std::endl;
      return;
   }

   const art::Ptr<simb::MCParticle>& initialPion = pionParticles.front();  // Get the first primary pion
   m_Trajectories.push_back(initialPion->Trajectory());

   if (m_Debug) {
      std::cout << ">>> [PionSimulationAnalyser] Primary charged pion found with PDG code: " << initialPion->PdgCode() << std::endl;
   }

   art::Ptr<simb::MCParticle> particle = initialPion; 
   bool pionscattering = true;
   while (pionscattering) {  
      std::vector<art::Ptr<simb::MCParticle>> daughters = FindDaughters(particle);
      bool scatterFound = false;

      for (const auto& daughter : daughters) {
         if (!isChargedPion(daughter->PdgCode())) continue;

         if (daughter->Process() == "hadElastic") {
            Scatter scatter(true, false, particle, daughter, daughters);

            m_Scatters.push_back(scatter);
            m_Trajectories.push_back(particle->Trajectory());

            if (m_Debug) {
                std::cout << ">>> [PionSimulationAnalyser] Elastic scatter found. Adding trajectory and scatter." << std::endl;
            }

            particle = daughter; 
            scatterFound = true;
            break;
         }
         else if (daughter->Process() == "Inelastic") {
            Scatter scatter(false, true, particle, daughter, daughters);
            m_Scatters.push_back(scatter);

            m_Trajectories.push_back(particle->Trajectory());

            if (m_Debug) {
                std::cout << ">>> [PionSimulationAnalyser] Inelastic scatter found. Adding trajectory and scatter." << std::endl;
            }

            particle = daughter;
            scatterFound = true;
            break;
         }
      }

      if (!scatterFound) {
         pionscattering = false;
      }
   }

   std::vector<art::Ptr<simb::MCParticle>> products;
   for (const auto finalParticle : FindDaughters(particle)) {
      if ((finalParticle->PdgCode() == 11 && finalParticle->Process() == "hIoni") || finalParticle->Process() == "hadElastic")
            continue;
      
      products.push_back(finalParticle); 

      if (m_Debug) {
            std::cout << ">>> [PionSimulationAnalyser] Final state product added with PDG code: " << finalParticle->PdgCode() << std::endl;
      }
   }

   FinalState finalState(particle, products);
   m_FinalState = finalState;

   if (m_Debug) {
         std::cout << ">>> [PionSimulationAnalyser] Final state determined." << std::endl;
   }
         
   if (m_Debug) {
       std::cout << ">>> [PionSimulationAnalyser] Event analysis complete." << std::endl;
       std::cout << "///----------------------------------------" << std::endl;
   }
}
//_________________________________________________________________________________________
std::vector<art::Ptr<simb::MCParticle>> PionSimulationAnalyser::FindDaughters(const art::Ptr<simb::MCParticle> particle)
{
   std::vector<art::Ptr<simb::MCParticle>> daughters;

   for (int i = 0; i < particle->NumberDaughters(); i++) {
      if (m_SimParticleMap.find(particle->Daughter(i)) == m_SimParticleMap.end()) continue;

      daughters.push_back(m_SimParticleMap[particle->Daughter(i)]);
   }

   if (m_Debug) {
       std::cout << ">>> [PionSimulationAnalyser] Found " << daughters.size() << " daughters for particle with TrackId: " << particle->TrackId() << std::endl;
   }

   return daughters;
}
//_________________________________________________________________________________________
}