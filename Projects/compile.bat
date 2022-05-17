@echo off
set include_folder="C:\Users\josel\Desktop\OpenGL\Include"
set libraries_folder="C:\Users\josel\Desktop\OpenGL\Libraries"

@echo on
g++ *.cpp glad.c -o out -I %include_folder% -L %libraries_folder% -lglfw3 -lopengl32 -mwindows
pause