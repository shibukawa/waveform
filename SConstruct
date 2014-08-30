import sys
env = Environment();
Export('env')

AddOption('--enable-debug',
          dest='enabledebug',
          action='store_true',
          default=False,
          help='Debug Build')

sconscript = ['third_party/libsndfile', 'third_party/zlib', 'third_party/libpng']
sources = ['src/main.cpp']
libs = ['sndfile', 'z', 'png']
libpath = ['third_party/libsndfile', 'third_party/zlib/', 'third_party/libpng/']
cpppath = ['third_party/libsndfile/src', 'third_party/zlib/', 'third_party/libpng/']
ccflags = []
linkflags = []

if GetOption('enabledebug'):
    ccflags.append('-g')
    ccflags.append('-Wall')
else:
    ccflags.append('-O3')

if sys.platform == 'darwin':
    ccflags.append('-m32')
    linkflags.append('-mmacosx-version-min=10.7')
    linkflags.append('-m32')
    linkflags.append('-stdlib=libc++')

env.SConscript(dirs=sconscript)
env.Program('waveform', sources, LIBS=libs, LIBPATH=libpath, CPPPATH=cpppath, CCFLAGS=ccflags, LINKFLAGS=linkflags)
