# -*- coding: utf-8 -*-
# Algorithm to organize Brazilian mining data in folders, by their phases

import tkinter as tk
from tkinter import filedialog
import os
from zipfile import ZipFile
import time
import keyboard
from tqdm import tqdm

'''
Function to generate a file dialog to select the right file to be open
Returns a string containing the selected file path
'''
def fileDialog():
    filetypes = (
        ('Google Earth (.kmz)', '*.KMZ'),
        ('Google Earth (.kml)', '*.KML'),
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
Receive a block from the file, to be analyzed
Returns a tuple of the start and end positions, in that order
'''
def boundPhase(block):
    start = block.find("<td>Fase</td>")
    start = start + 19
    end = start +1
    while block[start:end].find("<") == -1:
        end = end + 1
    end = end - 1
    return start, end

'''
Function to generate a list of phases types
Receives the file already splitted
Returns a list of phases types
'''
def phasesTypes(splitFile):
    setPhases = set()
    for block in splitFile:
        if block!=splitFile[0]:
            start, end = boundPhase(block)
            setPhases.add(block[start:end])
    phases = list(setPhases)
    return phases

'''
Function to write the final .kml file
Receives a list of phases types , the file already splitted
Returns a kml string containing the result of the algorithm
'''
def write(phases, splitFile):
    kmlList = [''] * len(phases)
    for i in tqdm(range(1, len(splitFile))):
        block = splitFile[i]
        if i == len(splitFile) - 1:
            last = block.split("</Placemark>")
            block = last[0] + "</Placemark>"
        start, end = boundPhase(block)
        for j in range(len(phases)):
            if phases[j] == block[start:end]:
                kmlList[j] = kmlList[j] + '<Placemark>'
                kmlList[j] = kmlList[j] + block
                break
    kmlResult = result(kmlList, splitFile, last, phases)
    return kmlResult

def result(kmlList, splitFile, last, phases):
    kmlResult = splitFile[0]
    for i in range(len(kmlList)):
        kmlResult = kmlResult + (f'\t\t<Folder>\n\t\t\t<name>{phases[i]}</name>\n') + kmlList[i] + '</Folder>\n'
    kmlResult = kmlResult + last[1]
    return kmlResult

def count(phases, splitFile):
    phasesCount = [0] * len(phases)
    for block in splitFile:
        if block!=splitFile[0]:
            start, end = boundPhase(block)
            phase = block[start:end]
            for i in range(len(phases)):
                if phases[i]==phase:
                    phasesCount[i] = phasesCount[i] + 1
                    break
    return phasesCount

def main():
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
        
        phases = phasesTypes(split)
        phases.sort()
        phasesCount = count(phases, split)
        print(phases)
        print(phasesCount)
        
        kml = write(phases, split)

        with open(filePath, "w", encoding='utf8') as f:
            f.write(kml)

    else:
        print("No file selected")
    return time.time() - begin

if __name__ == '__main__':
    executeTime = main()
    print(f'Time to execute in seconds: {executeTime: .5f}')
    print('Press enter to continue...')
    keyboard.wait("enter")