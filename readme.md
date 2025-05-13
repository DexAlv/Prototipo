- # Requerimientos
```bash
    sudo dnf install opencv opencv-devel
```

- # Para compilar
```bash
    g++ -o leervidio leervidio.cpp `pkg-config --cflags --libs opencv4`
```