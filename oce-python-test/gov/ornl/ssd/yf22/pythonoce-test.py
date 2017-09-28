'''
This script will create a sample of points from a 3D volume. It uses
OpenCascade's BRep API through PythonOCC.
Created on Sep 27, 2017

@author: Jay Jay Billings (billingsjj@ornl.gov)
'''

# Imports
from OCC._TopoDS import *
from OCC.Display.SimpleGui import init_display
from OCC.BRepPrimAPI import BRepPrimAPI_MakeBox
from OCC.BRepTools import breptools_Read
from OCC.TopoDS import *
from OCC.gp import gp_Pnt
from OCC.BRepBuilderAPI import BRepBuilderAPI_MakeVertex
from OCC.BRep import BRep_Builder
from OCC.BRepExtrema import BRepExtrema_DistShapeShape
from OCC.StlAPI import StlAPI_Reader
from OCC.Bnd import Bnd_Box
from OCC.BRepBndLib import brepbndlib_Add
from OCC._BRepTools import breptools_Clean, breptools_AddUVBounds,\
    breptools_IsReallyClosed
from OCC.TopExp import TopExp_Explorer, topexp
from OCC.TopAbs import *
from random import random 
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpmath import rand

if __name__ == '__main__':
    
    # Number of points
    n = 10000
    
    # Set the side length
    side_length = 10.0
    # The side length of the box that contains the shapes that will be tested
    xSideLength = ySideLength = zSideLength = 0.0
    xMin = xMax = yMin = yMax = zMin = zMax = 0.0
    
    # Create a shape to sample
    shape = BRepPrimAPI_MakeBox(side_length, side_length, side_length).Shape()
    print("Box type = ",shape.ShapeType())
    print(TopAbs_SOLID)
 
#     stlReader = StlAPI_Reader() 
#     shape = TopoDS_Shape() 
#     stlReader.Read(shape, "cantilever.stl")

    shape = TopoDS_Shape()
    builder = BRep_Builder()
    breptools_Read(shape, "cylinder.brep", builder);
    breptools_Clean(shape)

    print(shape.ShapeType())
    
    shapes = []
    explorer = TopExp_Explorer(shape,TopAbs_FACE)
    secondExplorer = TopExp_Explorer(shape,TopAbs_SOLID)
    cylSolid = secondExplorer.Current()
    print("CylSolid type = ",cylSolid.ShapeType())
    shape = explorer.Current()
    shapes.append(shape)
    print(shape.ShapeType())
    shape = explorer.Next()
    shape = explorer.Current()
    shapes.append(shape)
    print(shape.ShapeType())
    shape = explorer.Next()
    shape = explorer.Current()
    shapes.append(shape)
    print(shape.ShapeType())
    
    topoBuilder = TopoDS_Builder()
    shell = TopoDS_Shell()
    topoBuilder.MakeShell(shell)
    for face in shapes:
        topoBuilder.Add(shell,face)
        
    solid = TopoDS_Solid()
    topoBuilder.MakeSolid(solid)
    topoBuilder.Add(solid,shell)

    # Create a bounding box for the shape
    boundingBox = Bnd_Box()
    brepbndlib_Add(solid, boundingBox)
    xMin, yMin, zMin, xMax, yMax, zMax = boundingBox.Get()
    xSideLength = xMax - xMin
    ySideLength = yMax - yMin
    zSideLength = zMax - zMin
    print(xMin, xMax, yMin, yMax, zMin, zMax)
    print(xSideLength, ySideLength, zSideLength)

    # Create extrema sampler to measure if the point is in the shape. For now, 
    # just initialize it with the same shape. We'll load the vertex later.
    brepDistShapeShape = BRepExtrema_DistShapeShape(solid, shape)
    
    print("Shell type = ",shell.ShapeType())
    print("Solid type = ",solid.ShapeType())

    # Create a random number of vertices and check to see which ones are
    # in the shape.    
    vertices = pd.DataFrame(0.0, index=range(0, n), columns=('x', 'y', 'z', 'inShape'))
    vertices.inShape = vertices.inShape.astype('int')
    vertexList = []
    # Loop over the vertices
    for i in range(0, n):
        # Pick a random point
        x = xMin + random() * xSideLength
        y = yMin + random() * ySideLength
        z = zMin + random() * zSideLength
        # Create a vertex from a geometric point
        gpPoint = gp_Pnt(x, y, z)
        vertexBuilder = BRepBuilderAPI_MakeVertex(gpPoint)
        vertex = vertexBuilder.Vertex()
        vertexList.append(vertex)
        # Load the vertex into the extrema calculator
        brepDistShapeShape.LoadS2(vertex)
        brepDistShapeShape.Perform()
        # Compute the containment with the box and store the value
        inShape = 1 if brepDistShapeShape.InnerSolution() else 0
        # Store the shape value
        vertices.set_value(i, 'x', x)
        vertices.set_value(i, 'y', y)
        vertices.set_value(i, 'z', z)
        vertices.set_value(i, 'inShape', inShape)

    # Slice the data frame so that only the x,y,z variables for points in the box are saved.
    innerVertices = vertices[vertices.inShape == 1]
    innerCoords = innerVertices[['x', 'y', 'z']]
    # Write the coordinates that are saved to disk.
    innerCoords.to_csv('point.csv', index=False)
    
    # Just dump some diagnostics
    print(len(innerVertices))
    print(len(vertices))
    
    # Create a matplot lib figure and display the particles in 3D
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(innerCoords.x, innerCoords.y, innerCoords.z)
    plt.show()
     
    # Show the actual shape in 3D
    display, start_display, add_menu, add_function_to_menu = init_display()
    display.DisplayShape(vertexList, update=True)
    display.DisplayShape(shell, update=True)
    start_display()
    
