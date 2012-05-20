'''
Created on 2012-3-21

@author: zhi
'''
import socket
import os
import time


def send2server( filename, serverIP, serverPort ):
    client = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
    client.connect( ( serverIP, serverPort ) )
    
    fp = open( filename )
    buf = fp.read()
    client.send( buf )
    fp.close()
    os.remove( filename )

    client.send( "end-socket-connect" )
    client.close()

    print buf
    if buf.endswith( "/*The end*/\n" ):
        print "the end"
        return True
    return False

def logClient():
    destDir = 'Log/'
    exceptFile = 'axiba.log'
    seconds = 5

    serverIP = 'localhost'
    serverPort = 8001

    while True:
        files = os.listdir( destDir )
        files.remove( exceptFile )
        
        filesTmp = []
        for i in files:
            tmp = i.split(".")[-1]
            filesTmp.append(int(tmp))
        filesTmp.sort()
        
        files = []
        for i in filesTmp:
            tmp = destDir + exceptFile + "." + str(i)
            files.append(tmp)
                            
        bRet = False
        for i in files:
            bRet = send2server( i, serverIP, serverPort )
        if bRet:
            break

        time.sleep( seconds )

if __name__ == "__main__":
    #getFileName("E:\\git\\test", "hey.log.")
    logClient()
