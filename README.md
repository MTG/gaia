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
  * SWIG >= 1.3.31


INSTALL
-------

## Linux

- Install dependencies (Ubuntu/Debian)
   ```
   $ apt-get install build-essential libqt4-dev libyaml-dev swig python-dev pkg-config
   ```

   Note that Gaia build will fail if you are using swig 3.0.8. Install either a previous or later version. You will encounter this problem if you are using swig package distributed with Ubuntu 16.04. In this case install the newest swig version from source (https://github.com/swig/swig).


- Online help for WAF (build system)
   ```
   $ ./waf --help
   ```
   
- Configure with the desired options
    ```
    $ ./waf configure [--with-python-bindings] [--with-stlfacade] [--with-asserts] [--with-cyclops]
    ```    
    NOTE: in order to link Essentia library with Gaia, do not use --with-stlfacade option

- Compile libgaia.a:
    ```
    $ ./waf
    ```
    
- Install (to install system-wide you might need ```sudo```)
    ```
    $ ./waf install [--destdir=/where/ever/]
    ```
    
- Build documentation (optional), it will be located at build/doc/ folder
    ```
    $ ./python src/doc/regenerate_docstring.py
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

This library contains source code from FrogLogic command line parser
(http://www.froglogic.com/pg?id=PublicationsFreeware&category=getopt)
which is distributed under the BSD license.
