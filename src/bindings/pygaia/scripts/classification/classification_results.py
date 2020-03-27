import glob
import os.path

from gaia2.utils import TextProgress
import gaia2.fastyaml as yaml
from gaia2.classification import ConfusionMatrix



class ClassificationResults(object):
    def __init__(self):
        self.results = []

    def read_results(self, dir):
        """Reads all the results file contained in the given directory, and generates the
        associated ConfusionMatrix for each one."""

        resultFiles = glob.glob(os.path.join(dir, '*.result'))
        progress = TextProgress(len(resultFiles))

        for i, filename in enumerate(resultFiles):
            cm = ConfusionMatrix()
            cm.load(filename)

            paramFile = os.path.splitext(filename)[0] + '.param'
            params = yaml.load(open(paramFile).read())

            self.results += [(filename, cm, params)]

            progress.update(i + 1)

    def best(self, n=10, classifier_type=None):
        if classifier_type is not None:
            r = [(cm.correct() * 100. / cm.total(), cm.stdNfold(normalizedAccuracies=False),
                  cm.normalizedAccuracy(), cm.stdNfold(normalizedAccuracies=True), filename, param)
                 for (filename, cm, param) in self.results if param['model']['classifier'] == classifier_type]
        else:
            r = [(cm.correct() * 100. / cm.total(), cm.stdNfold(normalizedAccuracies=False),
                  cm.normalizedAccuracy(), cm.stdNfold(normalizedAccuracies=True), filename, param)
                 for (filename, cm, param) in self.results]

        print("number of results: {}".format(len(r)))
        return sorted(r, reverse=True)[:n]

    def filter_results(self, filters):
        result = self.results
        for param, value in filters.items():
            result = [(filename, cm, param) for (filename, cm, param) in result if param['model'][param] == value]

        return result
