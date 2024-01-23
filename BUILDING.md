# Building font-render-tester

## Requirements

__Font-Render-Tester__ is written in C++ and the build script is set to expect C++23 level of feature set.  At the moment only [Visual Studio 2022](https://visualstudio.microsoft.com/) is extensively compiled against. If you are using other compilers and have issues with, please feel free to report an issue or submit a pull request.

In order to compile with __Visual Studio 2022__, it must be setup with C++ features. Make sure to install __Desktop development with C++__ workload.

This project requires [CMake](https://cmake.org/) and [VCPKG Library Manager](https://github.com/microsoft/vcpkg). Please follow the installation instruction of each tools.

Also, the environment `VCPKG_ROOT` is needed and it should point to the root directory of __VCPKG__ (eg. `c:\vcpkg`)

## Building with IDE

### Visual Studio Code
Microsoft's [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) Extension is recommended.

You should be able to open the project in Visual Studio Code and it will automatically setup anything needed, given that you have the C++ extension installed. 

Upon the configure step at the first, the IDE will ask for the configure preset to use. Please select the `default` preset as it will setup build to use __VCPKG__. 

### Visual Studio 2022

In __Visual Studio 2022__, use `Open a local folder` or `File->Open->Folder...` in order to open the project.

Upon opening the project, __Visual Studio__ will configure the project. This includes downloading any dependencies the project needs. 

After the configure step is done, you should be able to run or debug the project right away.

## Building with command line

1. Change directory into the source directory.
2. Run the following command:
   ```sh
   $ cmake --preset=default
   ```
   This will create a build directory call `build` and configure the project inside that directory. 
   
   The configuring process includes downloading dependencies from the __VCPKG__ repository and building those dependencies. This will take a while.
3. To build the project, run the following command:
    ```sh
    $ cmake --build ./build --config=Release
    ```

The executable files, along with required dll files and font files will be created at `<source_dir>/build/Release`.
    