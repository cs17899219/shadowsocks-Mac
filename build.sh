#!/bin/bash

git submodule init
git submodule update

# Make sure you had installed Homebrew. see: http://brew.sh/
# Run "brew install openssl" if you got error related OpenSSL
cd shadowsocks-libev
./configure --with-openssl=/usr/local/opt/openssl/
cd ..

# Make sure you had installed cocoapods. see: https://cocoapods.org/
pod install

