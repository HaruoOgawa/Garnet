pyinstaller --onefile GarnetLauncher.py
rmdir build /s /q
del GarnetLauncher.spec

move dist\GarnetLauncher.exe .\
rmdir .\dist

pause