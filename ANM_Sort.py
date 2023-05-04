# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import filedialog
import os
from zipfile import ZipFile

def fileDialog():
    filetypes = (
        ('Google Earth (.kml)', '*.KML'),
        ('Google Earth (.kmz)', '*.KMZ'),
    )
    
    root = tk.Tk()
    filePath = tk.filedialog.askopenfilename(
        title='Select a file...',
        filetypes=filetypes,
    )
    root.destroy()
    
    return filePath

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
    print('KMZ converted successfully')
    return fileName[0] + ".kml"

def boundPhase(block, notFound):
    notFound = []
    start = block.find("<td>Fase</td>")
    if start!=-1:
        start = start + 19
        end = start +1
        while block[start:end].find("<") == -1:
            end = end + 1
        end = end - 1
    else:
        notFound.append(block)
    return start, end

def phasesName(splitFile, notFound, phases):
    for block in splitFile:
        if block!=splitFile[0]:
            start, end = boundPhase(block, notFound)
            try:
                phases.index(block[start:end])
            except:
                phases.append(block[start:end])

def main():
    notFound = []
    phases = []
    kml = ''

    filePath = fileDialog()
    if filePath != '':
        if filePath.endswith(".kmz"):
            filePath = kmzConverter(filePath)
        
        with open(filePath, "r", encoding='utf8') as f:
            lines = f.read()
        
        split = lines.split("<Placemark>")
        
        phasesName(split, notFound, phases)
        phases.sort()
        print(phases)
        
        for phase in phases:
            for block in split:
                if block!=split[0]:
                    if block == split[-1]:
                        last = block.split("</Placemark>")
                        block = last[0] + "</Placemark>"
                    start, end = boundPhase(block, notFound)
                    if phase == block[start:end]:
                        try:
                            kml.index(f'\t\t<Folder>\n\t\t\t<name>{phase}</name>\n')
                        except:
                            kml = kml + (f'\t\t<Folder>\n\t\t\t<name>{phase}</name>\n')
                        kml = kml + '<Placemark>'
                        kml = kml + block
            kml = kml + '</Folder>\n'
        kml = split[0] + kml + last[1]
        
        with open(filePath, "w", encoding='utf8') as f:
            f.write(kml) 
        
        if len(notFound) != 0:
            print("Some not found")

    else:
        print("No file selected")

if __name__ == '__main__':
    main()