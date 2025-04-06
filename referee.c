#include "common.h"
#include "referee.h"
#include <stdlib.h>  // لاستخدام qsort
#include "graphics.h"

// هيكل لتخزين بيانات اللاعب (مثل الجهد، الرقم)
typedef struct {
    int player_id;
    int effort;
} PlayerEffort;

// دالة مقارنة لتحديد ترتيب اللاعبين حسب الجهد
int compare_effort(const void *a, const void *b) {
    PlayerEffort *player_a = (PlayerEffort *)a;
    PlayerEffort *player_b = (PlayerEffort *)b;
    return player_b->effort - player_a->effort; // ترتيب تنازلي (الأعلى جهدًا أولاً)
}
void referee_process() {
    printf("Referee started!\n");
    signal(SIGINT, SIG_IGN); // تجاهل Ctrl+C

    int team1_score = 0, team2_score = 0;
    int round_count = 0;
    int last_winner = -1;         // -1 = لا أحد، 0 = فريق 1، 1 = فريق 2
    int consecutive_wins = 0;
    int team1_effort = 0, team2_effort = 0;

    time_t start_time = time(NULL); // بداية المباراة

    while (1) {
        printf("\n🏁 Round %d starts!\n", round_count + 1);

        if (round_count >= 8) {
            printf("🏁 Maximum number of rounds reached.\n");
            break;
        }

        time_t now = time(NULL);
        if (difftime(now, start_time) >= 120) {
            printf("⏰ Match time (2 minutes) is over.\n");
            break;
        }

        printf("\n🔔 Round %d is in progress...\n", round_count + 1);

        // مصفوفة لتخزين الجهود لكل اللاعبين
        PlayerEffort team1_efforts[TEAM_SIZE];
        PlayerEffort team2_efforts[TEAM_SIZE];

        time_t round_start = time(NULL);
        while (difftime(time(NULL), round_start) < 20) {
            printf("⏳ Waiting for players to report their effort...\n");

            for (int i = 0; i < NUM_PLAYERS; i++) {
                kill(player_pids[i], SIGUSR1); // إرسال الإشارة
                sleep(2);
            }

            sleep(1); // إعطاء اللاعبين وقت كافي للرد

            // قراءة الجهود من اللاعبين وتخزينها
            for (int i = 0; i < NUM_PLAYERS; i++) {
                int effort = 0;
                if (read(pipes[i][0], &effort, sizeof(int)) > 0) {
                    if (effort == 0) {
                        printf("⚠️ Player %d is injured!\n", i);
                    } else {
                        printf("✅ Player %d effort: %d\n", i, effort);
                    }

                    if (i < TEAM_SIZE) {
                        team1_efforts[i].player_id = i;
                        team1_efforts[i].effort = effort;
                        team1_effort += effort;
                    } else {
                        team2_efforts[i - TEAM_SIZE].player_id = i;
                        team2_efforts[i - TEAM_SIZE].effort = effort;
                        team2_effort += effort;
                    }

                    // ✅ تحديث الجهد بصريًا مباشرة
                    update_single_player_effort(i, effort);
                } else {
                    printf("⚠️ Player %d didn't respond!\n", i);
                }
            }
            // إرسال الجهود الفردية للرسم
            int t1[4], t2[4];
            for (int i = 0; i < 4; i++) {
                t1[i] = team1_efforts[i].effort;
                t2[i] = team2_efforts[i].effort;
            }
            update_player_efforts(t1, t2);
            // ترتيب اللاعبين في الفريق الأول حسب الجهد
            qsort(team1_efforts, TEAM_SIZE, sizeof(PlayerEffort), compare_effort);

            // ترتيب اللاعبين في الفريق الثاني حسب الجهد
            qsort(team2_efforts, TEAM_SIZE, sizeof(PlayerEffort), compare_effort);

            // عرض الترتيب داخل الفريق الأول
            printf("\n🔝 Team 1 players ranked by effort:\n");
            for (int i = 0; i < TEAM_SIZE; i++) {
                printf("Player %d: Effort = %d\n", team1_efforts[i].player_id, team1_efforts[i].effort);
            }

            // عرض الترتيب داخل الفريق الثاني
            printf("\n🔝 Team 2 players ranked by effort:\n");
            for (int i = 0; i < TEAM_SIZE; i++) {
                printf("Player %d: Effort = %d\n", team2_efforts[i].player_id, team2_efforts[i].effort);
            }

            // عرض الجهود التراكمية حتى الآن
            printf("📊 Total Effort So Far - Team1: %d | Team2: %d\n", team1_effort, team2_effort);
        }

        // تم انتهاء الجولة، نحدد من فاز ونحدث المتغيرات
        round_count++;
        if (team1_effort > team2_effort) {
            team1_score++;
            printf("🏆 Team 1 wins this round!\n");

            if (last_winner == 0)
                consecutive_wins++;
            else {
                consecutive_wins = 1;
                last_winner = 0;
            }

        } else if (team2_effort > team1_effort) {
            team2_score++;
            printf("🏆 Team 2 wins this round!\n");

            if (last_winner == 1)
                consecutive_wins++;
            else {
                consecutive_wins = 1;
                last_winner = 1;
            }

        } else {
            printf("🤝 It's a draw!\n");
            consecutive_wins = 0;
            last_winner = -1;
        }

        printf("📈 Score after Round %d: Team1 = %d | Team2 = %d\n", round_count, team1_score, team2_score);

        // فحص الفوز المتتالي
        if (consecutive_wins == 2) {
            printf("🏁 Team %d wins the match with two consecutive rounds!\n", last_winner + 1);
            break;
        }

        if (round_count < 4) {
            printf("⏸️ Resting for 5 seconds before next round...\n");
            sleep(5);
        }
    }

    // عرض النتيجة النهائية
    printf("\n🏆 Final Score: Team1 = %d | Team2 = %d\n", team1_score, team2_score);

    // إنهاء اللاعبين
    for (int i = 0; i < NUM_PLAYERS; i++) {
        kill(player_pids[i], SIGKILL);
    }
}
