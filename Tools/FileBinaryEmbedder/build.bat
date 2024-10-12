pyinstaller --onefile FileBinaryEmbedder.py
rmdir build /s /q
del FileBinaryEmbedder.spec

move dist\FileBinaryEmbedder.exe .\
rmdir .\dist

pause