# ChatMe
Simple client - serwer chat working in LAN (Qt, C++)

## Overview

ChatMe is application that using TCP/IP protocol to provide communication beetween users. Application contains server for handling connection beetwen sockets. Client app provides accessability to server which has map of all connected users and in for-loop iterates on that map and resend message from client.

At first we need to start our server. Then clients can connect to the server by passing IP of server and choosing name which will be displayed to other users.


![StartServer](https://github.com/matewol540/ChatMe/blob/master/gifs/Start.gif?raw=true)

Main gui provides text inputs to put message, text area which shows messages from other users and on the left list of current connected users. To send user need to click button. 
![StartServer](https://github.com/matewol540/ChatMe/blob/master/gifs/TestMSG.gif?raw=true)

Server provides methods to ban or kick user from further communication. When user is on black list he can't connect to server.

![StartServer](https://github.com/matewol540/ChatMe/blob/master/gifs/bankick.gif?raw=true)

To unban user user need to choose him from list and then click proper button on server.
![StartServer](https://github.com/matewol540/ChatMe/blob/master/gifs/unban.gif?raw=true)

After stopping server all users will be disconnected from server.
![StartServer](https://github.com/matewol540/ChatMe/blob/master/gifs/serverStop.gif?raw=true)

## Authors
[Konrad Wasiak](https://github.com/KonradWasiak)
[Mateusz Wolski](https://github.com/matewol540)
