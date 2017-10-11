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

'''
This function creates a Pandas data frame that is configured with the right
information for holding the points.
@param n the number of rows in the data frame, i.e. - the number of points
@return the data frame with columns x, y, z, and inShape. 'inShape' should be 0
if the particle is not in the shape, 1 if it is.
'''
def createPointsDataFrame(n):
    frame = pd.DataFrame(0.0, index=range(0, n), columns=('x', 'y', 'z', 'inShape'))
    return frame

'''
This function performs a simple Monte Carlo sampling of the solid
passed in as input and returns a list of coordinates in the solid.
@param n the maximum number of samples to throw
@param solid the solid that should be sampled
@return a Pandas data fraame listing of points discovered in the 
solid. Its length is less than n.
'''
def sampleSolid(n,solid,vertexList=None):
    # Create a bounding box for the shape
    boundingBox = Bnd_Box()
    brepbndlib_Add(solid, boundingBox)
    xMin, yMin, zMin, xMax, yMax, zMax = boundingBox.Get()
    xSideLength = xMax - xMin
    ySideLength = yMax - yMin
    zSideLength = zMax - zMin

    # Create extrema sampler to measure if the point is in the shape. For now, 
    # just initialize it with the same shape. We'll load the vertex later.
    brepDistShapeShape = BRepExtrema_DistShapeShape(solid, solid)

    # Create a random number of vertices and check to see which ones are
    # in the shape.    
    vertices = createPointsDataFrame(n)
    vertices.inShape = vertices.inShape.astype('int')
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
        if inShape != 0:
            vertexList.append(vertex)

    # Slice the data frame so that only the x,y,z variables for points in the box are saved.
    innerVertices = vertices[vertices.inShape == 1]
    innerCoords = innerVertices[['x', 'y', 'z']]
    
    return innerCoords

'''
This is the main method that lauches the sampling job.
'''
if __name__ == '__main__':
    
    # Number of points
    n = 100000
    
    # List for storing TopoDS_Shape instances of vertices if required.
    vertexList = []
    
    # Set the side length
    side_length = 10.0
    # The side length of the box that contains the shapes that will be tested
    xSideLength = ySideLength = zSideLength = 0.0
    xMin = xMax = yMin = yMax = zMin = zMax = 0.0
    
    # Read the file to get the shape to be sampled.
    shape = TopoDS_Shape()
    builder = BRep_Builder()
    breptools_Read(shape, "cantilever-repaired-unrefined.brep", builder);
    
    # Loop over the shape pulled from the input and look for solids.
    solids = [] # A list for storing solids from the file
    i = 0 # A counter for writing vertices/points
    allPointsDataFrame = pd.DataFrame([]) # A dataframe for ALL points
    # OpenCASCADE can only successfully execute point inclusion queries if the 
    # shape in question is a full solid, which is defined in their topology as 
    # TopAbs_SOLID. Each solid in the file can sampled individually by pulling
    # it from the compound.
    if shape.ShapeType() != TopAbs_SOLID:
        # Create a topology explorer and pull all the solids from the shape
        explorer = TopExp_Explorer(shape,TopAbs_SOLID)
        # Loop over all the solids
        while explorer.More():
            solid = explorer.Current()
            #pointsInSolid = sampleSolid(n,solid)
            pointsInSolid = sampleSolid(n,solid,vertexList)
            # Write the coordinates that are saved to disk.
            i = i + 1
            fileName = 'solid_' + str(i) + '.csv'
            pointsInSolid.to_csv(fileName, index=False)     
            # Store the points for later
            allPointsDataFrame = allPointsDataFrame.append(pointsInSolid.copy(), ignore_index=False)
            # Store the solid for later reference (i.e. - visualization)
            solids.append(solid)
            # Go to the next solid if one exists
            explorer.Next()
    else:
        # Otherwise, just quit
        exit("No solids found in input. Aborting.")
        
     # Create a matplot lib figure and display the particles in 3D
#     fig = plt.figure()
#     ax = fig.add_subplot(111, projection='3d')
#     ax.scatter(allPointsDataFrame.x, allPointsDataFrame.y, allPointsDataFrame.z)
#     plt.show()
     
    # Show the actual shape in 3D
#     display, start_display, add_menu, add_function_to_menu = init_display()
#     display.DisplayShape(vertexList, update=True)
#     start_display()
     
