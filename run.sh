#!/bin/bash

# Convenience script that runs the image and mounts the current directory into the container.
# The directory you would want to mount is probably the directory where your code resides.
#
# $@ - command + args to run (default: /bin/bash)

instance_name="rpi-pico-dev"
image="ubuntu-rpi-pico-env"
dir="/code"
mnt="/mnt/pico"
cmd=${@:-/bin/bash}

docker run --name $instance_name -it --rm --net=host \
    -w $dir \
    -v $PWD:$dir:delegated \
    --mount type=bind,source=$mnt,target=$mnt \
    $image \
    $cmd
