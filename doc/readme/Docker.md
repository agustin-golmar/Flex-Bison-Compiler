# Docker

For unsupported platforms (or just for convenience), follow the instructions to build an image that can run the entire application (and scripts).

## Requirements

* [Docker v24.0.6](https://docs.docker.com/get-docker/)

## Microsoft Windows

You need to build a proper image (only when cloning the repository for the first time):

```powershell
script\windows\docker-build.bat
```

Then, run an ephemeral container with:

```powershell
script\windows\docker-run.bat
```

After that, you can develop outside the container and execute any commands available for [Ubuntu](../../doc/readme/Ubuntu.md) inside.

## Ubuntu

You need to build a proper image (only when cloning the repository for the first time):

```bash
chmod u+x --recursive script/ubuntu
script/ubuntu/docker-build.sh
```

Then, run an ephemeral container with:

```bash
script/ubuntu/docker-run.sh
```

After that, you can develop outside the container and execute any commands available for [Ubuntu](../../doc/readme/Ubuntu.md) inside.
