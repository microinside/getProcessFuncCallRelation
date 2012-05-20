import subprocess
def do():
	cmd = "ps -e | grep test"
	data=subprocess.Popen(cmd,stdout=subprocess.PIPE,shell=True)

	str = data.stdout.read()
	print ">>", str
	str = str.split(' ')[1]
	print ">>", str

	cmd = "sudo /home/e/pin/pin -pid " + str + " -t /home/e/Desktop/getProcessFuncCallRelation/Obj/getProcessFuncCallRelation.so" 
	subprocess.call(cmd, shell=True)
#sudo /home/e/pin/pin -pid    -t /home/e/Desktop/getProcessFuncCallRelation/Obj/getProcessFuncCallRelation.so
if __name__ == '__main__':
	do()
