#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/src/WorkerManager.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "boost/shared_ptr.hpp"

#include <string>
#include <vector>

namespace edm {
  class SecondaryEventProvider {
  public:
    SecondaryEventProvider(std::vector<ParameterSet>& psets,
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

  SecondaryEventProvider::SecondaryEventProvider(std::vector<ParameterSet>& psets,
                     ProductRegistry& preg,
                     ActionTable const& actions,
                     boost::shared_ptr<ProcessConfiguration> processConfiguration) :
    workerManager_(boost::shared_ptr<ActivityRegistry>(new ActivityRegistry), actions) {
    std::vector<std::string> shouldBeUsedLabels;
    std::set<std::string> unscheduledLabels;
    for(auto& pset : psets) {
        std::string label = pset.getParameter<std::string>("@module_label");
        workerManager_.addToUnscheduledWorkers(pset,
                                               preg,
                                               processConfiguration,
                                               label,
                                               false,
                                               unscheduledLabels,
                                               shouldBeUsedLabels);
    }
    if(!unscheduledLabels.empty()) {
      workerManager_.setOnDemandProducts(preg, unscheduledLabels); 
    }
  } // SecondaryEventProvider::SecondaryEventProvider
  
  void SecondaryEventProvider::beginRun(RunPrincipal& run, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<RunPrincipal, BranchActionBegin> >(run, setup);
  }

  void SecondaryEventProvider::beginLuminosityBlock(LuminosityBlockPrincipal& lumi, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<LuminosityBlockPrincipal, BranchActionBegin> >(lumi, setup);
  }

  void SecondaryEventProvider::endRun(RunPrincipal& run, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<RunPrincipal, BranchActionEnd> >(run, setup);
  }

  void SecondaryEventProvider::endLuminosityBlock(LuminosityBlockPrincipal& lumi, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<LuminosityBlockPrincipal, BranchActionEnd> >(lumi, setup);
  }

  void SecondaryEventProvider::setupPileUpEvent(EventPrincipal& ep, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<EventPrincipal, BranchActionBegin> >(ep, setup);
  }

}
