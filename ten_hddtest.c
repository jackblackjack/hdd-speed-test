/**
 * =====================================================================================
 * @file   ten_hddtest.c
 * @brief  HDD speed tester
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define TESTFILE "hddtest.dat"
#define MB 1024 * 1024
struct TIME { int seconds; int minutes; int hours; };

/**
 * Calculate difference between t1 and t2 times.
 */
void mydiff_time(struct TIME t1, struct TIME t2, struct TIME *differ)
{
  if (t2.seconds > t1.seconds)
  {
    --t1.minutes;
    t1.seconds+=60;
  }

  differ->seconds = t1.seconds - t2.seconds;

  if (t2.minutes > t1.minutes)
  {
    --t1.hours;
    t1.minutes += 60;
  }
  differ->minutes = t1.minutes - t2.minutes;

  differ->hours = t1.hours - t2.hours;
}


int main()
{
  int i = 0;            /* Counter */
  int fsize = 0;        /* File size in megabytes */
  int fullsize = 0;     /* File size in bytes */
  int diff_seconds = 0; /* Difference time in seconds */
  float speed = 0;      /* HDD speed */
  char *mptr = 0;       /* Buffer */
  int fd = -1;          /* File descriptor */

  time_t rawtime;       
  struct tm * ptr_time;
  struct TIME tbegin, tfinish, diff;

  printf("What file size? (Mb): \n");
  scanf("%d", &fsize);

  /* Define full size file */
  fullsize = fsize * MB;

  /* Open file to write */
  if ((fd = open(TESTFILE, O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1)
  {
    fprintf(stderr, "%s: Fail to open file\n", TESTFILE);
    return 1;
  }

  printf("Start test...");

  /******************************************
   * Write file
   ******************************************/

  /* Save start time */
  time(&rawtime);
  ptr_time = localtime(&rawtime);
  tbegin.hours = ptr_time->tm_hour;
  tbegin.minutes = ptr_time->tm_min;
  tbegin.seconds = ptr_time->tm_sec;

  /* Write garbage to file */
  for(; i < fsize; i++)
  {
    if ((mptr = malloc(MB)) == NULL)
    {
      fprintf(stderr, "%s: Error - malloc(%d)\n", TESTFILE, MB);
      return 1;
    }
    memset(mptr, '\0', sizeof(mptr));

    write(fd, mptr, MB); 
  }
  close(fd);
  free(mptr);

  /* Save finish time */
  time(&rawtime);
  ptr_time = localtime(&rawtime);
  tfinish.hours = ptr_time->tm_hour;
  tfinish.minutes = ptr_time->tm_min;
  tfinish.seconds = ptr_time->tm_sec;

  /* Calculate time to create file */
  mydiff_time(tfinish, tbegin, &diff);

  /* Calculate speed */
  diff_seconds = diff.hours * 3600 + diff.minutes * 60 + diff.seconds;
  if (diff_seconds) speed = fullsize / 1024 / diff_seconds; else speed = fullsize / 1024;

  printf("\nHDD WRITE test: %dMb by %d:%d:%d (%.2f Kbytes/sec)", fsize, diff.hours, diff.minutes, diff.seconds, speed);

  /******************************************
   * Read file
   ******************************************/

  /* Save start time */
  time(&rawtime);
  ptr_time = localtime(&rawtime);
  tbegin.hours = ptr_time->tm_hour;
  tbegin.minutes = ptr_time->tm_min;
  tbegin.seconds = ptr_time->tm_sec;

  /* Open file */
  FILE * fr = fopen(TESTFILE, "rb");

  if (NULL == fr)
  {
    fprintf(stderr, "%s: Fail to read file\n", TESTFILE);
    return 1;
  }

  do {
    // Allocation buffer.
    if ((mptr = malloc(MB)) == NULL)
    {
      fprintf(stderr, "%s: Error - malloc(%d)\n", TESTFILE, MB);
      return 1;
    }

    // Read file
    i = fread(mptr, 1, MB, fr);
  }
  while(i == MB);
  free(mptr);
  fclose (fr);

  /* Save finish time */
  time(&rawtime);
  ptr_time = localtime(&rawtime);
  tfinish.hours = ptr_time->tm_hour;
  tfinish.minutes = ptr_time->tm_min;
  tfinish.seconds = ptr_time->tm_sec;

  /* Calculate time to read file */
  mydiff_time(tfinish, tbegin, &diff);

  /* Calculate speed */
  diff_seconds = diff.hours * 3600 + diff.minutes * 60 + diff.seconds;
  if (diff_seconds) speed = fullsize / 1024 / diff_seconds; else speed = fullsize / 1024;

  printf("\nHDD READ test: %dMb by %d:%d:%d (%.2f Kbytes/sec)\n", fsize, diff.hours, diff.minutes, diff.seconds, speed);

  return 0;
}