#!/usr/bin/env python
# encoding: utf-8

from __future__ import print_function
import sys, glob
##import Options, Scripting
from os.path import join, exists, normpath
import os

def get_git_version():
    """ try grab the current version number from git"""
    version = "Undefined"
    if os.path.exists(".git"):
        try:
            version = os.popen("git describe --dirty --always").read().strip()
        except Exception as e:
            print (e)
    return version

APPNAME = 'gaia'
VERSION = open('VERSION', 'r').read().strip('\n')
GIT_SHA = get_git_version();

top = '.'
out = 'build'


def options(opt):
    opt.load('compiler_cxx compiler_c qt5')
    opt.recurse('src')

    # whether or not to have all the asserts working
    opt.add_option('--with-asserts', action = 'store_false',
                   dest = 'optimized', default = True,
                   help = 'whether to enable all asserts. Code can be much slower, but safer as well')

    # whether to use TBB for multithreading queries
    opt.add_option('--with-tbb', action = 'store_true',
                   dest = 'tbb', default = False,
                   help = 'whether to use TBB for multithreading queries')

    # whether to compile the Cyclops RPC server
    opt.add_option('--with-cyclops', action = 'store_true',
                   dest = 'cyclops', default = False,
                   help = 'whether to compile the Cyclops RPC server')

    # whether to enable debug symbols and not optimize
    opt.add_option('--mode', action='store',
                   dest='MODE', default="release",
                   help='debug or release')
                   
    # whether to enable Qt5 or not
    opt.add_option('--with-gaia-qt5', action='store_true',
                   dest='gaia_qt5', default=False,
                   help='whether to use Qt5 and c++11 or not')


def debian_version():
    try:
        v = open('/etc/debian_version').read().strip()
        return [ int(n) for n in v.split('.') ]
    except IOError:
        return []
    except ValueError:
        # string version instead of numeric
        if 'wheezy' in v or 'sid' in v:
            return [7, 0]
        else:
            return [6, 0]


def check_tbb(conf):
    tbb = conf.check_cxx(header_name = 'tbb/task_scheduler_init.h',
                         mandatory = 1,
                         errmsg = 'Intel TBB is recommended in order to compile Gaia')

    if not tbb:
        return

    conf.env['CXXDEFINES_TBB'] = 'HAVE_TBB'
    conf.env['LIB_TBB'] = 'tbb'
    conf.env['USELIB'] += [ 'TBB' ]


def configure(conf):
    print ('System platform is',sys.platform)
    
    if sys.platform.startswith('linux') != True and sys.platform != 'darwin':
        print ('Please use the QtCreator project for building Gaia in Windows...')
        sys.exit(1)

    print('→ configuring the project in ' + conf.path.abspath())

    conf.env.APP_VERSION = VERSION

    # compiler flags
    if sys.platform.startswith('linux') and conf.options.gaia_qt5:
        conf.env.CXXFLAGS += [ '-std=c++11', '-msse2','-Wall', \
            '-DGAIA_QT5',  \
            '-Wint-in-bool-context', \
            '-Wno-misleading-indentation', \
            '-Wno-unused-result',\
            '-fno-strict-aliasing',\
            '-fPIC', '-fvisibility=hidden',\
            '-I/usr/include/x86_64-linux-gnu/c++/7.2.0',\
            '-I/usr/include/c++/7.2.0',\
            '-I/usr/lib/gcc/x86_64-linux-gnu/7.2.0/include',\
            '-I/opt/intel/mkl/include',\
            '-I/usr/include']
    elif sys.platform.startswith('linux'): 
        conf.env.CXXFLAGS += [ '-std=c++03', '-msse2','-Wall', \
                '-Wint-in-bool-context', \
                '-Wno-misleading-indentation', \
                '-Wno-unused-result',\
                '-fno-strict-aliasing',\
                '-fPIC', '-fvisibility=hidden',\
                '-I/usr/include/x86_64-linux-gnu/c++/7.2.0',\
                '-I/usr/include/c++/7.2.0',\
                '-I/usr/lib/gcc/x86_64-linux-gnu/7.2.0/include',\
                '-I/opt/intel/mkl/include',\
                '-I/usr/include']

    # define this to be stricter, but sometimes some libraries can give problems...
    #conf.env.CXXFLAGS += [ '-Werror' ]

    if conf.options.MODE == 'debug':
        print ('→ Building in debug mode')
        conf.env.CXXFLAGS += [ '-g' ]

    elif conf.options.MODE == 'release':
        print ('→ Building in release mode')
        conf.env.CXXFLAGS += [ '-O2', '-msse2' ]
    else:
        raise ValueError('mode should be either "debug" or "release"')

    if conf.options.optimized:
        conf.env.CXXFLAGS += [ '-DNDEBUG', '-DQT_NO_DEBUG' ]

    # super optimized flags
    #conf.env['CXXFLAGS'] += [ '-Wall -Werror -O3 -fPIC -DNDEBUG -DQT_NO_DEBUG -ffast-math -fomit-frame-pointer -funroll-loops' ]


    # NOTE: Debian Squeeze doesn't provide pkg-config files for libyaml, but
    #       Debian Wheezy does... Mac OS X (brew) does it also.
    debver = debian_version()
    is_squeeze = (debver and debver[0] < 7)

    if is_squeeze:
        conf.env.LINKFLAGS += [ '-lyaml' ]
    else:
        conf.check_cfg(package='yaml-0.1', uselib_store='YAML',
                      args=['--cflags', '--libs'])

    if sys.platform.startswith('linux') and conf.options.gaia_qt5:
        conf.env['USELIB'] = [ 'QT5CORE', 'QT5CONCURRENT', 'YAML' ]
    else :
        conf.env['USELIB'] = [ 'QTCORE', 'YAML' ]

    # optional dependency: tbb, if asked for it
    conf.env['WITH_TBB'] = conf.options.tbb
    if conf.env['WITH_TBB']:
        check_tbb(conf)

    # optional dependency: QtNetwork for Cyclops Server
    conf.env['WITH_CYCLOPS'] = conf.options.cyclops
    if conf.env['WITH_CYCLOPS']:
        conf.env['USELIB'] += [ 'QTNETWORK' ]

    conf.env.DEFINES = ['GAIA_VERSION="%s"' % VERSION, 'GAIA_GIT_SHA="%s"' % GIT_SHA]

    if sys.platform == 'darwin':
        # force the use of clang as compiler, we don't want gcc anymore on mac
        conf.env.CC = 'clang'
        conf.env.CXX = 'clang++'

        ###conf.env.DEFINES   += [ 'GTEST_HAS_TR1_TUPLE=0' ]
        conf.env.CXXFLAGS += [ '-stdlib=libc++', '-Wno-gnu' ] # '-std=c++11' produces errors in Eigen
        conf.env.LINKFLAGS = [ '-stdlib=libc++' ]
        # for defining static const variables in header
        conf.env.CXXFLAGS += [ '-Wno-static-float-init' ]
        # add /usr/local/include as the brew formula for yaml doesn't have
        # the cflags properly set
        conf.env.CXXFLAGS += [ '-I/usr/local/include' ]

    if sys.platform.startswith('linux') and conf.options.gaia_qt5:
        conf.load('compiler_cxx compiler_c qt5')
    else:
        conf.load('compiler_cxx compiler_c qt4')

    #conf.env['LINKFLAGS'] += [ '--as-needed' ] # TODO do we need this flag?

    # add this key otherwise gcc 4.8 will complain
    # conf.env['CXXFLAGS'] += [ '-Wno-unused-local-typedefs' ] #  --- outdated?

    # commented below as we don't care about centos for now:

    # big fat hack for centos, which is still in stone age...
    #if sys.version_info[1] > 4:
    #    # option not available in centos' gcc...
    #    conf.env['CXXFLAGS'] += [  '-Wno-unused-result' ]

    conf.recurse('src')

    if conf.options.tbb:
        tbblib = '-ltbb'
    else:
        tbblib = ''

    # write pkg-config file
    #   Libs: -L${libdir} -L${qtlibdir} -lgaia2 -lQtCore -lyaml %(tbblib)s
    prefix = normpath(conf.options.prefix)

    if sys.platform.startswith('linux') and conf.options.gaia_qt5:
        opts = { 'prefix': prefix,
            'qtlibdir': conf.env['USELIB'],
            'qtincludedir': '-I' + ' -I'.join(conf.env['INCLUDES_QT5CORE']),
            'version': VERSION,
            'tbblib': tbblib,
            }
        
        pcfile = '''prefix=%(prefix)s
        libdir=${prefix}/lib
        includedir=${prefix}/include
        qtlibdir=%(qtlibdir)s
        qtincludes=%(qtincludedir)s

        Name: libgaia2
        Description: A library for doing similarity in semimetric spaces
        Version: %(version)s
        Libs: -L${libdir} -L${qtlibdir} %(tbblib)s
        Cflags: -I${includedir} ${qtincludes}
        ''' % opts
        
    elif sys.platform.startswith('linux'):
        opts = { 'prefix': prefix,
            'qtlibdir': conf.env['USELIB'],
            'qtincludedir': '-I' + ' -I'.join(conf.env['INCLUDES_QT4CORE']),
            'version': VERSION,
            'tbblib': tbblib,
            }

        pcfile = '''prefix=%(prefix)s
        libdir=${prefix}/lib
        includedir=${prefix}/include
        qtlibdir=%(qtlibdir)s
        qtincludes=%(qtincludedir)s

        Name: libgaia2
        Description: A library for doing similarity in semimetric spaces
        Version: %(version)s
        Libs: -L${libdir} -L${qtlibdir} %(tbblib)s
        Cflags: -I${includedir} ${qtincludes}
        ''' % opts

    elif sys.platform == 'darwin':
        opts = { 'prefix': prefix,
             'qtlibdir': '-F' + conf.env['FRAMEWORKPATH_QTCORE'][0] +
                         ' -framework ' + conf.env['FRAMEWORK_QTCORE'][0],
             'qtincludedir': '-I' + ' -I'.join(conf.env['INCLUDES_QTCORE']),
             'version': VERSION,
             'tbblib': tbblib,
             }

        pcfile = '''prefix=%(prefix)s
        libdir=${prefix}/lib
        includedir=${prefix}/include
        qtlibdir=%(qtlibdir)s
        qtincludes=%(qtincludedir)s

        Name: libgaia2
        Description: A library for doing similarity in semimetric spaces
        Version: %(version)s
        Libs: -L${libdir} ${qtlibdir} -lgaia2 -lyaml %(tbblib)s
        Cflags: -I${includedir} ${qtincludes}
        ''' % opts


    pcfile = '\n'.join([ l.strip() for l in pcfile.split('\n') ])
    conf.env.pcfile = pcfile
    #open('build/gaia2.pc', 'w').write(pcfile) # we'll do it later on the build stage


def build(bld):
    print('→ building from ' + bld.path.abspath())
    bld.recurse('src')
