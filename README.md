# OpenCL™ Compatible Runtime

-   OpenCL 2.0 compatible language runtime
-   Supports offline and in-process/in-memory compilation

## Getting the source code
Download the git projects using the following commands:

```bash
git clone -b master-next https://github.com/RadeonOpenCompute/ROCm-OpenCL-Runtime.git
```

## Repository branches
The repository maintains several branches. The branches that are of importance are:

-    master-next: This is the default branch.

## Setup OpenCL
Copy the amdocl64.icd file to /etc/OpenCL/vendors

```bash
sudo cp api/opencl/config/amdocl64.icd /etc/OpenCL/vendors/
```

## Building
This is the previous commands to build OpenCL runtime.

```bash
cd $OPENCL_DIR
mkdir -p build; cd build
cmake -DVDI_DIR="$ROCclr_DIR" -DLIBVDI_STATIC_DIR="$ROCclr_DIR/build" ..
make -j$(nproc)
```

Previously, environment variable VDI_DIR was defined in [ROCclr ](https://github.com/ROCm-Developer-Tools/ROCclr). We did not use environment variable ROCclr_DIR. The cmake comand was:

```bash
cmake -DVDI_DIR="$VDI_DIR" -DLIBVDI_STATIC_DIR="$VDI_DIR/build" ..
```

---
OpenCL™ is registered Trademark of Apple
