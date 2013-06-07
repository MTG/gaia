%extend gaia2::Factory<QString, gaia2::Analyzer, gaia2::ParameterMap> {

  static gaia2::Analyzer* create(const std::string& name) {
    return AnalyzerFactory::create(QString::fromUtf8(name.c_str(), name.size()));
  }

  static gaia2::Analyzer* create(const std::string& name,
                                 const ParameterMap& pmap) {
    return AnalyzerFactory::create(QString::fromUtf8(name.c_str(), name.size()), pmap);
  }

}
