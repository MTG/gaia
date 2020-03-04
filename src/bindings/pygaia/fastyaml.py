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


from __future__ import print_function

try:
    import yaml

    if hasattr(yaml, 'CLoader'):
        load = lambda x: yaml.load(x, yaml.CLoader)
        load_all = lambda x: yaml.load_all(x, yaml.CLoader)
    else:
        load = yaml.load
        load_all = yaml.load_all

    if hasattr(yaml, 'CDumper'):
        dump = lambda x, stream = None: yaml.dump(x, stream, Dumper=yaml.CDumper)
        safe_dump = lambda x, stream = None: yaml.dump(x, stream, Dumper=yaml.CSafeDumper)
    else:
        dump = yaml.dump
        safe_dump = yaml.safe_dump

except ImportError:
    print('WARNING: could not import yaml module')


def loadfile(filename):
    return load(open(filename).read())
