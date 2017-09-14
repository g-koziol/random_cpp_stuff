/**
    Input data : File with the configuration of matrix of dimensions MxN with integers representing some resources.
    Task:
    - Starting at [0,0] and ending in [M-1 x N-1]
    - Jump from one value to other only by one index in direction:
        - right
        - down
    - At each jump add to actual value of resources value from given element in matrix
    - If actual number of resources is <= 0 - task is perceived as failed

    Problem : Found the minimum value of needed resources on start to acomplish task of not letting to decrease actual number of resources to 0 while jumping to end of matrix.

    Program loads file at default path "island.conf" if not given in command line arguments.
    File stores configuration of islands' resources.
    [Exmaple]
    0,-2,-3,1,5,-3
    -1,4,8,-25,5,-1
    1,-1,-20, 10,5,-1
    1,-1,-20, 10,5,-1
    1,-1,-20, 10,5,-1
    [Example end]
    Additional requirements:
    - Compile with min c++11
*/

#ifdef __cplusplus
#if __cplusplus < 201103L
#error Required min c++ 11
#endif
#endif

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string.h>


// used to print all paths during execution
// disabled by default - big input data needs a lot of memory to store descriptions of paths
// #define _PRINT_PATH_



using namespace std;
using RescourcesRow = vector<int>;
using RescourcesMap = vector<RescourcesRow>;

/// Represents the cordinates of element in matrix
struct POS
{
    size_t x;
    size_t y;
};
/// Collection of cordinates for one path
using Path = vector<POS>;
/// Collection of all created paths in runtime
using Paths = vector<Path>;
/// Define global object of paths
Paths paths;

/// Number of paths created
int numberOfCreatedPaths = 0;
/// Used to store the answer to the problem
int minNeededResources = 0;

/// Print readed configuration from file
void printCreatedIslands(const RescourcesMap &map)
{
    auto it = map.begin();
    auto itEnd = map.end();
    for (; it != itEnd; it++)
    {
        auto it2 = it->begin();
        auto it2End = it->end();
        for (; it2 != it2End; it2++)
            cout << setw(3) << *it2 << " ";
        cout << endl;
    }
}

/// Config file parser
int createResourcesMapingFromFile(RescourcesMap &output, string fileName = "island.conf")
{
    fstream fs;
    fs.open(fileName.c_str(), ios_base::in);
    if (fs.is_open())
    {
        // read file while not end
        while (fs.is_open() && !fs.eof())
        {
            char buf[100];
            fs.getline(buf, 99);
            if (fs.eof())
                break;
            vector<int> rowResources;
            istringstream iss(buf);
            while (iss.getline(buf, 100 - 1, ','))
            {
                if (strlen(buf))
                    rowResources.push_back(atoi(buf));
            }
            output.push_back(rowResources);
        }
        fs.close();
    }
    printCreatedIslands(output);
    return 0;
}

/// Main action recursive function
int calculateNeededResources(const RescourcesMap &map, size_t x, size_t y, int needOnStartCost, const size_t maxY, const size_t maxX, Path &path)
{

#ifdef _PRINT_PATH_
    path.push_back({x, y});
#endif
    // if not start of execution
    if (!(x == 0 && y == 0))
    {
        // if end of matrix
        if (x == maxX && y == maxY)
        {
#ifdef _PRINT_PATH_
            cout << "Path: ";
            for (auto it = path.begin(); it != path.end(); it++)
            {
                cout << "[" << it->y << "," << it->x << "] ";
                cout << "[" << y << "," << x << "] ";
            }
            cout << " Calulated requirment: " << needOnStartCost << endl;
            paths.push_back(path);
#endif

            // set first value of the global variable
            if (minNeededResources)
                minNeededResources = min(minNeededResources, needOnStartCost);
            else
            {
                minNeededResources = needOnStartCost;
            }
            //increase number of paths analyzed
            ++numberOfCreatedPaths;
            return needOnStartCost;
        }
    }

    // update answer basing on next value from matrix
    if ((needOnStartCost + map[y][x]) <= 0)
    {
        needOnStartCost += -(map[y][x]);
        if ((needOnStartCost + map[y][x]) == 0)
            needOnStartCost += 1;
    }

    // boundaries cases

    // we reached the 'right' bound of matrix - we can only go 'down'
    if (x == maxX)
    {
        Path new_path = path;
        int newNeedOnStart = calculateNeededResources(map, x, ++y, needOnStartCost, maxY, maxX, new_path);

        if (newNeedOnStart < needOnStartCost)
        {
            path = new_path;
            return newNeedOnStart;
        }
        else
            return needOnStartCost;
    }
    // we reached the 'bottom' bound of matrix - we can only go 'right'
    if (y == maxY)
    {
        Path new_path = path;
        int newNeedOnStart = calculateNeededResources(map, ++x, y, needOnStartCost, maxY, maxX, new_path);

        if (newNeedOnStart < needOnStartCost)
        {
            // update the new better answer
            path = new_path;
            return newNeedOnStart;
        }
        else
            return needOnStartCost;
    }

    // general case
    Path new_right = path;
    Path new_down = path;
    int newNeedOnStartRight = calculateNeededResources(map, x + 1, y, needOnStartCost, maxY, maxX, new_right);
    int newNeedOnStartDown = calculateNeededResources(map, x, y + 1, needOnStartCost, maxY, maxX, new_down);
    if (needOnStartCost < newNeedOnStartRight && needOnStartCost < newNeedOnStartDown)
    {
        return needOnStartCost;
    }
    else
    {
        // update the new better answer
        if (newNeedOnStartDown < newNeedOnStartRight)
        {
            path = new_down;
            return newNeedOnStartDown;
        }
        else
        {
            path = new_right;
            return newNeedOnStartRight;
        }
    }
}

int main(int argc, char *argv[])
{
    RescourcesMap map;
    // read input data
    if (argc == 2)
        createResourcesMapingFromFile(map, argv[1]);
    else
        createResourcesMapingFromFile(map);

    size_t maxX;
    size_t maxY;
    //basic checks
    if (map.size())
    {
        maxY = map.size() - 1;
        if (map[0].size())
        {
            maxX = map[0].size() - 1;

#ifdef _PRINT_PATH_
            cout << "All paths: " << endl;
#endif
            Path path;
            // start algorithm
            calculateNeededResources(map, 0, 0, 0, maxY, maxX, path);
            // some info on output
            cout << "Number of paths: " << numberOfCreatedPaths << endl;
            cout << "Min needed resources: " << minNeededResources << endl;
        }
    }
    return 0;
}
