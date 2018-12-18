#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <opencv2\opencv.hpp>
#include <cmath>

#include "node.h"

#define random(x) (rand()%x)

using namespace cv;

class node;
class point;
class edge;

std::string src = "D:\\Virtualbox\\VLSI_Routing\\debug\\";
std::string fileName = "test.txt";
std::string workFile = "test1.txt";
int dxy = 50;
int br = 5;
int brl = 1;

int * listX;
int * listY;
int ** netsin;

int row, col;
int netNum;
int nodeNum;

std::vector<point> nodes;
std::vector<int> nets;
std::vector<std::vector<edge>> edges;
int * regbest;

using namespace std;


int minval;
std::vector<std::vector<edge>> edgesbest;

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
			//std::cout << "No Ans" << std::endl;
			return point(-1, -1);
		}
		label.clear();
		label = temp;
	}
	return point(ansx, ansy);
}

int work(std::string fileName) {
	
	/*
	if (argc < 2) {
		std::cout << "Usage ./Maze_Router <parameters.txt> " << std::endl;
		system("pause");
		return -1;
	}
	std::string fileName = argv[1];
	*/
	std::fstream file(fileName, std::ios_base::in);
	
	file >> row;
	file >> col;
	file >> nodeNum;
	file >> netNum;

	std::vector<node> map(row * col);

	int rst = 0;

	for (int i = 0; i < nodeNum; i++) {
		int tx, ty;
		file >> tx;
		file >> ty;
		nodes.push_back(point(tx, ty));
		//std::cout << "point " << i << " : " << tx << " " << ty << std::endl;
		map[tx*col + ty].s = Block;
	}
	//std::cout << std::endl;
	for (int i = 0; i < netNum; i++) {
		int size;
		file >> size;
		nets.clear();
		for (int j = 0; j < size; j++) {
			int id;
			file >> id;
			nets.push_back(id);
			//std::cout << "net " << i << " : " << id << std::endl;
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
		std::vector<edge> tempEdge;
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
				file.close();
				return -1;
			}
			int size = path.size();
			map[closet.x*col + closet.y].s = Source;
			bool sourceReach = false;
//			std::vector<edge> tempEdge;
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
				//std::cout << "edge: " << tempEdge[k].start.x << " " << tempEdge[k].start.y << " " << tempEdge[k].end.x << " " << tempEdge[k].end.y << std::endl;
			}
//			edges.push_back(tempEdge);

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
		edges.push_back(tempEdge);
		for (int j = 0; j < nodes.size(); j++) {
			map[nodes[j].x*col + nodes[j].y].s = Block;
		}
		for (int j = 0; j < P.size(); j++) {
			map[P[j].x*col + P[j].y].s = Block;
			//std::cout << "P: " << P[j].x << " " << P[j].y << std::endl;
		}
		//std::cout << "net: " << i << ", length: " << P.size() - 1 << std::endl;
		rst += P.size() - 1;
		//std::cout << "====" << std::endl;
	}
	//system("pause");
	//std::cout << "===============" << std::endl;
	file.close();
	return rst;
}

void init() {
	std::fstream file(src + fileName, std::ios_base::in);
	file >> row;
	file >> col;
	file >> nodeNum;
	file >> netNum;
	listX = new int[nodeNum];
	listY = new int[nodeNum];
	netsin = new int *[netNum];

	for (int i = 0; i < nodeNum; i++) {
		int tx, ty;
		file >> listX[i];
		file >> listY[i];
	}
	for (int i = 0; i < netNum; i++) {
		int size;
		file >> size;
		netsin[i] = new int[size + 1];
		netsin[i][0] = size;
		for (int j = 0; j < size; j++) file >> netsin[i][j + 1];
	}
	file.close();
}

void creatfile(int * reg) {
	std::fstream fileout(src + workFile, std::ios_base::out);
	fileout << row << " ";
	fileout << col << " ";
	fileout << nodeNum << " ";
	fileout << netNum << " ";
	for (int i = 0; i < nodeNum; i++) {
		fileout << listX[i] << " ";
		fileout << listY[i] << " ";
	}
	for (int i = 0; i < netNum; i++) {
		fileout << netsin[reg[i]][0] << " ";
		for (int j = 0; j < netsin[reg[i]][0]; j++) fileout << netsin[reg[i]][j + 1] << " ";
	}
	fileout.close();
}

void drawnode(int x, int y, Mat& picture) {
	rectangle(picture, cvPoint(dxy * (x + 1) - br, dxy * (y + 1) - br), cvPoint(dxy * (x + 1) + br, dxy * (y + 1) + br), Scalar(255, 0, 0), -1, 1, 0);
}

void drawline(int x1, int y1, int x2, int y2, const Scalar& color, Mat& picture) {
	if (x1 == x2) {
		if (y2 < y1) {
			int temp = y2;
			y2 = y1;
			y1 = temp;
		}
		rectangle(picture, cvPoint(dxy * (x1 + 1) - brl, dxy * (y1 + 1) + br + 1), cvPoint(dxy * (x1 + 1) + brl, dxy * (y2 + 1) - br - 1), color, -1, 1, 0);
	}
	else {
		if (x2 < x1) {
			int temp = x2;
			x2 = x1;
			x1 = temp;
		}
		rectangle(picture, cvPoint(dxy * (x1 + 1) + br + 1, dxy * (y1 + 1) - brl), cvPoint(dxy * (x2 + 1) - br - 1, dxy * (y2 + 1) + brl), color, -1, 1, 0);
	}
}

int main(int argc, char *argv[]) {
	//std::vector<std::vector<edge>> edges;
	srand((int)time(0));
	init();
	
	int * reg;
	regbest = new int[netNum];
	reg = new int[netNum];

	for (int i = 0; i < netNum; i++) reg[i] = i;
	for (int i = 0; i < 100 * netNum; i++)
	{
		int ix = random(netNum);
		int iy = random(netNum);
		if (ix == iy) continue;
		int temp = reg[ix];
		reg[ix] = reg[iy];
		reg[iy] = temp;
	}

	minval = 2147483647;
	int lastminval = minval;
	int counter = 0;

	double runtime = 100;
	//std::cin >> runtime;
	double dur;
	clock_t start, end;
	start = clock();
	end = clock();
	dur = (double)(end - start);

	double alpha = 0.95;
	int R;
	int temp_acp = 100;
	double t = 5.0 * (row + col) * netNum * nodeNum;
	int defmaxf = (row + col)  * nodeNum;

	while ((dur / CLOCKS_PER_SEC) < runtime) {

		std::cout << "tempture = " << t << std::endl;

		//停止条件
		if (lastminval == minval) {
			if (counter > 10) {
				break;
			}
			counter++;
		}
		else {
			counter = 0;
			lastminval = minval;
		}

		int fi;
		creatfile(reg);
		//cout << "beforeget" << endl;
		fi = work(src + workFile);
		//cout << "get " << fi << endl;
		if ((fi > 0) && (fi < minval)) {
			minval = fi;
			/*
			cout << minval << endl;
			for (int set = 0; set < netNum; set++) {
				std::cout << "set = " << set << std::endl;
				for (int k = 0; k < edges[set].size(); k++) {
					std::cout << "edge: " << edges[set][k].start.x << " " << edges[set][k].start.y << " " << edges[set][k].end.x << " " << edges[set][k].end.y << std::endl;
				}
				//std::cout << std::endl;
			}
			std::cout << std::endl;
			*/
			//cout << "ch1" << endl;
			edgesbest = edges;
			for (int i = 0; i < netNum; i++) regbest[i] = reg[i];
			//system("pause");
			//return 0;
		}
		//cout << "ch2" << endl;
		if (fi < 0) fi = defmaxf;
		//nets.clear();
		//nodes.clear();
		//edges.clear();
		vector<point> eptnodes;
		vector<int> eptnets;
		vector<vector<edge>> eptedges;
		nodes.swap(eptnodes);
		edges.swap(eptedges);
		nets.swap(eptnets);
		//cout << "ch3" << endl;

		R = 0;
		while (true)
		{
			//平衡条件
			if (R > temp_acp) {
				break;
			}

			//选择解
			int ix = random(netNum);
			int iy = random(netNum);
			//cout << netNum << endl;
			//cout << ix << "," << iy << endl;
			while (ix == iy)
			{
				ix = random(netNum);
				//cout << ix << "," << iy << endl;
			}
			int * regj = new int[netNum];
			for (int i = 0; i < netNum; i++) regj[i] = reg[i];
			int temp = regj[ix];
			regj[ix] = regj[iy];
			regj[iy] = temp;

			//cout << "ch4" << endl;

			//计算f
			int fj;
			creatfile(regj);
			//cout << "beforeget" << endl;
			fj = work(src + workFile);
			//cout << "get " << fi << endl;
			if ((fj > 0) && (fj < minval)) {
				minval = fj;
				/*
				cout << minval << endl;
				for (int set = 0; set < netNum; set++) {
					std::cout << "set = " << set << std::endl;
					for (int k = 0; k < edges[set].size(); k++) {
						std::cout << "edge: " << edges[set][k].start.x << " " << edges[set][k].start.y << " " << edges[set][k].end.x << " " << edges[set][k].end.y << std::endl;
					}
					//std::cout << std::endl;
				}
				std::cout << std::endl;
				*/
				for (int i = 0; i < netNum; i++) regbest[i] = regj[i];
				edgesbest = edges;
				//system("pause");
				//return 0;
			}
			if (fj < 0) fj = defmaxf;
			//nets.clear();
			//nodes.clear();
			//edges.clear();
			vector<point> eptnodes;
			vector<int> eptnets;
			vector<vector<edge>> eptedges;
			nodes.swap(eptnodes);
			edges.swap(eptedges);
			nets.swap(eptnets);

			//接受判断
			if (fj < fi) {
				fi = fj;
				temp = reg[ix];
				reg[ix] = reg[iy];
				reg[iy] = temp;
				R++;
			}
			else {
				double p = exp((-1.0 * (fj - fi)) / t);
				double ran = (1.0 * random(100001)) / 100000;
				if (ran < p) {
					fi = fj;
					temp = reg[ix];
					reg[ix] = reg[iy];
					reg[iy] = temp;
					R++;
				}
			}

			R++;
		};

		//温度下降
		t = t * alpha;

		end = clock();
		dur = (double)(end - start);
	}

	//std::cout << minval << std::endl;
	if (minval == 2147483647) {
		system("pause");
		return 0;
	}

	Mat picture((row + 2) * dxy, (col + 2) * dxy, CV_8UC3, Scalar(254, 254, 254));
	for (int i = 0; i < nodeNum; i++)
	{
		drawnode(listX[i], listY[i], picture);
	}
	//drawline(1, 1, 2, 1, picture);
	for (int set = 0; set < netNum; set++) {
		int r = random(255);
		int g = random(255);
		int b = random(255);
		std::cout << "set = " << set << std::endl;
		for (int k = 0; k < edgesbest[set].size(); k++) {
			std::cout << "edge: " << edgesbest[set][k].start.x << " " << edgesbest[set][k].start.y << " " << edgesbest[set][k].end.x << " " << edgesbest[set][k].end.y << std::endl;
			drawline(edgesbest[set][k].start.x, edgesbest[set][k].start.y, edgesbest[set][k].end.x, edgesbest[set][k].end.y, Scalar(b, g, r), picture);
		}
		std::cout << std::endl;
	}

	for (int i = 0; i < netNum; i++) cout << regbest[i] << " ";
	std::cout << endl;
	std::cout << minval << std::endl;

	imshow("line", picture);
	waitKey(-1);

	/*
	for (int i = 0; i < netNum; i++) {
		for (int j = 0; j < netNum; j++) {
			if (i == j) continue;
			for (int k = 0; k < netNum; k++) {
				if (i == k) continue;
				if (j == k) continue;
				reg[0] = i;
				reg[1] = j;
				reg[2] = k;
				creatfile(reg);
				//system("pause");
				std::cout << "que : " << i << " " << j << " " << k << " ans = ";
				int rst = work(src + workFile);
				std::cout << rst << std::endl;

				if ((rst > 0) && (rst < minval)) {
					minval = rst;
					edgesbest = edges;
				}
				nets.clear();
				nodes.clear();
				edges.clear();
			}
		}
	}
	*/

	system("pause");
	
	return 0;
}