#pragma once

#include <map>
#include <vector>

template <typename NodeDataT, typename EdgeDataT>
class Graph {
public:
	struct Node;
	struct Edge
	{
		Node* to;
		EdgeDataT data;

		Edge() : to(NULL) {}
		Edge(Node* t) : to(t) {}
		Edge(Node* t, const EdgeDataT& d) : to(t), data(d) {}
	};

	struct Node
	{
		std::vector<Edge> edges;
		NodeDataT data;

		Node() {}
		Node(const NodeDataT& d) : data(d) {}
	};

	Graph() {}
	Graph(const Graph<NodeDataT, EdgeDataT>& rhs) {
		mNodes.reserve(rhs.mNodes.size());

		std::map<Node*, Node*> rhsToThis;
		for (unsigned int i = 0; i < rhs.mNodes.size(); i++) {
			Node* rhsNode = rhs.mNodes.at(i);
			Node* node = new Node(rhsNode->data);
			rhsToThis[rhsNode] = node;

			mNodes.push_back(node);
		}

		for (unsigned int i = 0; i < rhs.mNodes.size(); i++) {
			Node* rhsNode = rhs.mNodes.at(i);
			Node* node = rhsToThis[rhsNode];
			for (unsigned int j = 0; j < rhsNode->edges.size(); j++) {
				const Edge& rhsEdge = rhsNode->edges[j];
				node->edges.push_back(Edge(rhsToThis[rhsEdge.to], rhsEdge.data));
			}
		}
	}

	Graph& operator=(const Graph& rhs) {
		for (Node* n : mNodes)
			delete n;
		mNodes.clear();

		mNodes.reserve(rhs.mNodes.size());
		std::map<Node*, Node*> rhsToThis;
		for (unsigned int i = 0; i < rhs.mNodes.size(); i++) {
			Node* rhsNode = rhs.mNodes.at(i);
			Node* node = new Node(rhsNode->data);
			rhsToThis[rhsNode] = node;

			mNodes.push_back(node);
		}

		for (unsigned int i = 0; i < rhs.mNodes.size(); i++) {
			Node* rhsNode = rhs.mNodes.at(i);
			Node* node = rhsToThis[rhsNode];
			for (unsigned int j = 0; j < rhsNode->edges.size(); j++) {
				const Edge& rhsEdge = rhsNode->edges[j];
				node->edges.push_back(Edge(rhsToThis[rhsEdge.to], rhsEdge.data));
			}
		}

		return *this;
	}

	~Graph() {
		for (Node* n : mNodes) {
			delete n;
		}
	}

	Node* insert(const NodeDataT& data) {
		Node* n = new Node(data);
		mNodes.push_back(n);
		return n;
	}

protected:
	std::vector<Node*> mNodes;
};

#include <RoadMesh.h>

struct RoadGraphNodeData {
	CatmullRom<RoadVertex> spline;

	std::weak_ptr<Intersection> intersection;
	std::weak_ptr<Road> road;
};

class RoadGraph : public Graph<RoadGraphNodeData, int>
{
public:
	typedef RoadGraphNodeData NodeDataT;
	typedef int EdgeDataT;

	typedef std::shared_ptr<Road> RoadPtr;
	typedef std::shared_ptr<Intersection> IntersectionPtr;

	static RoadGraph build(const std::vector<RoadPtr>& roads, const std::vector<IntersectionPtr>& intersections);

	void visualize() const;
};