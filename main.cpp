#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
using namespace std;

//function prototypes
void showStatus(int [][10], int [][4]);
void draw(int, int*, int);
void put(int, int, int [][4], int []);
void takeRow(int, int [][4], int [][10], int, int*);
void resetStatus(int[][10], int [][4], int [], int*);
void sorting(int[][10]);
int winner(int[]);

//global variables and arrays
const int n=5;//n is the number of players
int num=0;//num of players
const string playername[5]={"Player","CPU1","CPU2","CPU3","CPU4"};
const string CardTypes[11]={"0",//useless index
    "紅色","橘色","黃色","綠色","藍色","粉紅","灰色","彩色","＋2",
    //point cards are index 1~9
    "結束卡"};//index of ending card is 10
int stack[77]={0};//there are 77 cards in the stack
const int points[7]={0,1,3,6,10,15,21};//index 0 stands for nothing, other index stands for num of cards and the value stands for the corresponding points

//main function
int main() {
    
    int player[n][10]={0};//index 0 is row status, if=0 means no row yet, if=1 means player has already taken a row
    int Row[n][4]={0};//index 3 is to see whether row is taken(1 for taken and 0 for not
    int RowFlag[n]={0};//point at the position to place cards in rows
    int counter[10]={0};//to count num of cards for different card types
    int limit[10]={0,9,9,9,9,9,9,9,3,10};//the limit for different card types
    int RowsTaken=0;//counts how many rows are taken
    
    cout << "請選擇玩家人數(3~5人):";
    cin >> num;
    //distribute one chameleon to each player
    for(int i=0, cardprevious=0, cardnext=0; i<num; i++){//cardprevious and cardnext is used to make sure no repeated colors
        srand(unsigned(time(NULL)));
        if(i==0){
            cardprevious=(num==3)?(rand()%6+2):(rand()%7+1);
            player[i][cardprevious]++;
            counter[cardprevious]++;
        }
        else{
            do{
                cardnext=(num==3)?(rand()%6+2):(rand()%7+1);
            }while(counter[cardnext]==1);
            player[i][cardnext]++;
            counter[cardnext]++;
            cardprevious=cardnext;
        }
    }
    showStatus(player, Row);
    //stack shuffling
    int stackTotalCards=(num==3)?68:77;
    int endingCard=stackTotalCards-16;
    for(int i=0; i<stackTotalCards-num; i++){//num of cards in stack are 77-num because num cards have been distributed to players
        if(i==endingCard-num){
            stack[i] = 10;//index (endingCard-num)th card is the ending card
        }
        else{
            stack[i] = (num==3)?rand()%8+2:rand()%9+1;//give stack[i] value from 1~9 corresponding to CardType but 2~9 when 3 players
            if(counter[stack[i]]>=limit[stack[i]]){//num of that type of card exceeds limit
                i--;//do again
            }
            else{//does not exceed limit
                counter[stack[i]]++;
            }
        }
        cout << i<<CardTypes[stack[i]] << endl;
    }//finish shuffling, the stack is ready to be drawn
    
    int stackFlag=0;//points at the card in stack
    
    //show initial status
    showStatus(player,Row);
    
    //game start
    int i=0;//i is "whose turn", the game starts from "Player"
    int step=1;//step is used to restrict player's movement,0=free,1=draw,2=take row
    //retrict the first player's movement to 1
    
    while(1){
        //movement selection
        if(player[i][0]!=1){//mean not taken a row yet
            
            int choice=0;//used later as the movement chosen by player
            cout << "輪到" << playername[i] << "..." << endl;
            if(step==1){//retsricted choice 1
                cout << "所有Row皆無卡。請按任何鍵以翻牌。" << endl;
            }
            else if(step==2){//retsricted choice 2
                cout << "您只能收走行列卡。請按任何鍵以收走所剩的Row。" << endl;
            }
            else{//free choice
                cout << "你要... (1)翻牌 (2)收走行列卡" << endl;
                cout << "(請輸入 1 或 2)：" << endl;
            }
            
            if(i==0){//user
                if(step==0){
                    cin >> choice;
                    while((choice!=1)&&(choice!=2)){//avoid invalid input
                        cout << "輸入無效！請再試一次。" << endl;
                        cout << "(請輸入 1 或 2)：" << endl;
                        cin >> choice;
                    }
                }
                else{//user restricted choice
                    cin >> choice;//get input but ignore the value
                    choice = step;//let choice=step directly
                }
                
            }
            else if((step==1)||(step==2)){//cpu restricted choice
                choice = step;
                cout << choice << endl;
            }
            else{//cpu free choice
                choice = rand()%2+1;//random choice
                cout << choice << endl;
            }
            
            //movement execution
            if(choice==1){//player wants to draw
                draw(choice, &stackFlag, endingCard);//draw a card
                cout << "放入哪一行？" << endl;
                cout << "(請輸入0~" << num-1 << "):" << endl;
                if(i==0){//user
                    bool valid=false;
                    while(valid==false){
                        cin >> choice;
                        //avoid invalid input
                        if((choice>num-1)||(choice<0)){
                            cout << "輸入無效！請再試一次。" << endl;
                            cout << "(請輸入0~" << num-1 << "):" << endl;
                        }
                        //avoid invalid choice
                        else if(Row[choice][3]==1){//row is taken
                            cout << "Row" << choice << "已被取走!請再輸入一次：";
                        }
                        else if(Row[choice][2]!=0){//row is full
                            cout << "Row" << choice << "已滿!請再輸入一次：";
                        }
                        else{
                            valid=true;
                        }
                    }
                    
                }
                else{//cpu
                    do{
                        choice=rand()%num;//random choice between 0~num-1
                    }while((Row[choice][2]!=0)||(Row[choice][3]==1));//avoid invalid choice
                    cout << choice << endl;
                }
                put(choice, stackFlag, Row, RowFlag);//put card in stack
                stackFlag++;//stackFlag point to next card in stack
            }
            else{//player wants to take a row
                cout << "收走哪一行？" << endl;
                if(i==0){//user
                    bool valid=false;
                    //avoid invalid input
                    while(valid==false){
                        cin >> choice;
                        if((choice>num-1)||(choice<0)){
                            cout << "輸入無效！請再試一次。" << endl;
                            cout << "(請輸入0~" << num-1 << "):" << endl;
                        }
                        //avoid invalid choice
                        else if(Row[choice][0]==0){//row is empty
                            cout << "Row" << choice << "無卡!請再輸入一次:";
                        }
                        else if(Row[choice][3]==1){//row has been taken
                            cout << "Row" << choice << "已被取走!請再輸入一次：";
                        }
                        else{
                            valid=true;
                        }
                    }
                    
                }
                else{//cpu
                    do{
                        choice = rand()%num;//random choosing a row between 0~num-1
                    }while((Row[choice][0]==0)||(Row[choice][3]==1));//avoid invalid choice
                    cout << choice << endl;
                }
                takeRow(choice, Row, player, i, &RowsTaken);//take row
            }
            i=(i+1)%num;//next player
            showStatus(player,Row);//show resulting status
        }
        else{//skip current player because the current player has already taken a row
            i=(i+1)%num;
        }
        
        step=0;//assume there are no restrictions to next player's movement
        
        //from here to line 185 are to check if restrict movements are required for next player
        //CASE ONE:when more than one row left but all of which are empty, only draw card(step=1) is permitted
        //CASE TWO:when more than one row left but all of which are full, only draw card(step=2) is permitted
        int blankRowCnt=0;
        int fullRowCnt=0;
        for(int r=0; r<num; r++){
            if(Row[r][3]!=1){//find the remaining rows
                if(RowFlag[r]==0){//if the remaining rows are empty
                    blankRowCnt++;//add to blank-roll counter
                }
                else if(RowFlag[r]==3){//else if the remaining rows are full
                    fullRowCnt++;//add to full-row counter
                }
            }
        }
        if(blankRowCnt==(num-RowsTaken)){//CASE ONE solution
            step=1;//movement restricted to 1
        }
        if(fullRowCnt==(num-RowsTaken)){//CASE TWO solution
            step=2;//movement restricted to 2
        }
        
        //when all rows are taken, start a new round
        if(RowsTaken==num){
            if(stackFlag>endingCard-num){//but end game when ending card is shown
                cout << "--------遊戲結束--------" << endl;
                break;
            }
            resetStatus(player,Row,RowFlag,&RowsTaken);
            cout << "--------新回合--------" << endl;
            showStatus(player,Row);
            step=1;//retrict the starting player's movement to 1
        }
    }//end of game
    
    //counting scores
    sorting(player);//sort the color cards of each player in decreasing order
    int total[n]={0};//total points for each player
    cout << "--------分數統計--------" << endl;
    
    for(int i=0; i<num; i++){
        int top=(num==3)?2:1;//the most is index 1(originally red) but index 2 when only 3 players
        //switch rainbow into the top3 colors for each player
        while(player[i][8]>0){//mean there's still rainbow cards not switched
            //note that if there are more than 6 cards in the top3 categories, the rainbow card does not make a difference in total scores, so tranformation happens only if there are less than 6 cards
            //transformation priority order:the most>the second>the third
            if(player[i][top]<6)  player[i][top]++;
            else if(player[i][top+1]<6) player[i][top+1]++;
            else if(player[i][top+2]<6)  player[i][top+2]++;
            else break;//if top3 all more than 6card than jump out of while-loop
            player[i][8]--;//one rainbow card is used
        }
        
        //start counting points by rule
        for(int pntFlag=top; pntFlag<10; pntFlag++){//pntFlag points at player's color cards and +2cards
            if(pntFlag==8)  continue;//skip rainbow (already switched into other colors)
            if(pntFlag<=top+2)  total[i] += points[player[i][pntFlag]];//top three colors are (+)
            else if(pntFlag==9)  total[i] += 2*player[i][pntFlag];//+2 cards
            else    total[i] -= points[player[i][pntFlag]];//others are (-)
        }
        cout << playername[i] << ":" << total[i] << endl;//show result
    }
    
    cout << "遊戲結果" << endl;
    cout << playername[winner(total)] << "勝利!" << endl;//show winner
    system("pause");
    return 0;
}//end main



//functions

void showStatus(int player[][10], int Row[][4])
{
    //show status(top line)
    int bar=0;
    while(bar<70){
        cout << "=";
        bar++;
    }
    cout << endl;
    cout << setw(6) << " ";
    cout << "   行列卡  " ;
    
    if(num!=3)  cout << "紅色   " ;
    
    cout << "橙色   " ;
    cout << "黃色  " ;
    cout << "綠色   " ;
    cout << "藍色   " ;
    cout << "粉紅   " ;
    cout << "灰色   " ;
    cout << "彩色  " ;
    cout << "＋2" ;
    cout << endl;
    //show status(players' status)
    for(int i=0; i<num; i++){
        cout << setw(6) << playername[i];
        for(int j=0; j<10; j++){
            if((num==3)&&(j==1))    continue;
            cout << setw(6) << player[i][j] ;
        }
        cout << endl;
    }
    //show Row status
    for(int i=0; i<num; i++){
        if(Row[i][3]==1){
            cout << endl;
        }
        else{
            cout << "Row" << i << ":";
            for(int j=0; j<3; j++){//don't need to show index 3
                if(CardTypes[Row[i][j]]=="0")   break;
                cout << CardTypes[Row[i][j]] << " ";
            }
            cout << endl;
        }
    }
    bar=0;
    while(bar<70){
        cout << "=";
        bar++;
    }
    cout << endl;
}

void draw(int choice, int *stackFlag, int endingCard){
    cout << "翻出的牌是「" << CardTypes[stack[*stackFlag]] << "」" << endl;//if stackFlag=61-num, it points to the ending card
    if(*stackFlag==endingCard-num){
        cout << "--------此為最終回合--------" << endl;
        *stackFlag+=1;
        cout << "再翻一張牌..." << endl;
        cout << "翻出的牌是「" << CardTypes[stack[*stackFlag]] << "」" << endl;
    }
}

void put(int choice, int stackFlag, int Row[][4], int RowFlag[]){
    Row[choice][RowFlag[choice]] = stack[stackFlag];//put card into row
    RowFlag[choice]++;
}

void takeRow(int choice, int Row[][4], int player[][10], int i, int *RowsTaken){
    int flag=0;//used to point at the cards in the chosen row to put into player's stack
    while(Row[choice][flag]!=0){//if=0 means no more cards left in row, so stop moving it into player's stack
        player[i][Row[choice][flag]]++;
        flag++;
    }
    *RowsTaken+=1;//one more row is taken
    player[i][0]=1;//status for indicating that player has taken a row
    Row[choice][3]=1;//status for indicating that row has been taken
}


void resetStatus(int player[][10], int Row[][4], int RowFlag[], int *RowsTaken){
    //reseting Row status when the round is over
    for(int i=0; i<num; i++){
        player[i][0]=0;//players have no rows after reseting
        for(int j=0; j<4; j++){//need to clear index 3 as well
            Row[i][j]=0;
        }
        RowFlag[i]=0;
    }
    *RowsTaken=0;
}

void sorting(int player[][10]){
    int temp=0;
    for(int i=0; i<num; i++){
        for(int next=((num==3)?3:2); next<=7; next++){
            temp = player[i][next];
            int moveItem = next;
            while((moveItem>((num==3)?2:1))&&(player[i][moveItem-1]<temp)){
                player[i][moveItem] = player[i][moveItem-1];
                moveItem--;
            }
            if(temp>6)  temp=6;
            player[i][moveItem] = temp;
        }
    }
}

int winner(int total[]){
    int winner = 0;//the winner's index
    for(int i=1; i<num; i++){
        if(total[i]>total[winner]){
            winner = i;
        }
    }
    return winner;
}

