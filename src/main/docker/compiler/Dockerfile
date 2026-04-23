# @see https://hub.docker.com/_/ubuntu
FROM ubuntu:24.04 AS runner

# Installs dependencies.
RUN apt-get update --yes \
	&& apt-get install --yes \
		bison \
		cmake \
		flex \
		gcc \
		make \
	&& rm --force --recursive /var/lib/apt/lists/*
