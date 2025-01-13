Installation
============

Requirements
------------

- **AlmaLinux 9**
- Python >= 3.8
- GCC >= 12.0
- CMake >= 3.18

Setup Steps
-----------

1. Clone the repository:
   ```
   git clone https://github.com/wangz1996/vcalo.git
   cd vcalo
   ```

2. Load the required environment:
   ```
   source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh
   ```

3. Build the project:
   ```
   mkdir build
   cd build
   cmake ..
   make
   ```

4. Run a test simulation:
   ```
   ./vcalo -c config/default.yaml
   ```
