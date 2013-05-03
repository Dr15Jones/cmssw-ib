// -*- C++ -*-
//
// Package:     FWCore/Framework
// Class  :     one::EDProducerBase
// 
// Implementation:
//     [Notes on implementation]
//
// Original Author:  Chris Jones
//         Created:  Thu, 02 May 2013 21:56:04 GMT
// $Id$
//

// system include files

// user include files
#include "FWCore/Framework/interface/one/EDProducerBase.h"
#include "FWCore/Framework/src/CPCSentry.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/src/edmodule_mightGet_config.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"


//
// constants, enums and typedefs
//
namespace edm {
  namespace one {
    //
    // static data member definitions
    //
    
    //
    // constructors and destructor
    //
    EDProducerBase::EDProducerBase():
    ProducerBase(),
    moduleDescription_(),
    current_context_(nullptr),
    previousParentage_(),
    previousParentageId_() { }
    
    EDProducerBase::~EDProducerBase()
    {
    }
    
    bool
    EDProducerBase::doEvent(EventPrincipal& ep, EventSetup const& c,
                        CurrentProcessingContext const* cpc) {
      detail::CPCSentry sentry(current_context_, cpc);
      Event e(ep, moduleDescription_);
      e.setConsumer(this);
      this->produce(e, c);
      commit_(e,&previousParentage_, &previousParentageId_);
      return true;
    }
    
    void
    EDProducerBase::doBeginJob() {
      this->beginJob();
    }
    
    void
    EDProducerBase::doEndJob() {
      this->endJob();
    }
    
    void
    EDProducerBase::doBeginRun(RunPrincipal& rp, EventSetup const& c,
                           CurrentProcessingContext const* cpc) {
      /*
      detail::CPCSentry sentry(current_context_, cpc);
      Run r(rp, moduleDescription_);
      r.setConsumer(this);
      Run const& cnstR = r;
      this->beginRun(cnstR, c);
      r.commit_();
       */
    }
    
    void
    EDProducerBase::doEndRun(RunPrincipal& rp, EventSetup const& c,
                         CurrentProcessingContext const* cpc) {
      /*
      detail::CPCSentry sentry(current_context_, cpc);
      Run r(rp, moduleDescription_);
      r.setConsumer(this);
      Run const& cnstR = r;
      this->endRun(cnstR, c);
      r.commit_();
       */
    }
    
    void
    EDProducerBase::doBeginLuminosityBlock(LuminosityBlockPrincipal& lbp, EventSetup const& c,
                                       CurrentProcessingContext const* cpc) {
      /*
      detail::CPCSentry sentry(current_context_, cpc);
      LuminosityBlock lb(lbp, moduleDescription_);
      lb.setConsumer(this);
      LuminosityBlock const& cnstLb = lb;
      this->beginLuminosityBlock(cnstLb, c);
      lb.commit_();
       */
    }
    
    void
    EDProducerBase::doEndLuminosityBlock(LuminosityBlockPrincipal& lbp, EventSetup const& c,
                                     CurrentProcessingContext const* cpc) {
      /*
      detail::CPCSentry sentry(current_context_, cpc);
      LuminosityBlock lb(lbp, moduleDescription_);
      lb.setConsumer(this);
      LuminosityBlock const& cnstLb = lb;
      this->endLuminosityBlock(cnstLb, c);
      lb.commit_();
       */
    }
    
    void
    EDProducerBase::doRespondToOpenInputFile(FileBlock const& fb) {
      //respondToOpenInputFile(fb);
    }
    
    void
    EDProducerBase::doRespondToCloseInputFile(FileBlock const& fb) {
      //respondToCloseInputFile(fb);
    }
    
    void
    EDProducerBase::doRespondToOpenOutputFiles(FileBlock const& fb) {
      //respondToOpenOutputFiles(fb);
    }
    
    void
    EDProducerBase::doRespondToCloseOutputFiles(FileBlock const& fb) {
      //respondToCloseOutputFiles(fb);
    }
    
    void
    EDProducerBase::doPreForkReleaseResources() {
      //preForkReleaseResources();
    }
    
    void
    EDProducerBase::doPostForkReacquireResources(unsigned int iChildIndex, unsigned int iNumberOfChildren) {
      //postForkReacquireResources(iChildIndex, iNumberOfChildren);
    }
    
    CurrentProcessingContext const*
    EDProducerBase::currentContext() const {
      return current_context_;
    }
    
    void
    EDProducerBase::fillDescriptions(ConfigurationDescriptions& descriptions) {
      ParameterSetDescription desc;
      desc.setUnknown();
      descriptions.addDefault(desc);
    }
    
    void
    EDProducerBase::prevalidate(ConfigurationDescriptions& iConfig) {
      edmodule_mightGet_config(iConfig);
    }
    
    static const std::string kBaseType("EDProducer");
    
    const std::string&
    EDProducerBase::baseType() {
      return kBaseType;
    }

  }
}