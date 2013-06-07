#ifndef GAIA_CYCLOPSPROXY_H
#define GAIA_CYCLOPSPROXY_H

#include "yamlrpcserver.h"
#include "yamlcpp.h"
#include "cyclops.h"


gaia2::yaml::Node toYaml(const CyclopsResults& results);


class CyclopsProxy : public YamlProxy {
 public:

  /**
   * Create a CyclopsProxy instance that can translate a Yaml-RPC call to an actual
   * method call on a Cyclops instance.
   */
  CyclopsProxy(Cyclops* cyclops);


  /**
   * Perform the given function call on the wrapped Cyclops instance and return the
   * result as a seriealized Yaml object.
   */
  gaia2::yaml::Node process(const gaia2::yaml::Mapping& query);

 protected:
  Cyclops* _cyclops;
};


#endif // GAIA_CYCLOPSPROXY_H
