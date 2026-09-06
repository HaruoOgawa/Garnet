import argparse
import subprocess
import os
from PIL import Image
import json


def Generate():
    parser = argparse.ArgumentParser()

    parser.add_argument("-c", "--cmd", help="msdf cmd path", type=str, required=True)
    parser.add_argument(
        "-i",
        "--input",
        help="embed charactors to texture. Either a literal string, or a path to a UTF-8 text file containing the characters (recommended for Japanese).",
        type=str,
        required=True,
    )
    parser.add_argument("-o", "--output", help="output texture path", type=str, required=True)
    parser.add_argument("-f", "--font", help="font", type=str, required=True)
    parser.add_argument("-dw", "--dimension_width", help="", type=int)
    parser.add_argument("-dh", "--dimension_height", help="", type=int)
    parser.add_argument("-m", "--mode", help="msdfgen mode", type=str)

    args = parser.parse_args()

    msdf_path = args.cmd

    # -i にファイルパスが渡された場合は、その中身を文字ソースとして読み込む。
    # 日本語(ひらがな/カタカナ/漢字)を大量に指定したい場合、コマンドライン
    # 引数として直接打つのは非現実的かつ文字化けの原因にもなるため、
    # UTF-8テキストファイル経由での指定を推奨する。
    if os.path.isfile(args.input):
        with open(args.input, "r", encoding="utf-8-sig") as f:
            input_text = f.read()
    else:
        input_text = args.input

    # 改行文字はグリフとして意味を持たないため除去する
    input_text = input_text.replace("\r", "").replace("\n", "")

    sdf_chars = sorted(set(input_text), key=input_text.index)

    output = args.output
    font = args.font if args.font is not None else r"C:\Windows\Fonts\arialbd.ttf"
    dimension_width = args.dimension_width if args.dimension_width is not None else 32
    dimension_height = args.dimension_height if args.dimension_height is not None else 32
    mode = args.mode if args.mode is not None else "sdf"

    distDir = os.path.dirname(output)
    temp = os.path.join(distDir, "temp")

    os.makedirs(distDir, exist_ok=True)
    os.makedirs(temp, exist_ok=True)

    images = []

    # MSDFGenを実行しテクスチャを作成
    index = 0
    for char in sdf_chars:
        temp_tex_dir = os.path.join(temp, str(index) + "_.png")

        # 文字そのものではなく Unicode コードポイント(10進数)で指定する。
        # msdfgen は 'A' / 65 / 0x41 のいずれの形式でも文字を受け付けるが、
        # 文字リテラルをコマンドライン経由(特にshell=True)で渡すと、
        # コンソールの文字コード変換によって日本語などのマルチバイト文字が
        # 文字化けし、msdfgen側でフォント内のグリフを見つけられず失敗する。
        # コードポイント(数値)であればこの問題が起きない。
        char_code = str(ord(char))

        command = [
            msdf_path,
            mode,
            "-font",
            font,
            char_code,
            "-o",
            temp_tex_dir,
            "-dimensions",
            str(dimension_width),
            str(dimension_height),
            "-autoframe",
        ]

        # shell=True は使わない。シェル経由だと余計な文字コード変換や
        # クォート処理が挟まり、非ASCII文字の扱いが不安定になるため、
        # 直接プロセスを起動する(shell=False がデフォルト)。
        result = subprocess.run(command, shell=False)
        if result.returncode != 0:
            print(f"[Error] Failed to run MSDF cmd for char '{char}' (code={char_code}).")
            return

        print("[TempOutput] ", temp_tex_dir)

        images.append(Image.open(temp_tex_dir))

        index += 1

    # 複数枚のMSDFテクスチャを1つにまとめる
    widths, heights = zip(*(img.size for img in images))
    total_width = sum(widths)
    max_width = max(widths)
    max_height = max(heights)
    numOfChar = len(sdf_chars)

    color_format = "RGBA"
    if mode == "sdf":
        # sdfの時は8ビットテクスチャを指定
        color_format = "L"

    # 横一列に結合する
    combined_image = Image.new(color_format, (total_width, max_height))

    x_offset = 0
    for img in images:
        combined_image.paste(img, (x_offset, 0))
        x_offset += img.width

    # 結合された画像を保存
    combined_image.save(output)

    # 文字リスト(日本語含む)
    sdf_chars_with_order = []
    index = 0
    for char in sdf_chars:
        sdf_chars_with_order.append((char, index))
        index += 1

    # 複合テクスチャに関する情報をJSONで書き出しておく
    # ensure_ascii=False にして日本語をそのまま(\uXXXXエスケープせず)出力する
    json_text = json.dumps(
        {
            "total_width": total_width,
            "charWidth": max_width,
            "numOfChar": numOfChar,
            "sdf_chars": sdf_chars_with_order,
        },
        indent=4,
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=False,
    )

    print("json_text: ", json_text)

    output_json = output.replace(".png", ".json")

    print("output_json:", output_json)

    # 日本語を含むため明示的にUTF-8で書き出す
    with open(output_json, "w", encoding="utf-8") as f:
        f.write(json_text)


#
Generate()
