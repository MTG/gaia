/**

@mainpage Gaia Documentation

@section intro_sec Introduction


Gaia is a general library to deal with points in a
<a href="http://en.wikipedia.org/wiki/Semimetric_space">semimetric space</a>,
although it can also deal with points in a space with any associated real-valued
binary function (but then you lose many nice properties that make Gaia useful
in the first place).

In simple words, Gaia allows you to define the following:

 - a structure for your space, which will be determined by the PointLayout.
   Each point has exactly one PointLayout, and as such, points which share the same
   layout are said to be in the same space. A PointLayout actually contains a list
   of dimensions (real or string) which contains attributes of each point, as well
   as their associated names and a tree structure to organize them (which you can
   forget about if you don't care for a specific reason)
 - a DataSet, which is basically a collection of Points. All Points in a DataSet
   share the same layout.
 - Transformations, which allow you to take Points in a DataSet (or the whole DataSet)
   and apply them a transformation that returns a new Point (or DataSet) in a
   possibly different output space (eg: with a different PointLayout)
 - distance functions, which return the distance between two points in a specific space

@section documentation Documentation

There is a @ref tutorial available to teach you the basics of using Gaia.

There is also a @ref api_overview available that gives a more general view, but which is
less guided than the tutorial.

Once you are familiar with Gaia's basics, you might want to check another
tutorial on a concrete task: @ref fingerprint_tutorial.

For the hardcore Gaia users, here's a page explaining @ref dealing_with_vld.

To know more about algorithms, check the @ref algorithms reference page.

For the metrics, please check the @ref metrics reference page.

*/
