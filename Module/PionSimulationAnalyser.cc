#include "Include/PionSimulationAnalyser.h"
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
PionSimulationAnalyser::PionSimulationAnalyser(art::Event const& event)
{
   m_ConfigManager = ConfigManager::GetInstance();

   if(!event.getByLabel(m_ConfigManager.getSimulationLabel(), m_SimHandle));

   art::fill_ptr_vector(m_SimParticles, m_SimHandle);

   for(const art::Ptr<simb::MCParticle> &particle : m_SimParticles){
      m_SimParticleMap.insert(std::make_pair(particle->TrackId(), particle));
   }

   AnalyseEvent(event);
}
//_________________________________________________________________________________________
void PionSimulationAnalyser::AnalyseEvent(art::Event const& event)
{  
   for(const art::Ptr<simb::MCParticle> &particle : m_SimParticles){
      if(isChargedPion(particle->PdgCode()) && particle->Process() == "Primary"){
         DataHandler::GetInstance()->Reset();
         DataHandler::GetInstance()->AddTrajectory(particle);

         bool pionScattering = false;
         while(!pionScattering){  
            follow_scatter: 
            
            for(const auto daughter : FindDaughters(particle)){
               if(!isChargedPion(daughter->PdgCode())) continue;

               if(daughter->Process() == "hadElastic"){
                  Scatter scatter(true, false, particle, daughter, daughters);
                  DataHandler::GetInstance->AddScatter(scatter);

                  particle = daughter;
                  DataHandler::GetInstance()->AddTrajectory(particle);
                  goto follow_scatter;
               }
               else if(daughter->Process() == "Inelastic"){
                  Scatter scatter(false, true, particle, daughter, daughters);
                  DataHandler::GetInstance->AddScatter(scatter);

                  particle = daughter;
                  DataHandler::GetInstance()->AddTrajectory(particle);
                  goto follow_scatter;
               }
            }
         }

         for(const auto finalParticle : FindDaughters(particle)){
            std::vector<art::Ptr<simb::MCParticle>> products;
            if((finalParticle->PdgCode() == 11 && finalParticle->Process() == "hIoni") || finalParticle->Process() == "hadElastic")
                  continue;
            
            products.push_back(finalParticle); 
         }

         finalState FinalState::FinalState(particle, products);
         DataHandler::GetInstance->AddFinalState(finalState);
         DataHandler::GetInstance->AddEntry(); 
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