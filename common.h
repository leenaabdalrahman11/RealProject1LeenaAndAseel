#ifndef COMMON_H
#define COMMON_H
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

#define NUM_PLAYERS 8
#define TEAM_SIZE 4

extern int *team1_player_efforts;
extern int *team2_player_efforts;

extern int pipes[NUM_PLAYERS][2];
extern pid_t player_pids[NUM_PLAYERS];
extern int ready_pipes[NUM_PLAYERS][2];
extern volatile sig_atomic_t signal_received;

#endif