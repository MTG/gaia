/**

@defgroup algorithms Algorithms



Algorithms in Gaia are used to process a DataSet. At the moment, there exist only two types
of algorithms, which are complementary: the @ref analyzers "Analyzers" and the @ref appliers "Appliers".

Analyzers generally are the action you want to apply to the DataSet, and represent the intention.
Example: clean, normalize, etc...
By analyzing a DataSet, you will get a Transfo object, which contains the parameters for the analysis,
but also the name of the corresponding Applier and its parameters. This is what you need to apply on a
DataSet to finalize the transformation. By themselves, analyzers do not touch the DataSet at all.

The Appliers are the actual transformations to apply to the DataSet to make the operation effective.
Sometimes they do not describe the original intention, or a same Applier can be used by different
Analyzers. For instance, both Clean and Remove Analyzers use the RemoveDesc applier, because they both
remove descriptors from the DataSet.

Let's take an example:

@verbatim
dataset = gaia.DataSet()
dataset.load('myfile.db')
@endverbatim

If you want to remove descriptors from a DataSet, you would take the @c Remove analyzer. By analyzing
the DataSet, it will return a Transformation object, which has the @c RemoveDesc applier with the list
of descriptors to remove. You just need to apply this on the DataSet and you will get a new DataSet
without these descriptors. This gives:

@verbatim
# first, create the desired Analyzer instance with parameters
remove = AnalyzerFactory.create('remove', { 'descriptorNames': [ '*.min', '*.max' ] })

# then, we need to analyze the dataset to retrieve the parameters of the actual transformation
transfo = remove.analyze(dataset)

# all done, let's apply this to our old dataset to get a fresh new one, without the min and max descriptors
newDataset = transfo.applyToDataSet(dataset)
@endverbatim

And now that you've understood the theory, here's how to accomplish the same in a single line!

@verbatim
newDataset = transform(dataset, 'remove', { 'descriptorNames': [ '*.min', '*.max' ] })
@endverbatim



@defgroup analyzers Analyzer Algorithms
@ingroup algorithms

Here is a reference list of available Analyzers. If you want more detailed explanation about what
an Analyzer is, please refer to the @ref algorithms page.

@defgroup datasetmanagementalgos Dataset Management Algorithms
@ingroup analyzers

These are the algorithms that are related to datasets management tasks, such as removing
certain descriptors, merging them, ...

@defgroup datatransformationalgos Numerical Data Transformations
@ingroup analyzers

These are the algorithms which perform an actual numerical transformation on the input data,
such as normalizing it, etc...



@defgroup appliers Applier Algorithms
@ingroup algorithms

Here is a reference list of available Appliers. If you want more detailed explanation about what
an Applier is, please refer to the @ref algorithms page.

*/
