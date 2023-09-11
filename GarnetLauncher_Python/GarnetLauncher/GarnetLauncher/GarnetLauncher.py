import argparse
from ast import arg
import sys
import shutil
import os

def CopyData(inputDir, dstPath, isFirst):
    RootPath = inputDir

    # Copy Src
    SrcPath = RootPath + "Src/"
    SrcSubfolders = os.listdir(SrcPath)

    for folder in SrcSubfolders:
        if(folder == "Library"):
            continue
        
        if(not isFirst and (folder == "App" or folder == "Scene")):
            continue

        currentPath = SrcPath + folder
        currentDstPath = dstPath + "Src/" + folder
        
        shutil.copytree(currentPath, currentDstPath, dirs_exist_ok=True)

        print("copy folder: %s" % (currentPath))
        
    # Copy Library
    if(isFirst):
        LibraryPath = RootPath + "Src/Library/"
        LibSubfolders = os.listdir(LibraryPath)    
    
        for folder in LibSubfolders:
                if(folder == "dawn" or folder == "DawnLegacy"):
                    continue

                currentPath = LibraryPath + folder
                currentDstPath = dstPath + "Src/Library/" + folder        

                shutil.copytree(currentPath, currentDstPath, dirs_exist_ok=True)

                print("copy folder: %s" % (currentPath))

    # Copy File
    os.makedirs(dstPath + "Resources/Shaders/", exist_ok=True)
    shutil.copy(RootPath + "Resources/Shaders/" + "depth.vert", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "depth.frag", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "loadingbar.vert", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "loadingbar.frag", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "pbr.vert", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "pbr.frag", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "blur.vert", dstPath + "Resources/Shaders/")
    shutil.copy(RootPath + "Resources/Shaders/" + "blur.frag", dstPath + "Resources/Shaders/")

    if(isFirst):
        shutil.copytree(RootPath + "Commands", dstPath + "Commands", dirs_exist_ok=True)
        shutil.copytree(RootPath + "EmscriptenBuild", dstPath + "EmscriptenBuild", dirs_exist_ok=True)
        shutil.copy(RootPath + "Garnet.sln", dstPath)
        shutil.copy(RootPath + "Garnet.vcxproj", dstPath)
        shutil.copy(RootPath + "Garnet.vcxproj.filters", dstPath)
        shutil.copy(RootPath + "../" + ".gitignore", dstPath)

def Main():
    parser = argparse.ArgumentParser(
        prog="HRMExporter",
        description="Export HRM or hlTF",
        epilog=""
    )

    parser.add_argument("-i", "--inputDir")
    parser.add_argument("-o", "--outDir")
    parser.add_argument("-f", "--isFirst", action="store_true")

    args = parser.parse_args()
    
    inputDir = args.inputDir
    if(inputDir != None and (inputDir[-1] != '/' and inputDir[-1] != '\\')):
         inputDir += "/"

    print("inputDir: %s" % (inputDir))

    dstPath = args.outDir

    if(dstPath != None and (dstPath[-1] != '/' and dstPath[-1] != '\\')):
         dstPath += "/"

    
    print("dstPath: %s" % (dstPath))

    CopyData(inputDir, dstPath, args.isFirst)
    
Main()