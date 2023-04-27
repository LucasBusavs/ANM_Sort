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
if filePath != '':
    if filePath.endswith(".kmz"):
        kmzConverter(filePath)
else:
    print("No file selected")