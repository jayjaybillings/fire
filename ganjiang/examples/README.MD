# Examples

This is the directory for the tutorial examples.

## Configuration file

Ganjiang supports ini type configuration file. There will be sections of "Geometry", "Parameter", "BoundaryCondition", "Material", and "Output".

### *Geometry*

This section sepecifies the path to the geometry file. It can be set as
```
filename = /path/to/geometry file
```
### *Parameter*

This section provides the variables "RefineMesh", "SecondOrder", and "NumOfCores". "RefineMesh" corresponds to how many mesh refinement is needed (0 means no refinement). "SecondOrder" sets the mesh to second order type, which would be tetrahedral 10 element. "NumOfCores" is only used for Warp3D FEA analysis. An example of setting these parameters is like this
```
refinemesh = 2
secondorder = on/off
numofcores = 20
```
### *BoundaryCondition*

This section points to the file to set the boundary/initial condition. It can be set as
```
filename = /path/to/boundary
```
### *Material*

This section points to the file containing material information. It can be set as
```
filename = /path/to/material file
```
### *Output*

This is for specifying the intermediate mesh file information. It supports 'Warp3D Format' (default) or netgen output mesh format, namely 'Neutral Format', 'Surface Mesh Format', 'DIFFPACK Format', 'TecPlot Format', 'Tochnog Format', 'Abaqus Format', 'Fluent Format', 'Permas Format', 'FEAP Format', 'Elmer Format', 'STL Format', 'STL Extended Format', 'VRML Format', 'Gmsh Format', 'Gmsh2 Format', 'OpenFOAM 1.5+ Format', 'OpenFOAM 1.5+ Compressed', 'JCMwave Format', and 'TET Format'. It can be set as
```
format = Warp3D Format
filename = /path/to/output file name
```
