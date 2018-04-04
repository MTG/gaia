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



from __future__ import absolute_import
import gaia2
import gaia2.fastyaml as yaml
from . import environment
from . import collection
from os.path import join

COLLECTIONS_FILE = join(gaia2.filedir(), 'mtgdb_collections.yaml')
ALL_MTGDB_COLLECTIONS = dict((c['name'], c) for c in yaml.load(open(COLLECTIONS_FILE).read()))


class MtgdbCollection(collection.Collection):
    """An mtgdb.MtgdbCollection instance is a collection on the MTG-DB server that
    has been classified as stable and may be accessed directly by its name."""

    def __init__(self, name, groundTruth = None):
        try:
            collection = ALL_MTGDB_COLLECTIONS[name]
        except KeyError:
            raise ValueError('Collection "%s" is not known by Gaia. Available collections are: %s' % (name, list(ALL_MTGDB_COLLECTIONS.keys())))

        super(MtgdbCollection, self).__init__(join(environment.MTGDB_AUDIO,collection['location']), groundTruth)
        self.name = name
        self._properties = collection

    def __repr__(self):
        return '<MTGDB Collection "%s", type = %s>' % (self.name, self._properties['type'])


