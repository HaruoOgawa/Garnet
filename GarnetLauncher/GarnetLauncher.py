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
        data = data.replace("GarnetDev", ProjectName)

    with open(TargetFileName, 'w',encoding=encodingType) as file:
        file.write(data)

def ReplaceText(TargetFilePath, SrcText, DstText, encodingType):
    data = []

    with open(TargetFilePath, 'r',encoding=encodingType) as file:
        data = file.read()
        data = data.replace(SrcText, DstText)

    with open(TargetFilePath, 'w',encoding=encodingType) as file:
        file.write(data)

def Main():
    # コマンドライン引数
    parser = argparse.ArgumentParser(
        prog="GarnetLauncher",
        description="Generate Garnet Project.",
        epilog=""
    )

    parser.add_argument("-i", "--input-garnet-sln-path", help="set Garnet.sln path")
    # parser.add_argument("-o", "--generate-path", help="Path to generate project")
    parser.add_argument("-p", "--project-name", help="Project Name")

    args = parser.parse_args()

    if(args.input_garnet_sln_path == None or args.input_garnet_sln_path == ""):
        return False

    # if(args.generate_path == None or args.generate_path == ""):
        # return False
    
    if(args.project_name == None or args.project_name == ""):
        return False
    
    print("Start to generate " + args.project_name)

    #
    SlnPath = args.input_garnet_sln_path

    # プロジェクトファイル
    ProjectName = args.project_name

    # 資材をまとめてコピー
    GarnetPath = AddPunct(os.path.dirname(SlnPath))
    GeneratePath = AddPunct(GarnetPath + "../../" + ProjectName)
    # GeneratePath = AddPunct(args.generate_path)
    GarnetDevPath = AddPunct(GarnetPath + "../GarnetDev/")

    # 最低限必要なものだけ選択する
    shutil.copytree(GarnetDevPath + "Src", GeneratePath + "Src", dirs_exist_ok=True)
    shutil.copytree(GarnetDevPath + "Commands", GeneratePath + "Commands", dirs_exist_ok=True)
    shutil.copytree(GarnetDevPath + "compress_tools", GeneratePath + "compress_tools", dirs_exist_ok=True)
    
    # 個別に必要なファイルをコピー
    shutil.copy(GarnetDevPath + "GarnetDev.sln", GeneratePath + ProjectName + ".sln")
    shutil.copy(GarnetDevPath + "GarnetDev.vcxproj", GeneratePath + ProjectName + ".vcxproj")
    shutil.copy(GarnetDevPath + "GarnetDev.vcxproj.filters", GeneratePath + ProjectName + ".vcxproj.filters")
    shutil.copy(GarnetDevPath + "GarnetDev.vcxproj.user", GeneratePath + ProjectName + ".vcxproj.user")

    # EmscriptenBuild
    if not os.path.exists(GeneratePath + "EmscriptenBuild"):
        os.makedirs(GeneratePath + "EmscriptenBuild")
        
    shutil.copy(GarnetDevPath + "EmscriptenBuild/index.html", GeneratePath + "EmscriptenBuild/index.html", )
    shutil.copy(GarnetDevPath + "EmscriptenBuild/Garnet_front.js", GeneratePath + "EmscriptenBuild/Garnet_front.js")
    shutil.copytree(GarnetDevPath + "EmscriptenBuild/obj_fbxsdk", GeneratePath + "EmscriptenBuild/obj_fbxsdk", dirs_exist_ok=True)
    shutil.copytree(GarnetDevPath + "EmscriptenBuild/obj_lib", GeneratePath + "EmscriptenBuild/obj_lib", dirs_exist_ok=True)

    # Resouces
    shutil.copytree(GarnetDevPath + "Resources/Shaders", GeneratePath + "Resources/Shaders", dirs_exist_ok=True)
    
    shutil.copytree(GarnetDevPath + "Resources/MaterialFrame", GeneratePath + "Resources/MaterialFrame", dirs_exist_ok=True)

    if not os.path.exists(GeneratePath + "Resources/Scene"):
        os.makedirs(GeneratePath + "Resources/Scene")

    shutil.copy(GarnetDevPath + "Resources/Scene/Sample.json", GeneratePath + "Resources/Scene/Sample.json")

    # Garnetのルートディレクトリとプロジェクトは同じディレクトリに配置する必要がある(GeneratePathもその前提)
    LocalGarnetPath = "..\\Garnet\\Garnet\\"

    # ファイルの『..\\Garnet\\』をGarnetPathに置換する
    ReplaceGarnetDir(LocalGarnetPath, ProjectName, GeneratePath + ProjectName + ".sln", "..\\Garnet\\", "utf-8_sig")
    ReplaceGarnetDir(LocalGarnetPath, ProjectName, GeneratePath + ProjectName + ".vcxproj", "..\\Garnet\\", "utf-8_sig")
    ReplaceGarnetDir(LocalGarnetPath, ProjectName, GeneratePath + "Commands/MakeEmccBuild.bat", "..\\Garnet\\", None)
    ReplaceText(GeneratePath + "Src/Main/main.cpp", "CDevApp", "CScriptApp", "utf-8")

    return True
#
Main()