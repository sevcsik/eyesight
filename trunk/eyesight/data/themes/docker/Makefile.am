EDJE_FLAGS = -v -id img/ -fd fonts/

filesdir = $(prefix)/share/eyesight/themes/docker
files_DATA = docker.edj

docker.edj: Makefile
	edje_cc $(EDJE_FLAGS) main.edc -o docker.edj
	
clean:
	rm -f *.edj


