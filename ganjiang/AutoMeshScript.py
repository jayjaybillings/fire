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
 -----------------------------------------------------------------------------
 '''

import netgen.NgOCC
import netgen.libngpy._NgOCC as nlOCC
import netgen.libngpy._meshing as nlmesh
import ngsolve.comp as ngcomp
import configparser

# program option for reading configuration file
def parseArg():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", help="configuration file location and name")
    args = parser.parse_args()
    return args.config

# read the configuration file for analysis
def readConfigFile(filename):
    config = configparser.ConfigParser()
    config.sections()
    config.read(filename)
    # check if there is geometry file
    if not config.has_option('Geometry', 'filename'):
        raise Exception('Geometry filename missing')
    # check if there is boundary condition
    if not config.has_section('BoundaryCondition'):
        raise Exception('BoundaryCondition missing')
    # check if material properties exist
    if not config.has_option('Material', 'filename'):
        raise Exception('Material file missing')
    return config

# main function for execute mesh generations
def generateMesh(config):
    # get geometry file name
    Geom_file = config['Geometry']['filename']
    
    # import step/iges file
    geo = nlOCC.LoadOCCGeometry(Geom_file)

    # mesh the file
    ngs_mesh = ngcomp.Mesh(geo.GenerateMesh())
    
    # refine mesh
    refine_para = config['Parameter'].getint('RefineMesh')
    for i in range(refine_para):
        ngs_mesh.Refine()
    
    # convert ngsolve mesh to netgen mesh
    nt_mesh = ngs_mesh.ngmesh
    
    return nt_mesh
    

if __name__=="__main__":
    # check program option for configuration file
    ConfigFile = ArgParse()
    # check if configuration file is in the right format
    config = ReadConfigFile(ConfigFile)
    
    # generate mesh
    nt_mesh = generateMesh(config)
    
    # export mesh
    nt_mesh.Export('output', 'Neutral Format')

    # output user defined format
    #import netgen.exportNeutral as exp
    #exp.Export(nt_mesh, "cube.neu")

