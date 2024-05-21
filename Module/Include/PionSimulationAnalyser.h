#ifndef PIONSIMULATIONANALYSER_H
#define PIONSIMULATIONANALYSER_H

#include <string>
#include <vector>

#include "ubana/PionTrajectory/Interface/Include/DataHandler.h"
#include "ubana/PionTrajectory/Interface/Include/ConfigManager.h"
#include "ubana/PionTrajectory/Interface/Include/ParticleTypes.h"
#include "ubana/PionTrajectory/Objects/FinalState.h"
#include "ubana/PionTrajectory/Objects/Scatter.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Persistency/Common/FindMany.h"		
#include "nusimdata/SimulationBase/MCParticle.h"

namespace ubpiontraj 
{
   class PionSimulationAnalyser 
   {
   public:
      PionSimulationAnalyser(art::Event const& event);
      ~PionSimulationAnalyser();

      void AnalyseEvent(art::Event const& event);

   private:
      ConfigManager* m_ConfigManager; 

      art::Handle<std::vector<simb::MCParticle>> m_SimHandle;
      std::vector<art::Ptr<simb::MCParticle>> m_SimParticles;

      std::map<int, art::Ptr<simb::MCParticle>> m_SimParticleMap;

      std::vector<art::Ptr<simb::MCParticle>> FindDaughters(const art::Ptr<simb::MCParticle> particle);
   };
}

#endif  // PIONSIMULATIONANALYSER_H