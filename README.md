# BoulderDash

```

 ▄▄▄▄▄▄▄▄▄▄▄  ▄▄        ▄       ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄               ▄  ▄▄▄▄▄▄▄▄▄▄▄  ▄         ▄  ▄       ▄  ▄ 
▐░░░░░░░░░░░▌▐░░▌      ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌▐░▌             ▐░▌▐░░░░░░░░░░░▌▐░▌       ▐░▌▐░▌     ▐░▌▐░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌░▌     ▐░▌      ▀▀▀▀█░█▀▀▀▀ ▐░█▀▀▀▀▀▀▀█░▌▐░█▀▀▀▀▀▀▀█░▌ ▐░▌           ▐░▌ ▐░█▀▀▀▀▀▀▀█░▌▐░▌       ▐░▌ ▐░▌   ▐░▌ ▐░▌
▐░▌          ▐░▌▐░▌    ▐░▌          ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌  ▐░▌         ▐░▌  ▐░▌       ▐░▌▐░▌       ▐░▌  ▐░▌ ▐░▌  ▐░▌
▐░█▄▄▄▄▄▄▄▄▄ ▐░▌ ▐░▌   ▐░▌          ▐░▌     ▐░█▄▄▄▄▄▄▄█░▌▐░█▄▄▄▄▄▄▄█░▌   ▐░▌       ▐░▌   ▐░█▄▄▄▄▄▄▄█░▌▐░▌       ▐░▌   ▐░▐░▌   ▐░▌
▐░░░░░░░░░░░▌▐░▌  ▐░▌  ▐░▌          ▐░▌     ▐░░░░░░░░░░░▌▐░░░░░░░░░░░▌    ▐░▌     ▐░▌    ▐░░░░░░░░░░░▌▐░▌       ▐░▌    ▐░▌    ▐░▌
▐░█▀▀▀▀▀▀▀▀▀ ▐░▌   ▐░▌ ▐░▌          ▐░▌     ▐░█▀▀▀▀█░█▀▀ ▐░█▀▀▀▀▀▀▀█░▌     ▐░▌   ▐░▌     ▐░█▀▀▀▀▀▀▀█░▌▐░▌       ▐░▌   ▐░▌░▌   ▐░▌
▐░▌          ▐░▌    ▐░▌▐░▌          ▐░▌     ▐░▌     ▐░▌  ▐░▌       ▐░▌      ▐░▌ ▐░▌      ▐░▌       ▐░▌▐░▌       ▐░▌  ▐░▌ ▐░▌   ▀ 
▐░█▄▄▄▄▄▄▄▄▄ ▐░▌     ▐░▐░▌          ▐░▌     ▐░▌      ▐░▌ ▐░▌       ▐░▌       ▐░▐░▌       ▐░▌       ▐░▌▐░█▄▄▄▄▄▄▄█░▌ ▐░▌   ▐░▌  ▄ 
▐░░░░░░░░░░░▌▐░▌      ▐░░▌          ▐░▌     ▐░▌       ▐░▌▐░▌       ▐░▌        ▐░▌        ▐░▌       ▐░▌▐░░░░░░░░░░░▌▐░▌     ▐░▌▐░▌
 ▀▀▀▀▀▀▀▀▀▀▀  ▀        ▀▀            ▀       ▀         ▀  ▀         ▀          ▀          ▀         ▀  ▀▀▀▀▀▀▀▀▀▀▀  ▀       ▀  ▀ 
                                                                                                                                 


```

# Compilation
## Visual Studio 2022
```shell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\vcpkg.exe install glfw3
.\vcpkg.exe install ray lib
cd ..
git clone https://github.com/rodolphe74/Boulder.git
cd Boulder
cmake . -DCMAKE_TOOLCHAIN_FILE=[vcpkg_folder]\scripts\buildsystems\vcpkg.cmake
.\Boulder.sln
```
Then in Visual Studio, set Boulder as active project.

## XCode
```shell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./vcpkg install glfw3
./vcpkg install raylib
cd ..
git clone https://github.com/rodolphe74/Boulder.git
cd Boulder
mkdir debug
cd debug
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg_folder]/scripts/buildsystems/vcpkg.cmake -GXcode
open Boulder.xcodeproj
```
