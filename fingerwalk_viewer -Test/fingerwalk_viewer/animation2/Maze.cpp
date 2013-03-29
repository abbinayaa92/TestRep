#include "Maze.h"
#include <stdio.h>

Maze::Maze()
{
	isPathCleared = true;
}

Maze::Maze(int ** tmazeConfig, int tWidth, int tHeight, double tUnitWidth, double tUnitHeight,
				double tXStart, double tZStart)
{
	mazeConfig = tmazeConfig;
	width = tWidth;
	height = tHeight;
	unitWidth = tUnitWidth;
	unitHeight = tUnitHeight;
	xStart = tXStart;
	zStart = tZStart;

	isPathCleared = true;

	setNeighbor();
}

void Maze::setMaze(int ** tmazeConfig, int tWidth, int tHeight, double tUnitWidth, double tUnitHeight,
				double tXStart, double tZStart)
{
	mazeConfig = tmazeConfig;
	width = tWidth;
	height = tHeight;
	unitWidth = tUnitWidth;
	unitHeight = tUnitHeight;
	xStart = tXStart;
	zStart = tZStart;

	isPathCleared = true;

	setNeighbor();
}

void Maze::setNeighbor()
{
	mazeVertice.clear();
	mazeVertice.resize(height);
	for (int i=0; i<height; i++)
	{
		mazeVertice[i].resize(width);
	}

	for (int i=0; i<height; i++)
	{
		for (int j=0; j<width; j++)
		{
			addNeighbor(i, j);
		}
	}
}

void Maze::addNeighbor(int tz, int tx)
{
	switch (mazeConfig[tz][tx]) {
		case 0:
			break;
		case 1:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			break;
		case 2:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			break;
		case 3:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			break;
		case 4:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 5:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			break;
		case 6:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 7:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 8:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			break;
		case 9:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 10:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			break;
		case 11:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 12:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 13:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
			break;
		case 14:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			break;
		case 15:
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point1(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point2(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point3(tz, tx));
			mazeVertice[tz][tx].neighbor.push_back(getMaze_Point4(tz, tx));
		default:
			break;
	}
}

Maze_Point Maze::getMaze_Point1(int tz, int tx)
{
	return Maze_Point(tx, tz-1);
}

Maze_Point Maze::getMaze_Point2(int tz, int tx)
{
	return Maze_Point(tx-1, tz);
}

Maze_Point Maze::getMaze_Point3(int tz, int tx)
{
	return Maze_Point(tx, tz+1);
}

Maze_Point Maze::getMaze_Point4(int tz, int tx)
{
	return Maze_Point(tx+1, tz);
}

void Maze::solveMaze(double txi, double tzi, double txf, double tzf, int x_init, int z_init, int x_end, int z_end)
{
	if (!isPathCleared)
	{
		path.clear();
		isPathCleared = true;
	}
	
	solveMaze(txi, tzi, x_init, z_init, x_end, z_end);

	path.push_back(Double_Point(txf, tzf));

	isPathCleared = false;
}

void Maze::clearpath()
{
	path.clear();
	isPathCleared = true;
}

void Maze::solveMaze(double txi, double tzi, int x_init, int z_init, int x_end, int z_end)
{
	if (!isPathCleared)
	{
		path.clear();
		isPathCleared = true;
	}

	//path.push_back(Double_Point(txi, tzi));

	std::vector <Maze_Point> maze_solution;

	djikstra(x_init, z_init, x_end, z_end, maze_solution);

	for (int i=0; i<maze_solution.size(); i++)
	{
		path.push_back(Double_Point(xStart + (width-maze_solution[i].x-0.5)*unitWidth, zStart + (maze_solution[i].z+0.5)*unitHeight));
	}

	//for debugging
	printf ("Path obtained (point = %d):\n", path.size());
	for (int i=0; i<path.size(); i++)
		printf("%lf %lf\n", path[i].x, path[i].z);
}

void Maze::djikstra(int x_init, int z_init, int x_end, int z_end, std::vector <Maze_Point>& maze_solution)
{
	std::vector <std::vector <bool> > visited(height, std::vector <bool> (width, false));
	std::vector <std::vector <bool> > added(height, std::vector <bool> (width, false));
	std::vector <Maze_Point> next;
	std::vector <std::vector <int> > distance(height, std::vector <int> (width, 100000000));

	next.push_back(Maze_Point(x_init, z_init));
	distance[z_init][x_init] = 0;
	added[z_init][x_init] = true;
	int ct = 0; 
	while (ct<next.size())
	{
		Maze_Point current_point = next[ct];printf("Loop %d\n", ct);
		for (int i=0; i<mazeVertice[current_point.z][current_point.x].neighbor.size(); i++)
		{
			int xn = mazeVertice[current_point.z][current_point.x].neighbor[i].x;
			int zn = mazeVertice[current_point.z][current_point.x].neighbor[i].z;

			if (xn>=0 && zn>=0 && xn<width && zn<height)
			{
				if (distance[zn][xn]>distance[current_point.z][current_point.x]+1)
				{
					distance[zn][xn] = distance[current_point.z][current_point.x] + 1;
				}

				if (!added[zn][xn])
				{
					added[zn][xn] = true;
					next.push_back(Maze_Point(xn, zn));
				}
			}
		}

		ct++;
	}

	//printf("%d %d %d\n", z_end, x_end, distance[z_end][x_end]);
	//printf("%d %d %d\n", z_end-1, x_end, distance[z_end-1][x_end]);
	//printf("%d %d %d\n", z_end, x_end-1, distance[z_end][x_end-1]);
	//printf("%d %d %d\n", z_end+1, x_end, distance[z_end+1][x_end]);
	//printf("%d %d %d\n", z_end, x_end+1, distance[z_end][x_end+1]);
	std::vector <Maze_Point> tmp;
	tmp.push_back(Maze_Point(x_end, z_end));
	int tx = x_end;
	int tz = z_end;
	for (int i=distance[z_end][x_end]-1; i>=0; i--)
	{
		for (int j=0; j<mazeVertice[tz][tx].neighbor.size(); j++)
		{
			int xn = mazeVertice[tz][tx].neighbor[j].x;
			int zn = mazeVertice[tz][tx].neighbor[j].z;

			if (xn>=0 && zn>=0 && xn<width && zn<height)
			{
				if (distance[zn][xn]==i)
				{
					tmp.push_back(Maze_Point(xn, zn));
					tx = xn;
					tz = zn;
					
					//printf("%d %d %d\n", zn, xn, distance[zn][xn]);

					break;
				}
			}
		}
	}

	for (int i=tmp.size()-1; i>=0; i--)
		maze_solution.push_back(tmp[i]);

	printf("Maze distance : %d\n", distance[z_end][x_end]);	
}
	