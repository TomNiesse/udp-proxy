# UDPProxy

This is an HTTPS proxy with configurable network paths. Instead of relying on one bidirectional physical network connection, this proxy can split the request and response traffic. This way, all network traffic that goes through the proxy can be sent over multiple unidirectional networks.

# How to build

## Install dependencies

```sh
sudo apt install gcc g++ qt6-base-dev cmake
```

## Compile the source

```sh
mkdir build/
cd build/
cmake ../src/
make
```
