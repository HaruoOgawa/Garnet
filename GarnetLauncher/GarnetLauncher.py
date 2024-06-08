import argparse
from ast import arg
import os
import shutil

def AddPunct(Path):
    if(Path[len(Path) - 1] != "\\" and Path[len(Path) - 1] != "/"):
        return Path + "/"
    
    return Path

def ReplaceGarnetDir(GarnetPath, ProjectName, TargetFileName, SearchText, encodingType):
    data = []

    with open(TargetFileName, 'r',encoding=encodingType) as file:
        data = file.read()
        data = data.replace(SearchText, GarnetPath)
        data = data.replace("GarnetFrame", ProjectName)

    with open(TargetFileName, 'w',encoding=encodingType) as file:
        file.write(data)

def Main():
    # コマンドライン引数
    parser = argparse.ArgumentParser(
        prog="GarnetLauncher",
        description="Generate Garnet Project.",
        epilog=""
    )

    parser.add_argument("-i", "--input-garnet-path", help="Base Garnet Project Dir")
    parser.add_argument("-o", "--generate-path", help="Path to generate project")
    parser.add_argument("-p", "--project-name", help="Project Name")

    args = parser.parse_args()

    if(args.input_garnet_path == None or args.input_garnet_path == ""):
        return False

    if(args.generate_path == None or args.generate_path == ""):
        return False
    
    if(args.project_name == None or args.project_name == ""):
        return False
    
    print("Start to generate " + args.project_name)

    # 資材のコピー
    GarnetPath = AddPunct(args.input_garnet_path)
    GeneratePath = AddPunct(args.generate_path)
    GarnetFramePath = AddPunct(GarnetPath + "../GarnetLauncher/GarnetFrame/")

    shutil.copytree(GarnetFramePath, GeneratePath, dirs_exist_ok=True)
    
    # ファイル名を変更
    ProjectName = args.project_name
    
    os.rename(GeneratePath + "GarnetFrame.sln", GeneratePath + ProjectName + ".sln")
    os.rename(GeneratePath + "GarnetFrame.vcxproj", GeneratePath + ProjectName + ".vcxproj")
    os.rename(GeneratePath + "GarnetFrame.vcxproj.filters", GeneratePath + ProjectName + ".vcxproj.filters")
    os.rename(GeneratePath + "GarnetFrame.vcxproj.user", GeneratePath + ProjectName + ".vcxproj.user")

    # ファイルの『..\..\Garnet』をGarnetPathに置換する
    ReplaceGarnetDir(GarnetPath, ProjectName, GeneratePath + ProjectName + ".sln", "..\\..\\Garnet\\", "utf-8_sig")
    ReplaceGarnetDir(GarnetPath, ProjectName, GeneratePath + ProjectName + ".vcxproj", "..\\..\\Garnet\\", "utf-8_sig")
    ReplaceGarnetDir(GarnetPath, ProjectName, GeneratePath + "Commands/MakeEmccBuild.bat", "../../../Garnet/", None)

    return True
#
Main()