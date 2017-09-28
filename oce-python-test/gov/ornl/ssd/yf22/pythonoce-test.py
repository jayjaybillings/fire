'''
This script will create a sample of points from a 3D volume. It uses
OpenCascade's BRep API through PythonOCC.
Created on Sep 27, 2017

@author: Jay Jay Billings (billingsjj@ornl.gov)
'''
from OCC._TopoDS import *
from OCC.Display.SimpleGui import init_display
from OCC.BRepPrimAPI import BRepPrimAPI_MakeBox
from OCC.BRepTools import breptools_Read
from OCC.TopoDS import TopoDS_Shape
from OCC.gp import gp_Pnt
from OCC.BRepBuilderAPI import BRepBuilderAPI_MakeVertex
from OCC.BRepExtrema import BRepExtrema_DistShapeShape
from random import random 
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

if __name__ == '__main__':
    
    # Number of points
    n = 100000
    
    # Set the side length
    side_length = 10.0
    # The side length of the box that contains the shapes that will be tested
    test_side_length = 20.0
    
    # Create a file for a shape to sample
    shape = BRepPrimAPI_MakeBox(side_length,side_length,side_length).Shape()

    # Create a random number of vertices and check to see which ones are
    # in the shape.    
    vertices = pd.DataFrame(0.0,index=range(0,n),columns=('x','y','z','inShape'))
    vertices.inShape = vertices.inShape.astype('int')
    # Loop over the vertices
    for i in range(0,n):
         # Create a vertex from a geometric point
         x = random()*test_side_length
         y = random()*test_side_length
         z = random()*test_side_length
         gpPoint = gp_Pnt(x,y,z)
         vertexBuilder = BRepBuilderAPI_MakeVertex(gpPoint)
         vertex = vertexBuilder.Vertex()
         # Compute the containment with the box and store the value
         inShape = 1 if BRepExtrema_DistShapeShape(shape,vertex).InnerSolution() else 0
         vertices.set_value(i, 'x', x)
         vertices.set_value(i, 'y', y)
         vertices.set_value(i, 'z', z)
         vertices.set_value(i, 'inShape', inShape)
    

    # Slice the data frame so that only the x,y,z variables for points in the box are saved.
    innerVertices = vertices[vertices.inShape == True]
    innerCoords = innerVertices[['x','y','z']]
    # Write the coordinates that are saved to disk.
    innerCoords.to_csv('point.csv',index=False)
    
    # Create a matplot lib figure and display the particles
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(innerCoords.x,innerCoords.y,innerCoords.z)
    plt.show()