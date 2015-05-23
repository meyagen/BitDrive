#!/bin/bash
gcc client.c -o client
echo "Client compilation completed!"
gcc server.c -o server -lpthread
echo "Server compilation completed!"
echo "Compilation completed!"
