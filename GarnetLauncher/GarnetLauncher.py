import argparse
import shutil

def AddPunct(Path):
    if(Path[len(Path) - 1] != "\\" and Path[len(Path) - 1] != "/"):
        return Path + "/"
    
    return Path

def Main():
    # コマンドライン引数
    parser = argparse.ArgumentParser(
        prog="GarnetLauncher",
        description="Generate Garnet Project.",
        epilog=""
    )

    parser.add_argument("-i", "--input-garnet-path", help="Base Garnet Project Dir")
    parser.add_argument("-o", "--generate-path", help="Path to generate project")

    args = parser.parse_args()

    if(args.input_garnet_path == None or args.input_garnet_path == ""):
        return False

    if(args.generate_path == None or args.generate_path == ""):
        return False

    GarnetPath = AddPunct(args.input_garnet_path)
    GeneratePath = AddPunct(args.generate_path)
    GarnetFramePath = AddPunct("./GarnetFrame/")

    shutil.copytree(GarnetFramePath, GeneratePath, dirs_exist_ok=True)
    
    return True
#
Main()