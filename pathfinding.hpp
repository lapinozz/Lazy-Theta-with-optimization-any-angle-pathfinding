#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>

class Pathfinder
{
public:

    using NodeId = uint32_t;
    using Cost = float;

    //The pathfinder is a general algorithm that can be used for mutliple purpose
    //So it use adaptor
    class PathfinderAdaptor
    {
    public:
        friend Pathfinder;

        virtual size_t getNodeCount() const = 0;
        virtual Cost distance(const NodeId n1, const NodeId n2) const = 0;
        virtual bool lineOfSight(const NodeId n1, const NodeId n2) const = 0;
        virtual std::vector<std::pair<NodeId, Cost>> getNodeNeighbors(const NodeId id) const = 0;
    };

    static constexpr const float EPSILON = 0.00001f;
    static constexpr Cost INFINITE_COST = std::numeric_limits<Cost>::max();

    enum ListType {NO_LIST, OPEN_LIST, CLOSED_LIST};

    struct Node
    {
        std::vector<std::pair<NodeId, Cost>> neighbors;
        uint32_t searchIndex = 0; //The last search at which it has bean generated, bassicacly used to decide if the node need to be reseted before we use it
        NodeId parent;	// Parent of the node.
        Cost g;			// Initialized to infinity when generated.
        Cost h;		    // Initialized to the heuristic distance to the goal when generated.
        ListType list;  // Initially NO_LIST, can be changed to OPEN_LIST or CLOSED_LIST.
    };

    struct HeapElement
    {
        NodeId id;
        Cost g;	// Used for tie-breaking
        Cost f;	// Main key

        //inverted so that the smaller is at the end of the vector
        bool operator<(const HeapElement& rhs) const
        {
            if(abs(f - rhs.f) < EPSILON)
//                return g > rhs.g;
                return g < rhs.g;

//            return f < rhs.f;
            return f > rhs.f;
        }
    };


    Pathfinder(PathfinderAdaptor& adaptor, Cost weight = 1.0f) : adaptor(adaptor), weight(weight)
    {
        generateNodes();
    }

    template<typename DataType>
    std::vector<DataType> search(const DataType& start, const DataType& end, std::function<DataType(const NodeId)> idToData, std::function<NodeId(const DataType&)> dataToId)
    {
        const auto path = search(dataToId(start), dataToId(end));
        std::vector<DataType> finalPath;
        finalPath.reserve(path.size());

        for(const auto id : path)
            finalPath.push_back(idToData(id));

        return finalPath;
    }

    std::vector<NodeId> search(const NodeId startId, const NodeId endId)
    {
        openList.clear();

        currentSearch++;

        generateState(startId, endId);
        generateState(endId, endId);

        nodes[startId].g = 0;
        nodes[startId].parent = startId;

        addToOpen(startId);

        while(!openList.empty() && nodes[endId].g > getMin().f + EPSILON)
        {
            NodeId currId = getMin().id;
            popMin();

            // Lazy Theta* assumes that there is always line-of-sight from the parent of an expanded state to a successor state.
            // When expanding a state, check if this is true.
            if(!adaptor.lineOfSight(nodes[currId].parent, currId))
            {
                // Since the previous parent is invalid, set g-value to infinity.
                nodes[currId].g = INFINITE_COST;

                // Go over potential parents and update its parent to the parent that yields the lowest g-value for s.
                for(const auto neighbordInfo : nodes[currId].neighbors)
                {
                    auto newParent = neighbordInfo.first;

                    generateState(newParent, endId);
                    if(nodes[newParent].list == CLOSED_LIST)
                    {
                        Cost newG = nodes[newParent].g + neighbordInfo.second;
                        if(newG < nodes[currId].g)
                        {
                            nodes[currId].g = newG;
                            nodes[currId].parent = newParent;
                        }
                    }
                }
            }

            for(const auto neighborInfo : nodes[currId].neighbors)
            {
                auto neighborId = neighborInfo.first;

                generateState(neighborId, endId);

                NodeId newParent = nodes[currId].parent;

                if(nodes[neighborId].list != CLOSED_LIST)
                {
                    Cost newG = nodes[newParent].g + adaptor.distance(newParent, neighborId);

                    if(newG + EPSILON < nodes[neighborId].g)
                    {
                        nodes[neighborId].g = newG;
                        nodes[neighborId].parent = newParent;
                        addToOpen(neighborId);
                    }
                }
            }
        }

        std::vector<NodeId> path;

        if(nodes[endId].g < INFINITE_COST)
        {
    //        ValidateParent(endId, endId);
            NodeId curr = endId;
            while(curr != startId)
            {
                path.push_back(curr);
                curr = nodes[curr].parent;
            }

            path.push_back(curr);
            std::reverse(path.begin(), path.end());
        }

        return path;
    }

    void generateNodes()
    {
        nodes.clear();
        nodes.resize(adaptor.getNodeCount());

        NodeId current = 0;
        for(auto& node : nodes)
            node.neighbors = adaptor.getNodeNeighbors(current++);
    }

private:

    std::vector<Node> nodes;
    std::vector<HeapElement> openList;

    PathfinderAdaptor& adaptor;

    const Cost weight;

    uint32_t currentSearch = 0;

    void generateState(NodeId s, NodeId goal)
    {
        if(nodes[s].searchIndex != currentSearch)
        {
            nodes[s].searchIndex = currentSearch;
            nodes[s].h = adaptor.distance(s, goal) * weight;
            nodes[s].g = INFINITE_COST;
            nodes[s].list = NO_LIST;
        }
    }

    void addToOpen(NodeId id)
    {
        // If it is already in the open list, remove it and do a sorted insert
        if (nodes[id].list == OPEN_LIST)
        {
            auto index = std::find_if(openList.begin(), openList.end(), [&](const auto& heap){return heap.id == id;});
            auto id = index->id;
            openList.erase(index);
            insert_sorted(openList, {id, nodes[id].g, nodes[id].g + nodes[id].h});
        }
        // Otherwise, add it to the open list
        else
        {
            nodes[id].list = OPEN_LIST;
            insert_sorted(openList, {id, nodes[id].g, nodes[id].g + nodes[id].h});
        }

    }

    const HeapElement getMin() const
    {
        return openList.back();
    }

    void popMin()
    {
        nodes[openList.back().id].list = CLOSED_LIST;
        openList.pop_back();

    }

    template< typename T >
typename std::vector<T>::iterator
   insert_sorted( std::vector<T> & vec, T const& item )
{
    return vec.insert
        (
            std::upper_bound( vec.begin(), vec.end(), item ),
            item
        );
}

template< typename T, typename Pred >
typename std::vector<T>::iterator
    insert_sorted( std::vector<T> & vec, T const& item, Pred pred )
{
    return vec.insert
        (
           std::upper_bound( vec.begin(), vec.end(), item, pred ),
           item
        );
}
};

