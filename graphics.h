#ifndef GRAPHICS_H
#define GRAPHICS_H

void init_graphics(int *argc, char **argv);
void start_graphics();
void update_effort(int t1, int t2);
void update_player_efforts(int team1[4], int team2[4]);
void update_single_player_effort(int player_id, int effort);  // ✅ الإضافة الجديدة

#endif
