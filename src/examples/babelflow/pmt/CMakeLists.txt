###############################################################################
# Copyright (c) 2015-2019, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory
#
# LLNL-CODE-716457
#
# All rights reserved.
#
# This file is part of Ascent.
#
# For details, see: http://ascent.readthedocs.io/.
#
# Please also read ascent/LICENSE
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the disclaimer below.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the disclaimer (as noted below) in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of the LLNS/LLNL nor the names of its contributors may
#   be used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
# LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
###############################################################################

###############################################################################
#
# BabelFlow PMT example that reads an output file
# Usage: mpirun -n N ./ascent_babelflow_example -f <input_data> 
# 		-d <Xdim> <Ydim> <Zdim> -p <dx> <dy> <dz> -m <fanin> -t <threshold>
#
##############################################################################


set(ascent_babelflow_example_sources
    ascent_babelflow_example.cpp)

set(ascent_babelflow_example_header)

if(MPI_FOUND)
	set(ascent_babelflow_example_deps ascent_mpi BabelFlow::babelflow ${MPI_CXX_LIBRARIES} PMT::pmt)

    blt_add_executable(
        NAME        ascent_babelflow_example
        SOURCES     ${ascent_babelflow_example_sources}
        DEPENDS_ON  ${ascent_babelflow_example_deps}
        OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

  install(TARGETS ascent_babelflow_example
      EXPORT  ascent
      LIBRARY DESTINATION examples/ascent/babelflow/pmt
      ARCHIVE DESTINATION examples/ascent/babelflow/pmt
      RUNTIME DESTINATION examples/ascent/babelflow/pmt)
endif()
