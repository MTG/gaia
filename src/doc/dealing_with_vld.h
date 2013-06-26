/* 
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 * 
 * Gaia is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU Affero General Public License as published by the Free 
 * Software Foundation (FSF), either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/**

@page dealing_with_vld How to deal with very large datasets

@section introduction Introduction

When trying to deal with very large datasets (VLD), depending on your hardware
configuration, you might encounter some problems, most likely memory-related.
For instance, you might not have enough memory to merge your dataset or transform
it to the adequate form.
Luckily, there are various ways to achieve your goals. This page will teach you
what you can do in order to save memory and be able to use Gaia more efficiently.

@section vld_merging Merging your DataSet of a million or more points

The biggest problem you will encounter with respect to memory will be the initial
merging of the dataset.

This happens because at this point you have a dataset which probably contains all the
descriptors that can be found in a signature file, multiplied by the number of
points you are actually trying to merge.

There are various ways to overcome this limit, which we will examine more in details
in the following sections. You can:
 - ignore some descriptors at the time of merging
 - merge by chunks, then remove useless descriptors
 - use the @c 'FixLength' transformation, which also provides some warranties on
   the size of descriptors, which might be needed by further transformations.

@section vld_transfo Useful (necessary) transformations you might want to apply

When merging a VLD, or parts of it, there are some transformations that you
might want to use.

First of all, regardless of whether you're working on large or small datasets,
you should not forget about the @c 'Cleaner' transformation. This will remove all
NaN and INFs numbers, and will ensure that your dataset is ready for numeric
transformations such as PCA, RCA, etc...

The next pair of transformations you might want to know about is
@c 'RemoveVL' / @c 'FixLength'.

@c 'RemoveVL' is a transformation that removes all the descriptors which have been
found to be of variable length, such as the onsets list, the chords progression,
etc... YMMV, but these descriptors are not used at the moment for recommendation
and they occupy quite a bit of memory, so you might want to drop them.

@c 'FixLength' is a transformation that doesn't touch the data in your dataset at all,
so you end up with exactly the same information after applying it than before.
However, it is very useful and you should think of applying it all the time, preferably
at the beginning of your transformation history. The reasons are two-fold:
 - first, it cuts down on the memory usage, and you might witness memory savings
   of more than a factor two.
 - second, it flags those descriptors to be of fixed-length, which has the advantage
   that most of the transformations might be faster on them. Moreover, in the future certain
   transformations will only work on fixed-length descriptors.

Then, you might want to use the @c 'Select' or the @c 'Remove' transformation, which
selects (respectively removes) the specified descriptors. Please refer to the
@ref algorithms documentation for more detailed information on how to use those.


@section vld_merging_chunks Merging the chunks you now have reduced to an acceptable size

Now that you have reduced your chunks to a smaller size, keeping only the
descriptors you need, it is time to finally merge it all in one single dataset.
There is a script, located at @c gaia/src/scripts/concatenate_datasets.py, which will
do exactly that for you.
Just run it on all your chunks to get your final dataset (there is some online
documentation).



@section vld_queries Querying of nearest-neighbors with filters

When doing nearest neighbor queries with filters, they might seem too slow. One
(quite efficient) way to deal with this issue is to index the @c View on a given
@c DataSet.
To do that, you need to call the @c View::indexOn(name) method from the View class.
Just specify the name of the descriptor you want to index on (either Real or String)
and the @c View will create an index on the space so that the View will provide
much faster filtering for that specific descriptor.

You can create as many indices as you want on a @c DataSet, and it is actually recommended
to create one for each descriptor you intend to use in filtering. The cost (computation
time) to build those is quite small, and will probably be amortized after the first or
second query.


<b>Release notes:</b> at the moment, negation (NOT X) is not (poorly?) implemented so
you can use it but you won't benefit of any optimization. Actually, having a single
negation in a filter will disable any optimization, hence it is recommended to try
to avoid it, for instance by changing filters like <tt>"WHERE NOT bpm > 120"</tt> to
<tt>"WHERE bpm <= 120"</tt>.

 */
