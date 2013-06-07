#include <QFile>
#include "dataset.h"
#include "utils.h"
using namespace gaia2;

#define MAGIC_NUMBER 0x6AEA7230

class SchizoDataSet : public DataSet {
 public:
  void load22NoHistory(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
      throw GaiaException("Could not open file: ", filename);
    }

    QDataStream in(&file);

    quint32 magic;
    in >> magic;

    if (magic != MAGIC_NUMBER) {
      throw GaiaException("File does not seem to be a Gaia dataset file.");
    }

    checkValidDataStream(in);

    if (gaiaVersion(in) != Gaia_2_1) {
      throw GaiaException("DataSet hasn't been created with Gaia 2.1 or 2.2");
    }

    clear();

    in >> _name;


    // we don't want to actually load the history in the dataset here
    QList<Transformation> dummy;
    in >> dummy;
    // there's the original layout as well
    PointLayout dummyl;
    in >> dummyl;


    // read layout
    in >> _layout;

    // read collections
    qint32 size, csize;
    QString name;
    PointLayout layout;

    // pretend to load collections, then load points from them
    in >> size;
    for (int i=0; i<size; i++) {
      in >> name;

      // load collection
      in >> name;
      in >> csize;

      for (int j=0; j<csize; j++) {
        Point* p = new Point();
        in >> *p;
        *this << p;
      }
    }

    checkUniqueIDs();
    unifyLayout();
  }

};


int main(int argc, char* argv[]) {
  if (argc != 3) {
    qDebug() << "You need to specify both an input and an output dataset filename.";
    qDebug() << "Exiting...";
    exit(1);
  }

  gaia2::init();

  SchizoDataSet ds;
  ds.load22NoHistory(argv[1]);

  DataSet* result = transform(&ds, "fixlength");

  QStringList dnames = result->layout().descriptorNames();
  foreach (const QString& dname, dnames) {
    // work around a bug in libyaml, key.size is maxed at 1024
    if (dname.size() > 1023) {
      ParameterMap params;
      params.insert("descriptorName", dname);
      params.insert("newName", dname.left(1023));

      // this leaks as hell, but we don't care, the OS can do the cleaning for us...
      result = transform(result, "rename", params);
    }
  }

  result->save(argv[2]);

  delete result;
  gaia2::shutdown();

  return 0;
}
