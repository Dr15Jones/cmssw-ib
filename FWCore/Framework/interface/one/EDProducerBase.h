#ifndef FWCore_Framework_EDProducerBase_h
#define FWCore_Framework_EDProducerBase_h
// -*- C++ -*-
//
// Package:     FWCore/Framework
// Class  :     one::EDProducerBase
// 
/**\class one::EDProducerBase EDProducerBase.h "FWCore/Framework/interface/one/EDProducerBase.h"

 Description: Base class for edm::one::EDProducer<>

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Thu, 02 May 2013 21:21:21 GMT
// $Id$
//

// system include files

// user include files
#include "FWCore/Framework/interface/ProducerBase.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Provenance/interface/ModuleDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSetfwd.h"

// forward declarations
namespace edm {
  namespace one {

    class EDProducerBase
    {
      
    public:
      template <typename T> friend class WorkerT;
      typedef EDProducerBase ModuleType;
      typedef WorkerT<EDProducerBase> WorkerType;

      
      EDProducerBase();
      virtual ~EDProducerBase();
      
      static void fillDescriptions(ConfigurationDescriptions& descriptions);
      static void prevalidate(ConfigurationDescriptions& descriptions);
      static const std::string& baseType();

    protected:
      // The returned pointer will be null unless the this is currently
      // executing its event loop function ('produce').
      CurrentProcessingContext const* currentContext() const;
      
    private:
      bool doEvent(EventPrincipal& ep, EventSetup const& c,
                   CurrentProcessingContext const* cpcp);
      void doBeginJob();
      void doEndJob();
      
      virtual void doBeginRun(RunPrincipal& rp, EventSetup const& c,
                      CurrentProcessingContext const* cpc);
      virtual void doEndRun(RunPrincipal& rp, EventSetup const& c,
                    CurrentProcessingContext const* cpc);
      virtual void doBeginLuminosityBlock(LuminosityBlockPrincipal& lbp, EventSetup const& c,
                                  CurrentProcessingContext const* cpc);
      virtual void doEndLuminosityBlock(LuminosityBlockPrincipal& lbp, EventSetup const& c,
                                CurrentProcessingContext const* cpc);
      
      //For now, the following are just dummy implemenations with no ability for users to override
      void doRespondToOpenInputFile(FileBlock const& fb);
      void doRespondToCloseInputFile(FileBlock const& fb);
      void doRespondToOpenOutputFiles(FileBlock const& fb);
      void doRespondToCloseOutputFiles(FileBlock const& fb);
      void doPreForkReleaseResources();
      void doPostForkReacquireResources(unsigned int iChildIndex, unsigned int iNumberOfChildren);

      
      void registerProductsAndCallbacks(EDProducer* module, ProductRegistry* reg) {
        registerProducts(module, reg, moduleDescription_);
      }
      std::string workerType() const {return "WorkerT<EDProducer>";}
      
      virtual void produce(Event&, EventSetup const&) = 0;
      virtual void beginJob() {}
      virtual void endJob(){}

      void setModuleDescription(ModuleDescription const& md) {
        moduleDescription_ = md;
      }
      ModuleDescription moduleDescription_;
      CurrentProcessingContext const* current_context_;
      std::vector<BranchID> previousParentage_;
      ParentageID previousParentageId_;

    };
    
  }
}


#endif
