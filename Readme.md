# ft_traceroute

## 📦 Overview

Traceroute (aka tracert on Windows) is a utility that tracks the path IP packets take from a source machine to a destination across an IP network. This project consists of recoding the standard traceroute command in C.

## ✅ Features



# Bonus Opportunities

DNS management (e.g., reverse lookup per hop)

Additional flags: -i, -m, -p, -s, -q, -N, -t, -l


## ⚠️ Permissions

`ft_traceroute` requires root privileges or appropriate capabilities to open a raw socket.

### Option 1: Run with `sudo`
sudo ./ft_traceroute 8.8.8.8

### Option 2 :Grant raw socket capability
sudo setcap cap_net_raw+ep ./ft_traceroute

### Option 2 :Grant raw socket capability at build time (recommended)
./ft_traceroute 

## 🛠 Build: 
make           # builds ./bin/ft_traceroute

make setcap    # builds and grants raw-socket capability to ./bin/ft_traceroute

make bonus     # builds and grants raw-socket capability to ./bin/ft_traceroute_bonus with extra flags

The executable will be located at /bin .

# Run
./ft_traceroute [--help] <hostname|IPv4>

For detailed help:

./ft_traceroute --help

