/*
 * Copyright 2011 Christian Hofstaedtler. Licensed under GPL Version 2.
 *
 * Most of this is copied from fgetty, which is
 *   Copyright Felix von Leitner, licensed under GPL Version 2.
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>

char* tty;
char* prog;
char* user;

void error(const char* message, int exitcode) {
  write(2, "E: ", 3);
  write(2, message, strlen(message));
  write(2, "\n", 1);
  syslog(LOG_CRIT, "%s", message);
  sleep(15);
  closelog();
  exit(exitcode);
}

void sigquit_handler(int signum) {
  error("SIGQUIT received\n",23);
}

void setup_tty() {
  struct sigaction sa;
  int fd;
  if (chown(tty,0,0) || chmod(tty,0600))
    error("could not chown/chmod tty device\n",1);
  sa.sa_handler=SIG_IGN;
  sa.sa_flags=0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGHUP,&sa,NULL);
  sa.sa_handler=sigquit_handler;
  sigaction(SIGQUIT,&sa,NULL);

  setsid();
  if ((fd=open(tty, O_RDWR, 0))<0)
    error("open tty failed", errno);
  if (ioctl (fd, TIOCSCTTY, (void *)1) == -1)
    error("ioctl TIOCSCTTY failed", errno);
  if (!isatty(fd))
    error("\"not a typewriter\" (isatty failed)", errno);
  if (vhangup())	/* linux specific */
    error("vhangup failed", errno);
  close(2); close(1); close(0); close(fd);
  if (open(tty, O_RDWR,0) != 0)
    error("could not (re)open tty", errno);
  if (dup(0) != 1 || dup(0) != 2)
    error("could not dup stdout and stderr", errno);

  sa.sa_handler=SIG_DFL;
  sa.sa_flags=0;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGHUP,&sa,NULL);
}

void setenvvar(char* envvar, char* value) {
  /* buf ends up directly in env. can't use alloca */
  char* buf = malloc(strlen(envvar) + strlen(value) + 2);
  strcpy(buf, envvar);
  strcat(buf, "=");
  strcat(buf, value);
  putenv(buf);
}

int main(int argc, char* argv[]) {
  struct passwd* pw;

  tty = argv[1];
  prog = argv[2];
  user = argv[3];

  openlog("startprog", LOG_CONS | LOG_PID, LOG_AUTHPRIV);

  if (tty == NULL || prog == NULL || user == NULL)
    error("Usage: startprog /dev/ttyX /bin/bash user", 1);

  pw = getpwnam(user);
  if (!pw)
    error("User does not exist", 1);

  clearenv();
  putenv("TERM=linux");
  setenvvar("TTY", tty);
  setenvvar("USER", pw->pw_name);
  setenvvar("LOGNAME", pw->pw_name);
  setenvvar("SHELL", pw->pw_shell);
  setenvvar("HOME", pw->pw_dir);
  {
    char uidbuf[15];
    sprintf(uidbuf, "%i", pw->pw_uid);
    setenvvar("UID", uidbuf);
  }

#ifndef DEBUG
  setup_tty();
#endif
  chown(tty, pw->pw_uid, pw->pw_gid);

  if (initgroups(pw->pw_name, pw->pw_gid) == -1)
    error("initgroups failed", 1);
  if (setgid(pw->pw_gid) == -1)
    error("setgid failed", 1);
  if (setuid(pw->pw_uid) == -1)
    error("setuid failed", 1);

  {
    char *Argv[] = {prog, 0};
    execve(prog, Argv, environ);
  }

  /* should never come here */
  error("Program startup failed", 99);
}
