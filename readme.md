- # Requerimientos
```bash
    sudo dnf install opencv opencv-devel
```

- # Para compilar
```bash
    g++ -o leervideo leervideo.cpp `pkg-config --cflags --libs opencv4`
```