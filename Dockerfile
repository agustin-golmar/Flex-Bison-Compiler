# @see https://hub.docker.com/_/ubuntu
FROM ubuntu:latest as Runner

# Define the default shell.
SHELL [ "/bin/bash", "-c" ]

# Create 'ubuntu' password-less administrator user and go home.
RUN apt-get update --yes
RUN apt-get install sudo --yes
RUN groupadd ubuntu
RUN useradd --create-home --home-dir /home/ubuntu --gid sudo --groups ubuntu --shell /bin/bash ubuntu
RUN passwd --delete ubuntu
USER ubuntu:ubuntu
WORKDIR /home/ubuntu
RUN sudo chown ubuntu:ubuntu /home/ubuntu

# Copy only the Ubuntu scripts inside image.
COPY script/ubuntu/ script/ubuntu/
RUN sudo chown ubuntu:ubuntu --recursive /home/ubuntu

# Install dependencies.
RUN chmod u+x --recursive script/ubuntu
RUN script/ubuntu/install.sh

# Cleaning image.
RUN rm --force --recursive script
