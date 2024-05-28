#include "Include/PionReconstructionAnalyser.h"
#include <iostream>
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
PionReconstructionAnalyser::PionReconstructionAnalyser(art::Event const& event, 
                                                      std::string recoparticlelabel, std::string recotracklabel, std::string recohitlabel, std::string recotrackhitassoclabel, std::string recohitsimparticlelabel, std::string recocalolabel, 
                                                      bool debug) :
   m_RecoParticleLabel(recoparticlelabel),
   m_RecoTrackLabel(recotracklabel),
   m_RecoHitLabel(recohitlabel),
   m_RecoTrackHitAssocLabel(recotrackhitassoclabel),
   m_RecoHitSimParticleLabel(recohitsimparticlelabel),
   m_RecoCaloLabel(recocalolabel),
   m_Debug(debug)
{
   if(!event.getByLabel(m_RecoParticleLabel, m_RecoParticleHandle)) 
      throw cet::exception("PionReconstructionAnalyser") << "No PFParticle Data Products Found!" << std::endl;

   if(!event.getByLabel(m_RecoTrackLabel, m_RecoTrackHandle))
      throw cet::exception("PionReconstrucionAnalyser") << "No Track Data Products Found!" << std::endl;

   if(!event.getByLabel(m_RecoHitLabel, m_RecoHitHandle)) 
      throw cet::exception("PionReconstructionAnalyser") << "No Track Data Products Found!" << std::endl;

   art::fill_ptr_vector(m_RecoParticles, m_RecoParticleHandle);
   art::fill_ptr_vector(m_RecoTracks, m_RecoTrackHandle);
   art::fill_ptr_vector(m_RecoHits, m_RecoHitHandle);

   if (m_Debug) {
       std::cout << ">>> [PionReconstructionAnalyser] Filled RecoParticles, RecoTracks, RecoHits vectors." << std::endl;
   }

   m_RecoParticleTrackAssoc = new art::FindManyP<recob::Track>(m_RecoParticles, event, m_RecoParticleLabel);     
   m_RecoTrackHitAssoc = new art::FindManyP<recob::Hit>(m_RecoTracks, event, m_RecoTrackHitAssocLabel);

   m_RecoHitSimParticleAssoc = new art::FindMany<simb::MCParticle, anab::BackTrackerHitMatchingData>(m_RecoHitHandle, event, m_RecoHitSimParticleLabel);
   m_RecoTrackCaloAssoc = new art::FindManyP<anab::Calorimetry>(m_RecoTracks, event, m_RecoCaloLabel);

   if (m_Debug) {
       std::cout << ">>> [PionReconstructionAnalyser] Initialized associations for particles, tracks, hits, and calorimetry." << std::endl;
   }
}
//_________________________________________________________________________________________
PionReconstructionAnalyser::~PionReconstructionAnalyser() 
{}
//_________________________________________________________________________________________
void PionReconstructionAnalyser::AnalyseTrajectory(ubpiontraj::Trajectory traj)
{  
   std::cout << ">>> [PionReconstructionAnalyser] Analysing trajectory..." << std::endl;
   m_Trajectory = traj;
   const std::vector<art::Ptr<simb::MCParticle>>& selectecSimParticles = traj.GetSimParticles();

   for(const art::Ptr<recob::PFParticle>& particle : m_RecoParticles){
      std::vector<art::Ptr<recob::Track>> tracks = m_RecoParticleTrackAssoc->at(particle.key());

      for(const art::Ptr<recob::Track>& track : tracks){
         std::vector<art::Ptr<recob::Hit>> hits = m_RecoTrackHitAssoc->at(track.key());
         std::vector<art::Ptr<anab::Calorimetry>> planesCalo = m_RecoTrackCaloAssoc->at(track.key());

         std::unordered_map<int, double> trackMap;
         int maxHits = -1;
         simb::MCParticle const* matchedSimParticle = nullptr;
         std::vector<simb::MCParticle const*> simParticles;
         std::vector<anab::BackTrackerHitMatchingData const*> matchingHits;

         for(size_t h = 0; h < hits.size(); ++h){
            simParticles.clear();
            matchingHits.clear();
            m_RecoHitSimParticleAssoc->get(hits[h].key(), simParticles, matchingHits);

            for(size_t p = 0; p < simParticles.size(); ++p){
               trackMap[simParticles[p]->TrackId()]++; 

               if(trackMap[simParticles[p]->TrackId()] > maxHits){
                  maxHits = trackMap[simParticles[p]->TrackId()];
                  matchedSimParticle = simParticles[p];
               }
            }
         }

         if (matchedSimParticle != nullptr){
            auto it = std::find_if(selectecSimParticles.begin(), selectecSimParticles.end(), 
                                    [&](const art::Ptr<simb::MCParticle>& ptr) { 
                                       return ptr.get() == matchedSimParticle; 
                                    });
            if(it != selectecSimParticles.end()) {
               if (m_Debug) {
                   std::cout << ">>> [PionReconstructionAnalyser] Matched sim particle with TrackId: " 
                             << matchedSimParticle->TrackId() << " found in selected sim particles." << std::endl;
               }

               std::vector<std::pair<TVector3, double>> xyz_dEdx;

               for(size_t plane = 0; plane < planesCalo.size(); plane++){
                  int planeType = planesCalo.at(plane)->PlaneID().Plane;
                  if(planeType != 0 && planeType != 1 && planeType != 2) continue;        

                  art::Ptr<anab::Calorimetry> calo = planesCalo.at(plane);

                  if(calo->XYZ().size() < 2) continue;

                  for(size_t sp = 0; sp < calo->XYZ().size(); sp++){
                     const auto& pos = calo->XYZ().at(sp);
                     TVector3 xyz(pos.X(), pos.Y(), pos.Z());
                     double dEdx = calo->dEdx().at(sp);

                     xyz_dEdx.emplace_back(xyz, dEdx);
                  }
               }

               if (!xyz_dEdx.empty()) {
                  std::vector<std::pair<TVector3, double>> sorted_xyz_dEdx;
                  sorted_xyz_dEdx.push_back(xyz_dEdx.front());
                  xyz_dEdx.erase(xyz_dEdx.begin());

                  // sort space points
                  while (!xyz_dEdx.empty()) {
                     auto last_point = sorted_xyz_dEdx.back().first;
                     auto closest_it = std::min_element(xyz_dEdx.begin(), xyz_dEdx.end(), 
                        [&](const std::pair<TVector3, double>& a, const std::pair<TVector3, double>& b) {
                              return (a.first - last_point).Mag() < (b.first - last_point).Mag();
                        });
                     sorted_xyz_dEdx.push_back(*closest_it);
                     xyz_dEdx.erase(closest_it);
                  }

                  for(const auto& pair : sorted_xyz_dEdx) {
                     traj.AddRecoPoint(pair.first, pair.second);
                  }
               }
            }
         }
      }
   }

   std::cout << ">>> [PionReconstructionAnalyser] Finished analysing trajectory..." << std::endl;
}
//_________________________________________________________________________________________
}