#!/usr/bin/env python
# encoding: utf-8

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
        except Exception, e:
            print e
    return version

APPNAME = 'gaia'
VERSION = '2.4.4'
GIT_SHA = get_git_version();

top = '.'
out = 'build'


def options(opt):
    opt.load('compiler_cxx compiler_c qt4')
    opt.recurse('src')

    # whether or not to use the "bz2-encryption"
    opt.add_option('--with-bz2-encryption', action = 'store_true',
                   dest = 'bz2_encryption', default = False,
                   help = 'whether to load signature files using the "bz2-encryption"')

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


def check_bz2(conf):
    conf.check_cxx(header_name = 'bzlib.h', mandatory = 1,
                   errmsg = 'libbz2 is required in order to compile Gaia with encryption support.')

    conf.env['LIB_BZ2'] = 'bz2'
    conf.env['CXXDEFINES_BZ2'] = 'USE_BZ2_ENCRYPTION'
    conf.env['USELIB'] += [ 'BZ2' ]





def configure(conf):
    if sys.platform != 'linux2' and sys.platform != 'darwin':
        print 'Please use the QtCreator project for building Gaia in Windows...'
        sys.exit(1)

    print('→ configuring the project in ' + conf.path.abspath())

    conf.env.APP_VERSION = VERSION

    # compiler flags
    conf.env.CXXFLAGS += [ '-Wall', '-fno-strict-aliasing', '-fPIC', '-fvisibility=hidden' ]

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

    conf.env['USELIB'] = [ 'QTCORE', 'YAML' ]

    # optional dependency: tbb, if asked for it
    conf.env['WITH_TBB'] = conf.options.tbb
    if conf.env['WITH_TBB']:
        check_tbb(conf)

    # optional dependency: libbz2, if asked for bz2-encryption
    conf.env['WITH_BZ2_ENCRYPTION'] = conf.options.bz2_encryption
    if conf.env['WITH_BZ2_ENCRYPTION']:
        check_bz2(conf)

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
    prefix = normpath(conf.options.prefix)

    if sys.platform == 'linux2':
        print conf.env
        print "========"

        opts = { 'prefix': prefix,
             'qtlibdir': conf.env['LIB_QTCORE'] or '/usr/lib',
             'qtincludedir': '-I' + ' -I'.join(conf.env['INCLUDES_QTCORE']),
             'version': VERSION,
             'tbblib': tbblib,
             }
        print opts

        pcfile = '''prefix=%(prefix)s
        libdir=${prefix}/lib
        includedir=${prefix}/include
        qtlibdir=%(qtlibdir)s
        qtincludes=%(qtincludedir)s

        Name: libgaia2
        Description: A library for doing similarity in semimetric spaces
        Version: %(version)s
        Libs: -L${libdir} -L${qtlibdir} -lgaia2 -lQtCore -lyaml %(tbblib)s
        Cflags: -I${includedir}/gaia2 ${qtincludes}
        ''' % opts
        print "---->"
        print pcfile

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
        Cflags: -I${includedir}/gaia2 ${qtincludes}
        ''' % opts


    pcfile = '\n'.join([ l.strip() for l in pcfile.split('\n') ])
    conf.env.pcfile = pcfile
    #open('build/gaia2.pc', 'w').write(pcfile) # we'll do it later on the build stage


def build(bld):
    print('→ building from ' + bld.path.abspath())
    bld.recurse('src')
