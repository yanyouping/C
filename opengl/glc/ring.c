#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <Windows.h>

__declspec(align(64)) struct ring {
	volatile long nW, nR;
	volatile int *buf;
};

int Write(struct ring *_r, int val) {
	int k = _r->nW - _r->nR;
	if (k == -1 || k == 15) {
		return -1;
	}

	*(_r->buf + _r->nW) = val;

	_r->nW++;
	if (_r->nW > 15)
		_r->nW = 0;

	return 0;
}

int Read(struct ring *_r, int *val) {
	int k = _r->nW - _r->nR;
	if (k == 1 || k == -15) {
		return -1;
	}
	_r->nR++;
	if (_r->nR > 15)
		_r->nR = 0;

	*val = *(_r->buf + _r->nR);


	return 0;
}


void __cdecl _rthread(void *p) {
	struct ring *k = (struct ring*)p;
	int r, l = -1, x = 0;
	while (1) {
		if (Read(k, &r) < 0) {
			continue;
		}
		if (r - l > 1) {
			break;
		}

		l = r;
		printf("%8.8d,", r);
		if (++x > 9) {
			x = 0;
			printf("\r\n");
		}
	}
}

void __cdecl _wthread(void *p) {
	struct ring *k = (struct ring*)p;
	int r = 0;
	while (1) {
		if (Write(k, r) < 0) {
			continue;
		}
		r++;
	}
}

int main()
{
	struct ring *k = (struct ring*)malloc(sizeof(struct ring));
	k->buf = (int*)malloc(sizeof(int) * 16);
	k->nR = 15;
	k->nW = 0;

	uintptr_t hdl1 = _beginthread(_rthread, 0, k);
	uintptr_t hdl2 = _beginthread(_wthread, 0, k);

	//int i;
	//for (i = 0; i < 2048*1024; ) {
	//	if (Write(k, i) == 0)
	//		i++;
	//}

	getc(stdin);
	return 0;
}

