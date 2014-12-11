#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

import sys, os.path, json
import gaia2.fastyaml as yaml
from optparse import OptionParser


def convertJsonToSig():
    parser = OptionParser(usage = '%prog [options] filelist_file result_filelist_file\n' +
"""
Converts json files found in filelist_file into *.sig yaml files compatible with
Gaia. The result files are written to the same directory where original files were 
located.
"""
        )

    options, args = parser.parse_args()

    try:
        filelist_file = args[0]
        result_filelist_file = args[1]
    except:
        parser.print_help()
        sys.exit(1)

    fl = yaml.load(open(filelist_file, 'r'))

    result_fl = fl
    errors = []

    for trackid, json_file in fl.iteritems():
        try:
            data = json.load(open(json_file))

            # remove descriptors, that will otherwise break gaia_fusion due to incompatibility of layouts
            if 'tags' in data['metadata']:
                del data['metadata']['tags']
            if 'sample_rate' in data['metadata']['audio_properties']:
                del data['metadata']['audio_properties']['sample_rate']

            sig_file = os.path.splitext(json_file)[0] + '.sig'
            
            yaml.dump(data, open(sig_file, 'w'))           
            result_fl[trackid] = sig_file

        except:
            errors += [json_file]

    yaml.dump(result_fl, open(result_filelist_file, 'w'))
    
    print "Failed to convert", len(errors), "files:"
    for e in errors:
        print e
    return len(errors)

if __name__ == '__main__':
    convertJsonToSig()
