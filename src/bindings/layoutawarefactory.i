%newobject gaia2::LayoutAwareFactory< QString, gaia2::DistanceFunction, gaia2::PointLayout, gaia2::ParameterMap >::create;
%newobject gaia2::LayoutAwareFactory< QString, gaia2::FrozenDistance, gaia2::FrozenDataSet, gaia2::ParameterMap >::create;


template < typename IdentifierType,
           typename AbstractProduct,
           typename LayoutType,
           typename ArgumentType >
class LayoutAwareFactory {

 public:

  static LayoutAwareFactory& instance() {
    static LayoutAwareFactory _instance;
    return _instance;
  }

  static AbstractProduct* create(const IdentifierType& id,
                                 const LayoutType&,
                                 const ArgumentType& arg = ArgumentType());

  AbstractProduct* create_i(const IdentifierType& id,
                            const LayoutType&,
                            const ArgumentType& arg) const;


};

%template(DistanceFunctionFactory) gaia2::LayoutAwareFactory< QString, gaia2::DistanceFunction, gaia2::PointLayout, gaia2::ParameterMap >;

%extend gaia2::LayoutAwareFactory< QString, gaia2::DistanceFunction, gaia2::PointLayout, gaia2::ParameterMap > {

  static std::vector<std::string> keys() {
    return convert::QStringList_to_VectorString(DistanceFunctionFactory::keys());
  }

  static gaia2::DistanceFunction* create(const std::string& name,
                                         const PointLayout& layout) {
    return DistanceFunctionFactory::create(QString::fromUtf8(name.c_str(), name.size()), layout);
  }

  static gaia2::DistanceFunction* create(const std::string& name,
                                         const gaia2::PointLayout& layout,
                                         const gaia2::ParameterMap& pmap) {
    return DistanceFunctionFactory::create(QString::fromUtf8(name.c_str(), name.size()), layout, pmap);
  }

}


%template(FrozenDistanceFactory) gaia2::LayoutAwareFactory< QString, gaia2::FrozenDistance,
                                                            gaia2::FrozenDataSet, gaia2::ParameterMap >;

%extend gaia2::LayoutAwareFactory< QString, gaia2::FrozenDistance, gaia2::FrozenDataSet, gaia2::ParameterMap > {

  static std::vector<std::string> keys() {
    return convert::QStringList_to_VectorString(FrozenDistanceFunctionFactory::keys());
  }

  static gaia2::FrozenDistance* create(const std::string& name,
                                       const FrozenDataSet& dataset) {
    return FrozenDistanceFunctionFactory::create(QString::fromUtf8(name.c_str(), name.size()), dataset);
  }

  static gaia2::FrozenDistance* create(const std::string& name,
                                       const gaia2::FrozenDataSet& dataset,
                                       const gaia2::ParameterMap& pmap) {
    return FrozenDistanceFunctionFactory::create(QString::fromUtf8(name.c_str(), name.size()), dataset, pmap);
  }

}
