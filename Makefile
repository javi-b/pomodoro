CC=gcc
CFLAGS=-Wall

ifeq ($(PREFIX),)
	PREFIX := /usr
endif

pomodoro : pomodoro.c
	$(CC) $(CFLAGS) -o pomodoro pomodoro.c

clean :
	rm -f pomodoro

install :
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 pomodoro $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/pomodoro/
	install -m 644 notification.mp3 $(DESTDIR)$(PREFIX)/share/pomodoro/

uninstall :
	rm -f $(DESTDIR)$(PREFIX)/bin/pomodoro
	rm -rf $(DESTDIR)$(PREFIX)/share/pomodoro
