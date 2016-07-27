/* Copyright 2016 Marc Volker Dickmann */
/* Project: Zyper */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "zyper.h"

static int zyper_cmpwrd (char *target, char *str)
{
	if (strlen (target) != strlen (str))
	{
		return -1;
	}
	
	return strcmp (target, str);
}

static void zyper_init_data (zyper_data *zdata)
{
	zdata->wrds_amount = 0;
}

static short zyper_read_wrds (zyper_data *zdata, char *filename)
{
	unsigned short i;
	char line[250];
	FILE *wrdsfile;
	
	i = 0;
	
	wrdsfile = fopen (filename, "r");
	
	if (wrdsfile == NULL)
	{
		return -1;
	}
	
	while (fgets (line, sizeof (line), wrdsfile) != NULL)
	{
		zdata->wrds_amount++;
	}
	
	zdata->wrds = (char**) malloc (sizeof (char*) * zdata->wrds_amount);
	rewind (wrdsfile);
	
	while (fgets (line, sizeof (line), wrdsfile) != NULL)
	{
		zdata->wrds[i] = (char*) malloc (sizeof (char) * (strlen (line) + 1));
		strcpy (zdata->wrds[i], line);
		zdata->wrds[i][strlen (line)] = '\0';
		i++; 
	}
	
	fclose (wrdsfile);
	
	return 0;
}

static void zyper_free_wrds (zyper_data *zdata)
{
	unsigned short i;
	
	for (i = 0; i < zdata->wrds_amount; i++)
	{
		free (zdata->wrds[i]);
	}
	
	free (zdata->wrds);
}

static char *zyper_get_wrd (zyper_data *zdata)
{
	return zdata->wrds[rand () % zdata->wrds_amount];
}

static void zyper_wait (unsigned short secs)
{
	struct timespec req, rem;
	
	req.tv_sec = secs;
	req.tv_nsec = 0;
	rem.tv_sec = 0;
	rem.tv_nsec = 0;
	
	nanosleep (&req, &rem);
}

static void zyper_counter (void)
{
	printf ("3\n");
	zyper_wait (1);
	
	printf ("2\n");
	zyper_wait (1);
	
	printf ("1\n");
	zyper_wait (1);
	
	printf ("GO!\n");
}

static void zyper_gameloop (zyper_data *zdata)
{
	unsigned short lives, correctwrds;
	char *wrd, wrd_in[50];
	time_t t, ts;
	
	lives = 3;
	correctwrds = 0;
	
	printf ("Welcome to Zyper!\nType the given wrd correctly. Youve got %hi lives. Each failure results in losing 1 live.\nYouve got 1 minute!\n", lives); 

	zyper_counter ();

	ts = time (NULL);
	
	while (lives > 0)
	{
		wrd = zyper_get_wrd (zdata);
		printf ("Type: %s\n", wrd);
		
		t = time (NULL);
		
		fgets (wrd_in, sizeof (wrd_in), stdin);
		
		t = time (NULL) - t;
		
		if (zyper_cmpwrd (wrd, wrd_in) == 0 && time (NULL) - ts <= 60)
		{
			printf ("Done in %i seconds!\n", (int) t);
			correctwrds++;
		}
		else
		{
			lives--;
			printf ("WRONG! Lives: %hi\n", lives); 
		}
	}

	printf ("\n\nCorrect words: %hi\n", correctwrds);
}

int main (int argc, char *argv[])
{
	short r;
	zyper_data zdata;
	
	printf ("Zyper v. 0.0.1 A (c) 2016 Marc Volker Dickmann\n\n");
	
	srand (time (NULL));
	
	zyper_init_data (&zdata);
	
	if (argc == 2)
	{
		r = zyper_read_wrds (&zdata, argv[1]);
	}
	else
	{
		r = zyper_read_wrds (&zdata, "testwrds.txt");
	}

	if (r == 0)
	{
		zyper_gameloop (&zdata);
		zyper_free_wrds (&zdata);
	}
	else
	{
		printf ("ERROR: Couldn't read the wordbook!\n");
	}
	
	return 0;
}
