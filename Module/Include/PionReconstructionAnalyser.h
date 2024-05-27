#ifndef PIONRECONSTRUCTIONANALYSER_H
#define PIONRECONSTRUCTIONANALYSER_H

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

#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/PFParticleMetadata.h"
#include "lardataobj/AnalysisBase/BackTrackerMatchingData.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "larevt/SpaceChargeServices/SpaceChargeService.h"

namespace ubpiontraj 
{
   class PionReconstructionAnalyser 
   {
   public:
      PionReconstructionAnalyser(art::Event const& event, ubpiontraj::Trajectory traj, 
                                    std::string recotracklabel, std::string recotrackhitlabel, std::string recohitsimparticlelabel, std::string recocalolabel, 
                                    bool debug);
      ~PionReconstructionAnalyser();

      void AnalyseEvent(art::Event const& event);

   private:
      ubpiontraj::Trajectory m_Trajectory;
      const std::vector<art::Ptr<simb::MCParticle>>& m_SelectedSimParticles;
      
      std::string m_RecoTrackLabel;
      std::string m_RecoTrackHitLabel;
      std::string m_RecoHitSimParticleLabel;
      std::string m_RecoCaloLabel;
      bool m_Debug;

      art::Handle<std::vector<recob::PFParticle>> m_RecoParticleHandle;
      std::vector<art::Ptr<recob::PFParticle>> m_RecoParticles;

      art::Handle<std::vector<recob::Track>> m_RecoTrackHandle;
      std::vector<art::Ptr<recob::Track>> m_RecoTracks;

      art::Handle<std::vector<recob::Hit>> m_RecoHitHandle;
      std::vector<art::Ptr<recob::Hit>> m_RecoHits;

      art::FindManyP<recob::Track>* m_RecoParticleTrackAssoc;
      art::FindManyP<recob::Hit>* m_RecoTrackHitAssoc;
      art::FindMany<simb::MCParticle, anab::BackTrackerHitMatchingData>* m_RecoHitSimParticleAssoc;
      art::FindManyP<anab::Calorimetry>* m_RecoTrackCaloAssoc;
   };
}

#endif  // PIONRECONSTRUCTIONANALYSER_H