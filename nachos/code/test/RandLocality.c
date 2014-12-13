/*
 * RandLocality.c
 *
 * A simple program that randomly references pages.
 */

int Exec(char *, int, char **, int);
int Exit(int);

int array[50][50];

unsigned int
irand(int l, int h)
{
  unsigned int a = 1588635695, m = 429496U, q = 2, r = 1117695901;
  unsigned int val;
  unsigned int seed = 93186752;

  seed = a*(seed % q) - r*(seed / q);
  val = (seed / m) % (h - l) + l;
 
  return val;
}


int
main ()
{
    int sum = 0;
    
    int i, j, k, r, x;
    
    
    for(i = 0; i < 50; i++){
        for(j = 0; j < 50; j++)
            array[i][j] = i + j;
    }

    for (r = 0; r < 1000; r++){
        for (k = 0; k < 10; k++) {
            x = irand(0, 50);
	    sum += array[x][x];
        }
    }



    Exit(sum);

}
