# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import filedialog
import os
from zipfile import ZipFile

filetypes = (
    ('Google Earth (.kmz)', '*.KMZ'),
)
# open-file dialog
root = tk.Tk()
filename = tk.filedialog.askopenfilename(
    title='Select a file...',
    filetypes=filetypes,
)
root.destroy()
if filename != '':
    name = filename.split('/')
    name = name[-1]
    if name.endswith(".kmz"):
        name = filename.split('.kmz')
        zip = name[0] + ".zip"
        os.rename(filename, zip)
        path = os.path.dirname(zip)
        dir1 = os.listdir(path)
        with ZipFile(zip, "r") as z:
            z.extractall(path)
        dir2 = os.listdir(path)
        kmlFile = [i for i in dir2 if i not in dir1]
        kmlFile = kmlFile[0]
        os.remove(zip)
        os.rename(path + "/" + kmlFile, name[0] + ".kml")
        print('KMZ converterd successfully')