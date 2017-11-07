'''
-------------------------------------------------------------------------------
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
 --------------------------------------------------------------------------------
 '''
 
import sys

# Export mesh function
def Export (mesh, filename):
    """ export Netgen mesh to Warp3D input format """
    
    print ("export mesh in Warp3D input format to file = ", filename)

    f = open (filename, 'w')
    
    # get nodes
    points = mesh.Points()
    print ("c", file=f)
    print ("c", len(points), "nodes", file=f)
    print ("coordinates\n*echo off", end="", file=f)
    # node id
    i = 0
    # print all nodes coordinates
    for p in points:
        i = i + 1
        print ("\n ", i, end="\t", file=f)
        for j in range(3):
            print (p.p[j], end="\t", file=f)

    # get mesh
    volels = mesh.Elements3D();
    print ("\nc\nc\nc", file=f)
    print ("c", len(volels), "elements", file=f)
    print ("incidences", end="", file=f)
    # mesh id
    e = 0
    # print nodes order in the mesh
    for el in volels:
        e = e + 1
        print ("\n ", e, end="\t", file=f)
        nds_t = el.points
        # transform node order
        nds_t[1], nds_t[2] = nds_t[2], nds_t[1]
        for p in nds_t:
            print (p.nr, end="\t", file=f)
    print("\n*echo on", file=f)
