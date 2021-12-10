# Shell-Toolkits
 This is a simple c program for the project 1, COP5570.
The myshell.c will realize the simple functions in bash shell.
The myls.c will realize the funtions like the command "ls -l".

You should type "make" to compile the .c files to get the execute files of myshell and myls.
You can type "make clean" to clean up the files.

## myshell
If you type the ./myshell to run the shell program. It has the following functions:
1. Build-in commands:
a. cd <path_name>--change your path to a nwe path, like: cd ..
b. pwd--show your local dictionary path name
c. set-- set the environmental varibales, in my shell, I only support MYPATH, like: set MYPATH=/home:/usr/bin
d. exit--exit my shell
P.S: you can terminate the shell forcely by EOF(control+D)

2.external commands:
you can execute external commands like: cat,env,sleep and so on

3.background command:
you can run the command in the background process like:sleep 10 &

4. redirect command:
input redirect: like cat<myls.c
output redirecr like ls>ls_result
input and output redirect like: cat<test>test2

5.pipe command:
my shell only support 2 or 3 commands in pipe.
for 2 commands: ps -ef|more
for 3 commands: cat myls.c|head -5|tail 2

## myls
You should type ./myls+<path name> to show the detailed files' information under the path name.
like:  ./myls .   or ./myls usr/bin


## Disadvantage/Limitations:
1.pipe only support at most 3 commands in each time.
2. When you read a commands files, it will occur extra $,but it will be fine if you input line by line by your keyboard.
P.S. : in most case, function of pipe is fine, but sometime it will occur error. When it happens, please restart the shell.  



