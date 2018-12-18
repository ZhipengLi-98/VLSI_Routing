#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <cmath>


#define random(x) (rand()%x)

using namespace std;

int main()
{

srand((int)time(0));

	int n = 15;
	int m = 15;
	int node = 10;
	int net = 3;
	int x[1000];
	int y[1000];
	int nets[100][100];
	int s = 4;

	for (int i = 0; i < node; i++)
	{
		x[i] = random(n);
		y[i] = random(m);
		for (int j = 0; j < i; j++)
		{
			if ((x[i] == x[j]) && (y[i] == y[j]))
			{
				x[i] = random(n);
				y[i] = random(m);
				j = 0;
				continue;
			}
		}
	}
	for (int i = 0; i < net; i++)
	{
		int num = random(3) + s;
		nets[i][0] = num;
		for (int j = 1; j <= num; j++) 
		{
			int t = random(node);
			for (int k = 1; k < j; k++)
			{
				if (t == nets[i][k]) 
				{
					t = random(node);
					k = 1;
					continue; 
				}
			}
			nets[i][j] = t;
		}
	}

	cout << n << " " << m << " " << node << " ";
	cout << net << " ";
	for (int i = 0; i < node; i++) cout << x[i] << " " << y[i] << " ";
	for (int i = 0; i < net; i++) 
	{
		cout << nets[i][0] << " ";
		for (int j = 1; j <= nets[i][0]; j++)
			cout << nets[i][j] << " ";
	}

	cout << "     " << endl;

	return 0;
}