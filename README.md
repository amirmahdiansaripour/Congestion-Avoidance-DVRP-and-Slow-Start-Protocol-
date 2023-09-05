# Congestion Avoidance (DVRP and Slow-Start-Protocol)

This project consists of two main parts:

1. Designing a simple TCP connection between two nodes with a router between them. The router uses slow-start-protocol to avoid congestion of packets.

2. A larger topology containing multiple nodes and routers. The DVRP algorithm is implemented in the network to ensure that each packet reaches its destination in the most efficint (shortest) path.

## How to run (First Part)

First, create the object file of sender, router, and receiver through $\textbf{Makefile}$ in their respective folders. Then, run the object files on different terminals:

./sender.out 127.0.0.1 8001 8002

./router.out 172.16.0.0 8002 8001 8003 8004

./receiver.out 172.16.0.1 8004 8003


check reports.txt in sender folder to view the output.


## How to run (Second Part)

Create the object files like the first part. Then, run the following codes on separate terminals:

./sender.out 172.16.0.1 172.16.29.1

./sender.out 172.16.19.172 172.16.0.10

./sender.out 172.16.1.50 172.16.2.1

./router.out 172.16.0.0

./router.out 172.16.1.0

./router.out 172.16.2.0

./router.out 172.16.10.0

./router.out 172.16.29.0

./receiver.out 172.16.29.1

./receiver.out 172.16.2.1

./receiver.out 172.16.0.10
