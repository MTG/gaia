%newobject gaia2::Factory< QString, gaia2::Analyzer, gaia2::ParameterMap >::create;


template < typename IdentifierType,
           typename AbstractProduct,
           typename ArgumentType >
class Factory {

 public:

  static Factory& instance() {
    static Factory _instance;
    return _instance;
  }

  static AbstractProduct* create(const IdentifierType& id,
                                 const ArgumentType& arg = ArgumentType());

  AbstractProduct* create_i(const IdentifierType& id, const ArgumentType& arg) const;


};

%template(AnalyzerFactory) gaia2::Factory< QString, gaia2::Analyzer, gaia2::ParameterMap >;

%extend gaia2::Factory< QString, gaia2::Analyzer, gaia2::ParameterMap > {

  static std::vector<std::string> keys() {
    return convert::QStringList_to_VectorString(AnalyzerFactory::keys());
  }
}
