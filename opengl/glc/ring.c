// Win32Project1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <Windows.h>

struct ring {
	volatile long nW, nS;
	int *buf;
};

int Write(struct ring *_r, int val) {

	int k = _r->nS;
	if (k < 1) {
		return -1;
	}

	*(_r->buf + _r->nW) = val;
	_r->nW++;
	_r->nS--;
	if (_r->nW > 15)
		_r->nW = 0;

	return 0;
}

int Read(struct ring *_r, int *val) {
	int k = _r->nS;
	if (k == 16) {
		return -1;
	}

	int nR = _r->nW - 16 + k;
	if (nR < 0)
		nR += 16;

	*val = *(_r->buf + nR);
	_r->nS++;
	return 0;
}


void __cdecl _thread(void *p) {
	struct ring *k = (struct ring*)p;
	int r;

	while (1) {
		if (Read(k, &r) < 0) {
			continue;
		}

		printf("%d\t", r);
	}
}


int main()
{
	struct ring *k = (struct ring*)malloc(sizeof(struct ring));
	k->buf = (int*)malloc(sizeof(int) * 16);
	k->nS = 16;
	k->nW = 0;

	_beginthread(_thread, 0, k);

	int i;
	for (i = 0; i < 2048; ) {
		if (Write(k, i) == 0)
			i++;

		Sleep(1);
	}

	system("pause");
	return 0;
}

