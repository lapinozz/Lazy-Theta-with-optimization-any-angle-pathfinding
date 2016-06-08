#include <iostream>

#include "tileadaptor.hpp"

int main()
{
    constexpr int mapSizeX = 70;
    constexpr int mapSizeY = 20;

    //Normaly would have used a std::vector of size x*y but just for this test it's
    //Gonna be way easier and readeable to do it this way
    std::array<std::array<char, mapSizeY>, mapSizeX> map;

    Vectori startPoint = {1, 1};
    Vectori endPoint = {mapSizeX - 2, mapSizeY - 2};

    auto makeWall = [&map](const Vectori& pos, const Vectori& size)
    {
        for(int x = 0; x < size.x; x++)
        {
            for(int y = 0; y < size.y; y++)
            {
                map[pos.x + x][pos.y + y] = '#';
            }
        }
    };

    //Instantiating our path adaptor
    //passing the map size and a lambda that return false if the tile is a wall
    TileAdaptor adaptor({mapSizeX, mapSizeY}, [&map](const Vectori& vec){return map[vec.x][vec.y] != '#';});
    //This is a bit of an exageration here for the weight, but it did make my performance test go from 8s to 2s
    Pathfinder pathfinder(adaptor, 100.f /*weight*/);

    //set everythings to space
    for(auto& cs : map)
        for(auto& c : cs)
            c = ' ';

    //borders
    makeWall({0, 0}, {mapSizeX, 1});
    makeWall({0, 0}, {1, mapSizeY});
    makeWall({0, mapSizeY - 1}, {mapSizeX, 1});
    makeWall({mapSizeX - 1, 0}, {1, mapSizeY});

    //walls
    makeWall({5, 0}, {1, mapSizeY - 6});
    makeWall({mapSizeX - 6, 5}, {1, mapSizeY - 6});

    makeWall({mapSizeX - 6, 5}, {4, 1});
    makeWall({mapSizeX - 4, 8}, {4, 1});

    makeWall({20, 0}, {1, mapSizeY - 4});
    makeWall({mapSizeX - 20, 5}, {14, 1});

    //start and end point
    map[startPoint.x][startPoint.y] = 'S';
    map[endPoint.x][endPoint.y] = 'E';

    //The map was edited so we need to regenerate teh neighbors
    pathfinder.generateNodes();

    //doing the search
    //merly to show the point of how it work
    //as it would have been way easier to simply transform the vector to id and pass it to search
    auto nodePath = pathfinder.search(adaptor.posToId(startPoint), adaptor.posToId(endPoint));

    //Convert Ids onto map position
    std::vector<Vectori> path;
    path.reserve(nodePath.size());

    for(const auto id : nodePath)
        path.push_back(idToData(id));


    //There is also a serach function that do the conversion between your data type
    //And the id, it take lambdas to convert between the two
    //Here's an exemple of how it would be called for this code
//    auto path = pathfinder.search(startPoint, endPoint,
//                                        {
//                                            [&adaptor](const auto id)
//                                            {
//                                                return adaptor.idToPos(id);
//                                            }
//                                        },
//                                        {
//                                            [&adaptor](const auto& data)
//                                            {
//                                                return adaptor.posToId(data);
//                                            }
//                                        });


    //If we found a path we just want to remove the first and last node
    //Because it will be at our start and end position
    if(path.size())
    {
        path.pop_back();
        path.erase(path.begin());
    }

    //nodes
    int x = 0;
    for(const auto& node : path)
        map[node.x][node.y] = '0' + x++;

    //draw map
    for(int y = 0; y < map[0].size(); y++)
    {
        for(int x = 0; x < map.size(); x++)
            std::cout << map[x][y];

        std::cout << std::endl;
    }

    std::cout << "#  = walls" << std::endl;
    std::cout << "S  = start" << std::endl;
    std::cout << "E  = end" << std::endl;
    std::cout << "number = path nodes" << std::endl;

    return 0;
}
