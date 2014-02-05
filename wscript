#!/usr/bin/env python
# encoding: utf-8

import sys, glob
import Options, Scripting
from os.path import join, exists, normpath

# the following two variables are used by the target "waf dist"
VERSION = '2.4-dev'
APPNAME = 'gaia'

# files to exclude from "waf dist"
Scripting.excludes += glob.glob('.waf-1.*')
Scripting.excludes += [ 'dataset_small', 'verysmall' ] # test directories
Scripting.excludes += [ 'debian' ] # people who want to compile packages should be able to get the source from git...

Scripting.dist_exts.remove('.tar.bz2')
Scripting.dist_exts += [ 'dataset_small.db', 'verysmall.db' ]


# these variables are mandatory ('/' are converted automatically)
top = '.'
out = 'build'


def init():
    pass

def check_yaml(conf):
    conf.check_cxx(header_name = 'yaml.h', mandatory = 1,
                   errmsg = 'libyaml is required in order to compile Gaia.')
    conf.check_cxx(lib = 'yaml', mandatory = 1,
                   errmsg = 'libyaml is required in order to compile Gaia.')

    # allow it to be compiled as a static library if we are on a 32-bit platform
    # (gives problems with yaml not compiled with -fPIC otherwise)
    if sys.maxint == 2**31-1:
        conf.env['STATICLIB_YAML'] = 'yaml'


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


def set_options(opt):
    # options provided by the modules
    opt.tool_options('compiler_cc')
    opt.tool_options('cxx')
    opt.tool_options('qt4')

    # options provided in subdirectories
    opt.sub_options('src')

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
    opt.add_option('--debug', action = 'store_true',
                   dest = 'debug', default = False,
                   help = 'whether to enable debug symbols and remove optimization')



def configure(conf):
    if sys.platform != 'linux2':
        print 'Please use the QtCreator projects for building Gaia in Windows and/or MacOS X...'
        sys.exit(1)

    print 'Configuring with Qt open source edition'

    # check that we have everything required
    conf.check_tool('compiler_cc g++ qt4')

    check_yaml(conf)
    conf.env['USELIB'] = [ 'QTCORE', 'YAML' ]

    # optional dependency: tbb, if asked for it
    conf.env['WITH_TBB'] = Options.options.tbb
    if conf.env['WITH_TBB']:
        check_tbb(conf)

    # optional dependency: libbz2, if asked for bz2-encryption
    conf.env['WITH_BZ2_ENCRYPTION'] = Options.options.bz2_encryption
    if conf.env['WITH_BZ2_ENCRYPTION']:
        check_bz2(conf)

    # system-wide flags
    conf.env['CXXDEFINES'] = 'GAIA_VERSION="' + VERSION + '"'
    if type(conf.env['CXXFLAGS']) == str:
        conf.env['CXXFLAGS'] = [ conf.env['CXXFLAGS'] ]

    conf.env['CXXFLAGS'] += [ '-Wall', '-Werror', '-fno-strict-aliasing', '-fPIC', '-fvisibility=hidden' ]

    # add this key otherwise gcc 4.8 will complain
    conf.env['CXXFLAGS'] += [ '-Wno-unused-local-typedefs' ]

    # big fat hack for centos, which is still in stone age...
    if sys.version_info[1] > 4:
        # option not available in centos' gcc...
        conf.env['CXXFLAGS'] += [  '-Wno-unused-result' ]

    if Options.options.debug:
        conf.env['CXXFLAGS'] += [ '-g' ]
    else:
        conf.env['CXXFLAGS'] += [ '-O2', '-msse2' ]

    if Options.options.optimized:
        conf.env['CXXFLAGS'] += [ '-DNDEBUG', '-DQT_NO_DEBUG' ]

    conf.env['LINKFLAGS'] += [ '-Wl,--as-needed' ]

    # super optimized flags
    #conf.env['CXXFLAGS'] += [ '-Wall -Werror -O3 -fPIC -DNDEBUG -DQT_NO_DEBUG -ffast-math -fomit-frame-pointer -funroll-loops' ]

    # also import sub-directories' config
    conf.sub_config('src')

    # write pkg-config file
    prefix = normpath(Options.options.prefix)

    if Options.options.tbb:
        tbblib = '-ltbb'
    else:
        tbblib = ''

    opts = { 'prefix': prefix,
             'qtlibdir': conf.env['LIBPATH_QTCORE'] or '/usr/lib',
             'qtincludedir': '-I' + ' -I'.join(conf.env['CPPPATH_QTCORE']),
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
    Libs: -L${libdir} -L${qtlibdir} -lgaia2 -lQtCore -lyaml %(tbblib)s
    Cflags: -I${includedir}/gaia2 ${qtincludes}
    ''' % opts

    pcfile = '\n'.join([ l.strip() for l in pcfile.split('\n') ])
    open('build/gaia2.pc', 'w').write(pcfile)



def build(bld):
    bld.add_subdirs('src')
