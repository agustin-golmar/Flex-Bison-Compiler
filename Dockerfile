FROM ubuntu:latest

RUN apt-get update --yes
RUN apt-get install bison --yes
RUN apt-get install cmake --yes
RUN apt-get install flex --yes
RUN apt-get install gcc --yes
RUN apt-get install make --yes
