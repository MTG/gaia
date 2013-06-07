%extend gaia2::Segment {
}

%extend gaia2::Region {

  std::string name() {
    return self->name.toUtf8().data();
  }

  std::vector<gaia2::Segment> segments() {
    QList<gaia2::Segment> qlist = self->segments;
    std::vector<gaia2::Segment> svector;

    for (int i=0; i<qlist.size(); i++) {
      svector.push_back(qlist[i]);
    }

    return svector;
  }

}

