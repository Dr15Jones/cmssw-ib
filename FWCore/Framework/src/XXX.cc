#include "FWCore/Framework/src/XXX.h"

#include "DataFormats/Provenance/interface/ProcessConfiguration.h"
#include "DataFormats/Provenance/interface/ProductRegistry.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/OutputModuleDescription.h"
#include "FWCore/Framework/interface/TriggerNamesService.h"
#include "FWCore/Framework/interface/TriggerReport.h"
#include "FWCore/Framework/src/Factory.h"
#include "FWCore/Framework/interface/OutputModule.h"
#include "FWCore/Framework/src/OutputWorker.h"
#include "FWCore/Framework/src/WorkerInPath.h"
#include "FWCore/Framework/src/WorkerMaker.h"
#include "FWCore/Framework/src/WorkerT.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/FillProductRegistryTransients.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/Registry.h"
#include "FWCore/Utilities/interface/Algorithms.h"
#include "FWCore/Utilities/interface/ConvertException.h"
#include "FWCore/Utilities/interface/ExceptionCollector.h"
#include "FWCore/Utilities/interface/DictionaryTools.h"

#include "boost/bind.hpp"
#include "boost/ref.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <list>
#include <map>
#include <exception>

namespace edm {
  namespace {

    // -----------------------------

    bool binary_search_string(std::vector<std::string> const& v, std::string const& s) {
      return std::binary_search(v.begin(), v.end(), s);
    }
  }

  // -----------------------------

  XXX::XXX(ParameterSet& proc_pset,
                     ProductRegistry& preg,
                     ActionTable const& actions,
                     boost::shared_ptr<ProcessConfiguration> processConfiguration) :
    
    worker_reg_(boost::shared_ptr<ActivityRegistry>(new ActivityRegistry)),
    act_table_(&actions),
    all_workers_(),
    unscheduled_(new UnscheduledCallProducer) {

    //See if all modules were used
    std::set<std::string> usedWorkerLabels;
    for (AllWorkers::iterator itWorker = workersBegin();
        itWorker != workersEnd();
        ++itWorker) {
      usedWorkerLabels.insert((*itWorker)->description().moduleLabel());
    }
    std::vector<std::string> unusedLabels(proc_pset.getParameter<std::vector<std::string> >("@all_modules"));
    bool allowUnscheduled = true;
    std::set<std::string> unscheduledLabels;
    std::vector<std::string>  shouldBeUsedLabels;
    if (!unusedLabels.empty()) {
      //Need to
      // 1) create worker
      // 2) if it is a WorkerT<EDProducer>, add it to our list
      // 3) hand list to our delayed reader

      for (std::vector<std::string>::iterator itLabel = unusedLabels.begin(), itLabelEnd = unusedLabels.end();
          itLabel != itLabelEnd;
          ++itLabel) {
        if (allowUnscheduled) {
          bool isTracked;
          ParameterSet* modulePSet(proc_pset.getPSetForUpdate(*itLabel, isTracked));
          assert(isTracked);
          assert(modulePSet != 0);
          WorkerParams params(proc_pset, modulePSet, preg,
                              processConfiguration, *act_table_);
          Worker* newWorker(worker_reg_.getWorker(params, *itLabel));
          if (newWorker->moduleType() == Worker::kProducer ||
              newWorker->moduleType() == Worker::kFilter) {
            unscheduledLabels.insert(*itLabel);
            unscheduled_->addWorker(newWorker);
            //add to list so it gets reset each new event
            addToAllWorkers(newWorker);
          } else {
            //not a producer so should be marked as not used
            shouldBeUsedLabels.push_back(*itLabel);
          }
        } else {
          //everthing is marked are unused so no 'on demand' allowed
          shouldBeUsedLabels.push_back(*itLabel);
        }
      }
      if (!shouldBeUsedLabels.empty()) {
        std::ostringstream unusedStream;
        unusedStream << "'" << shouldBeUsedLabels.front() << "'";
        for (std::vector<std::string>::iterator itLabel = shouldBeUsedLabels.begin() + 1,
              itLabelEnd = shouldBeUsedLabels.end();
            itLabel != itLabelEnd;
            ++itLabel) {
          unusedStream << ",'" << *itLabel << "'";
        }
        LogInfo("path")
          << "The following module labels are not assigned to any path:\n"
          << unusedStream.str()
          << "\n";
      }
    }
    if (!unscheduledLabels.empty()) {
      for (ProductRegistry::ProductList::const_iterator it = preg.productList().begin(),
          itEnd = preg.productList().end();
          it != itEnd;
          ++it) {
        if (it->second.produced() &&
            it->second.branchType() == InEvent &&
            unscheduledLabels.end() != unscheduledLabels.find(it->second.moduleLabel())) {
          it->second.setOnDemand();
        }
      }
    }

    // This is used for a little sanity-check to make sure no code
    // modifications alter the number of workers at a later date.
    size_t all_workers_count = all_workers_.size();

    // Sanity check: make sure nobody has added a worker after we've
    // already relied on all_workers_ being full.
    assert (all_workers_count == all_workers_.size());
  } // XXX::XXX
  
  void XXX::endJob() {
    AllWorkers::iterator ai(workersBegin()), ae(workersEnd());
    for (; ai != ae; ++ai) {
      (*ai)->endJob();
    }
  }

  void XXX::beginJob(ProductRegistry const& iRegistry) {
    auto const runLookup = iRegistry.productLookup(InRun);
    auto const lumiLookup = iRegistry.productLookup(InLumi);
    auto const eventLookup = iRegistry.productLookup(InEvent);
    for(auto & worker: all_workers_) {
      worker->updateLookup(InRun,*runLookup);
      worker->updateLookup(InLumi,*lumiLookup);
      worker->updateLookup(InEvent,*eventLookup);
    }
    
    for_all(all_workers_, boost::bind(&Worker::beginJob, _1));
    loadMissingDictionaries();
  }

  void
  XXX::resetAll() {
    for_all(all_workers_, boost::bind(&Worker::reset, _1));
  }

  void
  XXX::addToAllWorkers(Worker* w) {
    if (!search_all(all_workers_, w)) {
      all_workers_.push_back(w);
    }
  }

  void
  XXX::setupOnDemandSystem(EventPrincipal& ep, EventSetup const& es) {
    // NOTE: who owns the productdescrption?  Just copied by value
    unscheduled_->setEventSetup(es);
    ep.setUnscheduledHandler(unscheduled_);
  }
  
}
