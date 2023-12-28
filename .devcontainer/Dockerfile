FROM mcr.microsoft.com/devcontainers/cpp

# Install dependencies
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends libomp-dev \
    && apt-get -y install --no-install-recommends r-base \
    && apt-get -y install --no-install-recommends python3-full python3-pip python3-venv

# Install R packages
RUN Rscript -e "install.packages('dplyr')"
RUN Rscript -e "install.packages('ggplot2')"

# Setup Python virtual environment
#RUN python3 -m venv /opt/venv
#ENV PATH="/opt/venv/bin:$PATH"

# Install Python packages in the virtual environment
#COPY ext_libs/run /ext_libs/run
#RUN pip install /ext_libs/run
#RUN pip install ./ext_libs/run
