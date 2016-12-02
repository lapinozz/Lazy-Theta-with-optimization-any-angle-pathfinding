#pragma once

#include "utility.hpp"
#include "pathfinding.hpp"

//The pathfinder is a general algorithm that can be used for mutliple purpose
//So it use adaptor
//This adaptor is for tile grid
class TileAdaptor: public Pathfinder::PathfinderAdaptor
{
public:

    using NodeId = Pathfinder::NodeId;
    using Cost = Pathfinder::Cost;

    TileAdaptor(const Vectori& mapSize, const std::function<bool(const Vectori&)>& mIsTraversable) : mMapSize(mapSize), mIsTraversable(mIsTraversable)
    {

    }

    virtual size_t getNodeCount() const override
    {
        return mMapSize.x * mMapSize.y;
    }

    //return the distance between two node
    virtual Cost distance(const NodeId n1, const NodeId n2) const override
    {
        return dist((Vectorf)idToPos(n1), (Vectorf)idToPos(n2));
    }

    //Return true if there is a direct path between n1 and n2
    //Totally not stole this code and did some heavy rewrite
    //The original code was way worse, trust me
    virtual bool lineOfSight(const NodeId n1, const NodeId n2) const override
    {
        // This line of sight check uses only integer values. First it checks whether the movement along the x or the y axis is longer and moves along the longer
        // one cell by cell. dx and dy specify how many cells to move in each direction. Suppose dx is longer and we are moving along the x axis. For each
        // cell we pass in the x direction, we increase variable f by dy, which is initially 0. When f >= dx, we move along the y axis and set f -= dx. This way,
        // after dx movements along the x axis, we also move dy moves along the y axis.

        Vectori l1 = idToPos(n1);
        Vectori l2 = idToPos(n2);

        Vectori diff = l2 - l1;

        int f = 0;
        Vectori dir; // Direction of movement. Value can be either 1 or -1.

        // The x and y locations correspond to nodes, not cells. We might need to check different surrounding cells depending on the direction we do the
        // line of sight check. The following values are used to determine which cell to check to see if it is unblocked.
        Vectori offset;

        if(diff.y < 0)
        {
            diff.y = -diff.y;
            dir.y = -1;
            offset.y = 0; // Cell is to the North
        }
        else
        {
            dir.y = 1;
            offset.y = 1; // Cell is to the South
        }

        if(diff.x < 0)
        {
            diff.x = -diff.x;
            dir.x = -1;
            offset.x = 0; // Cell is to the West
        }
        else
        {
            dir.x = 1;
            offset.x = 1; // Cell is to the East
        }

        if(diff.x >= diff.y)
        { // Move along the x axis and increment/decrement y when f >= diff.x.
            while(l1.x != l2.x)
            {
                f += diff.y;
                if(f >= diff.x)
                {  // We are changing rows, we might need to check two cells this iteration.
                    if (!mIsTraversable(l1 + offset))
                        return false;

                    l1.y += dir.y;
                    f -= diff.x;
                }

                if(f != 0 && !mIsTraversable(l1 + offset))
                    return false;

                // If we are moving along a horizontal line, either the north or the south cell should be unblocked.
                if (diff.y == 0 && !mIsTraversable({l1.x + offset.x, l1.y}) && !mIsTraversable({l1.x + offset.x, l1.y + 1}))
                    return false;

                l1.x += dir.x;
            }
        }
        else
        {  //if (diff.x < diff.y). Move along the y axis and increment/decrement x when f >= diff.y.
            while (l1.y != l2.y)
            {
                f += diff.x;
                if(f >= diff.y)
                {
                    if(!mIsTraversable(l1 + offset))
                        return false;

                    l1.x += dir.x;
                    f -= diff.y;
                }

                if(f != 0 && !mIsTraversable(l1 + offset))
                    return false;

                if (diff.x == 0 && !mIsTraversable({l1.x, l1.y + offset.y}) && !mIsTraversable({l1.x + 1, l1.y + offset.y}))
                    return false;

                l1.y += dir.y;
            }
        }

        return true;
    }

    //return a vector of all the neighbors ids and the cost to travel to them
    //In this adaptor we only need to check the four tileneibors and the cost is always 1
    virtual std::vector<std::pair<NodeId, Cost>> getNodeNeighbors(const NodeId id) const override
    {
        auto pos = idToPos(id);

        const Pathfinder::Cost cost = 1;

        std::vector<std::pair<NodeId, Cost>> neighbors;

        //check if we are not on most left if not check if the tile to the left is traversable
        //if so then add it to the neighbor list with its cost(1 for all neighbors)
        if(pos.x != 0 && mIsTraversable({pos.x - 1, pos.y}))
            neighbors.push_back({posToId({pos.x - 1, pos.y}), cost});

        if(pos.y != 0 && mIsTraversable({pos.x, pos.y - 1}))
            neighbors.push_back({posToId({pos.x, pos.y - 1}), cost});

        if(pos.x != mMapSize.x - 1 && mIsTraversable({pos.x + 1, pos.y}))
            neighbors.push_back({posToId({pos.x + 1, pos.y}), cost});

        if(pos.y != mMapSize.y - 1 && mIsTraversable({pos.x, pos.y + 1}))
            neighbors.push_back({posToId({pos.x, pos.y + 1}), cost});

        return neighbors;
    }

    //custom function used to map tile to id
    Pathfinder::NodeId posToId(const Vectori& pos) const
    {
        return pos.y * mMapSize.x + pos.x;
    }

    //custom function used to map id to tile
    Vectori idToPos(const Pathfinder::NodeId id) const
    {
        return {id % mMapSize.x, id / mMapSize.x};
    }

private:
    const Vectori mMapSize;

    //Function used to know if one tile of the map is travesable
    //The adaptor could also store a reference or a pointer to the map directly
    //But I find this way make the adaptor way more reusable
    const std::function<bool(const Vectori&)> mIsTraversable;
};
