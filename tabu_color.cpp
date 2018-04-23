#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASTABLE -1
#define OVERFLOW -2
#define K 49
#define N 500


typedef int status;

int sol[N+1];                  //存储颜色
int M[N+1][K+1];              //Adjacent Color
int tabu[N+1][K+1];              //禁忌表
int iter = 0;               //迭代次数
int best_tmove[4];            //存储当前最优禁忌移动
int best_nmove[4];            //存储当前最优非禁忌移动
int best_move[4];            //存储当前最优移动
int delta = 0;                     //当前最优解所对应的delta值
int bestf = 1000;                   //记录最少冲突数
int f = 1000;                       //记录当前冲突数
int con_num = 0;
int con[N+1];
int pos[N+1];
int Gra[N+1][N+1];


status Create_G1(){
    int head = 0,tail = 0;
    char e[10];
    int n = 0;
    char filename[]="/Users/xuyunjie/DSJC500.5.col";
    FILE *fp = NULL;
    
    fp=fopen(filename, "r");
    if(fp)printf("Succeed!\n");
    else printf("Fail!\n");
    while(1){
        //        if (!fscanf(fp, "%s",e))break;
        fscanf(fp, "%s",e);
        if (strcmp(e, "e")) {
            break;
        }
        fscanf(fp, "%d",&head);
        fscanf(fp, "%d",&tail);
        Gra[head][tail] = 1;
        Gra[tail][head] = 1;
        n++;
    }
    printf("当前边数为%d\n",n);
    return OK;
    
}


void init_conflict(){
    int i = 1,j = 1;
    con_num = 0;
    for (i=1; i<N+1; i++) {
        for (j=1; j<N+1; j++) {
            if (Gra[i][j]==1&&sol[i]==sol[j]) {
                con[con_num+1] = i;
                pos[i] = ++con_num;
                break;
            }
        }
    }
}

int radom(int k){          // 1/k概率返回0
    int t;
    t = rand()%k;
    return t;
}

status ini_color(int color[]){              //初始化颜色表
    int n = 1;
    srand(time(NULL));
    while ((n<N+1)) {
        color[n] = rand()%K+1;
        n++;
    }
    return OK;
}

status init_acolor(){              //初始化邻色表
    int row = 1,col = 1;
    while(row<N+1){
        col = 1;
        while(col<K+1){
            M[row][col] = 0;
            col++;
        }
        row++;
    }
    return OK;
}

status Create_Adj(){         //创建邻色表
    int i = 1,j = 1;
    int color_num = 0;
    for(i=1;i<N+1;i++) {
        for (j=1; j<N+1; j++) {
            if (Gra[i][j]==1) {
                color_num = sol[j];
                M[i][color_num]++;
            }
        }
    }
    return OK;
}

int cal_f(){                            //计算冲突数
    int num = 0;
    int i = 1,j = 1;
    while (i<N+1) {
        for (j=1; j<N+1; j++) {
            if (Gra[i][j]==1&&sol[i]==sol[j]) {
                num++;
            }
        }
        i++;
    }
    num = num/2;
    return num;
}

void Find_Move1(){
    int i = 0;
    int k = 0;
    int delta1 = 0;
    int cndelta = 10;               //非禁忌最小delta
    int ctdelta = 10;               //禁忌中最小delta
    int rand_mar1 = 1;
    int rand_mar2 = 1;
    int q = 1;
    srand(time(NULL));
    for (q=1; q<con_num+1; q++) {
        i = con[q];
        for (k=1; k<K+1; k++) {                 //寻找最优邻域动作
            if (k!=sol[i]) {
                delta1 = M[i][k]-M[i][sol[i]];
                if(tabu[i][k]>iter){            //禁忌情况
                    if (delta1<ctdelta) {           //更新最佳移动
                        ctdelta = delta1;
                        best_tmove[0] = i;
                        best_tmove[1] = sol[i];
                        best_tmove[2] = k;
//                        best_tmove[3] = ctdelta;
                        rand_mar1 = 2;
                    }
                    else if (delta1==ctdelta) {          //delta相等时随机处理
                        if (!radom(rand_mar1)) {
                            best_tmove[0] = i;
                            best_tmove[1] = sol[i];
                            best_tmove[2] = k;
//                            best_tmove[3] = ctdelta;
                        }
                        rand_mar1++;
                    }
                }
                else{                               //非禁忌情况
                    if (delta1<cndelta) {
                        cndelta = delta1;
                        best_nmove[0] = i;
                        best_nmove[1] = sol[i];
                        best_nmove[2] = k;
//                        best_nmove[3] = cndelta;
                        rand_mar2 = 2;
                    }
                    if (delta1==cndelta) {
                        if (!radom(rand_mar2)) {
                            best_nmove[0] = i;
                            best_nmove[1] = sol[i];
                            best_nmove[2] = k;
//                            best_nmove[3] = cndelta;
                        }
                        rand_mar2++;
                    }
                }
            }
        }
      
    }
    
    if(ctdelta<cndelta&&f+ctdelta<bestf){            // 处理aspiration 情况
        best_move[0]=best_tmove[0];
        best_move[1]=best_tmove[1];
        best_move[2]=best_tmove[2];
        best_move[3]=ctdelta;
        printf("Now is aspiration\n");
    }
    else{
        best_move[0]=best_nmove[0];
        best_move[1]=best_nmove[1];
        best_move[2]=best_nmove[2];
        best_move[3]=cndelta;
        
    }
}

void update_ac1(){                               //更新邻色表
    int u = best_move[0];
    int i = best_move[1];
    int j = best_move[2];
    
    int n = 1;
    if (M[u][i]!=0&&M[u][j]==0) {               //u从冲突点变为不冲突点
        con[pos[u]] = con[con_num];
        pos[con[con_num]] = pos[u];
        con[con_num] = 0;                   //??
        con_num--;
    }
    if (M[u][i]==0&&M[u][j]!=0) {           //u变为冲突点
        con[con_num+1] = u;
        pos[u] = ++con_num;
    }
    for (n=1; n<N+1; n++) {
        if (Gra[u][n]==1) {
            if (sol[n] == i&&M[n][i] == 1) {        //冲突减少
                con[pos[n]] = con[con_num];
                pos[con[con_num]] = pos[n];
                con[con_num] = 0;                      //??
                con_num--;
            }
            if (sol[n] == j&&M[n][j] == 0) {           //冲突增加
                con[con_num+1] = n;
                pos[n] = ++con_num;
            }
            M[n][i]--;
            M[n][j]++;
        }
    }
}

void Makemove(){
    int u = best_move[0];
    int i = best_move[1];
    int j = best_move[2];
    
    sol[u] = j;
    f = f+best_move[3];
    if (f<bestf) {                  //更新最少冲突
        bestf = f;
    }
    tabu[u][i] = iter+f+rand()%10;
    update_ac1();
}

void ttt(){
    int m =1;
    int n =1;
    for (n=1; n<N+1; n++) {
        for (m=1; m<N+1; m++) {
            if(M[n][m]<0)printf("%d %d error!\n",n,m);
        }
    }
}

int cal_con(){
    int i = 1,j = 1;
    int num = 0;
    for (i=1; i<N+1; i++) {
        for (j=1; j<N+1; j++) {
            if (Gra[i][j]==1&&sol[i]==sol[j]) {
                num++;
                break;
            }
        }
    }
    return num;
}

int main(void){
    clock_t stt = clock();
    clock_t ett;
    int c = 100;
    int con_n = 200;
    double cost_sec = 0;
    Create_G1();
    ini_color(sol);
    init_conflict();
    Create_Adj();
    f=cal_f();
    bestf = f;
    con_n = cal_con();
    
    
    while (f!=0) {
        Find_Move1();
        Makemove();
//        c= cal_f();
//        con_n = cal_con();
        if(iter%100000 == 0){
            printf("当前最优解： %d\t迭代次数：%d\n",bestf,iter);
        }
        iter++;
    }
    ett = clock();
    cost_sec = ((double)ett-(double)stt)/CLOCKS_PER_SEC;
    if (f == 0) {
        printf("当前最优解： %d\t迭代次数：%d 当前解 %d\n",bestf,iter,cal_f());
        printf("总耗时 %f",cost_sec);
    }
    return 0;
    
}
