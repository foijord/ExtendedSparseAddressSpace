sudo apt-get update && sudo apt-get upgrade -y

sudo apt-get install -y build-essential cmake libvulkan-dev gcc-12 g++-12
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 60 --slave /usr/bin/g++ g++ /usr/bin/g++-12

wget https://developer.nvidia.com/downloads/vulkan-beta-5354311-linux
chmod +x vulkan-beta-5354311-linux
sudo ./vulkan-beta-5354311-linux

git clone https://github.com/KhronosGroup/Vulkan-Headers.git
cmake -S Vulkan-Headers -B build/
cmake --install build --prefix install

git clone https://github.com/foijord/ExtendedSparseAddressSpace.git
