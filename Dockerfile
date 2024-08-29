# @see https://hub.docker.com/_/ubuntu
FROM ubuntu:latest AS runner

# Define the default shell.
SHELL [ "/bin/bash", "-c" ]

# Update platform.
RUN apt-get update --yes
RUN apt-get install sudo --yes

WORKDIR /home/ubuntu

# Copy only the Ubuntu scripts inside image.
COPY script/ubuntu/ script/ubuntu/
RUN chown ubuntu:ubuntu --recursive /home/ubuntu

# Install dependencies.
RUN chmod u+x --recursive script/ubuntu
RUN script/ubuntu/install.sh

# Cleaning image.
RUN rm --force --recursive script
