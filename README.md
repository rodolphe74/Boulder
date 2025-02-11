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
                                                                                                                                 
<img src="Videos/boulder.gif" alt="niveau" width=400 />

```

# Compilation
## Visual Studio 2022
in a developper powershell :
```shell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\vcpkg.exe install glfw3
.\vcpkg.exe install raylib
cd ..
git clone https://github.com/rodolphe74/Boulder.git
cd Boulder
cmake . -DCMAKE_TOOLCHAIN_FILE=[vcpkg_folder]\scripts\buildsystems\vcpkg.cmake
.\Boulder.sln
```
Then in Visual Studio, set Boulder as active project and run it.

## XCode
in a terminal :
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
Then in XCode set the Boulder project and run it.
