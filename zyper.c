/* Copyright 2016 - 2017 Marc Volker Dickmann */
/* Project: Zyper */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "zyper.h"

static bool
zyper_cmpwrd (const char *target, const char *str)
{
	return strlen (target) == strlen (str) && strcmp (target, str) == 0;
}

static void
zyper_init_data (zyper_data *zdata)
{
	zdata->wrds_amount = 0;
}

static bool
zyper_read_wrds (zyper_data *zdata, const char *filename)
{
	FILE *wrdsfile = fopen (filename, "r");
	
	if (wrdsfile == NULL)
	{
		return false;
	}
	
	char line[250];
	while (fgets (line, sizeof (line), wrdsfile) != NULL)
	{
		zdata->wrds_amount++;
	}
	
	zdata->wrds = malloc (sizeof (char*) * zdata->wrds_amount);
	rewind (wrdsfile);
	
	size_t i = 0;
	while (fgets (line, sizeof (line), wrdsfile) != NULL)
	{
		zdata->wrds[i] = strdup (line);
		i++; 
	}
	
	fclose (wrdsfile);
	return true;
}

static void
zyper_free_wrds (zyper_data *zdata)
{
	for (size_t i = 0; i < zdata->wrds_amount; i++)
	{
		free (zdata->wrds[i]);
	}
	free (zdata->wrds);
}

static const char*
zyper_get_wrd (const zyper_data *zdata)
{
	return zdata->wrds[rand () % zdata->wrds_amount];
}

static void
zyper_wait (const unsigned short secs)
{
	struct timespec req, rem;
	
	req.tv_sec = secs;
	req.tv_nsec = 0;
	rem.tv_sec = 0;
	rem.tv_nsec = 0;
	
	nanosleep (&req, &rem);
}

static void
zyper_counter (void)
{
	printf ("3\n");
	zyper_wait (1);
	
	printf ("2\n");
	zyper_wait (1);
	
	printf ("1\n");
	zyper_wait (1);
	
	printf ("GO!\n");
}

static void
zyper_gameloop (zyper_data *zdata)
{
	unsigned short lives		= 3;
	unsigned short correctwrds	= 0;
	
	printf ("Welcome to Zyper!\nType the given wrd correctly. Youve got %hu lives. Each failure results in losing 1 live.\nYouve got 1 minute!\n", lives); 

	zyper_counter ();

	char wrd_in[250];
	const char *wrd;
	const time_t ts = time (NULL);
	time_t t;
	
	while (lives > 0)
	{
		wrd = zyper_get_wrd (zdata);
		printf ("Type: %s\n", wrd);
		
		t = time (NULL);
		
		fgets (wrd_in, sizeof (wrd_in), stdin);
		
		t = time (NULL) - t;
		
		if (zyper_cmpwrd (wrd, wrd_in) &&
		    time (NULL) - ts <= 60)
		{
			printf ("Done in %i seconds!\n", (int) t);
			correctwrds++;
		}
		else
		{
			printf ("WRONG! Lives: %hu\n", --lives); 
		}
	}

	printf ("\n\nCorrect words: %hu\n", correctwrds);
}

int
main (int argc, char *argv[])
{
	zyper_data zdata;
	
	printf ("Zyper v. 0.0.1 A (c) 2016 - 2017 Marc Volker Dickmann\n\n");
	
	srand (time (NULL));
	zyper_init_data (&zdata);
	
	if (zyper_read_wrds (&zdata, (argc == 2 ? argv[1] : "testwrds.txt")))
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
