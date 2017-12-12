'''
-------------------------------------------------------------------------------
 Copyright (c) 2017-, UT-Battelle, LLC
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
 Author(s): Robert Smith (smithrw <at> ornl <dot> gov)
 -----------------------------------------------------------------------------
 '''

import copy
import json
import sys

#Check that a filename was fiven
if len(sys.argv) < 6:
    print "Usage: weld.py input.json Warp3d_nodefile Warp3d_elementfile Ganjiang_boundaryfile Ganjiang_materialfile"
    exit()

#The json formatted content
json_content = ""

#Read and convert the input to json
for line in open(sys.argv[1]).readlines():
    json_content += line.replace("=>",":").replace(":nil",":null")

#Convert the content to a dictionary
orig_input = json.loads(json_content)

#Get the arguments for the Weld predictor file's contents
coordinate_path = sys.argv[2]
incidence_path = sys.argv[3]

#Output the Weld predictor file
W3Dfile = open("Warp3DInput.ini", "w")

W3Dfile.write("[Body]\n")
W3Dfile.write("structurename = beam\n")
W3Dfile.write("materialname = steel_304\n")
W3Dfile.write("materialproperties = creep\n")
W3Dfile.write("\n")

W3Dfile.write("[Material]\n")
W3Dfile.write("e = 1.0e11\n")
W3Dfile.write("nu = 0.29\n")
W3Dfile.write("n_power = 1.26e-10\n")
W3Dfile.write("yld_pt = 0.8e8\n")
W3Dfile.write("alpha = " + str(orig_input["data"]["joint_data"]["dimensions"]["alpha"]) + "\n") #There are two different alpha values in the input, under dimensions and joint angle. Which to use?
W3Dfile.write("rho = 7.9e3\n")
W3Dfile.write("\n")

W3Dfile.write("[Mesh]\n")
W3Dfile.write("secondorder = on\n")
W3Dfile.write("nodenumber = 6885\n")
W3Dfile.write("elementnumber = 5120\n")
W3Dfile.write("elementtype = tet4\n")
W3Dfile.write("matlinearity = linear\n")
W3Dfile.write("order = 1pt_rule\n")
W3Dfile.write("nodefile = " + coordinate_path + "\n")
W3Dfile.write("elementfile = " + incidence_path + "\n")
W3Dfile.write("\n")

W3Dfile.write("[BoundaryCondition]\n")
W3Dfile.write("bcfile = boundary.inp\n")
W3Dfile.write("\n")

W3Dfile.write("[Step]\n")
W3Dfile.write("timestep = 5.0\n")
W3Dfile.write("computestep = 1\n")
W3Dfile.write("maxiteration = 10\n")
W3Dfile.write("miniteration = 1\n")
W3Dfile.write("extrapolote = on\n")
W3Dfile.write("linesearch = off\n")
W3Dfile.write("divergencecheck = on\n")
W3Dfile.write("convergencetol = 0.05\n")
W3Dfile.write("adaptive = on\n")
W3Dfile.write("batchmessage = off\n")
walltimelimit = "on" if orig_input["data"]["start_panel_data"]["walltime_hours"] > 0 else "off"
W3Dfile.write("walltimelimit = " + walltimelimit + "\n")
W3Dfile.write("bbarfactor = 0.0\n")
W3Dfile.write("q-matrix = on\n")
W3Dfile.write("tracesolution = on\n")
W3Dfile.write("\n")

W3Dfile.write("[Output]\n")
W3Dfile.write("outputfile = Warp3D.inp\n")
W3Dfile.write("flatstream = on\n")
W3Dfile.write("displacement = on\n")
W3Dfile.write("stress = on\n")
W3Dfile.write("strain = on\n")
W3Dfile.write("reaction = on\n")
W3Dfile.write("temperature = on\n")
W3Dfile.write("\n")

W3Dfile.close()

#Get the arguments for Ganjiang's input file contents
boundary_path = sys.argv[4]
material_path = sys.argv[5]

#Write the Ganjiang 
GJfile = open("GanjiangInput.ini", "w")

GJfile.write("[Geometry]\n")
GJfile.write("filename = /path/to/cube.step\n")
GJfile.write("\n")

GJfile.write("[Parameter]\n")
GJfile.write("refinemesh = 2\n")
GJfile.write("numofcores = 20\n")
GJfile.write("secondorder = on\n")
GJfile.write("\n")

GJfile.write("[BoundaryCondition]\n")
GJfile.write("filename = " + boundary_path + "\n")
GJfile.write("\n")

GJfile.write("[Material]\n")
GJfile.write("filename = " + material_path + "\n")
GJfile.write("\n")

GJfile.write("[Output]\n")
GJfile.write("format = Warp3D Format\n")
GJfile.write("filename = cube.inp\n")

GJfile.close()





