from operator import index
import socket
import random
import string

bufferSize = 1024
msg = ""
for number in range(10):
    TCPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    TCPClientSocket.connect(('127.0.0.1', 8800))

    if number % 2 == 1:
        cryptoMsgFromClient        = "crypto:" + ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(32))
        cryptoBytesToSend          = str.encode(cryptoMsgFromClient)
        TCPClientSocket.send(cryptoBytesToSend)
    else:
        generalMsgFromClient       = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(32))
        generalBytesToSend         = str.encode(generalMsgFromClient)
        TCPClientSocket.send(generalBytesToSend)

    msgFromServer = TCPClientSocket.recv(bufferSize)
    msg = "Message from Server: {}".format(msgFromServer)

    print(msg)
    TCPClientSocket.close()