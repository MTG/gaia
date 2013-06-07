#!/usr/bin/env python
# -*- coding: utf-8 -*-

import gaia2
import gaia2.fastyaml as yaml
import environment
import collection
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
            raise ValueError('Collection "%s" is not known by Gaia. Available collections are: %s' % (name, ALL_MTGDB_COLLECTIONS.keys()))

        super(MtgdbCollection, self).__init__(collection['location'], groundTruth)
        self.name = name
        self._properties = collection

    def __repr__(self):
        return '<MTGDB Collection "%s", type = %s>' % (self.name, self._properties['type'])


