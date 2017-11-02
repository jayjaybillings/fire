#Ganjiang

Ganjiang is a auto mesh python script for running Netgen/NGSolve meshing tool under parallel.  It supports 
STEP geometry file and IGES geometry file input.


##Prerequisite

It needs OCE/OCC, Python3, GCC, OpenMPI, CMake, and Netgen/NGSolve, where Netgen/NGSolve package can be
obtained from https://ngsolve.org/.  During the CMake installation of Netgen/NGSolve, please make sure 
parameters "USE_OCC" and "USE_MPI" are set to on.

##Usage
An ini configuration file is needed for running the script, which you find the instructions and examples 
under the directory of examples.  The bash command to run the Python script is 

'''bash
python3 AutoMeshScript.py -c examples/ex1.ini
'''bash

where "examples/ex1.ini" is the configuration file name and path, or

'''bash
python3 AutoMeshScript.py --config examples/ex1.ini
'''bash