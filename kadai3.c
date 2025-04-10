//時刻t
//待合室にいる人数N
//サ最も早く空く窓口のサービス終了時刻nextFreeTime
//最も早く空く窓口のインデックスnextWindow
//到着時刻arrive[]
//サービス時間service[]
//待合室の人の列queue[]
//待合室の先頭と末尾のインデックスqueueFront, queueRear
/*1) システムに入れず退去する客の割合
2) システム内（サービス窓口+待合室）にいる平均の客数
3) サービスを受け始めるまでに各客が待合室で待つ平均時間
（システムに入れず退去する客は考慮しない）*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE 100 // 待合室の最大人数
#define INF 1e18 // 無限大
#define M 1 // サービス窓口の数

int main(void) {
    double *arrive = NULL; // 到着時刻の動的配列
    double *service = NULL; // サービス時間の動的配列
    double queue[MAX_QUEUE]; // 待合室
    double windows[M]; // 各窓口のサービス終了時刻
    int queueFront = 0, queueRear = 0; 
    double t = 0.0;
    int i = 0, N = 0, totalCustomers = 0, dismissed = 0;
    double systemN = 0; //ある時点でのシステム内の人数の合計
    double sumN = 0; //システム内の人数の累積和
    double lastEventTime = 0.0; //最後のイベント時刻

    // 窓口の初期化
    for (int k = 0; k < M; k++) {
        windows[k] = 0.0; // 全ての窓口を空にする
    }

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
    rewind(arriveFile);
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
    while (i < totalCustomers) { // 最後の客が到着するまでループ
        // 最も早く空く窓口を探す
        double nextFreeTime = INF;
        int nextWindow = -1;
        for (int k = 0; k < M; k++) {
            if (windows[k] < nextFreeTime) {
                if(windows[k] != 0.0){
                    nextFreeTime = windows[k]; // 最も早く空く窓口のサービス終了時刻
                    nextWindow = k; // 最も早く空く窓口のインデックス
                } else{
                    nextFreeTime = INF; // サービス窓口が空いている場合は無限大にする
                    nextWindow = k;
                }
            }
        }


        if (arrive[i] < nextFreeTime) { // 到着イベント
            t = arrive[i]; // 到着した時間

            systemN = N; //システム全体にいる人数を求める
            for (int k = 0; k < M; k++) {
                if(windows[k] != 0.0){
                    systemN++;
                }
            }
            sumN += systemN * (t - lastEventTime); //システム全体にいる人数の累積和
            lastEventTime = t;
            // 空いている窓口を探す
            int freeWindow = -1;
            for (int k = 0; k < M; k++) {
                if (windows[k] == 0.0) { // 窓口が空いている
                    freeWindow = k;
                    break;
                }
            }


            if (freeWindow != -1) { // サービス窓口が空いている場合
                windows[freeWindow] = t + service[i]; // サービス終了時刻を更新
            } else { // 窓口が空いていない場合
                if (N < MAX_QUEUE) { // 待合室が空いている場合
                    queue[queueRear] = service[i]; // サービス時間をキューに追加
                    queueRear = (queueRear + 1) % MAX_QUEUE; // queueRearを更新
                    N++; // キューの人数を増やす
                } else { // 待合室が満員の場合
                    dismissed++; // 帰宅した人数を増やす
                }
            }
            i++; // 到着イベントが処理されたので次の客に進む
        } else { // サービス終了イベント
            t = nextFreeTime; // サービス終了した時間

            systemN = N; //システム全体にいる人数を求める
            for (int k = 0; k < M; k++) {
                if(windows[k] != 0.0){
                    systemN++;
                }
            }
            sumN += systemN * (t - lastEventTime);
            lastEventTime = t;

            if (N > 0) { // 待合室に人がいる場合
                windows[nextWindow] = t + queue[queueFront]; // サービス終了時刻を更新
                queueFront = (queueFront + 1) % MAX_QUEUE; // 待合室の先頭を更新
                N--;
            } else { // 待合室が空の場合
                windows[nextWindow] = 0.0; // 窓口を空にする
            }
        }
    }

    for (int k = 0; k < M; k++) {
        if(windows[k] != 0.0){
            N++;
        }
    }

    sumN += N * (t - lastEventTime);
    double aveN = (double)sumN / t; //システム内の平均人数


    // 結果を出力
    printf("システム内の客数: %d\n", N);
    printf("帰宅した客の割合: %f\n", dismissed / (double)totalCustomers);
    printf("t : %f\n", t);
    printf("システム内にいる客数の合計: %f\n", sumN);
    printf("システム内にいる平均の客数: %f\n", aveN);


    // 動的配列を解放
    free(arrive);
    free(service);

    return 0;
}
