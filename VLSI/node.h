#pragma once

typedef enum state {
	Source, Block, Target, Init
};

class node {
public:
	node () {
		s = Init;
		flag[0] = false;
		flag[1] = false;
		flag[2] = false;
		flag[3] = false;
	}

	state s;
	bool visit;
	bool flag[4];
};

class point {
public:
	point () {
		x = 0;
		y = 0;
	}
	point(int _x, int _y) {
		x = _x;
		y = _y;
	}
	const point operator+ (const point& p) {
		return point(this->x + p.x, this->y + p.y);
	}

	int x;
	int y;
};