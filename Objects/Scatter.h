#ifndef SCATTER_HH
#define SCATTER_HH

#include <vector>
#include "TVector3.h"

#include "nusimdata/SimulationBase/MCParticle.h"

#include "ubana/PionTrajectory/Interface/Include/ParticleTypes.h"

namespace ubpiontraj
{
   class Scatter {
   public:
      Scatter(bool isElastic, bool isInelastic, 
               const TVector3& initialMomentum, const TVector3& finalMomentum,
               const art::Ptr<simb::MCParticle>& incidentParticle, 
               const art::Ptr<simb::MCParticle>& scatterParticle,
               const std::vector<art::Ptr<simb::MCParticle>>& products);

      ~Scatter();

      bool isElastic() const;
      bool isInelastic() const;

      TVector3 initialMomentum() const;
      TVector3 finalMomentum() const;

      art::Ptr<simb::MCParticle> incidentParticle() const;
      art::Ptr<simb::MCParticle> scatterParticle() const;

      std::vector<art::Ptr<simb::MCParticle>> products() const;

      double MomentumFractionLoss() const;
      double CosTheta() const;

   private:
      bool m_isElastic;
      bool m_isInelastic;

      TVector3 m_initialMomentum;
      TVector3 m_finalMomentum;

      art::Ptr<simb::MCParticle> m_incidentParticle;
      art::Ptr<simb::MCParticle> m_scatterParticle;

      std::vector<art::Ptr<simb::MCParticle>> m_products;
   };

   Scatter::Scatter(bool isElastic, bool isInelastic, 
                  const TVector3& initialMomentum, const TVector3& finalMomentum,
                  const art::Ptr<simb::MCParticle>& incidentParticle, 
                  const art::Ptr<simb::MCParticle>& scatterParticle,
                  const std::vector<art::Ptr<simb::MCParticle>>& products)
      : m_isElastic(isElastic), m_isInelastic(isInelastic),
         m_initialMomentum(initialMomentum), m_finalMomentum(finalMomentum),
         m_incidentParticle(incidentParticle), m_scatterParticle(scatterParticle),
         m_products(products) {}

   Scatter::~Scatter() {}

   bool Scatter::isElastic() const {
      return m_isElastic;
   }

   bool Scatter::isInelastic() const {
      return m_isInelastic;
   }

   TVector3 Scatter::initialMomentum() const {
      return m_initialMomentum;
   }

   TVector3 Scatter::finalMomentum() const {
      return m_finalMomentum;
   }

   art::Ptr<simb::MCParticle> Scatter::incidentParticle() const {
      return m_incidentParticle;
   }

   art::Ptr<simb::MCParticle> Scatter::scatterParticle() const {
      return m_scatterParticle;
   }

   std::vector<art::Ptr<simb::MCParticle>> Scatter::products() const {
      return m_products;
   }

   double Scatter::MomentumFractionLoss() const {
      double initialMagnitude = m_initialMomentum.Mag();
      double finalMagnitude = m_finalMomentum.Mag();

      if (initialMagnitude == 0) {
         return 0;
      }

      return (initialMagnitude - finalMagnitude) / initialMagnitude;
   }

   double Scatter::CosTheta() const {
      double dotProduct = m_initialMomentum.Dot(m_finalMomentum);

      double initialMagnitude = m_initialMomentum.Mag();
      double finalMagnitude = m_finalMomentum.Mag();

      if (initialMagnitude == 0 || finalMagnitude == 0) {
         return 0;
      }
      
      return dotProduct / (initialMagnitude * finalMagnitude);
   }
}

#endif  // SCATTER_HH