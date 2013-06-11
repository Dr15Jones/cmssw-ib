#include "FWCore/Framework/src/WorkerManager.h"

#include "DataFormats/Provenance/interface/ProductRegistry.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "FWCore/Utilities/interface/Algorithms.h"
#include "FWCore/Utilities/interface/ExceptionCollector.h"

namespace edm {
  // -----------------------------

  WorkerManager::WorkerManager(boost::shared_ptr<ActivityRegistry> areg, ActionTable const& actions) :
    worker_reg_(areg),
    act_table_(&actions),
    all_workers_(),
    unscheduled_(new UnscheduledCallProducer) {
  } // WorkerManager::WorkerManager

  void WorkerManager::createWorker(ParameterSet& pset,
                                   ProductRegistry& preg,
                                   boost::shared_ptr<ProcessConfiguration> processConfiguration,
                                   std::string label,
                                   bool useStopwatch,
                                   std::set<std::string>& unscheduledLabels) {
    //Need to
    // 1) create worker
    // 2) if it is a WorkerT<EDProducer>, add it to our list
    WorkerParams params(&pset, preg, processConfiguration, *act_table_);
    Worker* newWorker(worker_reg_.getWorker(params, label));
    if(newWorker->moduleType() == Worker::kProducer || newWorker->moduleType() == Worker::kFilter) {
      unscheduledLabels.insert(label);
      unscheduled_->addWorker(newWorker);
      //add to list so it gets reset each new event
      addToAllWorkers(newWorker, useStopwatch);
    }
  }

  void WorkerManager::setOnDemandProducts(ProductRegistry& pregistry, std::set<std::string> const& unscheduledLabels) const {
    for(auto& prod : pregistry.productList()) {
      if(prod.second.produced() &&
          prod.second.branchType() == InEvent &&
          unscheduledLabels.end() != unscheduledLabels.find(prod.second.moduleLabel())) {
        prod.second.setOnDemand();
      }
    }
  }
  
  void WorkerManager::endJob() {
    for(auto& worker : all_workers_) {
      worker->endJob();
    }
  }

  void WorkerManager::endJob(ExceptionCollector& collector) {
    for(auto& worker : all_workers_) {
      try {
        try {
          worker->endJob();
        }
        catch (cms::Exception& e) { throw; }
        catch (std::bad_alloc& bda) { convertException::badAllocToEDM(); }
        catch (std::exception& e) { convertException::stdToEDM(e); }
        catch (std::string& s) { convertException::stringToEDM(s); }
        catch (char const* c) { convertException::charPtrToEDM(c); }
        catch (...) { convertException::unknownToEDM(); }
      }      
      catch (cms::Exception const& ex) {
        collector.addException(ex);
      }
    }
  }


  void WorkerManager::beginJob(ProductRegistry const& iRegistry) {
    auto const runLookup = iRegistry.productLookup(InRun);
    auto const lumiLookup = iRegistry.productLookup(InLumi);
    auto const eventLookup = iRegistry.productLookup(InEvent);
    for(auto& worker : all_workers_) {
      worker->updateLookup(InRun,*runLookup);
      worker->updateLookup(InLumi,*lumiLookup);
      worker->updateLookup(InEvent,*eventLookup);
    }
    
    for_all(all_workers_, boost::bind(&Worker::beginJob, _1));
    loadMissingDictionaries();
  }

  void
  WorkerManager::resetAll() {
    for_all(all_workers_, boost::bind(&Worker::reset, _1));
  }

  void
  WorkerManager::addToAllWorkers(Worker* w, bool useStopwatch) {
    if(!search_all(all_workers_, w)) {
      if(useStopwatch) {
        w->useStopwatch();
      }
      all_workers_.push_back(w);
    }
  }

  void
  WorkerManager::setupOnDemandSystem(EventPrincipal& ep, EventSetup const& es) {
    // NOTE: who owns the productdescrption?  Just copied by value
    unscheduled_->setEventSetup(es);
    ep.setUnscheduledHandler(unscheduled_);
  }
  
}
