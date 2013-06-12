#ifndef FWCore_Framework_XXX_h
#define FWCore_Framework_XXX_h

/*

*/

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/src/WorkerManager.h"

#include "boost/shared_ptr.hpp"

#include <string>
#include <vector>

namespace edm {
  class XXX {
  public:
    XXX(std::vector<ParameterSet>& psets,
             ProductRegistry& pregistry,
             ActionTable const& actions,
             boost::shared_ptr<ProcessConfiguration> processConfiguration);

    void beginRun(RunPrincipal& run, const edm::EventSetup& setup);
    void beginLuminosityBlock(LuminosityBlockPrincipal& lumi, const edm::EventSetup& setup);

    void endRun(RunPrincipal& run, const edm::EventSetup& setup);
    void endLuminosityBlock(LuminosityBlockPrincipal& lumi, const edm::EventSetup& setup);

    void setupPileUpEvent(EventPrincipal& ep, const EventSetup& setup);

    void beginJob(ProductRegistry const& iRegistry) {workerManager_.beginJob(iRegistry);}
    void endJob() {workerManager_.endJob();}

  private:
    WorkerManager workerManager_;
  };

}
#endif
