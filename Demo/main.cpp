#include <iostream>
#include "color.h"

using namespace std;
using namespace Petter;

int pow(int,int);

struct Board
{
    static const int ROW = 30;
    static const int COL = 30;
    char board[ROW][COL];

};

int main()
{

    Board aboard;

    cout << RED << "Hello " << YELLOW << "There!!\n" << NORMAL;
    int x = 2;
    int p = 0;
    cout << pow(x,p) << endl;
/*
    char * fname = new char[5];
    fname = "bobby the third";
    delete fname;
    fname = new char[35];
*/
    int bob = atoi("10bob");

    //cout << bob << endl;

    return 0;
}
int pow(int x,int p)
{
    if(p == 0)
        return 1;
    else
        return x * pow(x, p-1);
}
