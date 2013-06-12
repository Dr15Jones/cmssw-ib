#include "Mixing/Base/src/XXX.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace edm {
  // -----------------------------

  XXX::XXX(std::vector<ParameterSet>& psets,
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
  } // XXX::XXX
  
  void XXX::beginRun(RunPrincipal& run, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<RunPrincipal, BranchActionBegin> >(run, setup);
  }

  void XXX::beginLuminosityBlock(LuminosityBlockPrincipal& lumi, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<LuminosityBlockPrincipal, BranchActionBegin> >(lumi, setup);
  }

  void XXX::endRun(RunPrincipal& run, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<RunPrincipal, BranchActionEnd> >(run, setup);
  }

  void XXX::endLuminosityBlock(LuminosityBlockPrincipal& lumi, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<LuminosityBlockPrincipal, BranchActionEnd> >(lumi, setup);
  }

  void XXX::setupPileUpEvent(EventPrincipal& ep, const EventSetup& setup) {
    workerManager_.processOneOccurrence<OccurrenceTraits<EventPrincipal, BranchActionBegin> >(ep, setup);
  }

}
