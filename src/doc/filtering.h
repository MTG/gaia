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
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
/**
 * @page filtering Filtering queries
 *
 * When doing a search on a DataSet using a View, you are allowed to use a
 * filtering term. That means that of all the results found by the search, only
 * those that comply to the condition expressed in the filtering term will be
 * returned. In particular, that means that the filter term should be an
 * expression that evaluates to true or false for each point.
 *
 * @section prerequisites Prerequisites
 *
 * In order to be able to filter queries on a DataSet, you will first need 2 things:
 * <ul>
 *   <li>the DataSet in which you want to do the queries (you should have this
 *       one ready by now!)
 *   <li>a reference DataSet, which will be used to get the values used when
 *       filtering.
 * </ul>
 *
 * Let's say you have a DataSet on which you applied PCA, then all your dimensions
 * are mangled, and you have lost the original values. However, you still want to
 * be able to do stuff like "bpm > 120" on it. The reference DataSet is the DataSet
 * where the filter should be looking for these values.
 *
 * To set a DataSet as a reference DataSet for another one, just do the following:
 * <pre class="fragment">
 * dsQuery.setReferenceDataSet(dsRef)</pre>
 *
 * The reference DataSet needs to comply to 2 conditions before you can call this:
 * <ol>
 *   <li>it needs to be isomorph to the original one, which means that they must
 *       both have exactly the same collections with the same points inside. The
 *       layout can be different (otherwise it would be useless!), but there must
 *       be an exact mapping from points in one DataSet to the points in the other
 *       one.
 *   <li>it needs to be self-referencing. DataSets are self-referencing by default,
 *       so most of the time you won't have any problems. However, you cannot use as
 *       as reference a DataSet which is itself referencing another DataSet.
 * </ol>
 *
 * @section grammar Filter Grammar
 *
 * The corresponding grammar (expressed using EBNF notation) is the following:
 *
 * <pre class="fragment">
 * Filter    ::= [ 'WHERE' Predicate ]
 * Predicate ::= Boolean | PredComparison | PredBinaryOp | PredUnaryOp | '(' Predicate ')'
 * Predicate ::= Value 'BETWEEN' VALUE_CONSTANT 'AND' VALUE_CONSTANT
 * Predicate ::= String 'NOT'? 'IN' '(' StringList ')' | Value 'NOT'? 'IN' '(' ValueList ')'
 * &nbsp;
 * StringList ::= String | String ',' StringList
 * ValueList  ::= Value  | Value ',' ValueList
 * &nbsp;
 * ValueComparisonType  ::= '=' | '!=' | '<' | '<=' | '>' | '>='
 * StringComparisonType ::= '=' | '!='
 * PredComparison       ::= Value ValueComparisonType Value | String StringComparisonType String
 * &nbsp;
 * BinaryOp ::= '&&' | '||' | 'AND' | 'OR'
 * UnaryOp  ::= 'NOT'
 * &nbsp;
 * PredBinaryOp ::= Predicate BinaryOp Predicate
 * PredUnaryOp  ::= UnaryOp Predicate
 * &nbsp;
 * Boolean ::= 'TRUE' | 'FALSE'
 * Value   ::= VALUE_CONSTANT | VALUE_VARIABLE
 * String  ::= STRING_CONSTANT | STRING_VARIABLE</pre>
 *
 * <b>NB:</b> due a little weakness in the parser, you have to specify the type
 * of a variable before its name, like this:<br>
 * <code>value.tempotap_bpm.mean</code> to refer to the real value named tempotap_bpm.mean, or<br>
 * <code>label.key_mode.value</code> to refer the the string label named key_mode.value.
 *
 * @section examples Examples
 *
 * Here are some concrete examples of possible filter terms for those of you not
 * used to reading EBNF grammars! :-)
 *
 * <pre class="fragment">
 * WHERE value.tempotap_bpm.mean > 100
 * WHERE value.danceability < 3 AND (label.genre = "classical" OR label.genre = "jazz")
 * etc...</pre>
 *
 * @section indexing Indexing views on certain descriptors
 *
 * Starting with Gaia 2.2, you can now use indexation to speed up queries which use filters.
 * The idea is the same as a normal database, where you explicitly index using a certain
 * descriptor, and then all subsequent queries which make use of this descriptor will be faster.
 *
 * To index on a descriptor, just call the following:
 *
 * <pre class="fragment">
 * descName = 'rhythm.bpm'
 * v = View(dataset, dist)
 * &nbsp;
 * # here's the interesting part
 * v.indexOn(descName)</pre>
 *
 * And that's all there is to it!
 *
 */
