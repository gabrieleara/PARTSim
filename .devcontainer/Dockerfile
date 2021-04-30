# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.158.0/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# PARTSim requires boost-program-options library to build
RUN apt update && export DEBIAN_FRONTEND=noninteractive \
    && apt -y install --no-install-recommends libboost-program-options-dev \
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Other development tools that I frequently use
RUN apt update && export DEBIAN_FRONTEND=noninteractive \
    && apt -y install silversearcher-ag expect \
    && apt -y autoremove \
    && apt -y clean \
    && rm -rf /var/lib/apt/lists/*

# Function that I use to search for stuff in the project
RUN echo '\n\
function search(){\n\
    if [ $# -lt 1 ]; then\n\
        ag\n\
    else\n\
        unbuffer ag --noheading "$@" | less -r\n\
    fi\n\
}' >>/home/vscode/.bashrc

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
