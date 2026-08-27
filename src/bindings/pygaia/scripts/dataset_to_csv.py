#!/usr/bin/env python

# Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
#
# This file is part of Gaia
#
# Gaia is free software: you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation (FSF), either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the Affero GNU General Public License     
# version 3 along with this program. If not, see http://www.gnu.org/licenses/


from __future__ import print_function
import argparse
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
    parser = argparse.ArgumentParser(description='Convert a Gaia dataset to CSV format.')
    parser.add_argument('gaia_dataset', help='the Gaia dataset file (.db)')
    parser.add_argument('output_csv', help='the output CSV file')
    args = parser.parse_args()

    dataset_to_csv(args.gaia_dataset, args.output_csv)
