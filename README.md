Gaia 2
======

ABOUT
-----

Gaia is a C++ library with python bindings which implements similarity measures and classiﬁcations on the results of audio analysis, and generates classiﬁcation models that Essentia can use to compute high-level description of music. 

Licence: [Affero GPLv3 license](http://www.gnu.org/licenses/agpl.html)

Documentation: http://essentia.upf.edu/documentation/gaia 


Dependencies:

  * Qt >= 4.5
  * libYAML >= 0.1.1
  * Python >= 2.4
  * SWIG != 3.0.8
  * waf >= 1.9
  * Eigen 3.3.4 patched version (included in source)


INSTALL
-------

## Linux

- All features work except `project_file.py`.  Build with `--with-cyclops`(for `qt4) `works as before.  Testing `--with-cyclops --with-gaia-qt5`but it seems to function as before. 

- With `python3` running `python3  tests.py`

   `Ran 91 tests in 61.252s`

   `FAILED (failures=5, errors=36)` Unresolved `python3` issues with numerics and enumerate.

   After building documentation `build/src/bindings/gaia2.py` must have `# -*- coding: utf-8 -*-` as first or second line.  This is automatically handled.  Switching from `python2` to `python3`may require  `build` folder to be removed.

- With `python2` running `python2  tests.py`

   `Ran 91 tests in 61.252s`

   `FAILED (failures=5, errors=37)` Unresolved `python2` issues.

- Install dependencies (Ubuntu/Debian)

   ```
   $ apt-get install build-essential libqt4-dev libyaml-dev swig python-dev pkg-config doxygen 
   ```
   ​     or for `Qt5`
   ```
   $ apt-get install build-essential qt5-default qtbase5-dev libyaml-dev swig python3-dev pkg-config doxygen
   ```

   Note that `Gaia` build may fail if you are using `swig 3.0.8`. Install either a previous or later version. The newest `swig` version is available from source `(https://github.com/swig/swig)`.

   ​


- Relase Notes: `Eigen 3.3.4`is provided with source code since one header file was missing from the `release`archive and for `c++11` compatibility.  Previous `Eigen`had too many missing header or `include` files for version certainty.  `#define Vector` was replaced by `std::vector` to eliminate `c++11` errors.  Location of `Eigen` headers changed to `3rdparty/Eigen`rather than `Eigen. Waf2.0.6`is used primarily because it handles `Qt5`and the`moc` files required for `cyclops`. `libtbb-dev`can optionally be installed but the check for `tbb` has been removed  from the build process because it didn't work.  `Python2.7.15`and `python3.6.3`both have been tested.  However, not all versions of `python` have been used.  Python packages are located at either `/usr/local/lib/python2.7/dist-packages` or `/usr/local/lib/python3.6/dist-packages`and utilize the `major` and `minor` `python` version numbers.  Addendum:  All tabs converted to spaces

   `find ./ -iname '*.h' -type f -exec bash -c 'expand -t 4 "$0" | sponge "$0"' {} \;`

   `find ./ -iname '*.cpp' -type f -exec bash -c 'expand -t 4 "$0" | sponge "$0"' {} \;`

   `find ./ -iname '*.py' -type f -exec bash -c 'expand -t 2 "$0" | sponge "$0"' {} \;`

- Online help is available for WAF or in build system home folder.

   ```
   $ git clone https://github.com/waf-project/waf.git
   $ cd waf
   $ ./waf-light --help
   $ ./waf-light configure --tools=swig,qt5,qt4 build
   $ cp waf ~/gaia/.
   ```

- Configure `gaia` with the desired options, if `python3` wanted use
    ```
    $ python3 ./waf configure [--with-python] [--with-stlfacade] [--with-asserts] 
    [--with-cyclops] [--with-tests] [--with-gaia-qt5]
    ```
    ***NOTE: in order to link Essentia library with Gaia, do not use `--with-stlfacade` option***

- Compile `libgaia.a`or `_gaia2.so`:
    ```
    $ python3 ./waf
    ```
    
- Install (to install system-wide you might need ```sudo```)
    ```
    $ python3 ./waf install [--destdir=/where/ever/]
    ```
    ***NOTE: be careful to use `python3` consistently as `./waf install`on its own will use the default `python.`***

- Build documentation (optional or online), it will be located at build/doc/ folder

    ```
    $ python3 src/doc/regenerate_docstring.py
    ```

## MacOS X

### Build from command-line using Homebrew (recommended):
- Install Qt4:
    ```
    brew install cartr/qt4/qt
    ```

- Install homebrew tap:
    ```
    brew tap MTG/essentia
    ```

- Build Gaia (this will also install all the dependencies)

    ```
    brew install gaia --HEAD
    ```

### Build from command-line:

- Install python, qt libraries 4.8, libYAML and swig dependencies using homebrew:	
    ```
    $ brew install python --framework
    $ brew install swig libyaml cartr/qt4/qt
    ```

- Install pyyaml pip package in case use want to build with python bindings:
    ```
    $ pip install pyyaml
    ```
    
- Configure and build similarly to Linux (see above).


### Build with QtCreator (alternative):

(Gaia2lib)

- install qt libraries 4.8 (including debug libraries) and QtCreator from http://qt-project.org/downloads

- install libYAML and swig dependencies using homebrew (we assume you already have a python 
  installation, otherwise you can also install it using homebrew):
    ```
    $ brew install swig
    $ brew install libyaml
    ```

- use QtCreator to open the project file 'Gaia2lib.pro' in packaging/darwin/Gaia2lib/

- compile the project (you will probably need to configure QtCreator to work with your Qt 
  installation and also to set up the build folder for the project)

(Gaia2Python - python bindings)

- use swig to generate the file 'gaia_wrap.cxx' that will be needed to compile 'Gaia2Python':
    ```
    $ swig -c++ -python -w451 /path_to_gaia_source/src/bindings/gaia.swig 
    ```

- copy the generated 'gaia_wrap.cxx' to the Gaia2Python project folder:
    ```
    $ cp /path_to_gaia_source/src/bindings/gaia_wrap.cxx /path_to_gaia_source/packaging/darwin/Gaia2Python/
    ```
    
- use QtCreator to open the project file 'Gaia2Python.pro' in packaging/darwin/Gaia2Python/ and compile

- run ./make_release_tarball in packaging/darwin:
    ```
    $ ./make_release_tarball
    ```

- copy the folder packaging/darwin/tmp/gaia2/python/gaia2 (created when running make_release_tarball.sh) 
  to the site-packages directory of your python distribution. you can now import gaia2 from python


## Windows

- use the QtCreator projects inside the packaging/win32 directory (not tested).
- using `msys64`and `mingw64_shell.bat`partially works but is currently not available and untested.



3RD PARTY
---------

This library contains source code from the LibSVM project, which is distributed
under the revised BSD license.
Please refer to the src/3rdparty/libsvm/COPYRIGHT file for more information.

This library contains the Mersenne Twister random number generator, which
is distributed under the BSD license.

This library contains source code from the Alglib project (http://www.alglib.net),
which is distributed under the 3-clause BSD license.

This library contains source code from the Eigen project (http://eigen.tuxfamily.org/),
which is distributed under the LGPLv3 license.

Cyclops for Qt5 contains source code from the Qt project deprecated APIs which have been removed from Qt (https://github.com/qt/qthttp), which is distributed under the terms specified under GNU GPLv3 license.  These have been heavily reorder to eliminate compile errors.

This library contains source code from FrogLogic command line parser
(http://www.froglogic.com/pg?id=PublicationsFreeware&category=getopt)
which is distributed under the BSD license.
