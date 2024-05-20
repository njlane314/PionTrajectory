#include "ubana/NeutralKaonProduction/Analyser/Include/PionSimulationAnalyser.h"
//_________________________________________________________________________________________
namespace ub_pion_trajectory {
//_________________________________________________________________________________________
PionSimulationAnalyser::PionSimulationAnalyser(art::Event const& event) :
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
      if(isPion(particle->PdgCode()) && particle->Process() == "Primary"){
         bool pionScattering = false;
         while(!pionScatering){  
            follow_scatter: 
            std::vector<art::Ptr<simb::MCParticle>> daughters;

            for(int i = 0; i < particle->NumberDaughters(); i++){
               if(m_SimParticleMap.find(particle->Daughter(i)) == m_SimParticleMap.end()) continue;

               daughters.push_back(m_SimParticleMap[particle->Daughter(i)]);
            }

            for(const auto daughter : daughters){
               if(!isPion(daughter->PdgCode())) continue;

               if(daughter->Process() == "hadElastic"){
                  Scatter scatter(true, false, particle, daughter, daughters);
                  DataHandler::GetInstance->AddScatter(scatter);

                  particle = daughter;
                  goto follow_scatter;
               }
               else if(daughter->Process() == "Inelastic"){
                  Scatter scatter(false, true, particle, daughter, daughters);
                  DataHandler::GetInstance->AddScatter(scatter);

                  particle = daughter;
                  goto follow_scatter;
               }
            }
         }

         std::vector<art::Ptr<simb::MCParticle>> finalState;

         for(int i = 0; i < particle->NumberDaughters(); i++){
            if(m_SimParticleMap.find(particle->Daughter(i)) == m_SimParticleMap.end()) continue;

            finalState.push_back(m_SimParticleMap[particle->Daughter(i)]);
         }

         for(const auto finalParticle : finalState){
            std::vector<art::Ptr<MCParticle>> products;
            if((finalParticle->PdgCode() == 11 && finalParticle->Process() == "hIoni") || finalParticle->Process() == "hadElastic")
                  continue;
            
            products.push_back(finalParticle); 
         }

         FinalState finalState();
         DataHandler::GetInstance->AddFinalState(); 
      }
   }
}
//_________________________________________________________________________________________
std::vector<art::Ptr<simb::MCParticle>> PionSimulationAnalyser::FindDaughters(const art::Ptr<simb::MCParticle> particle)
{
   

   return daughters;
}
//_________________________________________________________________________________________
}