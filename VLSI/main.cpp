#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

#include "node.h"

class node;
class point;
class edge;

int row, col;
int netNum;
int nodeNum;
std::vector<point> nodes;
std::vector<int> nets;
std::vector<std::vector<edge>> edges;

bool edgeValid(point a, point b) {
	for (int i = 0; i < edges.size(); i++) {
		for (int j = 0; j < edges[i].size(); j++) {
			if ((edges[i][j].start == a && edges[i][j].end == b) || (edges[i][j].start == b && edges[i][j].end == a)) {
				return false;
			}
		}
	}
	// std::cout << "edgeValid: " << a.x << " " << a.y << " " << b.x << " " << b.y << std::endl;
	return true;
}

point bfs(std::vector<point> S, std::vector<point> P, std::vector<node> &map) {
	bool TargetReached = false;
	int ansx = 0, ansy = 0;
	std::vector<point> label = S;
	while (!TargetReached) {
		std::vector<point> temp;
		for (int h = 0; h < label.size(); h++) {
			point p = label[h];
			for (int i = p.x - 1; i <= p.x + 1; i++) {
				for (int j = p.y - 1; j <= p.y + 1; j++) {
					if ((i >= 0) && (i < row) && (j >= 0) && (j < col) &&
						(abs(i - p.x) * abs(j - p.y) == 0)) {
						// std::cout << "bfs: " << i << " " << j << std::endl;
						// std::cout << map[i * col + j].s << std::endl;
						if (map[i * col + j].s == Target && edgeValid(point(i, j), p)) {
							TargetReached = true;
							ansx = i;
							ansy = j;
							if ((i - p.x) == -1) 
								map[i * col + j].flag[3] = true; // 左
							if ((j - p.y) == -1)
								map[i * col + j].flag[2] = true; // 下
							if ((j - p.y) == 1) 
								map[i * col + j].flag[1] = true; // 上
							if ((i - p.x) == 1) 
								map[i * col + j].flag[0] = true; // 右
							break;
						}
						else if (!map[i * col + j].visit && map[i * col + j].s != Block && edgeValid(point(i, j), p)) {
							temp.push_back(point(i, j));
							if ((i - p.x) == -1) 
								map[i * col + j].flag[3] = true; // 左
							if ((j - p.y) == -1) 
								map[i * col + j].flag[2] = true; // 下
							if ((j - p.y) == 1) 
								map[i * col + j].flag[1] = true; // 上
							if ((i - p.x) == 1) 
								map[i * col + j].flag[0] = true; // 右
							map[i * col + j].visit = true;
						}
					}
				}
			}
			if (TargetReached) 
				break;
		}
		if (label.size() == 0) {
			std::cout << "No Ans" << std::endl;
			return point(-1, -1);
		}
		label.clear();
		label = temp;
	}
	return point(ansx, ansy);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage ./Maze_Router <parameters.txt> " << std::endl;
		system("pause");
		return -1;
	}
	std::string fileName = argv[1];
	std::fstream file(fileName, std::ios_base::in);

	file >> row;
	file >> col;
	file >> nodeNum;
	file >> netNum;
	std::vector<node> map(row * col);

	for (int i = 0; i < nodeNum; i++) {
		int tx, ty;
		file >> tx;
		file >> ty;
		nodes.push_back(point(tx, ty));
		std::cout << "point " << i << " : " << tx << " " << ty << std::endl;
		map[tx*col + ty].s = Block;
	}
	std::cout << std::endl;
	for (int i = 0; i < netNum; i++) {
		int size;
		file >> size;
		nets.clear();
		for (int j = 0; j < size; j++) {
			int id;
			file >> id;
			nets.push_back(id);
			std::cout << "net " << i << " : " << id << std::endl;
		}
		std::vector<point> P;
		std::vector<point> T;
		std::vector<point> S;
		point start = nodes[nets[0]];
		map[start.x*col + start.y].s = Source;
		S.push_back(start);
		P.push_back(start);
		for (int j = 1; j < size; j++) {
			T.push_back(nodes[nets[j]]);
		}
		for (int j = 0; j < T.size(); j++) {
			P.push_back(T[j]);
			map[T[j].x * col + T[j].y].s = Target;
		}

		for (int j = 0; j < T.size(); j++) {
			for (int k = 0; k < map.size(); k++) {
				map[k].visit = false;
				map[k].flag[0] = false;
				map[k].flag[1] = false;
				map[k].flag[2] = false;
				map[k].flag[3] = false;
			}

			std::vector<point> path;
			point closet = bfs(S, P, map);
			if (closet.x == -1 && closet.y == -1) {
				return -1;
			}
			int size = path.size();
			map[closet.x*col + closet.y].s = Source;
			bool sourceReach = false;
			std::vector<edge> tempEdge;
			while (!sourceReach) {
				S.push_back(closet);
				// std::cout << closet.x << " " << closet.y << std::endl;
				point tempPoint = closet;
				// std::cout << "target: " << closet.x << " " << closet.y << std::endl;
				if (map[closet.x*col + closet.y].flag[0]) {
					closet = closet + point(-1, 0);
				}
				else if (map[closet.x*col + closet.y].flag[1]) {
					closet = closet + point(0, -1);
				}
				else if (map[closet.x*col + closet.y].flag[2]) {
					closet = closet + point(0, 1);
				}
				else if (map[closet.x*col + closet.y].flag[3]) {
					closet = closet + point(1, 0);
				}
				if (map[closet.x*col + closet.y].s == Source) {
					sourceReach = true;
				}
				tempEdge.push_back(edge(tempPoint, closet));
				// edges.push_back(edge(tempPoint, closet));
				path.push_back(closet);
			}
			for (int k = 0; k < tempEdge.size(); k++) {
				std::cout << "edge: " << tempEdge[k].start.x << " " << tempEdge[k].start.y << " " << tempEdge[k].end.x << " " << tempEdge[k].end.y << std::endl;
			}
			edges.push_back(tempEdge);
			
			for (int k = 0; k < path.size() - 1; k++) {
				// std::cout << "path: " << path[k].x << " " << path[k].y << std::endl;
				P.push_back(path[k]);
				map[path[k].x * col + path[k].y].s = Source;
				// edges.push_back(edge(path[k], path[k + 1]));
			}
			/*
			for (int k = 0; k < edges.size(); k++) {
				std::cout << "edge: " << edges[k].start.x << " " << edges[k].start.y << " " << edges[k].end.x << " " << edges[k].end.y << std::endl;
			}
			std::cout << std::endl;
			*/
		}
		for (int j = 0; j < nodes.size(); j++) {
			map[nodes[j].x*col + nodes[j].y].s = Block;
		}
		for (int j = 0; j < P.size(); j++) {
			map[P[j].x*col + P[j].y].s = Block;
			std::cout << "P: " << P[j].x << " " << P[j].y << std::endl;
		}
		std::cout << "net: " << i << ", length: " << P.size() - 1 << std::endl;
		std::cout << std::endl;
	}
	system("pause");
	return 0;
}