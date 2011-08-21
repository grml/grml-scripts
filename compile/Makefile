PROGS = pong vmware-detect align dpkg_not_running reread_partition_table grml-runtty

#ifndef CFLAGS
CFLAGS = -O2 -Wall -s
#endif

#ifndef LDFALGS
LDFLAGS = -s
#endif

#ifndef CC
CC = gcc
#endif

all: $(PROGS)

pong: pong.c

vmware-detect: vmware-detect.c
	diet $(CC) $(CFLAGS) -o $@ $^

dpkg_not_running: dpkg_not_running.c
	diet $(CC) $(CFLAGS) -o $@ $^

align: align.c
	$(CC) -Wall -o align align.c

reread_partition_table: reread_partition_table.c
	diet $(CC) $(CFLAGS) -o $@ $^

grml-runtty: grml-runtty.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(PROGS)
