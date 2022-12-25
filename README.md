# tinit - A tiny but valid init for microvm


## What is tinit?
tinit, basic and tiny init for microvm, it's a valid init, but it's not a full init, it's just a simple init, it's not a replacement for systemd, it's just a simple init for microvm.

- It mounts required filesystems.
- It configures the network.
- It configures the hostname.
- It runs the user specified command.

## Compile from source

- Clone the repository:
```
git clone https://github.com/mofm/tinit.git
```

- Build the binary:
```
$ ./autogen.sh (only first time)
$ ./configure
$ make
```
