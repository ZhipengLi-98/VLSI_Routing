#pragma once

typedef enum state {
	Source, Block, Target, Init
};

class node {
public:
	node() {
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
	point() {
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
	const bool operator== (const point& p) {
		if (p.x == this->x && p.y == this->y) {
			return true;
		}
		else {
			return false;
		}
	}

	int x;
	int y;
};

class edge {
public:
	edge() {

	}
	edge(point a, point b) {
		start = a;
		end = b;
	}

	point start;
	point end;
	bool visit;
};