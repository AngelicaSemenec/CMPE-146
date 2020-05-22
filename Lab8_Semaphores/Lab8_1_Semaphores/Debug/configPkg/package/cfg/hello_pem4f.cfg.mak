# invoke SourceDir generated makefile for hello.pem4f
hello.pem4f: .libraries,hello.pem4f
.libraries,hello.pem4f: package/cfg/hello_pem4f.xdl
	$(MAKE) -f C:\Users\semen\workspace_v9\Lab8_1_Semaphores/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\semen\workspace_v9\Lab8_1_Semaphores/src/makefile.libs clean

