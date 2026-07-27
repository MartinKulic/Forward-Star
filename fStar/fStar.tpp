//
// Created by martin on 16. 4. 2026.
//

#include <iostream>
#include <ostream>
#include <stdexcept>

#include "fStar.hpp"

using namespace f_star;


template<typename Tnode>
FStar<Tnode>::FStar() {
    //this->Nodes = new map<int, Node*>();
    this->Edges = new std::map<Tnode, FStarNodeEdges<Tnode>*>();
}
template<typename Tnode>
FStarNodeEdges<Tnode> * FStar<Tnode>::_findNodeEdges_encap(Tnode node) {
    auto it = this->Edges->find(node);
    if (it == this->Edges->end()) {
        return nullptr;
    }
    return it->second;
}
template<typename Tnode>
FStarNodeEdges<Tnode> * FStar<Tnode>::_addNode_encap(Tnode node) {
    // if (this->Edges->find(node) != this->Edges->end()) {
    //     throw std::invalid_argument("Node with id already exists.");
    // }
    //
    // FStarNodeEdges<Tnode>* nodeRecord = new FStarNodeEdges<Tnode>(node);
    // //nodeRecord->node_from = node;
    // (*this->Edges)[node] = nodeRecord;
    // return nodeRecord;
    auto [it, inserted] = Edges->try_emplace(node, FStarNodeEdges<Tnode>{node});
    if (!inserted)
        throw std::invalid_argument("Node already exists.");
    return it->second;
}

/// Return: index of node_to in FStarNodeEdges _edges vector; -1 if not found
template<typename Tnode>
int FStar<Tnode>::_findEdgeEntryIndex_encap(Tnode node_to, FStarNodeEdges<Tnode> *_fsr_edges) {
    int index = 0;
    for (FStarEdgeEntry<Tnode*> edge: *_fsr_edges->_edges) {
        if (*edge.node_to == node_to) {
            return index;
        }
        index++;
    }
    return -1;
}

template<typename Tnode>
void FStar<Tnode>::addNode(Tnode node) {
    this->_addNode_encap(node);
}

template<typename Tnode>
void FStar<Tnode>::deleteNode(Tnode node) {
    auto mapEntry = this->Edges->find(node);
    if (mapEntry == this->Edges->end()) {
        //node is not in fStar
        return;
    }
    FStarNodeEdges<Tnode>* nodeEdgesToDelete = mapEntry->second;
    for (FStarEdgeEntry<Tnode> edge: *nodeEdgesToDelete->_edges) {
        Tnode* neigbour = edge.node_to;
        auto mapEntryN = this->Edges->find(neigbour->id);
        if (mapEntryN == this->Edges->end()) {
            //node is not in fStar
            continue;
        }
        FStarNodeEdges<Tnode>* nodeEdgesN = mapEntryN->second;
        nodeEdgesN->deleteEdge(nodeEdgesToDelete->node_from);
        this->numEdges--;
    }
    this->numEdges -= nodeEdgesToDelete->_edges->size();

    Tnode* nodeToDelete = nodeEdgesToDelete->node_from;
    this->Edges->erase(mapEntry);
    delete nodeEdgesToDelete;
    //delete nodeToDelete;
}

template<typename Tnode>
void FStar<Tnode>::addEdge(Tnode from, Tnode to, float weight, bool oneway) {
    FStarNodeEdges<Tnode>* _fsr_nodeFromEdges = this->_findNodeEdges_encap(from);
    if (_fsr_nodeFromEdges == nullptr) {
        //Node does not exist jet
        _fsr_nodeFromEdges = this->_addNode_encap(from);
    }

    FStarNodeEdges<Tnode>* _fsr_nodeToEdges = this->_findNodeEdges_encap(to);
    if (_fsr_nodeToEdges == nullptr) {
        _fsr_nodeToEdges = this->_addNode_encap(to);
    }
    Tnode* node_to_ptr = &_fsr_nodeToEdges->node_from;

    int edgeIndex = this->_findEdgeEntryIndex_encap(to, _fsr_nodeFromEdges);
    if (edgeIndex != -1) {
        //std::cerr << "Edge from " << _fsr_nodeFromEdges->node_from << " to " << to << " already exists. Ignoring error rewriting value" << std::endl;
        std::cerr << "Edge from - to already exists. Ignoring error rewriting value" << std::endl;
        _fsr_nodeFromEdges->_edges->erase(_fsr_nodeFromEdges->_edges->begin() + edgeIndex);
        this->numEdges--;
    }

    FStarEdgeEntry<Tnode*> edge {node_to_ptr ,weight};
    // edge.node_to = to;
    // edge.weight = weight;

    _fsr_nodeFromEdges->_edges->push_back(edge);
    ++this->numEdges;
    if (!oneway) {
        this->addEdge(to,from,weight, true);
    }
}

template<typename Tnode>
void FStar<Tnode>::deleteEdge(Tnode fromNodeId, Tnode toNodeId, bool oneway) {
    auto mapEntryFrom = this->Edges->find(fromNodeId);
    if (mapEntryFrom == this->Edges->end()) {
        return; //node is not in fStar
    }
    auto mapEntryTo = this->Edges->find(toNodeId);
    if (mapEntryTo == this->Edges->end()) {
        return; //node is not in fStar
    }

    FStarNodeEdges<Tnode>* nodeEdgesFrom = mapEntryFrom->second;
    FStarNodeEdges<Tnode>* nodeEdgesTo = mapEntryTo->second;
    nodeEdgesFrom->deleteEdge(nodeEdgesTo->node_from);
    numEdges--;

    if (!oneway) {
        nodeEdgesTo->deleteEdge(nodeEdgesFrom->node_from);
        numEdges--;
    }

}

template<typename Tnode>
void FStar<Tnode>::modifieEdge(Tnode from, Tnode to, float newWeight, bool oneway) {
    auto mapEntryFrom = this->Edges->find(from);
    if (mapEntryFrom == this->Edges->end()) {
        return; //node is not in fStar
    }
    FStarNodeEdges<Tnode>* fromEntry = mapEntryFrom->second;
    FStarEdgeEntry<Tnode>* edgeEntryToMod = nullptr;
     for (FStarEdgeEntry<Tnode>& edge: *fromEntry->_edges) {
        if (edge.node_to == to) {
            edgeEntryToMod = &edge;
            break;
        }
     }
    if (edgeEntryToMod == nullptr) {
        return; //node is not found
    }
    (edgeEntryToMod->weight) = newWeight;

    if (!oneway) {
        this->modifieEdge(to, from, newWeight, true);
    }
}

template<typename Tnode>
void FStar<Tnode>::nuke() {
    for (auto const& rec: *Edges) {
        //rec.first //key
        //rec.second //value

        auto node_edges = rec.second;
        //delete((node_edges)->_edges);
        //delete(node_edges)->node_from;
        delete(node_edges);
    }
    Edges->clear();
}

template<typename Tnode>
FStar<Tnode>::~FStar() {
    for (auto const& rec: *Edges) {
        //rec.first //key
        //rec.second //value

        auto node_edges = rec.second;
        //delete((node_edges)->_edges);
        //delete(node_edges)->node_from;
        delete(node_edges);
    }

    delete(Edges);
    // for (auto const& rec : *Nodes) {
    //     auto node = rec.second;
    //     delete(node);
    // }
}