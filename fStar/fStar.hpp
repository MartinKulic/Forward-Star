#ifndef FORWARD_STAR_FSTAR_HPP
#define FORWARD_STAR_FSTAR_HPP

#include <map>
#include <string>
#include <vector>
#include <type_traits>
#include <concepts>


namespace f_star {
    // Type checks
    template <typename Tnode>
    concept EqualityComparable = requires(Tnode n1, Tnode n2)
    {
        n1==n2;
    };
    template <typename Tnode>
    concept Comparable_less = requires(Tnode n1, Tnode n2)
    {
        n1<n2;
    };
    // -----------

    // Helpers
    struct base_Node {
        int id;
    };

    template<typename Tnode>
    struct Edge {
        Tnode from;
        Tnode to;
        float weight;
    };

    template<typename PTnode>
    struct FStarEdgeEntry {
        PTnode node_to;
        float weight;
    };
    // -----------

    // Iterators
    template<typename Tnode>
    struct FStarNodeEdges {
        Tnode node_from;
        std::vector<FStarEdgeEntry<Tnode*> > *_edges = new std::vector<FStarEdgeEntry<Tnode*> >();
        //FStarNodeEdges() { this->_edges = new std::vector<FStarEdgeEntry<Tnode*> >(); };
        FStarNodeEdges(Tnode node) { node_from = node; };
        ~FStarNodeEdges() { delete this->_edges; };

        void deleteEdge(Tnode *to) {
            int index = 0;
            for (FStarEdgeEntry<Tnode*> edge: *this->_edges) {
                if (edge.node_to == to) {
                    break;
                }
                index++;
            }
            if (_edges->at(index).node_to != to) {
                return;
                //not found
            }

            this->_edges->erase(this->_edges->begin() + index);
        }
    };

    template<typename Tnode>
    class NodeIterator {
        friend class fStar;

    private:
        typename std::map<Tnode, FStarNodeEdges<Tnode> *>::iterator it;

    public:
        NodeIterator(typename std::map<Tnode, FStarNodeEdges<Tnode> *>::iterator it) : it(it) {
        }

        virtual NodeIterator &operator++() {
            ++it;
            return *this;
        }

        virtual bool operator!=(const NodeIterator &other) const {
            return it != other.it;
        }

        virtual Tnode operator*() const {
            return it->second->node_from;
        }
    };

    template<typename Tnode>
    class EdgeIterator {
    private:
        typename std::map<Tnode, FStarNodeEdges<Tnode> *>::iterator map_it;
        typename std::map<Tnode, FStarNodeEdges<Tnode> *>::iterator map_end;
        typename std::vector<FStarEdgeEntry<Tnode*> >::iterator vec_it;

        void advance_to_valid() {
            while (map_it != map_end) {
                auto edges = map_it->second->_edges;
                if (vec_it != edges->end()) {
                    return;
                }
                ++map_it;
                if (map_it != map_end) {
                    vec_it = map_it->second->_edges->begin();
                }
            }
        }

    public:
        EdgeIterator(
            typename std::map<Tnode, FStarNodeEdges<Tnode> *>::iterator m_it,
            typename std::map<Tnode, FStarNodeEdges<Tnode> *>::iterator m_end) : map_it(m_it), map_end(m_end) {
            if (map_it != map_end) {
                vec_it = map_it->second->_edges->begin();
                advance_to_valid();
            }
        }

        EdgeIterator &operator++() {
            ++vec_it;
            advance_to_valid();
            return *this;
        }

        bool operator!=(const EdgeIterator &other) const {
            return map_it != other.map_it ||
                   (map_it != map_end && vec_it != other.vec_it);
        }

        Edge<Tnode> operator*() const {
            FStarEdgeEntry<Tnode*> edge_entry = *vec_it;
            return Edge<Tnode>({map_it->second->node_from, *edge_entry.node_to, edge_entry.weight});
        }
    };

    template<typename Tnode>
    class OutEdgeIterator {
    private:
        typename std::vector<FStarEdgeEntry<Tnode*> >::iterator it;
        Tnode node_from;

    public:
        OutEdgeIterator(typename std::vector<FStarEdgeEntry<Tnode*> >::iterator iterator, Tnode from) : it(iterator),
            node_from(from) {
        };

        virtual OutEdgeIterator &operator++() {
            ++it;
            return *this;
        }

        virtual bool operator!=(const OutEdgeIterator &other) const {
            return it != other.it;
        }

        virtual Edge<Tnode> operator*() const {
            return Edge<Tnode>({node_from, *it->node_to, it->weight});
        }
    };
    // -----------

// Main FStar
    template<typename Tnode>
    class FStar {
        static_assert(EqualityComparable<Tnode>,
            "FStar<Tnode>: Tnode must implement operator== (specialize operator== or just use a pointer)");
        static_assert(Comparable_less<Tnode>,
            "FStar<Tnode>: Tnode implement operator< (specialize operator< or just use a pointer)");

    protected:
        std::map<Tnode, FStarNodeEdges<Tnode> *> *Edges;
        size_t numEdges = 0;

        FStarNodeEdges<Tnode> *_findNodeEdges_encap(Tnode node);

        FStarNodeEdges<Tnode> *_addNode_encap(Tnode node);

        int _findEdgeEntryIndex_encap(Tnode node_to, FStarNodeEdges<Tnode> *_fsr_edges);

    public:
        FStar();

        void addNode(Tnode node);

        void addEdge(Tnode from, Tnode to, float weight, bool oneway = false);

        void deleteNode(Tnode nodeId);

        void deleteEdge(Tnode fromNodeId, Tnode toNodeId, bool oneway = false);

        void modifieEdge(Tnode from, Tnode to, float newWeight, bool oneway = false);

        void nuke();

        //Tnode *getNode(Tnode id) { return (*Edges)[id]->node_from; };

        size_t sizeNodes() { return this->Edges->size(); }
        size_t sizeEdges() { return numEdges; };


        NodeIterator<Tnode> begin_nodes() {
            return NodeIterator<Tnode>(Edges->begin());
        }

        NodeIterator<Tnode> end_nodes() {
            return NodeIterator<Tnode>(Edges->end());
        }

        EdgeIterator<Tnode> begin_edges() {
            return EdgeIterator<Tnode>(Edges->begin(), Edges->end());
        }

        EdgeIterator<Tnode> end_edges() {
            return EdgeIterator<Tnode>(Edges->end(), Edges->end());
        }

        OutEdgeIterator<Tnode> begin_out_edges(Tnode node_from_id) {
            FStarNodeEdges<Tnode> *_node_edg = (*this->Edges)[node_from_id];

            return OutEdgeIterator<Tnode>(_node_edg->_edges->begin(), _node_edg->node_from);
        }

        OutEdgeIterator<Tnode> end_out_edges(Tnode node_from_id) {
            FStarNodeEdges<Tnode> *_node_edg = (*this->Edges)[node_from_id];

            return OutEdgeIterator<Tnode>(_node_edg->_edges->end(), _node_edg->node_from);
        }

        ~FStar();
    };
} // f_star

#include "fStar.tpp"


#endif //FORWARD_STAR_FSTAR_HPP
