Gaia 2.4.3
==========

**ABOUT**
-----

Dependencies:
  
  * Qt >= 4.5
  * libYAML >= 0.1.1
  * Python >= 2.4
  * SWIG >= 1.3.31


**INSTALL**
-------

*Linux*:

- Install dependencies (Ubuntu/Debian):

    $ apt-get install build-essential libqt4-dev libyaml-dev swig python-dev pkg-config

- Online help for WAF (build system)

    $ ./waf --help

- Configure with the desired options:

    $ ./waf configure --download [--with-python-bindings] [--with-stlfacade] [--with-asserts] [--with-cyclops]
    
    NOTE: in order to link Essentia library with Gaia, do not use --with-stlfacade option

- Compile libgaia.a:

    $ ./waf

    $ ./waf install [--destdir=/where/ever/]

- Build documentation (optional), it will be located at build/doc/ folder:

    $ ./python src/doc/regenerate_docstring.py

-------

*Mac OS X*:

Build from commmand-line (recommended):

- Install python, qt libraries 4.8, libYAML and swig dependencies using [Homebrew](http://brew.sh):	

    $ brew install python

    $ brew install swig libyaml qt

- Install pyyaml pip package in case you want to build with python bindings:

    $ pip install pyyaml

- Configure and build similarly to Linux (see above).

-------
*Build with QtCreator (alternative)*:

(Gaia2lib)

- Install qt libraries 4.8 (including debug libraries) and QtCreator from [here](http://qt-project.org/downloads).

- Install libYAML and swig dependencies using [Homebrew](http://brew.sh) (we assume you already have a python installation, otherwise you can also install it using Homebrew):

	$ brew install swig libyaml

- Use QtCreator to open the project file 'Gaia2lib.pro' in packaging/darwin/Gaia2lib/

- Compile the project (you will probably need to configure QtCreator to work with your Qt installation and also to set up the build folder for the project)

(Gaia2Python - python bindings)

- Use swig to generate the file 'gaia_wrap.cxx' that will be needed to compile 'Gaia2Python':

	$ swig -c++ -python -w451 /path_to_gaia_source/src/bindings/gaia.swig 

- Copy the generated 'gaia_wrap.cxx' to the Gaia2Python project folder:

	$ cp /path_to_gaia_source/src/bindings/gaia_wrap.cxx /path_to_gaia_source/packaging/darwin/Gaia2Python/

- Use QtCreator to open the project file 'Gaia2Python.pro' in packaging/darwin/Gaia2Python/ and compile

- Run ./make_release_tarball in packaging/darwin:

	$ ./make_release_tarball
	
- Copy the folder packaging/darwin/tmp/gaia2/python/gaia2 (created when running make_release_tarball.sh) to the site-packages directory of your python distribution. You can now import gaia2 from python.

-------
*Windows*:

- Use the QtCreator projects inside the packaging/win32 directory.


**3RD PARTY**
---------

This library contains source code from the LibSVM project, which is distributed under the revised BSD license.
Please refer to the src/3rdparty/libsvm/COPYRIGHT file for more information.

This library contains the Mersenne Twister random number generator, which is distributed under the BSD license.

This library contains source code from the [Alglib project](http://www.alglib.net), which is distributed under the 3-clause BSD license.

This library contains source code from the [Eigen project](http://eigen.tuxfamily.org/), which is distributed under the LGPLv3 license.

This library contains source code from [FrogLogic command line parser](http://www.froglogic.com/pg?id=PublicationsFreeware&category=getopt) which is distributed under the BSD license.
