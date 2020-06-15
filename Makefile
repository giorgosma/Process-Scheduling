all: 	myport  \
	monitor \
	port_master \
	vessel \
	make_vessels

myport: myport.c
	gcc -g myport.c structures.c -o myport -lpthread

monitor: monitor.c
	gcc -g monitor.c structures.c -o monitor -lpthread

port_master: port_master.c
	gcc  -g port_master.c structures.c -o port_master -lpthread

vessel: vessel.c
	gcc -g vessel.c structures.c -o  vessel -lpthread

make_vessels: make_vessels.c
	gcc -g make_vessels.c structures.c -o  make_vessels -lpthread

clean:
	rm -f myport monitor  \
		port_master vessel \
		structures make_vessels \
		public_ledger.txt \
		shared_memory_id.csv

