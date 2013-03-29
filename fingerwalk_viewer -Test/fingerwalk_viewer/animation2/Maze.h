#ifndef _MAZE_H_
#define _MAZE_H_

#include <vector>

struct Maze_Point{
	int x, z;

	Maze_Point(int tx, int tz)
	{
		x = tx;
		z = tz;
	}
};

struct Double_Point {
	double x, z;

	Double_Point()
	{
		x = 0;
		z = 0;
	}

	Double_Point(double tx, double tz)
	{
		x = tx;
		z = tz;
	}
};

struct Neighbor_Set {
	std::vector <Maze_Point> neighbor;
};

class Maze{
private:
	//initial configuration
	int **mazeConfig;
	int width, height;
	double unitWidth, unitHeight;
	double xStart, zStart;

	//neighbor
	std::vector <std::vector <Neighbor_Set> > mazeVertice;

public:
	Maze();
	Maze(int **tmazeConfig, int tWidth, int tHeight, double tUnitWidth, double tUnitHeight,
				double tXStart, double tZStart);
	void setMaze(int **tmazeConfig, int tWidth, int tHeight, double tUnitWidth, double tUnitHeight,
				double tXStart, double tZStart);
	void setNeighbor();
	void addNeighbor(int tz, int tx);
	Maze_Point getMaze_Point1(int tz, int tx);
	Maze_Point getMaze_Point2(int tz, int tx);
	Maze_Point getMaze_Point3(int tz, int tx);
	Maze_Point getMaze_Point4(int tz, int tx);

	std::vector <Double_Point> path;
	bool isPathCleared;

	void clearpath();
	void solveMaze(double txi, double tzi, double txf, double tzf, int x_init, int z_init, int x_end, int z_end);
	void solveMaze(double txi, double tzi, int x_init, int z_init, int x_end, int z_end);
	void djikstra(int x_init, int z_init, int x_end, int z_end, std::vector <Maze_Point>& maze_solution);
};

#endif