# -*- coding: utf-8 -*-
# Algorithm to organize Brazilian mining data in folders, by their phases

import tkinter as tk
from tkinter import filedialog
import os
from zipfile import ZipFile
import time

'''
Function to generate a file dialog to select the right file to be open
Returns a string containing the selected file path
'''
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

'''
Function to convert a .kmz file into a .kml file
Receive the path of.kmz file
Returns the new path of the .kml file
'''
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

'''
Function to discover the start and end positions of the phase type passed by parameter
Receive a block from the file, to be analyzed, and if "<td>Phase</td>" is not found, add the block to the notFound list
Returns a tuple of the start and end positions, in that order
'''
def boundPhase(block, notFound):
    notFound = []
    start = block.find("<td>Fase</td>")
    start = start + 19
    end = start +1
    if start!=-1:
        while block[start:end].find("<") == -1:
            end = end + 1
        end = end - 1
    else:
        notFound.append(block)
    return start, end

'''
Procedure to generate a list of phase types, each file has different types
Receives the file already splitted, notFound list, to pass as a parameter when calling boundPhase, and list of phases
Change the list of phases declared in main()
'''
def phasesTypes(splitFile, notFound, phases):
    for block in splitFile:
        if block!=splitFile[0]:
            start, end = boundPhase(block, notFound)
            try:
                phases.index(block[start:end])
            except:
                phases.append(block[start:end])

'''
Procedure to generate a list of phase types, each file has different types
Receives the file already splitted, notFound list, to pass as a parameter when calling boundPhase, and list of phases
Change the list of phases declared in main()
'''
def phasesTypesV2(splitFile, notFound):
    setPhases = set()
    for block in splitFile:
        if block!=splitFile[0]:
            start, end = boundPhase(block, notFound)
            setPhases.add(block[start:end])
    print(setPhases)
    phases = list(setPhases)
    print(phases)
    return phases

'''
Function to write the final .kml file
Receives a list of phases types , the file already splitted, and notFound list, to pass as a parameter when calling boundPhase
Returns a kml string containing the result of the algorithm
'''
def write(phases, splitFile, notFound):
    kml = ''
    for phase in phases:
        for block in splitFile:
            if block!=splitFile[0]:
                if block == splitFile[-1]:
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
    kml = splitFile[0] + kml + last[1]
    return kml

def test(phases, splitFile, notFound):
    kml = ''
    flag = 0
    for phase in phases:
        for i in range(1, len(splitFile)):
            block = splitFile[i]
            if i == len(splitFile) - 1:
                last = block.split("</Placemark>")
                block = last[0] + "</Placemark>"
            start, end = boundPhase(block, notFound)
            if phase == block[start:end]:
                if flag == 0:
                    kml = kml + (f'\t\t<Folder>\n\t\t\t<name>{phase}</name>\n')
                    flag = 1
                kml = kml + '<Placemark>'
                kml = kml + block
        kml = kml + '</Folder>\n'
        flag = 0
    kml = splitFile[0] + kml + last[1]
    return kml

def test2(phases, splitFile, notFound):
    kmlList = [''] * len(phases)
    for i in range(1, len(splitFile)):
        block = splitFile[i]
        if i == len(splitFile) - 1:
            last = block.split("</Placemark>")
            block = last[0] + "</Placemark>"
        start, end = boundPhase(block, notFound)
        for j in range(len(phases)):
            if phases[j] == block[start:end]:
                kmlList[j] = kmlList[j] + '<Placemark>'
                kmlList[j] = kmlList[j] + block
    kmlResult = result(kmlList, splitFile, last, phases)
    return kmlResult

def result(kmlList, splitFile, last, phases):
    kmlResult = splitFile[0]
    for i in range(len(kmlList)):
        kmlResult = kmlResult + (f'\t\t<Folder>\n\t\t\t<name>{phases[i]}</name>\n') + kmlList[i] + '</Folder>\n'
    kmlResult = kmlResult + last[1]
    return kmlResult

def count(phases, splitFile, notFound):
    phasesCount = [0] * len(phases)
    for block in splitFile:
        if block!=splitFile[0]:
            start, end = boundPhase(block, notFound)
            phase = block[start:end]
            for i in range(len(phases)):
                if phases[i]==phase:
                    phasesCount[i] = phasesCount[i] + 1
                    break
    return phasesCount

def sortPhases(phases, phasesCount):
    tupleList = list(zip(phases, phasesCount))
    orderlyTuple = sorted(tupleList, key=lambda x: x[1], reverse=True)
    phasesOrderly = [t[0] for t in orderlyTuple]
    return phasesOrderly

def main():
    notFound = []
    phases = []
    phasesCount = []

    filePath = fileDialog()
    begin = time.time()
    if filePath != '':
        if filePath.endswith(".kmz"):
            filePath = kmzConverter(filePath)
        
        with open(filePath, "r", encoding='utf8') as f:
            lines = f.read()
        
        split = lines.split("<Placemark>")
        
        phases = phasesTypesV2(split, notFound)
        phases.sort()
        phasesCount = count(phases, split, notFound)
        print(phases)
        print(phasesCount)
        #phases = sortPhases(phases, phasesCount)
        #print(phases)
        
        kml = test2(phases, split, notFound)

        with open(filePath, "w", encoding='utf8') as f:
            f.write(kml) 
        
        if len(notFound) != 0:
            print("Some not found")

    else:
        print("No file selected")
    return begin

if __name__ == '__main__':
    begin = main()
    end = time.time()
    executeTime = end - begin
    print(f'Time to execute in seconds: {executeTime: .5f}')