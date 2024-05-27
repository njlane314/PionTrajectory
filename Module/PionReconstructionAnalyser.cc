#include "Include/PionReconstructionAnalyser.h"
#include <iostream>
//_________________________________________________________________________________________
namespace ubpiontraj {
//_________________________________________________________________________________________
PionReconstructionAnalyser::PionReconstructionAnalyser(art::Event const& event, ubpiontraj::Trajectory traj, 
                                                      std::string recotracklabel, std::string recotrackhitlabel, std::string recohitsimparticlelabel, std::string recocalolabel, 
                                                      bool debug) :
   m_Trajectory(traj),
   m_SelectedSimParticles(traj.GetSimParticles()),
   m_RecoTrackLabel(recotracklabel),
   m_RecoTrackHitLabel(recotrackhitlabel),
   m_RecoHitSimParticleLabel(recohitsimparticlelabel),
   m_RecoCaloLabel(recocalolabel),
   m_Debug(debug)
{
   art::fill_ptr_vector(m_RecoParticles, m_RecoParticleHandle);
   art::fill_ptr_vector(m_RecoTracks, m_RecoTrackHandle);
   art::fill_ptr_vector(m_RecoHits, m_RecoHitHandle);
 
   m_RecoParticleTrackAssoc = new art::FindManyP<recob::Track>(m_RecoParticles, event, m_RecoTrackLabel);     
   m_RecoTrackHitAssoc = new  art::FindManyP<recob::Hit>(m_RecoTracks, event, m_RecoTrackHitLabel);
   m_RecoHitSimParticleAssoc = new art::FindMany<simb::MCParticle, anab::BackTrackerHitMatchingData>(m_RecoHitHandle, event, m_RecoHitSimParticleLabel);
   m_RecoTrackCaloAssoc = new art::FindManyP<anab::Calorimetry>(m_RecoTracks, event, m_RecoCaloLabel);

   AnalyseEvent(event);
}
//_________________________________________________________________________________________
PionReconstructionAnalyser::~PionReconstructionAnalyser() 
{}
//_________________________________________________________________________________________
void PionReconstructionAnalyser::AnalyseEvent(art::Event const& event)
{  
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
            auto it = std::find_if(m_SelectedSimParticles.begin(), m_SelectedSimParticles.end(), 
                                   [&](const art::Ptr<simb::MCParticle>& ptr) { 
                                      return ptr.get() == matchedSimParticle; 
                                   });
            if(it !=m_SelectedSimParticles.end()) {
               for(size_t plane = 0; plane < planesCalo.size(); plane++){
                  int planeType = planesCalo.at(plane)->PlaneID().Plane;
                  if(planeType != 0 && planeType != 1 && planeType != 2) continue;        

                  art::Ptr<anab::Calorimetry> calo = planesCalo.at(plane);
                  if(calo->XYZ().size() < 2) continue;

                  for(size_t sp = 0; sp < calo->TpIndices().size() - 1; sp++){
                     std::cout << calo->TpIndices().at(sp) << std::endl;
                  }
               }
            } 
         }
      }
   }
}
//_________________________________________________________________________________________
}














/*const std::vector<art::Ptr<recob::SpacePoint>>& spacePoints = m_RecoTrackSpacePointsAssoc.at(trkkey);
std::vector<art::Ptr<recob::Hit>> spacePointHits;

double elifetime = 21; //[ms]
double driftvelocity = 0.16; // [cm/us]

for(size_t sp = 0; sp < spacePoints.size(); sp++){
   auto const& spkey = spacePoints.at(sp).key();
   const auto &position(spacepoint->XYZ());
   
   const std::vector<art::Ptr<recob::Hit>> foundHits = m_RecoSpacePointHitAssoc.at(spkey);

   double chargeCollected = 0;
   double lifetimecorrection = std::exp((position[0]) / (elifetime * driftvelocity * 1000.0));
   for(size_t h = 0; h < foundHits.size(); h++) {
         const auto charge(foundHits.at(h)->Integral());
         chargeCollected += charge * lifetimecorrection;

         // calculate plane weight based on the angle between space points and wire plane
         // perpendicular tracks are more favoured
   }
}*/