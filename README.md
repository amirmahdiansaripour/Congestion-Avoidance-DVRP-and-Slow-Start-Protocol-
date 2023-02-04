# Routing

Enter the following codes on different terminals:

./sender.out 172.16.0.1 172.16.29.1
./sender.out 172.16.19.172 172.16.0.10
./sender.out 172.16.1.50 172.16.2.1

./router.out 172.16.0.0
./router.out 172.16.1.0
./router.out 172.16.2.0
./router.out 172.16.10.0
./router.out 172.16.29.0

./receiver.out 172.16.29.1
1/1010/172.16.29.1/172.16.0.1/ACK0

./receiver.out 172.16.2.1
1/1010/172.16.2.1/172.16.1.50/ACK0

./receiver.out 172.16.0.10
1/1010/172.16.0.10/172.16.19.172/ACK0
