# ROCm-OpenCL-Runtime
ROCm OpenOpenCL Runtime 

## GETTING REPO

Repo is a git wrapper that manages a collection of git repositories. Install this tool and add it to the command search PATH:

    curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
    chmod a+x ~/bin/repo

## GETTING THE SOURCE CODE

Main OpenCL Components 

* https://github.com/RadeonOpenCompute/ROCm-Device-Libs 
* https://github.com/RadeonOpenCompute/ROCm-OpenCL-Driver 
* https://github.com/RadeonOpenCompute/llvm 
* https://github.com/RadeonOpenCompute/clang
* https://github.com/RadeonOpenCompute/lld 
* https://github.com/KhronosGroup/OpenCL-ICD-Loader

Download the git projects with the following commands:

    ~/bin/repo init -u https://github.com/RadeonOpenCompute/ROCm-OpenCL-Runtime.git -b amd-master -m opencl.xml
    ~/bin/repo sync## BUILDING

Use out-of-source CMake build and create separate directory to run CMake.

The following build steps are performed:

    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
    make
    
## RUNNING clinfo

    LLVM_BIN=./compiler/llvm/bin LD_LIBRARY_PATH=./api/opencl/amdocl clinfo

