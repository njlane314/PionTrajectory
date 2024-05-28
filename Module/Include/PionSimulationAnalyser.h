#ifndef PIONSIMULATIONANALYSER_H
#define PIONSIMULATIONANALYSER_H

#include <string>
#include <vector>

#include "ubana/PionTrajectory/Interface/Include/ParticleTypes.h"
#include "ubana/PionTrajectory/Objects/Include/Trajectory.h"

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
      PionSimulationAnalyser(art::Event const& event, std::string simparticlelabel, bool debug);
      ~PionSimulationAnalyser();

      void AnalyseEvent(art::Event const& event);

      Trajectory GetTrajectory() {return m_Trajectory;}

   private:
      art::Handle<std::vector<simb::MCParticle>> m_SimParticleHandle;
      std::string m_SimParticleLabel;
      
      bool m_Debug;

      std::vector<art::Ptr<simb::MCParticle>> m_SimParticles;
      std::map<int, art::Ptr<simb::MCParticle>> m_SimParticleMap;

      Trajectory m_Trajectory;

      std::vector<art::Ptr<simb::MCParticle>> FindDaughters(const art::Ptr<simb::MCParticle> particle);
      void FollowScatters(art::Ptr<simb::MCParticle> particle, art::Ptr<simb::MCParticle> scatteredparticle);
      void AnalyseTrajectory(const simb::MCTrajectory traj);
   };
}

#endif  // PIONSIMULATIONANALYSER_H