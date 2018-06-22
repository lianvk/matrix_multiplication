//#include <stdlib.h>
#include <chrono>
#include <iostream>
#define N 1000

void main()
{
	typedef float elemtype;
	static elemtype a[N][N];
	static elemtype b[N][N];
	static elemtype c[N][N];
	for (int j1 = 0; j1 < N; j1++)
	{
		for (int j2 = 0; j2 < N; j2++)
		{
			a[j1][j2] = elemtype(rand());
			b[j1][j2] = elemtype(rand());
			c[j1][j2] = 0;
		}
	}
	std::cout << "Multiplication started" << std::endl;
	auto startTime = std::chrono::system_clock::now();
	for (int j1 = 0; j1 < N; j1++)
	{
		for (int j2 = 0; j2 < N; j2++)
		{
			for (int j3 = 0; j3 < N; j3++)
			{
				c[j1][j2] += a[j1][j3] * b[j3][j2];
			}
		}
	}
	std::chrono::duration<double> delay = std::chrono::system_clock::now() - startTime;
	std::cout << "Multiplication ended: delay = " << delay.count() << std::endl;
}