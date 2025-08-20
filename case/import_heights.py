import FreeCAD
import Spreadsheet

def import_csv_to_spreadsheet(filename, sheet_name="Heights", delimiter=','):
    """Properly imports CSV with column separation"""
    doc = FreeCAD.ActiveDocument
    sheet = doc.getObject(sheet_name) or doc.addObject("Spreadsheet::Sheet", sheet_name)
    
    # Clear previous content if needed
    sheet.clearAll()
    
    # Use FreeCAD's import function
    sheet.importFile(filename, delimiter)
    
    sheet.recompute()
    doc.recompute()
    return sheet

# Usage:
import_csv_to_spreadsheet("heights.csv", delimiter=",")
