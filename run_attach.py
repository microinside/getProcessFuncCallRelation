import subprocess
def do():
	cmd = "ps -e | grep test"
	data=subprocess.Popen(cmd,stdout=subprocess.PIPE,shell=True)

	process_info = data.stdout.read()
	print ">>", process_info
	pid = process_info.split(' ')[1]
	print ">> pid:", pid
	
	my_so = "/home/e/getProcessFuncCallRelation/Obj/Main.so"
	pin = "/home/e/pin/pin"
	flag1 = " -pid "
	flag2 = " -t "

	cmd = pin + flag1 + pid + flag2 + my_so
	subprocess.call(cmd, shell=True)
#sudo /home/e/pin/pin -pid    -t /home/e/Desktop/getProcessFuncCallRelation/Obj/getProcessFuncCallRelation.so
if __name__ == '__main__':
	do()
