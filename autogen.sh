#!/bin/bash
aclocal -I m4 --install
autoconf -Wall
autoheader
automake -a --copy --foreign --add-missing