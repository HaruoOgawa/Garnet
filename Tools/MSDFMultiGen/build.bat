pyinstaller --onefile MSDFMultiGen.py
rmdir build /s /q
del MSDFMultiGen.spec

move dist\MSDFMultiGen.exe .\
rmdir .\dist

pause