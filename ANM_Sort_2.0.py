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

def write(dictSorted, phases, lines):
    i = 0
    kml = ""
    writeList = [len(phases)]
    phase = dictSorted[0][1]
    for process in dictSorted:
        start, end = process[0]
        end = end + 1
        if phase == process[1]:
            kml = lines[start:end]
        else:
            ...

def main():
    dictIndex = {}
    aux = set()
    start = 0
    end = 0
    value = ''

    filePath = fileDialog()
    begin = time.time()
    if filePath != '':
        if filePath.endswith(".kmz"):
            filePath = kmzConverter(filePath)
        
        with open(filePath, "r", encoding='utf8') as f:
            lines = f.readlines()
            for i in range(len(lines)):
                if lines[i] == '    <Placemark>\n':
                    start = i
                elif lines[i] == '<td>Fase</td>\n':
                    value = lines[i+2]
                    value = value[4:-6]
                    aux.add(value)
                    phases = list(aux)
                    phases.sort()
                elif lines[i] == '    </Placemark>\n':
                    end = i
                    seTuple = (start, end)
                    dictIndex[seTuple] = value
        dictSorted = sorted(dictIndex.items(), key=lambda x: x[1])
        write(dictSorted, phases, lines)
    else:
        print("No file selected")
    return time.time() - begin

if __name__ == '__main__':
    executeTime = main()
    print(f'Time to execute in seconds: {executeTime: .5f}')
    print('Press enter to continue...')
    keyboard.wait("enter")