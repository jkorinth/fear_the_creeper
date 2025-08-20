import FreeCAD, FreeCADGui, Part
import Spreadsheet

class PixelStampFeature:
    def __init__(self, obj):
        obj.Proxy = self
        # Basic grid properties
        obj.addProperty("App::PropertyFloat", "CubeSize", "Grid", "Cube base size").CubeSize = 10.0
        obj.addProperty("App::PropertyFloat", "MinHeight", "Grid", "Height of Cube with depth 1").MinHeight = 1.0
        obj.addProperty("App::PropertyFloat", "MaxHeight", "Grid", "Height of Cube with depth 0").MaxHeight = 10.0
        obj.addProperty("App::PropertyFloat", "Spacing", "Grid", "Space between cubes").Spacing = 1.0
        obj.addProperty("App::PropertyInteger", "Rows", "Grid", "Number of rows").Rows = 8 
        obj.addProperty("App::PropertyInteger", "Columns", "Grid", "Number of columns").Columns = 8
        obj.addProperty("App::PropertyInteger", "Reversed", "Grid", "Reverse z directioh").Reversed = True
        
        # Spreadsheet connection
        obj.addProperty("App::PropertyLink", "HeightSheet", "HeightData", "Spreadsheet with height data").HeightSheet = next((s for s in App.ActiveDocument.Objects if s.TypeId == "Spreadsheet::Sheet" and s.Label == "D"), None)
        obj.addProperty("App::PropertyString", "HeightStartCell", "HeightData", "Top-left cell of height data (e.g., 'B2')").HeightStartCell = "H2"
        
        # Allow expressions for basic properties
        obj.setExpression("MaxHeight", None)
        obj.setExpression("MinHeight", None)
        obj.setExpression("CubeSize", None)
        obj.setExpression("Spacing", None)
    
    def execute(self, obj):
        try:
            cube_size = obj.CubeSize
            spacing = obj.Spacing
            
            # Get height data from spreadsheet
            heights = []
            if obj.HeightSheet and obj.HeightStartCell:
                start_col = ord(obj.HeightStartCell[0].upper()) - ord('A')
                start_row = int(''.join(filter(str.isdigit, obj.HeightStartCell))) - 1
                
                for row in range(obj.Rows):
                    row_heights = []
                    for col in range(obj.Columns):
                        cell_col = chr(ord('A') + start_col + col)
                        cell_row = start_row + row + 1  # +1 because spreadsheet rows are 1-based
                        cell_ref = f"{cell_col}{cell_row}"
                        try:
                            row_heights.append(float(obj.HeightSheet.get(cell_ref)))
                        except:
                            row_heights.append(10.0)  # Default height if cell is empty/invalid
                    heights.append(row_heights)
            
            cubes = []
            totall = cube_size * obj.Columns + (obj.Columns - 1) * obj.Spacing
            totalw = cube_size * obj.Rows + (obj.Rows - 1) * obj.Spacing
            startx = -totall / 2
            starty =  totalw / 2
            for row in range(obj.Rows):
                for col in range(obj.Columns):
                    #hf = 1.0 - heights[row][col] if (obj.HeightSheet and heights) else 1.0
                    hf = heights[row][col] if (obj.HeightSheet and heights) else 1.0
                    height = obj.MaxHeight * hf + obj.MinHeight
                    if height > 0:
                        cube = Part.makeBox(cube_size, cube_size, height)
                        cube.translate(FreeCAD.Vector(
                            startx + col * (cube_size + spacing),
                            starty - (row+1) * (cube_size + spacing),
                            -height if obj.Reversed else 0,
                        ))
                        cubes.append(cube)
            obj.Shape = cubes[0].fuse(cubes[1:]).removeSplitter()
            #obj.Shape = Part.makeCompound(cubes)
        except Exception as e:
            FreeCAD.Console.PrintError(f"Error generating stamp: {str(e)}\n")

def makePixelStampFeature():
    doc = FreeCAD.ActiveDocument or FreeCAD.newDocument()
    obj = doc.addObject("PartDesign::FeaturePython", "PixelStamp")
    PixelStampFeature(obj)
    obj.ViewObject.Proxy = 0
    doc.recompute()
    return obj

# Create the object
stamp = makePixelStampFeature()
