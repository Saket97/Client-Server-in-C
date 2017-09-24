# Client-Server-in-C
This repository contains various modules implemented related to computer networks. All the modules are covered in a first level undergraduate course on Computer Networks.
At present, it contains 2 modules:
1. ClientServer: It has 2 folders client and server. Client folder conotains the module client.c which sets up a socket, and then requests the server for a file. Server folder has a file server.c which listens to various connection requests, **authenticate** the client, and then serve the requested file.

2. WireSharkPacketAnalysis: It contains the various packets captured while establishing connection to a website. This repo contains the analysis of packets at TCP layer level and Network Layer level. We establich connection using Ethernet, and NAT.
