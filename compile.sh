#!/bin/bash
gcc-4.9 client.c -o client -Wall
echo "Client compilation completed!"
gcc-4.9 server.c -o server -lpthread -Wall
echo "Server compilation completed!"
echo "Compilation completed!"
