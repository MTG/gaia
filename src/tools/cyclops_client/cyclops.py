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
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see http://www.gnu.org/licenses/



import httplib, urllib, time
import gaia2.fastyaml as yaml

VERBOSE = False

class YamlRPCMethod(object):
    def __init__(self, endPoint, methodName):
        self.endPoint = endPoint
        self.methodName = methodName

    def __call__(self, *args, **kwargs):
        if kwargs:
            raise NotImplementedError('Cannot use keyword arguments with YamlRPC at the moment...')

        if VERBOSE: serializeStart = time.time()

        try:
            q = yaml.dump({ 'method': self.methodName,
                            'params': list(args),
                            'id': 'gloubi-boulga'
                            })
        except:
            raise RuntimeError('Could not serialize Yaml request, most likely one of the arguments could not be serialized:\n%s' % list(args))

        if VERBOSE:
            responseTime = time.time() - serializeStart
            print 'serialized request in %f seconds' % responseTime


        # we don't want the '+'-quoting
        params = urllib.urlencode({ 'q': q }).replace('+', ' ')

        headers = { 'Content-type': 'application/x-www-form-urlencoded',
                    'Accept': 'text/plain'
                    }

        if VERBOSE: startTime = time.time()

        conn = httplib.HTTPConnection(self.endPoint)

        try:
            conn.request('POST', '/', params, headers)
        except Exception, e:
            raise RuntimeError('request failed', self.endPoint, self.methodName, args, e)

        response = conn.getresponse()

        if VERBOSE:
            responseTime = time.time() - startTime
            print 'received answer in %f seconds' % responseTime
            #print response.status, response.reason

            startParseTime = time.time()

        result = yaml.load(response.read())

        if VERBOSE:
            responseTime = time.time() - startParseTime
            print 'parsed answer in %f seconds' % responseTime

            responseTime = time.time() - serializeStart
            print 'total time: %f seconds' % responseTime

        if 'error' in result:
            raise RuntimeError(result['error']['message'])

        return result['result']


class ResultSet(YamlRPCMethod):
    def __init__(self, endPoint, methodName, args, kwargs):
        YamlRPCMethod.__init__(self, endPoint, 'chainedSearch')

        self.queries = []
        if 'inputSpace' in kwargs:
            self.queries = kwargs.pop('inputSpace').queries

        if kwargs:
            raise NotImplementedError('Cannot use keyword arguments (apart from inputSpace) with YamlRPC at the moment...')

        self.queries += [ (methodName, args) ]

    def __call__(self, *args, **kwargs):
        raise NotImplementedError('Cannot directly call a ResultSet. Please use the get() method...')

    def get(self, n, offset = 0, keys = []):
        results = YamlRPCMethod.__call__(self, self.queries, keys, n, offset)
        """header = results['header']
        if len(header) > 0:
            results = [(id, weight, dict(zip(header, values))) for ((id, weight), values) in zip(results['results'], results['values'])]
        else:
            results = [(id, weight, {}) for (id, weight) in results['results']]
        return results"""
        header = results['header']
        if not header:
            for pid, dist in results['results']:
                yield (str(pid), dist, {})
        else:
            for ((pid, dist), v) in zip(results['results'], results['values']):
                yield (str(pid), dist, dict(zip(header, v)))

    def limit(self, n):
        raise NotImplementedError('ResultSet does not allow calling limit() on it yet...')

    def thresholdLimit(self, maxDist):
        self.queries += [ ('thresholdLimit', (maxDist,)) ]
        return self


class CyclopsRPCMethod(YamlRPCMethod):
    def __call__(self, *args, **kwargs):
        # pre-check for errors that might happen very often and where 1 good error message would
        # be real nice to have
        if (self.methodName.startswith('nnSearch') and
            self.methodName not in ('nnSearchById', 'nnSearchByIdWithFilter',
                                    'nnSearchByExample', 'nnSearchByExampleWithFilter')):
            raise AttributeError('You need to use either nnSearchById{WithFilter} or nnSearchByExample{WithFilter}')

        # pre-processing for certain specific methods
        if self.methodName.startswith('nnSearchByExample'):
            args = (args[0].toBase64(),) + args[1:]

        # in the case of an nnSearch request, we shouldn't do the query immediately but rather
        # return a proxy object that allows to chain queries using the search_space argument.
        # the actual query should only be resolved when the user calls the get() method on this
        # proxy object
        if self.methodName.startswith('nnSearch'):
            return ResultSet(self.endPoint, self.methodName, args, kwargs)

        # actual processing by the server
        result = YamlRPCMethod.__call__(self, *args, **kwargs)

        # post-processing for certain specific methods
        if self.methodName == 'layout':
            result = yaml.load(result)

        elif self.methodName == 'getPoint':
            try:
                import gaia2
            except ImportError:
                raise ImportError('You need to have the gaia2 python module installed in order to be able to retrieve single points')
            p = gaia2.Point()
            p.fromBase64(result)
            result = p

        elif self.methodName == 'getPoints':
            try:
                import gaia2
            except ImportError:
                raise ImportError('You need to have the gaia2 python module installed in order to be able to retrieve points')
            ds = gaia2.DataSet()
            ds.fromBase64(result)
            result = ds

        return result


class Cyclops(object):
    def __init__(self, endPoint):
        self.endPoint = endPoint

    def __getattr__(self, methodName):
        # a little tip so that ipython doesn't get lost when autocompleting
        if methodName in [ '__methods__', 'trait_names', '_getAttributeNames', '__members__' ]:
            raise AttributeError, methodName

        return CyclopsRPCMethod(self.endPoint, methodName)
