include config.mk

OBJECTS = deco.o arch.o exec.o extr.o fs.o link.o mem.o msg.o opts.o str.o

deco: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

.c.o:
	$(CC) -D SHARE=\"$(SHARE)\" $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f deco *.o

install: deco
	mkdir -p -m 755 "$(DESTDIR)$(BIN)"
	cp deco "$(DESTDIR)$(BIN)"
	chmod 755 "$(DESTDIR)$(BIN)"/deco

uninstall:
	rm -f "$(DESTDIR)$(BIN)"/deco

deco $(OBJECTS): config.mk
arch.o: arch.h deco.h exec.h extr.h fs.h lang.h link.h math.h mem.h msg.h str.h
deco.o: arch.h exec.h extr.h lang.h link.h msg.h opts.h ver.h
exec.o: lang.h msg.h
extr.o: arch.h deco.h extr.h fs.h lang.h link.h mem.h msg.h str.h
fs.o: arch.h exec.h fs.h lang.h link.h msg.h
link.o: arch.h deco.h extr.h fs.h lang.h link.h mem.h msg.h str.h
mem.o: lang.h msg.h
msg.o: msg.h
opts.o: opts.h
