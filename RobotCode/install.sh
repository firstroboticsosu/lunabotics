#!/bin/sh

# Function to install packages using apt (Debian/Ubuntu)
install_with_apt() {
    sudo apt-get update
    sudo apt-get install -y build-essential gdb git pkg-config meson cmake libopencv-dev
}

# Function to install packages using yum (Red Hat/CentOS)
install_with_yum() {
    sudo yum groupinstall -y "Development Tools"
    sudo yum install -y gdb meson cmake pkgconfig git opencv-dev
}

# New package manager used in Fedora
install_with_dnf() {
    sudo dnf groupinstall -y "Development Tools"
    sudo dnf install -y gdb meson cmake pkgconfig git opencv-dev
}

# Function to install packages using pacman (Arch Linux)
install_with_pacman() {
    sudo pacman -Sy --noconfirm base-devel gdb meson cmake pkgconf git opencv
}

if [ -f /etc/arch-release ]; then
    install_with_pacman
elif [ -f /etc/debian_version ]; then
    install_with_apt
elif [ -f /etc/redhat-release ] || [ -f /etc/centos-release ]; then
    if command -v dnf >/dev/null 2>&1; then
        install_with_dnf
    else
        install_with_yum
    fi
else
    echo "Your linux distro is not supported currently."
    echo "You need to manually install those packages: exiftool, jq, glfw"
fi

# Install apriltag to system
git clone https://github.com/AprilRobotics/apriltag.git
cd apriltag || exit
sudo cmake -B build -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build --target install
cd ..
sudo rm -rf ./apriltag
