#ifndef FORWARD_STAR_TEST_NODE_HPP
#define FORWARD_STAR_TEST_NODE_HPP


#include <string>
#include <map>
#include "fStar.hpp"

class Test_Node  {
    private
    :
    std::map<int, Test_Node *> neighbors;
    int id;
    public
    :
    Test_Node(int a_id)
    {
        id = a_id;
    };

    bool Is_neighbor(int a_id)
    {
        return neighbors.find(a_id) != neighbors.end();
    }

    void Add_neighbor(Test_Node *neighbor)
    {
        if (Is_neighbor(neighbor->id)) {
            throw std::invalid_argument(
                "Will not add neighbor. Node " + std::to_string(neighbor->id) + " is already neighbor to " +
                std::to_string(this->id));
        }

        this->neighbors[neighbor->id] = neighbor;
    };

    void Delete_neighbor(int a_id)
    {
        if (!Is_neighbor(a_id)) {
            throw std::invalid_argument(
                "Cannot delete neighbor. Node " + std::to_string(a_id) + " is not neighbor to " + std::to_string(
                    this->id));
        }

        this->neighbors.erase(a_id);
    };

    Test_Node * Get_neighbor(int a_id)
    {
        auto it = neighbors.find(a_id);
        return it == neighbors.end() ? nullptr : it->second;
    }


    auto begin_neighbors()
    {
        return neighbors.begin();
    }
    auto end_neighbors()
    {
        return neighbors.end();
    }
};

#endif //FORWARD_STAR_TEST_NODE_HPP
