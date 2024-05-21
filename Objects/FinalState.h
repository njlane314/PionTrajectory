#ifndef FINALSTATE_H
#define FINALSTATE_H

#include <vector>
#include "TVector3.h"

#include "nusimdata/SimulationBase/MCParticle.h"

namespace ubpiontraj 
{
   class FinalState {
   public:
      FinalState(const TVector3& finalParticleMomentum, 
                 const std::vector<art::Ptr<simb::MCParticle>>& products);

      ~FinalState();

      enum Type : unsigned int {
         None, 
         DecayToMuon, 
         InelasticAbsorption, 
         NeutralPionChargeExchange, 
         Other
      };

      Type getType() const;
      TVector3 getFinalParticleMomentum() const;
      const std::vector<art::Ptr<simb::MCParticle>>& getProducts() const;

      void print() const;

   private:
      Type m_type;
      TVector3 m_finalParticleMomentum;
      std::vector<art::Ptr<simb::MCParticle>> m_products;
   };

   FinalState::FinalState(const Type type, const TVector3& finalParticleMomentum, 
                          const std::vector<art::Ptr<simb::MCParticle>>& products)
      : m_type(type), m_finalParticleMomentum(finalParticleMomentum), m_products(products) {
         FinalState::Type type = FinalState::Type::Other; 
         
         bool hasNeutralParticle = false;
         bool hasMuon = false;
         bool hasNeutrino = false;

         for(const auto particle : m_products){
            if(isNeutralPion(finalParticle->PdgCode()) && finalParticle->Process() == "pi+inelastic"){
               hasNeutralPion = true;
            }

            if(isMuon(finalParticle->PdgCode()) && finalParticle->Process() == "Decay"){
               hasMuon = true; 
            }

            if(isMuonNeutrino(finalParticle->PdgCode()) && finalParticle->Process() == "Decay"){
               hasNeutrino = true;
            }
         }

         if(products.empty()){
            type = FinalState::Type::None;
         }
         else if (hasMuon && hasMuonNeutrino && products.size() == 2){
            type = EndState::Type::DecayToMuon;
         }
         else if (particle->EndProcess() == "pi+Inelastic"){
            type = hasPi0 ? EndState::Type::Pi0ChargeExchange : EndState::Type::InelasticAbsorption;
         }
      }

   FinalState::~FinalState() {}

   FinalState::Type FinalState::getType() const {
      return m_type;
   }

   TVector3 FinalState::getFinalParticleMomentum() const {
      return m_finalParticleMomentum;
   }

   const std::vector<art::Ptr<simb::MCParticle>>& FinalState::getProducts() const {
      return m_products;
   }

   void FinalState::print() const {
      std::cout << "Interaction Type: " << m_type << "\n"
                << "Final Particle Momentum: (" << m_finalParticleMomentum.X() << ", " 
                << m_finalParticleMomentum.Y() << ", " << m_finalParticleMomentum.Z() << ")\n"
                << "Number of Products: " << m_products.size() << "\n";

      for (const auto& product : m_products) {
         std::cout << "Product ID: " << product->TrackId() << "\n";
      }
   }
} 

#endif  // FINALSTATE_H