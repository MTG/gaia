#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os.path

class MtgdbPath:
    def __init__(self, collection, essentiaVersion, audioFormat = '', mtgdbRoot = '/raid/audio'):
        self._mtgdbRoot = mtgdbRoot
        self._collection = collection
        self._essentiaVersion = essentiaVersion
        self._audioFormat = audioFormat

    def __str__(self):
        return os.path.join(self._mtgdbRoot,
                            self._collection,
                            'descriptors',
                            self._essentiaVersion,
                            self._audioFormat)
