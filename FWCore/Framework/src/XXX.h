#ifndef FWCore_Framework_XXX_h
#define FWCore_Framework_XXX_h

/*

*/

#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/Framework/interface/Actions.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/ExceptionHelpers.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/OccurrenceTraits.h"
#include "FWCore/Framework/interface/UnscheduledCallProducer.h"
#include "FWCore/Framework/src/Worker.h"
#include "FWCore/Framework/src/WorkerRegistry.h"
#include "FWCore/MessageLogger/interface/ExceptionMessages.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Algorithms.h"
#include "FWCore/Utilities/interface/BranchType.h"
#include "FWCore/Utilities/interface/ConvertException.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "boost/shared_ptr.hpp"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <sstream>

namespace edm {
  class ActivityRegistry;
  class EventSetup;
  class ExceptionCollector;
  class OutputWorker;
  class RunStopwatch;
  class UnscheduledCallProducer;
  class WorkerInPath;
  class XXX {
  public:
    typedef std::vector<std::string> vstring;
    typedef boost::shared_ptr<Worker> WorkerPtr;
    typedef std::vector<Worker*> AllWorkers;

    typedef std::vector<Worker*> Workers;

    XXX(ParameterSet& proc_pset,
             ProductRegistry& pregistry,
             ActionTable const& actions,
             boost::shared_ptr<ProcessConfiguration> processConfiguration);

    template <typename T>
    void processOneOccurrence(typename T::MyPrincipal& principal,
                              EventSetup const& eventSetup,
                              bool cleaningUpAfterException = false);

    void beginJob(ProductRegistry const& iRegistry);
    void endJob();

    void beginRun(const edm::EventSetup& setup);
    void beginLuminosityBlock(const edm::EventSetup& setup);

    void endRun(const edm::EventSetup& setup);
    void endLuminosityBlock(const edm::EventSetup& setup);

  private:

    AllWorkers::const_iterator workersBegin() const {
      return all_workers_.begin();
    }

    AllWorkers::const_iterator workersEnd() const {
      return all_workers_.end();
    }

    AllWorkers::iterator workersBegin() {
      return  all_workers_.begin();
    }

    AllWorkers::iterator workersEnd() {
      return all_workers_.end();
    }

    void resetAll();

    void setupOnDemandSystem(EventPrincipal& principal, EventSetup const& es);


    void addToAllWorkers(Worker* w);
    
    WorkerRegistry                                worker_reg_;
    ActionTable const*                            act_table_;

    AllWorkers               all_workers_;

    boost::shared_ptr<UnscheduledCallProducer> unscheduled_;
  };

  template <typename T>
  void
  XXX::processOneOccurrence(typename T::MyPrincipal& ep,
                                 EventSetup const& es,
                                 bool cleaningUpAfterException) {
    this->resetAll();


    if (T::isEvent_) {
      setupOnDemandSystem(dynamic_cast<EventPrincipal&>(ep), es);
    }
    try {
      try {
        try {
          //make sure the unscheduled items see this transition [Event will be a no-op]
          unscheduled_->runNow<T>(ep, es);
        }
        catch(cms::Exception& e) {
          actions::ActionCodes action = (T::isEvent_ ? act_table_->find(e.category()) : actions::Rethrow);
          assert (action != actions::IgnoreCompletely);
          assert (action != actions::FailPath);
          if (action == actions::SkipEvent) {
            edm::printCmsExceptionWarning("SkipEvent", e);
          } else {
            throw;
          }
        }
      }
      catch (cms::Exception& e) { throw; }
      catch(std::bad_alloc& bda) { convertException::badAllocToEDM(); }
      catch (std::exception& e) { convertException::stdToEDM(e); }
      catch(std::string& s) { convertException::stringToEDM(s); }
      catch(char const* c) { convertException::charPtrToEDM(c); }
      catch (...) { convertException::unknownToEDM(); }
    }
    catch(cms::Exception& ex) {
      if (ex.context().empty()) {
        addContextAndPrintException("Calling function XXX::processOneOccurrence", ex, cleaningUpAfterException);
      } else {
        addContextAndPrintException("", ex, cleaningUpAfterException);
      }
      throw;
    }
  }
}

#endif
