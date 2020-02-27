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
import sys
from gaia2.classification import ConfusionMatrix


try:
    results = sys.argv[1]
    output_html = sys.argv[2]
except:
    print('Usage: %s <results_file> <confusion_matrix_html_file>' % sys.argv[0])
    exit(1)

cm = ConfusionMatrix()
cm.load(results)
open(output_html, 'w').write(cm.toHtml())


