
#include "syscall.h"

int
main(){
        
    int i, j;
    int array[50][50];
    int sum1 = 0;
    int sum2 = 0;
    int sum3 = 0;
 

    for(i = 0; i < 50; i++){
        for(j = 0; j < 50; j++)
            array[i][j] = i + j;
    }
    
    for(i = 0; i < 3; i++){
        for(j = 0; j < 2; j++){
            sum1 = sum1 + array[i][j];
        }
    }
    
    for(i = 0; i < 50; i = i + 10){
        sum2 = sum2 + array[i][j];
    }
    
    for(i = 0; i < 50; i = i + 10){
        for(j = 0; j < 50; j = j + 5){
            sum3 = sum3 + array[i][j];
        }
    }

    Exit(sum1);
 
}     
