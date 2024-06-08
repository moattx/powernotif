/*
 *people can only save themselves -- Nodius* 
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

extern char *__progname;


// TODO: 
// 1- conf file.
// 2-handling of the daemon:-
// 1- Could send integers, or at least strings, for timeouts, or to
// increase the notification size(?). 2- signal suspend to kill off
// process. 


typedef struct
{
  unsigned int cap;
  char status[40];
  // size_t size;
} power;


static void
notifysend (char *str)
{
  char command[80];
  (void) snprintf (command, 80, "notify-send --urgency=critical '%s' '%s'",
	    __progname, str);
  (void) system (command);
}

static unsigned int
str2unsigned (char *str)
{
  unsigned int rint = 0;
  for (unsigned int x = 0; str[x] != '\0'; x++)
    {
      rint = rint * 10 + str[x] - '0';
    }
  return rint;
}

static bool
compare (char *str1, char *str2)
{
  unsigned int x = 0;
  while (str1[x] != '\0' || str2[x] != '\0')
    {
      if (str1[x] != str2[x])
	{
	  return false;
	}
      x++;
    }

  if ((str1[x] != '\0' && str2[x] == '\0')
      || (str2[x] != '\0' && str1[x] == '\0'))
    return false;

  return true;
}

static int
daemonize ()
{
  int retval = 0;
  int fd;
  FILE *nf;

  switch (fork ())
    {
    case -1:
      retval = 1;
      break;
    case 0:
      break;
    default:
      // exit parent
      _exit (0);
    }

  if (setsid () == -1)
    retval = 1;

  (void) chdir ("/");

  nf = fopen ("/dev/null", "r");
  fd = fileno (nf);
  (void) dup2 (fd, STDIN_FILENO);
  (void) dup2 (fd, STDOUT_FILENO);
  (void) dup2 (fd, STDERR_FILENO);
  return retval;
}


static void usage(void){
    fprintf(stderr, "%s [-p power] [-t timeout] [-h] \n", __progname);
    exit(EXIT_FAILURE);
}

void update (power * p);
void readfile (const char *path, char *rstr, size_t size);

int
main (int argc, char **argv)
{

        unsigned int cap = 20;
        unsigned int timeout = 1;
        // p - power capacity
        // t - timeout
        int ch;
	while ((ch = getopt(argc, argv, "p:t:h")) != -1) {
		switch (ch) {
		case 'p':
			cap = str2unsigned(optarg);
			break;
		case 't':
			timeout = str2unsigned(optarg);
			break;
                case 'h':
                    usage();
                    break;
                case '?':
		default:
			usage();
			break;
		}
	}

	argc -= optind;
	argv += optind;
        if (argc == 0){
            usage();
        }


  // Daemon time
  if (daemonize ())
    goto err;

  power pow;

  // now run
  for (;;)
    {
      update (&pow);
      if (pow.cap <= cap && compare (pow.status, "Discharging") == true)
	{
	  notifysend ("Plug in now!");
	}
      sleep (timeout);
    }

  return EXIT_SUCCESS;

err:
  return EXIT_FAILURE;
}


void
update (power *p)
{

  char capstr[3];
  // char status[40];
  readfile ("/sys/class/power_supply/BAT0/capacity", capstr, 3);
  p->cap = str2unsigned (capstr);
  readfile ("/sys/class/power_supply/BAT0/status", p->status, 40);
}

// Read text from a file and trim newline
void
readfile (const char *path, char *rstr, size_t size)
{
  assert (size > 1);
  FILE *fp;
  char *str = malloc (size);
  assert (str != NULL);

  if ((fp = fopen (path, "r")) == NULL)
    goto out;

  // read char bytes in fp until size and store it in str
  (void) fread (str, sizeof (char), size, fp);

  // remove trailing newline
  str[strcspn (str, "\n")] = 0;

  memcpy (rstr, str, size);

out:
  free (str);
  if (fclose (fp) != 0)
    err (EXIT_FAILURE, " ");
}
