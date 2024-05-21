#ifndef FINALSTATE_H
#define FINALSTATE_H

#include <vector>
#include "TVector3.h"

#include "nusimdata/SimulationBase/MCParticle.h"

#include "ubana/PionTrajectory/Interface/Include/ParticleTypes.h"

namespace ubpiontraj 
{
   class FinalState {
   public:
      FinalState(const art::Ptr<simb::MCParticle>& mother, const std::vector<art::Ptr<simb::MCParticle>>& products);

      ~FinalState();

      enum Type : unsigned int {
         None, 
         DecayToMuon, 
         InelasticAbsorption, 
         NeutralPionChargeExchange, 
         Other
      };

      Type getType() const;
      const std::vector<art::Ptr<simb::MCParticle>>& getProducts() const;

      void print() const;

   private:
      Type m_type;
      art::Ptr<simb::MCParticle> m_mother;
      std::vector<art::Ptr<simb::MCParticle>> m_products;
   };

   FinalState::FinalState(const art::Ptr<simb:: MCParticle>& mother, const std::vector<art::Ptr<simb::MCParticle>>& products)
      : m_type(Type::Other), m_mother(mother), m_products(products) {
         
         bool hasNeutralPion = false;
         bool hasMuon = false;
         bool hasMuonNeutrino = false;

         for(const auto particle : m_products){
            if(isNeutralPion(particle->PdgCode()) && particle->Process() == "pi+inelastic"){
               hasNeutralPion = true;
            }

            if(isMuonLepton(particle->PdgCode()) && particle->Process() == "Decay"){
               hasMuon = true; 
            }

            if(isMuonNeutrino(particle->PdgCode()) && particle->Process() == "Decay"){
               hasMuonNeutrino = true;
            }
         }

         if(m_products.empty()){
            m_type = FinalState::Type::None;
         }
         else if(hasMuon && hasMuonNeutrino && m_products.size() == 2){
            m_type = FinalState::Type::DecayToMuon;
         }
         else if(m_mother->EndProcess() == "pi+Inelastic"){
            m_type = hasNeutralPion ? FinalState::Type::NeutralPionChargeExchange : FinalState::Type::InelasticAbsorption;
         }
      }

   FinalState::~FinalState() {}

   FinalState::Type FinalState::getType() const {
      return m_type;
   }

   const std::vector<art::Ptr<simb::MCParticle>>& FinalState::getProducts() const {
      return m_products;
   }

   void FinalState::print() const {
      std::cout << "Interaction Type: " << m_type << "\n"
                << "Number of Products: " << m_products.size() << "\n";

      for (const auto& product : m_products) {
         std::cout << "Product ID: " << product->TrackId() << "\n";
      }
   }
} 

#endif  // FINALSTATE_H