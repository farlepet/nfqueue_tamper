NFQueue Tamper
==============

A tool to help in testing client/server robustness in the presence of malformed
data.

Supported protocols:
 - IPv4
   - UDP
   - TCP

Building
--------

Requirements:
 - `g++`/`clang++`
 - `make`
 - boost: https://www.boost.org/
 - libnetfilter_queue: https://www.netfilter.org/projects/libnetfilter_queue/

Build with `make`

Usage
-----

    nfqueue_tamer -q <queue> -t "method1;opt1;opt2" -t "method2;opt1"

Queue number defaults to 0. Must be run as root.

To create a queue:

    iptables -A <chain> [filter criteria] -j NFQUEUE --queue-num <queue>

For example, to trap all outbound UDP traffic coming from port 63:

    iptables -A OUTPUT -p udp --sport 63 -J NFQUEUE --queue-num 0

Available methods and associated options:
 - `rand` - Randomly tamper with data
   - `off` - Offset, or offset range, at which to apply randomization
     - Defaults to 0:-1 (0 - end)
   - `con` - If offset is a range, whether or not modified bytes must be consecutive
     - Defaults to 0 (non-consecutive allowed)
     - NOT CURRENTLY SUPPORTED
   - `sz`  - Number of bytes to modify, can be a range
     - Defaults to 1
 - `fixed` - Overwrite region of data with supplied values
   - `off` - Offset at which to apply data
     - Cannot be a range
   - `data` - Hex data

Global options:
 - `chance` - How likely the tamper method is to be used on a given packet
   - Probability value between 0 and 1
   - Defaults to 1

Example:

    nfqueue_tamper -q 0 -t "rand;chance=.5;off=0:4;sz=1:2" -t "fixed;chance=0.1;off=16;data=0FEA0011"

This will have a 50% chance on every packet of replacing one or two of the first
five bytes in the payload (application-layer data) with a random value, and a 10%
chance of replacing data at offset 16-19 with 0F,EA,00,11.