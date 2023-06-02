# -*- coding: utf-8 -*-
# Algorithm to organize Brazilian mining data in folders, by their phases
import time
import keyboard

def main():
    begin = time.time()
    
    with open("PB.kml", "r", encoding='utf8') as f:
        for line in f:
            print(line, end="")
    
    return time.time() - begin

if __name__ == '__main__':
    executeTime = main()
    print(f'Time to execute in seconds: {executeTime: .5f}')
    print('Press enter to continue...')
    keyboard.wait("enter")