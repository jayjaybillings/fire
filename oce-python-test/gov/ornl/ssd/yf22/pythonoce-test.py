'''
This script will create a sample of points from a 3D volume. It uses
OpenCascade's BRep API through PythonOCC.
Created on Sep 27, 2017

@author: Jay Jay Billings (billingsjj@ornl.gov)
'''
from OCC._TopoDS import topods_Vertex
from OCC.Display.SimpleGui import init_display
from OCC.BRepPrimAPI import BRepPrimAPI_MakeBox
from OCC.TopoDS import TopoDS_Shape
from OCC.gp import gp_Pnt
from OCC.BRepBuilderAPI import BRepBuilderAPI_MakeVertex
from OCC.BRepExtrema import BRepExtrema_DistShapeShape   
from random import random 

if __name__ == '__main__':
    
    # Set the side length
    side_length = 10.0
    # The side length of the box that contains the shapes that will be tested
    test_side_length = 20.0
    
    # Create the display
    #isplay, start_display, add_menu, add_function_to_menu = init_display()
    
    # Create a box that will be sampled
    my_box = BRepPrimAPI_MakeBox(side_length,side_length,side_length).Shape()

    # Create a random number of vertices and check to see which ones are
    # in the shape.    
    innerVertices = []
    outerVertices = []
    # Loop over the vertices
    for i in range(0,100000):
         # Create the vertices from geometry points
         gpPoint = gp_Pnt(random()*test_side_length,random()*test_side_length,random()*test_side_length)
         vertexBuilder = BRepBuilderAPI_MakeVertex(gpPoint)
         vertex = vertexBuilder.Vertex()
         # Compute the containment with the box
         distShapeShape = BRepExtrema_DistShapeShape(my_box,vertex)
         # If the vertex is in the shape, add it to the list
         if distShapeShape.InnerSolution() == True:
           print("Point is in box")
           innerVertices.append(vertex)
         else:
           print("Point is not in the box")
           outerVertices.append(vertex)
        
        
    #display.DisplayShape(innerVertices, update=True);
    #display.DisplayShape(my_box, update=True);
    
    #start_display()