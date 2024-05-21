#include "Include/PionSimulationAnalyser.h"
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
PionSimulationAnalyser::PionSimulationAnalyser(art::Event const& event)
{
   m_ConfigManager = ConfigManager::GetInstance();

   if(!event.getByLabel(m_ConfigManager->getSimulationLabel(), m_SimHandle));

   art::fill_ptr_vector(m_SimParticles, m_SimHandle);

   for(const art::Ptr<simb::MCParticle> &particle : m_SimParticles){
      m_SimParticleMap.insert(std::make_pair(particle->TrackId(), particle));
   }

   AnalyseEvent(event);
}
//_________________________________________________________________________________________
void PionSimulationAnalyser::AnalyseEvent(art::Event const& event)
{  
   for(const art::Ptr<simb::MCParticle> &initialParticle : m_SimParticles){
      if(isChargedPion(initialParticle->PdgCode()) && initialParticle->Process() == "Primary"){
         DataHandler::GetInstance()->Reset();
         DataHandler::GetInstance()->AddTrajectory(initialParticle);

         bool pionScattering = false;
         art::Ptr<simb::MCParticle> particle = initialParticle; 
         while(!pionScattering){  
            follow_scatter: 

            std::vector<art::Ptr<simb::MCParticle>> daughters = FindDaughters(particle);
            for(const auto& daughter : daughters){
               if(!isChargedPion(daughter->PdgCode())) continue;

               if(daughter->Process() == "hadElastic"){
                  Scatter scatter(true, false, particle, daughter, daughters);
                  DataHandler::GetInstance()->AddScatter(scatter);

                  particle = daughter; 
                  DataHandler::GetInstance()->AddTrajectory(particle);
                  goto follow_scatter;
               }
               else if(daughter->Process() == "Inelastic"){
                  Scatter scatter(false, true, particle, daughter, daughters);
                  DataHandler::GetInstance()->AddScatter(scatter);

                  particle = daughter;
                  DataHandler::GetInstance()->AddTrajectory(particle);
                  goto follow_scatter;
               }
            }
         }

         std::vector<art::Ptr<simb::MCParticle>> products;
         for(const auto finalParticle : FindDaughters(particle)){
            if((finalParticle->PdgCode() == 11 && finalParticle->Process() == "hIoni") || finalParticle->Process() == "hadElastic")
                  continue;
            
            products.push_back(finalParticle); 
         }

         FinalState finalState(particle, products);
         DataHandler::GetInstance()->AddFinalState(finalState);
         DataHandler::GetInstance()->AddEntry(); 
      }
   }
}
//_________________________________________________________________________________________
std::vector<art::Ptr<simb::MCParticle>> PionSimulationAnalyser::FindDaughters(const art::Ptr<simb::MCParticle> particle)
{
   std::vector<art::Ptr<simb::MCParticle>> daughters;

   for(int i = 0; i < particle->NumberDaughters(); i++){
      if(m_SimParticleMap.find(particle->Daughter(i)) == m_SimParticleMap.end()) continue;

      daughters.push_back(m_SimParticleMap[particle->Daughter(i)]);
   }

   return daughters;
}
//_________________________________________________________________________________________
}