# ft_ping

A reimplementation of the `ping` command in C, based on the behavior of `inetutils 2.0`, with both a mandatory and a bonus mode that adds extra flags.

## 📦 Description

`ft_ping` is a network utility that sends ICMP Echo Request packets to a given host and measures the round-trip time (RTT) for each reply. This project replicates the basic functionality of the Unix `ping` command using raw sockets and manual packet construction.

Developed in C as part of the 42 school curriculum.

## ✅ Features

- Manual construction of ICMP Echo Request packets
- IPv4 support
- Raw socket usage (`SOCK_RAW`)
- Verbose mode with `-v` or `--verbose`
- Help output with `-?` or `--help`
- Statistics report on exit
- RTT calculation per packet
- Signal handling (`SIGINT`)
- Bonus part with additional features

## ⚠️ Permissions

`ft_ping` requires root privileges or appropriate capabilities to open a raw socket.

### Option 1: Run with `sudo`
sudo ./ft_ping 8.8.8.8

### Option 2 :Grant raw socket capability (recommended)
sudo setcap cap_net_raw+ep ./ft_ping

## 🛠 Build: 
make           # builds ./bin/ft_ping

make setcap    # builds and grants raw-socket capability to ./bin/ft_ping

make bonus     # builds and grants raw-socket capability to ./bin/ft_ping_bonus with extra flags

The executable will be located at /bin .

## 🧪 Usage:
./ft_ping [options] <destination> 
./bin/ft_ping_bonus [options] <destination>

Supported Flags

  **. --usage**              *display usage and exit*

  **. -?, --help**           *display this help and exit*

  **. -v, --verbose**        *verbose output (print errors and control messages)*

  **. -c <count>**           *stop after <count> replies*

  **. -i <interval>**        *wait <interval> seconds between packets (float)*

  **. -p <pattern>**         *pad packet with up to 16 bytes specified as a hex string*

  **. -s <packetsize>**     *number of data bytes to send (default 56)*

  **. -t <ttl>**            *set the IP time-to-live (1–255)*

  **. -r, --route**          *bypass the routing table (send only to directly connected hosts)*

  **. -d**                  *debug option for flag activation information and packet pattern info*


## *Example*

*Basic ping*

./bin/ft_ping google.com

*Verbose, limit to 3 pings, 0.5 s apart*

./bin/ft_ping_bonus -v -c3 -i0.5 google.com

*Send a custom 4-byte pad pattern “4dab”*

./bin/ft_ping_bonus -p4dab -c3 google.com

*Send zero-byte payload (8 bytes ICMP header only)*

./bin/ft_ping_bonus -s0 google.com

*Bypass routing table (only on local LAN)*

./bin/ft_ping_bonus -r 192.168.0.1



