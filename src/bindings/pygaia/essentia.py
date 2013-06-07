#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, os.path
import fnmatch
import subprocess
import multiprocessing
import sys
import signal


def analyzeSingleFile(extractorPath, inputFile, outputFile, progress = None, overwrite = False, captureStdout = False):
    # make sure output directory exists
    outputDir = os.path.split(outputFile)[0]
    try:
        os.makedirs(outputDir)
    except OSError:
        pass

    curdir = os.getcwd()
    extractorDir, extractor = os.path.split(extractorPath)

    inputFile = os.path.abspath(inputFile)
    outputFile = os.path.abspath(outputFile)

    stdout = '*' * 100 + '\n'
    stdout += 'Analyzing file: %s%s\n' % (inputFile, ' [%d/%d]' % progress if progress else '')
    stdout += 'Results into:   %s\n\n' % outputFile

    if not overwrite and os.path.exists(outputFile):
        stdout += 'Skipping file, result already computed...\n'

        if captureStdout:
            return stdout
        else:
            print stdout,
            sys.stdout.flush()
            return

    # always run from the extractor directory, as it might be accompanied by SVM models
    os.chdir(extractorDir)
    args = [ './'+extractor, inputFile, outputFile ]
    if captureStdout:
        pstdout, _ = subprocess.Popen(args, stdout = subprocess.PIPE).communicate()
        stdout += pstdout

    else:
        print stdout,
        sys.stdout.flush()
        subprocess.Popen(args).communicate()

    os.chdir(curdir)
    if captureStdout:
        return stdout


def analyze(args):
    try:
        extractorPath, (inputFile, outputFile), progress = args
        stdout = analyzeSingleFile(extractorPath, inputFile, outputFile, progress, captureStdout = True)
        # print all at once to try to prevent output to be interwoven
        # Note: this just makes it unlikely, not impossible. To do this, one would
        #       have to lock stdout when printing on it
        print stdout,

    except KeyboardInterrupt:
        print 'Child process interrupted, stopping parent as well...'
        os.kill(os.getppid(), signal.SIGQUIT) # SIGQUIT might seem a bit brutal as we will kill the interpreter if running it interactively, but I couldn't figure out a way to propagate the exception to the parent process properly
        sys.exit(1)


def _analyzeFiles(extractorPath, filelist):
    """Analyze the given list of files.
    'filelist' is a list of tuples (inputFile, outputFile)."""
    NCPUS = multiprocessing.cpu_count()
    nfiles = len(filelist)
    filelist = sorted(filelist)

    print 'Found %d files to analyze...' % nfiles
    if not filelist:
        return

    if NCPUS == 1:
        print 'Found 1 CPU core, running the analysis in a single process...'

        for i, (inputFile, outputFile) in enumerate(filelist):
            analyzeSingleFile(extractorPath, inputFile, outputFile, (i+1, nfiles))
    else:
        print 'Found %d CPU cores, running the analysis using that many processes...' % NCPUS
        pool = multiprocessing.Pool(NCPUS)

        arglist = [ (extractorPath, f, (i+1, nfiles)) for i, f in enumerate(filelist) ]
        pool.map(analyze, arglist, chunksize = 1) # chunksize = 1 so as not to fuck up the progress ordering


def analyzeFiles(extractorPath, outputDir, audioDir, exts = [ '*' ]):
    """Analyze all audio files found in the given audioDir using the extractor and
    write the resulting sigfiles in the output dir, preserving the same hierarchy."""

    if isinstance(exts, basestring):
        exts = [ exts ]

    filelist = []

    print 'Finding list of files to analyze...'

    for root, dirs, files in os.walk(audioDir, followlinks = True):
        for f in files:
            for ext in exts:
                if fnmatch.fnmatch(f, ext):
                    inputFile = os.path.join(root, f)
                    outputFile = os.path.join(outputDir, os.path.relpath(root, audioDir), f) + '.sig'

                    filelist.append((inputFile, outputFile))
                    break

    _analyzeFiles(extractorPath, filelist)


def sigfileList(collection, outputDir):
    """Return a list of associated pairs (audio, sigfile) for this collection."""
    filelist = []

    absfiles = collection.files()
    relfiles = collection.relativePathFiles()

    for id, path in absfiles.items():
        filelist.append((path, os.path.join(outputDir, relfiles[id]) + '.sig'))

    return filelist


def analyzeCollection(extractorPath, outputDir, collection):
    """Analyze all audio files found in the given mtgdb.Collection using the extractor and
    write the resulting sigfiles in the output dir, preserving the same hierarchy."""

    print 'Analyzing %r' % collection
    filelist = sigfileList(collection, outputDir)

    _analyzeFiles(extractorPath, filelist)



def usage(msg = None):
    print 'Usage: %s essentia_extractor audio_dir sigs_dir' % sys.argv[0]
    print 'Where:'
    print '  essentia_extractor: is the path to the Essentia extractor that you want to'
    print '                      use to extract the features'
    print '  audio_dir: is the directory in which your audio files are located; it will'
    print '             be searched recursively for all files contained in there'
    print '  sigs_dir: is the directory where to write the signature files. The hierarchy'
    print '            found in audio_dir will be preserved here.'

    if msg:
        print
        print 'ERROR:', msg

    sys.exit(1)


if __name__ == '__main__':
    if len(sys.argv) != 4:
        usage('wrong number of arguments')

    extractor = sys.argv[1]
    audioDir = sys.argv[2]
    outputDir = sys.argv[3]

    if not (os.path.isfile(extractor) and os.access(extractor, os.X_OK)):
        usage('extractor needs to be an executable file')

    analyzeFiles(extractor, outputDir, audioDir)
