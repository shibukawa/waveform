waveform
=================

This program converts into wave form PNG image.

Build
-------------

It needs SCons environment. This repository already includes third party libraries (zlib, libpng, libsndfile).

.. code-block:: bash

   $ scons

Usage
-------------

.. code-block:: none

   usage> waveform [options] input.wav output.png

      [options]
      -w, --width [num]     : output image width (default 256)
      -h, --height [num]    : output image height (default 256)
      -m, --margin [num]    : margin size (defult 16)
      -fg, --fgcolor RRGGBB : foreground color (default 0000FFFF)
      -bg, --bgcolor RRGGBB : background color (default FFFFFFFF)

      --help                : show this message

License
-------------

MIT license.


