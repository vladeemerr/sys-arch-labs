#!/bin/sh
exec docker run -it --rm -p 8080:8080 -v ${1:-.}:/usr/local/structurizr structurizr/lite
