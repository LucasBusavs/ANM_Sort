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

def phasesIndex(lines):
    dictIndex = {}
    phases = []
    start = 0
    end = 0
    value = ''
    aux = set()
    for i in tqdm(range(len(lines))):
        if lines[i] == '    <Placemark>\n':
            start = i
        elif lines[i] == '<td>Fase</td>\n':
            value = lines[i+2]
            value = value[4:-6]
            aux.add(value)
        elif lines[i] == '    </Placemark>\n':
            end = i + 1
            seTuple = (start, end)
            dictIndex[seTuple] = value
    phases = list(aux)
    phases.sort()
    dictSorted = sorted(dictIndex.items(), key=lambda x: x[1])
    return phases, dictSorted

def listToWrite(phasesIndexed, phases, lines):
    phasesCount = [0]*len(phases)
    i = 0
    kml = ''
    writeList = ['']*len(phases)
    phase = phasesIndexed[0][1]
    for process in tqdm(phasesIndexed):
        start, end = process[0]
        if phase != process[1]:
            writeList[i] = kml
            i = i + 1
            kml = ''
            phase = process[1]
        kml = kml + ''.join(lines[start:end])
        phasesCount[i] += 1
    writeList[i] = kml
    print(phasesCount)
    return writeList

def write(listToWrite, filePath, phases):
    
    with open(filePath, "r", encoding='utf8') as f:
        file = f.read()
    
    begin = file.split("<Placemark>")[0]
    end = file.split("</Placemark>")[-1]
    finalKml = begin
    for i in tqdm(range(len(listToWrite))):
        finalKml = finalKml + (f'\t\t<Folder>\n\t\t\t<name>{phases[i]}</name>\n') + listToWrite[i] + '</Folder>\n'
    finalKml = finalKml + end
    
    with open(filePath, "w", encoding='utf8') as f:
        f.write(finalKml)

def main():
    phasesIndexed = []
    phases = []

    filePath = fileDialog()
    begin = time.time()
    if filePath != '':
        if filePath.endswith(".kmz"):
            filePath = kmzConverter(filePath)

        with open(filePath, "r", encoding='utf8') as f:
            lines = f.readlines()

        phases, phasesIndexed = phasesIndex(lines)
        print(phases)
        writeList = listToWrite(phasesIndexed, phases, lines)
        write(writeList, filePath, phases)
    else:
        print("No file selected")
    return time.time() - begin

if __name__ == '__main__':
    executeTime = main()
    print(f'Time to execute in seconds: {executeTime: .5f}')
    print('Press enter to continue...')
    keyboard.wait("enter")