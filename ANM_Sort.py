# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import filedialog

filetypes = (
    ('Google Earth (.kml)', '*.KML'),
    ('Google Earth (.kmz)', '*.KMZ'),
)
# open-file dialog
root = tk.Tk()
filename = tk.filedialog.askopenfilename(
    title='Select a file...',
    filetypes=filetypes,
)
root.destroy()