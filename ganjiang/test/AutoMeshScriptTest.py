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
import unittest
import sys
sys.path.append('..')
import AutoMeshScript as am

# test class for AutoMeshScript.py
class TestAutoMeshScript(unittest.TestCase):
    # ReadConfigFile test function
    def test_readConfigFile(self):
        import os
        # get current directory
        cwd = os.getcwd()
        # read a test configuration ini file
        filename = cwd + "/data/test1.ini"
        # set test case
        config = am.readConfigFile(filename)
        # check geometry file
        geom_f = config['Geometry']['FileName']
        self.assertEqual(geom_f, '/home/Documents/file/cube.step')
        # check refinemesh parameter
        self.assertEqual(config['Parameter']['RefineMesh'], '2')
        # check boundarycondition
        self.assertEqual(config['BoundaryCondition']['face1'], 'fixed')
        # check material information
        mat_f = config['Material']['FileName']
        self.assertEqual(mat_f, '/home/Documents/file/material.ini')

# run test
if __name__ == '__main__':
    unittest.main()