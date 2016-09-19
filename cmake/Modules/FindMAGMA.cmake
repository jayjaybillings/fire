#------------------------------------------------------------------------------
# Copyright (c) 2015-, Jay Jay Billings
# All rights reserved.
#
# Author Contact: Jay Jay Billings, jayjaybillings <at> gmail <dot> com
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of fern nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author(s): Jay Jay Billings
#----------------------------------------------------------------------------*/
#
# This file was adapted from:
#    http://stackoverflow.com/questions/17747470/cmake-find-library-does-not-find-the-library
#
# I saved the header, but rewrote the rest to use PkgConfig.
#
# - Find the MAGMA library
#
# Usage:
#   find_package(MAGMA [REQUIRED] [QUIET] )
#
# It sets the following variables:
#   MAGMA_FOUND               ... true if magma is found on the system
#   MAGMA_LIBRARY_DIRS        ... full path to magma library
#   MAGMA_INCLUDE_DIRS        ... magma include directory
#   MAGMA_LIBRARIES           ... magma libraries
#
# The following variables will be checked by the function
#   MAGMA_USE_STATIC_LIBS     ... if true, only static libraries are found
#   MAGMA_ROOT                ... if set, the libraries are exclusively searched
#                                 under this path

#Reconfigure the pkgconfig path to contain the MAGMA lib directory, which
#is where the magma.pc file is located.
set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${MAGMA_ROOT}/lib/pkgconfig" )

#Find MAGMA using PKGCONFIG
find_package(PkgConfig REQUIRED)
pkg_search_module(MAGMA REQUIRED magma magma_sparse)

if (MAGMA_FOUND)
   message(STATUS "MAGMA libraries found in ${MAGMA_LIBRARY_DIRS}.")
   #Modify the library list to include the magma_sparse library
   set(MAGMA_LIBRARIES "magma_sparse:${MAGMA_LIBRARIES}")
endif(MAGMA_FOUND)