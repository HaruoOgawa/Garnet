chcp 65001
mkdir ./dist
MSDFMultiGen.exe -c "msdfgen.exe" -i "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" -o "./dist/msdf.png" -f "C:\Windows\Fonts\arialbd.ttf" -dw 32 -dh 32 -m sdf

pause