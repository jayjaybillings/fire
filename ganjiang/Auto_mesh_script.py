'''----------------------------------------------------------------------------
 Copyright (c) 2015-, UT-Battelle, LLC
 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of fern nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 Author(s): Guoqiang Deng (dgquvn <at> gmail <dot> com)
 -----------------------------------------------------------------------------
 '''
 
import netgen.gui
from ngsolve.solve import Draw, Redraw # just for visualization
import netgen.NgOCC
import netgen.libngpy._NgOCC as nlOCC
import netgen.libngpy._meshing as nlmesh
import ngsolve.comp as ngcomp

# import step file
geo = nlOCC.LoadOCCGeometry("cube.step")

# show the geometry
Redraw()

# mesh the file
ngs_mesh = ngcomp.Mesh(geo.GenerateMesh())

# Refine mesh
#ngs_mesh.Refine()
#ngs_mesh.Refine()

# Draw mesh
Draw(ngs_mesh)

# convert ngsolve mesh to netgen mesh
nt_mesh = ngs_mesh.ngmesh

# refinie netgen mesh
#nt_mesh.Refine()

# save the file
nt_mesh.Export('cube2', 'Neutral Format')

# output user defined format
#import netgen.exportNeutral as exp
#exp.Export(nt_mesh, "cube.neu")

# Redraw the mesh
#Redraw() 
