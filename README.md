# pass-fd-demo

When I read the quick_start document of [pingora](https://github.com/cloudflare/pingora),
I noticed it provides a graceful way to upgrade the service: [Gracefully upgrade the service](https://github.com/cloudflare/pingora/blob/main/docs/quick_start.md#gracefully-upgrade-the-service)

> The old running server will wait and hand over its listening sockets to the new server. Then the old server runs until all its ongoing requests finish

It passes file descriptors over a UNIX domain socket.

https://stackoverflow.com/questions/2358684/can-i-share-a-file-descriptor-to-another-process-on-linux-or-are-they-local-to-t
https://stackoverflow.com/questions/62139881/how-does-passing-file-descriptors-between-processes-work
http://stackoverflow.com/questions/8481138/how-to-use-sendmsg-to-send-a-file-descriptor-via-sockets-between-2-processes
http://stackoverflow.com/questions/1997622/can-i-open-a-socket-and-pass-it-to-another-process-in-linux
http://www.normalesup.org/~george/comp/libancillary/
http://man7.org/tlpi/code/online/dist/sockets/scm_rights_send.c.html


What I'm doing here is not that tricky, I just fork and execve a new process.


```sh
make
./a.out
```

```sh
nc 127.0.0.1 8666
nc 127.0.0.1 8666
```
