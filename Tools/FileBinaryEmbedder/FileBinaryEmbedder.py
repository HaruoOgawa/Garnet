import argparse
import subprocess
import os

import json

def CreateHeaderFile(cppFileName, dstDir):
    dst = '''
#pragma once

#include <vector>
#include <string>

namespace resource
{
	class %s
	{
            public:
                static std::vector<unsigned char> GetBinary(const std::string& Key);
	};
}
    ''' % (cppFileName)

    # WriteFile
    dstPath = dstDir + "%s.h" % (cppFileName)

    f = open(dstPath, 'w')
    f.write(dst)
    f.close()

def CreateSourceFile(resouces, fileList, prefix, cppFileName, dstDir):
    startDir = os.getcwd()

    # 作業Dirへ移動(Resoucesフォルダの親ディレクトリ)
    os.chdir(resouces)  
    
    binaryDataList = []
    
    for localPath in fileList:
        # print("localPath: %s" % (localPath))
        with open(localPath, 'rb', ) as f:
            content = f.read()

            data = ', '.join(f'0x{byte:02x}' for byte in content)

            binaryDataList.append((localPath, data))

    #
    # print(binaryDataList)
    
    # 元のディレクトリに戻る
    os.chdir(startDir)  

    #
    dstBinaryLine = ''
    for localPath,binaryData in binaryDataList:
        dstBinaryLine += '''        else if (Key == \"%s\")
        {
            DstBuffer = { %s };
        }
    ''' % (localPath, binaryData)

    #
    dst = '''
#include "CEmbeddedFileList.h"

namespace resource
{
	std::vector<unsigned char> %s::GetBinary(const std::string& Key)
	{
		std::vector<unsigned char> DstBuffer;

        if (Key == "")
        {
        }
%s

		return DstBuffer;
	}
}
    ''' % (cppFileName, dstBinaryLine)

    # WriteFile
    dstPath = dstDir + "%s.cpp" % (cppFileName)

    f = open(dstPath, 'w')
    f.write(dst)
    f.close()

def Generate():
    parser = argparse.ArgumentParser()
    
    parser.add_argument("-r", "--resouces", help="resouces dir", type=str, required=True)
    parser.add_argument("-i", "--input", help="file input list", nargs='+', default=[], required=True)
    parser.add_argument("-o", "--output", help="output path", type=str, required=True)
    parser.add_argument("-c", "--cppname", help="cppname", type=str, required=True)
    parser.add_argument("-p", "--prefix", help="prefix", type=str, required=False)

    args = parser.parse_args()
    
    #
    CreateHeaderFile(args.cppname, args.output)

    #
    CreateSourceFile(args.resouces, args.input, args.prefix, args.cppname, args.output)

Generate()