FROM gitpod/workspace-full

RUN sudo apt-get update \
 && sudo apt-get install -y \
    freeglut3-dev xorg-dev \
 && sudo rm -rf /var/lib/apt/lists/*
