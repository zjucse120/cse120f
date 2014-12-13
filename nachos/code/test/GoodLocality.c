/*
 * GoodLocality.c
 *
 * A simple program that generates good locality.
 */

int Exec(char *, int, char **, int);
int Exit(int);

int array[50][50];

int
main ()
{
    int sum = 0;
    
    int i, j, k, r;
    
    
    for(i = 0; i < 50; i++){
        for(j = 0; j < 50; j++)
            array[i][j] = i + j;
    }

    for (r = 0; r < 1000; r++){
        for (k = 0; k < 10; k++) {
	    sum += array[0][1];
        }
    }



    Exit(sum);

}
