FROM mcr.microsoft.com/devcontainers/cpp

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends libomp-dev
RUN apt-get -y install --no-install-recommends r-base
RUN sudo Rscript -e "install.packages('dplyr')"
RUN sudo Rscript -e "install.packages('ggplot2')"

