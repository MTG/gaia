#!/usr/bin/env python

import sys
import gaia2

def dataset_to_csv(filename, csv_filename):
    ds = gaia2.DataSet()
    ds.load(filename)

    out = open(csv_filename, 'w')

    valueNames = ds.layout().descriptorNames(gaia2.RealType)
    labelNames = ds.layout().descriptorNames(gaia2.StringType)

    out.write('Track name;')
    for name in labelNames:
        out.write('%s;' % name)
    for name in valueNames:
        out.write('%s;' % name)
    out.write('\n')

    for cname in ds.collectionNames():
        for pname in ds.collection(cname).pointNames():
            p = ds.collection(cname).point(pname)

            out.write('%s;' % pname)
            
            for name in labelNames:
                out.write('%s;' % p.label(name))

            for name in valueNames:
                out.write('%s;' % str(p.value(name)))

            out.write('\n')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'USAGE: %s gaia_dataset.db output.csv' % sys.argv[0]
        sys.exit(1)
        
    ds_filename = sys.argv[1]
    csv_filename = sys.argv[2]

    dataset_to_csv(ds_filename, csv_filename)
