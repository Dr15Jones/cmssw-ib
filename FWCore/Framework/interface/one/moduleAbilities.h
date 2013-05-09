#ifndef FWCore_Framework_moduleAbilities_h
#define FWCore_Framework_moduleAbilities_h
// -*- C++ -*-
//
// Package:     FWCore/Framework
// Class  :     moduleAbilities
// 
/**\file moduleAbilities moduleAbilities.h "FWCore/Framework/interface/one/moduleAbilities.h"

 Description: Template arguments which only apply to one::{Module} classes

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Tue, 07 May 2013 19:38:53 GMT
// $Id$
//

// system include files

// user include files
#include "FWCore/Framework/interface/moduleAbilities.h"

// forward declarations

namespace edm {
  namespace one {
    struct SharedResources {
      static constexpr module::Abilities kAbilities=module::Abilities::kSharedResources;
      typedef module::Empty Type;
    };

    struct WatchRuns {
      static constexpr module::Abilities kAbilities=module::Abilities::kWatchRuns;
      typedef module::Empty Type;
    };

    struct WatchLuminosityBlocks {
      static constexpr module::Abilities kAbilities=module::Abilities::kWatchLuminosityBlocks;
      typedef module::Empty Type;
    };
  }
}


#endif
