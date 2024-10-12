import argparse
import subprocess
import os

import json

def Generate():
    parser = argparse.ArgumentParser()
    
    parser.add_argument("-r", "--resouces", help="resouces dir", type=str, required=True)
    parser.add_argument("-i", "--input", help="file input list", nargs='+', default=[], required=True)
    parser.add_argument("-o", "--output", help="output path", type=str, required=True)
    parser.add_argument("-p", "--font", help="prefix", type=str, required=False)

    args = parser.parse_args()
    # print(args)
    
    startDir = os.getcwd()
    # print("startDir: %s" %(startDir))

    os.chdir(args.resouces)  

    cwd = os.getcwd()
    # print("cwd: %s" %(cwd))

    binaryDataList = []
    
    for localPath in args.input:
        # print("localPath: %s" % (localPath))
        with open(localPath, 'rb', ) as f:
            content = f.read()

            data = ', '.join(f'0x{byte:02x}' for byte in content)

            binaryDataList.append((localPath, data))

    #
    print(binaryDataList)

Generate()