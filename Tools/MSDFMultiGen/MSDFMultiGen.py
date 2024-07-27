import argparse
import subprocess
import os
from PIL import Image

def Generate():
    parser = argparse.ArgumentParser()
    
    parser.add_argument("-c", "--cmd", help="msdf cmd path", type=str, required=True)
    parser.add_argument("-i", "--input", help="embed charactors to texture.", type=str, required=True)
    parser.add_argument("-o", "--output", help="output texture path", type=str, required=True)
    parser.add_argument("-f", "--font", help="font", type=str, required=True)
    parser.add_argument("-dw", "--dimension_width", help="", type=int)
    parser.add_argument("-dh", "--dimension_height", help="", type=int)
    parser.add_argument("-m", "--mode", help="msdfgen mode", type=str)

    args = parser.parse_args()

    msdf_path = args.cmd
    sdf_chars = sorted(set(args.input), key=args.input.index)
    output = args.output
    font = args.font if args.font != None else "C:\Windows\Fonts\arialbd.ttf"
    dimension_width = args.dimension_width if args.dimension_width != None else 32
    dimension_height = args.dimension_height if args.dimension_height != None else 32
    mode = args.mode if args.mode != None else "sdf"

    distDir = os.path.dirname(output)
    temp = os.path.join(distDir, "temp")

    os.makedirs(distDir, exist_ok=True)
    os.makedirs(temp, exist_ok=True)

    images = []

    # MSDFGenを実行しテクスチャを作成
    for char in sdf_chars:
        temp_tex_dir = os.path.join(temp, char + "_.png")
        
        command = [msdf_path, mode, "-font", font, "\'" + str(char) + "\'", "-o", temp_tex_dir, "-dimensions", str(dimension_width), str(dimension_height), "-autoframe"]

        if(not subprocess.run(command, check=True, shell=True)):
            print("[Error] Failed to MSDF cmd.")
            return
        
        print("[TempOutput] ", temp_tex_dir)

        images.append(Image.open(temp_tex_dir))
        
    # 複数枚のMSDFテクスチャを1つにまとめる
    # 合計サイズ
    widths, heights = zip(*(img.size for img in images))
    total_width = sum(widths)
    max_height = max(heights)

    #
    color_format = 'RGBA'
    if(mode == "sdf"):
        # sdfの時は8ビットテクスチャを指定
        color_format = 'L'

    # 横一列に結合する
    combined_image = Image.new(color_format, (total_width, max_height))

    x_offset = 0
    for img in images:
        combined_image.paste(img, (x_offset, 0))
        x_offset += img.width

    # 結合された画像を保存
    combined_image.save(output)

#
Generate()