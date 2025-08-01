# ft_traceroute

## 📦 Overview

Traceroute (aka tracert on Windows) is a utility that tracks the path IP packets take from a source machine to a destination across an IP network. This project consists of recoding the standard traceroute command in C.

## ✅ Features

- Custom implementation of `traceroute` in C
- Uses raw sockets (ICMP) and UDP sockets
- Reverse DNS resolution per hop (configurable)
- Signal handling (`Ctrl+C`)
- Detailed timing per probe

## 🎁 Bonus Features

- DNS management (optional reverse lookup)
- Custom port selection (`-p`)
- Custom TTL maximum (`-m`)
- Number of queries per hop (`-q`)
- Disable reverse DNS resolution (`-n`)
- Debug output (`-d`)
- Full packet length setting (argument)

## 🔧 Options 

  - -p port         Set the destination port to use as initial UDP port value (incremented by each probe, default is 33434)
  - -m max_ttl      Set the max number of hops (TTL). Default is 30
  - -q nqueries     Set the number of probes per hop. Default is 3
  - -n              Disable reverse DNS resolution
  - -d              Enable debug output
  - -V, --version   Print version info and exit
  - --help          Show help and exit

## 🔧 Arguments:
  
   `host`            The host to traceroute to
  
   `packetlen`       Full packet length (default is header + 40). Can be increased.

## ⚠️ Permissions

`ft_traceroute` requires root privileges or appropriate capabilities to open a raw socket.

### Option 1: Run with `sudo`
```bash
sudo ./ft_traceroute 8.8.8.8
```
### Option 2: Grant raw socket capability (recommended)
```bash
sudo setcap cap_net_raw+ep ./ft_traceroute
```
## 🛠 Build
```bash
make           # builds ./bin/ft_traceroute
make setcap    # builds and grants raw socket capability to ./bin/ft_traceroute
make bonus     # builds and grants capability to ./bin/ft_traceroute_bonus
```
The executable will be located at `./bin`.

## 🚀 Run
```bash
./ft_traceroute [options] <hostname|IPv4> [packetlen]
```
For help:
```bash
./ft_traceroute --help
```
