#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"

using namespace reco;

GsfElectron::GsfElectron()
 : mva_(0), fbrem_(0), class_(UNKNOWN) {}

GsfElectron::GsfElectron
 ( const LorentzVector & p4,
   int charge, const ChargeInfo & chargeInfo,
   const GsfElectronCoreRef & core,
   const TrackClusterMatching & tcm, const TrackExtrapolations & te,
   const ClosestCtfTrack & ctfInfo,
   const FiducialFlags & ff, const ShowerShape & ss, float fbrem,
   float mva
 )
 : chargeInfo_(chargeInfo),
   core_(core),
   trackClusterMatching_(tcm), trackExtrapolations_(te),
   closestCtfTrack_(ctfInfo),
   fiducialFlags_(ff), showerShape_(ss),
   mva_(mva),
   fbrem_(fbrem), class_(UNKNOWN)
 {
  setCharge(charge) ;
  setP4(p4) ;
  setVertex(te.positionAtVtx) ;
  setPdgId(-11*charge) ;
  /*if (isEcalDriven())*/ corrections_.ecalEnergy = superCluster()->energy() ;
}

GsfElectron::GsfElectron
 ( const GsfElectron & electron,
   const GsfElectronCoreRef & core,
   const CaloClusterPtr & electronCluster,
   const TrackRef & closestCtfTrack,
   const GsfTrackRefVector & ambiguousTracks )
 : RecoCandidate(electron),
   chargeInfo_(electron.chargeInfo_),
   core_(core),
   trackClusterMatching_(electron.trackClusterMatching_),
   trackExtrapolations_(electron.trackExtrapolations_),
   closestCtfTrack_(electron.closestCtfTrack_),
   ambiguousGsfTracks_(ambiguousTracks),
   fiducialFlags_(electron.fiducialFlags_),
   showerShape_(electron.showerShape_),
   dr03_(electron.dr03_),
   dr04_(electron.dr04_),
   mva_(electron.mva_),
   fbrem_(electron.fbrem_),
   class_(electron.class_),
   corrections_(electron.corrections_)
 {
  trackClusterMatching_.electronCluster = electronCluster ;
  closestCtfTrack_.ctfTrack = closestCtfTrack ;
  // TO BE DONE
  // Check that the new edm references are really
  // to clones of the former references, and therefore other attributes
  // stay valid :
  // * electron.core_ ~ core ?
  // * electron.trackClusterMatching_.electronCluster ~ electronCluster ?
  // * electron.closestCtfTrack_.ctfTrack ~ closestCtfTrack ?
  // * electron.ambiguousGsfTracks_ ~ ambiguousTracks ?
 }

void GsfElectron::correctEcalEnergy( float newEnergy, float newEnergyError )
 {
  math::XYZTLorentzVectorD momentum = p4() ;
  momentum *= newEnergy/momentum.e() ;
  setP4(momentum) ;
  showerShape_.hcalDepth1OverEcal *= corrections_.ecalEnergy/newEnergy ;
  showerShape_.hcalDepth2OverEcal *= corrections_.ecalEnergy/newEnergy ;
  trackClusterMatching_.eSuperClusterOverP *= newEnergy/corrections_.ecalEnergy ;
  trackClusterMatching_.eSeedClusterOverP *= newEnergy/corrections_.ecalEnergy ;
  trackClusterMatching_.eEleClusterOverPout *= newEnergy/corrections_.ecalEnergy ;
  corrections_.ecalEnergy = newEnergy ;
  corrections_.ecalEnergyError = newEnergyError ;
  corrections_.isEcalEnergyCorrected = true ;
 }

void GsfElectron::correctMomentum
 ( const reco::Candidate::LorentzVector & momentum,
   float trackErr, float electronErr )
 {
  setP4(momentum) ;
  corrections_.trackMomentumError = trackErr ;
  corrections_.electronMomentumError = electronErr ;
  corrections_.isMomentumCorrected = true ;
 }

bool GsfElectron::overlap( const Candidate & c ) const {
  const RecoCandidate * o = dynamic_cast<const RecoCandidate *>( & c );
  return ( o != 0 &&
	   ( checkOverlap( gsfTrack(), o->gsfTrack() ) ||
	     checkOverlap( superCluster(), o->superCluster() ) )
	   );
  //?? return false;
}

GsfElectron * GsfElectron::clone() const
 { return new GsfElectron(*this) ; }

GsfElectron * GsfElectron::clone
 (
  const GsfElectronCoreRef & core,
  const CaloClusterPtr & electronCluster,
  const TrackRef & closestCtfTrack,
  const GsfTrackRefVector & ambiguousTracks
 ) const
 { return new GsfElectron(*this,core,electronCluster,closestCtfTrack,ambiguousTracks) ; }

