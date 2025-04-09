//時刻t
//サービス中の人と待合室の人を合わせたすべての人数N
//サービス中の人と待合室の人を合わせたすべてのサービス時間A
//サービス中の人のサービス終了時刻f
//到着時刻arrive[]
//サービス時間service[]
//待合室の人の列queue[]
//待合室の先頭と末尾のインデックスqueueFront, queueRear

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE 101 // カウンターと列を合わせた最大人数
#define INF 1e18 // 無限大

int main(void) {
    double *arrive = NULL; // 到着時刻の動的配列
    double *service = NULL; // サービス時間の動的配列
    double queue[MAX_QUEUE]; // 待合室
    int queueFront = 0, queueRear = 0; 
    double t = 0.0, A = 0.0, f = INF;
    int i = 0, N = 0, totalCustomers = 0;
    int dismissed = 0; // 帰宅した人数
    double sumN = 0; // システム内の人数の合計
    double lastEventTime = 0.0; // 最後のイベント時刻
    double sumWaitTime = 0.0; //サービスを受け始めるまでの待ち時間の合計

    // ファイルからデータを読み込む
    FILE *arriveFile = fopen("arrive.txt", "r");
    FILE *serviceFile = fopen("service.txt", "r");
    if (arriveFile == NULL || serviceFile == NULL) {
        printf("ファイルを開けませんでした。\n");
        return 1;
    }

    // データ数をカウント
    double temp;
    while (fscanf(arriveFile, "%lf", &temp) != EOF) {
        totalCustomers++;
    }
    rewind(arriveFile); // ファイルポインタを先頭に戻す
    rewind(serviceFile);

    // 動的配列を確保
    arrive = (double *)malloc(totalCustomers * sizeof(double));
    service = (double *)malloc(totalCustomers * sizeof(double));
    if (arrive == NULL || service == NULL) {
        printf("メモリの確保に失敗しました。\n");
        fclose(arriveFile);
        fclose(serviceFile);
        return 1;
    }

    // データを読み込む
    for (int j = 0; j < totalCustomers; j++) {
        fscanf(arriveFile, "%lf", &arrive[j]);
        fscanf(serviceFile, "%lf", &service[j]);
    }
    fclose(arriveFile);
    fclose(serviceFile);

    // シミュレーション開始
    while (i < totalCustomers) {
        if (arrive[i] < f) { // 到着イベント
            t = arrive[i]; // 到着した時間
            sumN += N * (t - lastEventTime);
            lastEventTime = t;
            if (N < MAX_QUEUE) { // 待合室が空いている場合
                if (N == 0) { // サービス窓口が空いている場合
                    f = t + service[i]; // サービス終了時刻を更新
                    sumWaitTime += 0.0; // 待ち時間は0
                } else if (N == 1) { // サービス窓口が空いておらず場合
                    double waitTime = f; //現在のサービス終了時刻
                    sumWaitTime += waitTime - t; // 待ち時間を加算
                } else {
                    double waitTime = f;
                    for(int j = queueFront + 1 ; j != queueRear ; j = (j + 1) % MAX_QUEUE){
                        waitTime += queue[j]; // 待ち時間を加算
                    }
                    sumWaitTime += waitTime - t;
                }
                // 窓口が開いてない場合はキューに追加するだけ
                queue[queueRear] = service[i];
                queueRear = (queueRear + 1) % MAX_QUEUE; // queueRearを更新
                N++; // キューの人数を増やす
            } else { // 待合室が満員の場合
                dismissed++; // 帰宅した人数を増やす
            }
            // 待合室が満員の場合は帰宅
            i++;
        } else { // サービス終了イベント
            t = f; // サービス終了した時間
            sumN += N * (t - lastEventTime);
            lastEventTime = t;

            queueFront = (queueFront + 1) % MAX_QUEUE; // 待合室の先頭を更新
            N--; // 待合室の人数を減らす
            if (N > 0) { // 待合室にまだ人がいる場合
                f = t + queue[queueFront]; // サービス終了時刻を更新
            } else { // 待合室が空の場合
                f = INF; // サービス終了時刻を無限大にする
            }
        }
    }

    //最後のイベント後の時間を加算
    sumN += N * (t - lastEventTime);
    double aveN = (double)sumN / t; // システム内の平均人数
    double aveWaitTime = (double)sumWaitTime / (totalCustomers - dismissed); // 平均待ち時間

    // 結果を出力
    printf("システム内の客数: %d\n", N);
    printf("帰宅した客の割合: %f\n", dismissed / (double)totalCustomers);
    printf("t : %f\n", t);
    printf("システム内にいる客数の合計: %f\n", sumN);
    printf("システム内にいる平均の客数: %f\n", aveN);
    printf("サービスを受けるまでの平均待ち時間: %f\n", aveWaitTime);
  
    FILE *outputFile = fopen("result1.txt", "w");
    if (outputFile != NULL) {
        fprintf(outputFile, "到達した直後にシステム内にいる客数: %d\n", N);
        fprintf(outputFile, "システムに入れず退去した客数: %f\n", dismissed / (double)totalCustomers);
        fprintf(outputFile, "システム内にいる平均の客数: %f\n", aveN);
        fclose(outputFile);
    } else {
        printf("結果ファイルを開けませんでした。\n");
    }

    // 動的配列を解放
    free(arrive);
    free(service);

    return 0;
}
