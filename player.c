#include "common.h"
#include "player.h"

volatile sig_atomic_t signal_received = 0;

void handle_signal(int sig) {
    signal_received = 1;
}

void player_process(int id) {
    signal(SIGUSR1, handle_signal);

    close(ready_pipes[id][0]);
    int ready = 1;
    write(ready_pipes[id][1], &ready, sizeof(int));
    close(ready_pipes[id][1]);

    srand(getpid());
    int energy = 70 + rand() % 31;
    int decay = 1 + rand() % 5;

    close(pipes[id][0]);

    while (1) {
        while (!signal_received) pause();
        signal_received = 0;
        energy -= decay;
        if (energy < 0) energy = 0;

        int effort = 0;
        if (rand() % 20 == 0) {
            sleep(1 + rand() % 3);
            effort = 0;
        } else {
            int position = id % TEAM_SIZE;
            effort = energy * (position + 1);
        }

        write(pipes[id][1], &effort, sizeof(int));


        printf("Player %d: Effort = %d, Energy = %d\n", id, effort, energy);
    }
}