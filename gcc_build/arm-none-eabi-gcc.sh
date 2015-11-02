#! /usr/bin/env bash


#Taken from:
#https://gist.githubusercontent.com/cjmeyer/4251208/raw/fceeaa4de9fa020f5fa2d6043b30a360a45f2778/arm-none-eabi-gcc.sh
# removed GDB build and updated sources

# Target and build configuration.
TARGET=arm-none-eabi
PREFIX=/opt/arm-none-eabi-5.2.0

# Sources to build from.
BINUTILS=binutils-2.25.1
GCC=gcc-5.2.0
NEWLIB=newlib-2.2.0-1

# Grab the souce files...but only if they don't already exist.
if [ ! -e ${BINUTILS}.tar.bz2 ]; then
    echo "Grabbing ${BINUTILS}.tar.bz2"
    curl ftp://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.bz2 -o ${BINUTILS}.tar.bz2
fi
if [ ! -e ${GCC}.tar.bz2 ]; then
    echo "Grabbing ${GCC}.tar.bz2"
    curl ftp://ftp.gnu.org/gnu/gcc/${GCC}/${GCC}.tar.bz2 -o ${GCC}.tar.bz2
fi
if [ ! -e ${NEWLIB}.tar.gz ]; then
    echo "Grabbing ${NEWLIB}.tar.gz"
    curl ftp://sources.redhat.com/pub/newlib/${NEWLIB}.tar.gz -o ${NEWLIB}.tar.gz
fi

# Extract the sources.
echo -n "extracting binutils... "
tar -jxf ${BINUTILS}.tar.bz2
echo "done"
echo -n "extracting gcc... "
tar -jxf ${GCC}.tar.bz2
echo "done"
echo -n "extracting newlib... "
tar -zxf ${NEWLIB}.tar.gz
echo "done"

# Build a set of compatible Binutils for this architecture.  Need this before
# we can build GCC.
mkdir binutils-build
cd binutils-build
../${BINUTILS}/configure --target=${TARGET} --prefix=${PREFIX} --disable-nls \
    --enable-interwork --enable-multilib --disable-werror
make all install 2>&1 | tee ../make.log
cd ..

# Add the new Binutils to the path for use in building GCC and Newlib.
export PATH=$PATH:${PREFIX}:${PREFIX}/bin

# Build and configure GCC with the Newlib C runtime. Note that the 'with-gmp',
# 'with-mpfr' and 'with-libconv-prefix' are needed only for Mac OS X using the
# MacPorts system.
cd ${GCC}
# The following symbolic links are only needed if building Newlib as well.
ln -s ../${NEWLIB}/newlib .
ln -s ../${NEWLIB}/libgloss .
mkdir ../gcc-build
cd ../gcc-build
../${GCC}/configure --target=${TARGET} --prefix=${PREFIX} \
    --with-newlib --with-gnu-as --with-gnu-ld --disable-nls --disable-libssp \
    --disable-gomp --disable-libstcxx-pch --enable-threads --disable-shared \
    --disable-libmudflap --enable-interwork --enable-languages=c,c++ \
    --with-gmp=/opt/local 
make all install 2>&1 | tee -a ../make.log
# Use the following instead if only building and installing GCC (i.e. without Newlib).
#make all-gcc install-gcc 2>&1 | tee -a ../make.log

# We are done, let the user know were the new compiler and tools are.
echo ""
echo "Cross GCC for ${TARGET} installed to ${PREFIX}"
echo ""
