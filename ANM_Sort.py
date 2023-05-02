# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import filedialog
import os
from zipfile import ZipFile

def kmzConverter(filePath):
    fileName = filePath.split(".kmz")
    zip = fileName[0] + ".zip"
    os.rename(filePath, zip)
    path = os.path.dirname(zip)
    dir1 = os.listdir(path)
    with ZipFile(zip, "r") as z:
        z.extractall(path)
    dir2 = os.listdir(path)
    kmlFile = [i for i in dir2 if i not in dir1]
    kmlFile = kmlFile[0]
    os.remove(zip)
    os.rename(path + "/" + kmlFile, fileName[0] + ".kml")
    print('KMZ converterd successfully')
    return fileName[0] + ".kml"

def phasesName(file):
    split = file.split("<Placemark>")
    
    for block in split:
        if block!=split[0]:
            start = block.find("<td>Fase</td>")
            if start!=-1:
                start = start + 19
                end = start +1
                while block[start:end].find("<") == -1:
                    end = end + 1
                end = end - 1

                try:
                    phases.index(block[start:end])
                except:
                    phases.append(block[start:end])
                
            else:
                notFound.append(block)

filetypes = (
    ('Google Earth (.kml)', '*.KML'),
    ('Google Earth (.kmz)', '*.KMZ'),
)
# open-file dialog
root = tk.Tk()
filePath = tk.filedialog.askopenfilename(
    title='Select a file...',
    filetypes=filetypes,
)
root.destroy()

notFound = []
phases = []

if filePath != '':
    if filePath.endswith(".kmz"):
        filePath = kmzConverter(filePath)
    
    with open(filePath, "r", encoding='utf8') as f:
        lines = f.read()
    
    phasesName(lines)
    
    print(phases)
    
    if len(notFound) != 0:
        print("Some not found")

else:
    print("No file selected")