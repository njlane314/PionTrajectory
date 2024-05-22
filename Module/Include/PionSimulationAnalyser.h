#ifndef PIONSIMULATIONANALYSER_H
#define PIONSIMULATIONANALYSER_H

#include <string>
#include <vector>

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
#include "nusimdata/SimulationBase/MCTrajectory.h"

namespace ubpiontraj 
{
   class PionSimulationAnalyser 
   {
   public:
      PionSimulationAnalyser(art::Event const& event, std::string simlabel);
      ~PionSimulationAnalyser();

      std::vector<simb::MCTrajectory> GetTrajectories() { return m_Trajectories; }
      std::vector<Scatter> GetScatters() { return m_Scatters; }
      FinalState GetFinalState() { return m_FinalState; }

   private:
      void AnalyseEvent(art::Event const& event);
      
      std::vector<simb::MCTrajectory> m_Trajectories;
      std::vector<Scatter> m_Scatters;
      FinalState m_FinalState;

      art::Handle<std::vector<simb::MCParticle>> m_SimHandle;
      std::string m_SimLabel;

      std::vector<art::Ptr<simb::MCParticle>> m_SimParticles;
      std::map<int, art::Ptr<simb::MCParticle>> m_SimParticleMap;

      std::vector<art::Ptr<simb::MCParticle>> FindDaughters(const art::Ptr<simb::MCParticle> particle);
   };
}

#endif  // PIONSIMULATIONANALYSER_H