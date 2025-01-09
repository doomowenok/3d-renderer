# Instructions

This project utilizes CMake as its build system. To generate the necessary build files, please run the following command in the project's root directory:

```
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
```

The project also depends on **SDL2**, which needs to be available in your environment for successful compilation. Ensure **SDL2** is correctly installed and configured before proceeding.
For detailed instructions on installing **SDL2**, refer to [SDL2 Installation Guide](https://wiki.libsdl.org/SDL2/Installation).