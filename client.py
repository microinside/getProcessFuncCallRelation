'''
Created on 2012-3-21

@author: zhi
'''
import socket
import os
import time
import re

def send2server( filename, serverIP, serverPort, destDir ):
    client = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
    client.connect( ( serverIP, serverPort ) )

    indexFromServer = client.recv(1024)
    global indexInServer
    if "" == indexInServer:
        indexInServer = indexFromServer
        fp = open( destDir + "flg_" + indexInServer, "w" )
        fp.close()
    client.send( indexInServer )

    fp = open( filename )
    buf = fp.read()
    client.send( buf )
    fp.close()
    os.remove( filename )

    client.send( "end-socket-connect" )
    client.close()

    if buf.endswith( "/*The end*/" ):
        print "the end"
        return True
    return False

def logClient():
    destDir = 'Log/'
    exceptFile = 'axiba.log'
    seconds = 5

    serverIP = 'localhost'
    serverPort = 8001

    global indexInServer
    indexInServer = ""

    while True:
        files = os.listdir( destDir )
        patternLog = re.compile(r'axiba.log.\d+')
        filesTmp = []
        for i in files:
            if patternLog.match(i):
                filesTmp.append(i)
        files = filesTmp

        filesTmp = []
        for i in files:
            tmp = i.split(".")[-1]
            filesTmp.append(int(tmp))
        filesTmp.sort()
        
        files = []
        for i in filesTmp:
            tmp = destDir + exceptFile + "." + str(i)
            files.append(tmp)

        print "get to send files:", files
                            
        bRet = False
        for i in files:
            bRet = send2server( i, serverIP, serverPort, destDir )
        if bRet:
            print "client to be teminaled"
            break

        print "client go to sleep"
        time.sleep( seconds )

if __name__ == "__main__":
    #getFileName("E:\\git\\test", "hey.log.")
    logClient()
