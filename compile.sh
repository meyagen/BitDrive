#!/bin/bash
gcc client.c -o client
gcc server.c -o server -lpthread

echo "Compilation completed!"
