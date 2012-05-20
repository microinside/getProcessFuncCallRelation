import socket

def logServer():
    serverIP = 'localhost'
    serverPort = 8001
    maxConnection = 5

    server = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
    server.bind( ( serverIP, serverPort ) )
    server.listen( maxConnection )

    while True:
        connection, address = server.accept()

        fp = open( "Log/" + address[0] + ".log", "a+" )
        while True:
            buf = connection.recv( 1024 )
            if "end-socket-connect" == buf:
                print "S:end-socket-connect"
                break
            fp.write( buf )
        fp.close()

        connection.close()
        
if __name__ == '__main__':
    logServer()
