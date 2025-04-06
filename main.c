#include "common.h"
#include "player.h"
#include "referee.h"
#include "graphics.h"
#include <X11/Xlib.h>  // لإضافة XInitThreads
#include <sys/mman.h>  // ✅ للمشاركة بالذاكرة


int *team1_player_efforts;
int *team2_player_efforts;

int pipes[NUM_PLAYERS][2];
pid_t player_pids[NUM_PLAYERS];
int ready_pipes[NUM_PLAYERS][2];

int main(int argc, char** argv) {
    XInitThreads();  // ✅ ضروري لتفادي مشاكل تعدد الخيوط في X11

    init_graphics(&argc, argv); // تهيئة OpenGL
    team1_player_efforts = mmap(NULL, 4 * sizeof(int), PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    team2_player_efforts = mmap(NULL, 4 * sizeof(int), PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (team1_player_efforts == MAP_FAILED || team2_player_efforts == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    for (int i = 0; i < NUM_PLAYERS; i++) {
        pipe(pipes[i]);
        pipe(ready_pipes[i]);
        fcntl(pipes[i][0], F_SETFL, O_NONBLOCK);
        setbuf(stdout, NULL);

        pid_t pid = fork();
        if (pid == 0) {
            player_process(i);
            exit(0);
        } else {
            player_pids[i] = pid;
            sleep(1);
            close(pipes[i][1]);
            close(ready_pipes[i][1]);
        }
    }

    for (int i = 0; i < NUM_PLAYERS; i++) {
        int dummy;
        sleep(1);
        if (read(ready_pipes[i][0], &dummy, sizeof(int)) != -1)
            printf("Player %d is ready!\n", i);
        else
            printf("Player %d is not ready!\n", i);
        close(ready_pipes[i][0]);
    }

    // تشغيل referee_process داخل fork منفصل
    pid_t referee_pid = fork();
    if (referee_pid == 0) {
        referee_process();
        exit(0);
    }

    // تشغيل واجهة OpenGL (آخر شيء)
    start_graphics();

    // بعد انتهاء واجهة الرسوميات
    for (int i = 0; i < NUM_PLAYERS; i++) {
        kill(player_pids[i], SIGKILL);
    }
    kill(referee_pid, SIGKILL);
    exit(0);

    // الانتظار حتى ينتهي اللاعبون (في حال النافذة أغلقت)
    for (int i = 0; i < NUM_PLAYERS; i++) {
        wait(NULL);
    }

    return 0;
}