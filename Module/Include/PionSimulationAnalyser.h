#ifndef PIONSIMULATIONANALYSER_H
#define PIONSIMULATIONANALYSER_H

#include <string>
#include <vector>

#include "art/Framework/Principal/Event.h"

#include "nusimdata/SimulationBase/MCParticle.h"

#include "ubana/PionTrajectory/Interface/Include/DataHandler.h"
#include "ubana/PionTrajectory/Interface/Include/ConfigManager.h"
#include "ubana/PionTrajectory/Interface/Include/ParticleTypes.h"

namespace ubpiontraj 
{
   class PionSimulationAnalyser 
   {
   public:
      PionSimulationAnalyser(art::Event const& event);
      ~PionSimulationAnalyser();

   private:
      ConfigManager* m_ConfigManager; 
      std::vector<art::Ptr<simb::MCParticle>> m_SimParticles;
      std::vector<std::pair<int, <art::Ptr<simb::MCParticle>>>> m_SimParticleMap;

      AnalyseEvent(art::Event const& event);

      std::vector<art::Ptr<simb::MCParticle>> FindDaughters(const art::Ptr<simb::MCParticle> particle);
   };
}

#endif  // PIONSIMULATIONANALYSER_H