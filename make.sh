##first create the library 
x86_64-w64-mingw32-gcc -c renderer.c -o renderer.o
ar rcs librenderer.a renderer.o

x86_64-w64-mingw32-gcc client.c -L. -lrenderer -o client.exe -g -lgdi32 

cp renderer.exe /mnt/c/msys64/home/emmit/renderer.exe
cp renderer.c /mnt/c/msys64/home/emmit/renderer.c
cp client.exe /mnt/c/msys64/home/emmit/client.exe
cp client.c /mnt/c/msys64/home/emmit/client.c
