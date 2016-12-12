#include <RoadGraph.h>
#include <openglpp/Debug.h>

#include <utility>  // std::pair
#include <iostream>

RoadGraph RoadGraph::build(const std::vector<RoadPtr>& roads, const std::vector<IntersectionPtr>& intersections)
{
	RoadGraph graph;

	std::map< Road*, std::map<int, Node*> > roadNodes;
	for (const auto& road : roads)
	{
		for (int lane = 0; lane < road->lanes(); lane++) {
			RoadGraphNodeData data;
			data.road = road;
			data.spline = road->generateSpline(lane);

			Node* node = graph.insert(data);
			roadNodes[road.get()][lane] = node;
		}
	}

	for (const auto& intersection : intersections)
	{
		const unsigned int nConnections = intersection->connections.size();
		for (unsigned int i = 0; i < nConnections; i++) {
			const auto& c1 = intersection->connections[i];
			for (unsigned int j = 0; j < nConnections; j++) {
				const auto& c2 = intersection->connections[j];

				if (i == j)
					continue;

				// connect inroad i to outroad j
				Road* inRoad = c1->road.lock().get();
				int inRoadEnd = c1->roadEnd;
				Road* outRoad = c2->road.lock().get();
				int outRoadEnd = c2->roadEnd;

				assert(inRoad->lanes() == outRoad->lanes());
				for (int lane = 0; lane < inRoad->lanes(); lane += 2) {
					RoadGraphNodeData data;
					data.intersection = intersection;
					data.spline = intersection->generateSpline(c1->intersectionVertIdx, c2->intersectionVertIdx, lane);

					std::pair<int, int> lanes(lane + !inRoadEnd, lane + outRoadEnd);

					Node* intersectionNode = graph.insert(data);
					roadNodes[inRoad][lanes.first]->edges.push_back(Edge(intersectionNode));
					intersectionNode->edges.push_back(roadNodes[outRoad][lanes.second]);
				}
			}
		}
	}

	return graph;
}

void splineToPath(const CatmullRom<RoadVertex>& spline,
	std::vector<glm::vec3>* pts_out, std::vector<glm::vec3>* normals_out)
{
	pts_out->clear();
	normals_out->clear();

	const unsigned int n_slices = (int)ceil(spline.total_length() / 0.5f);
	const float step = spline.total_length() / n_slices;
	pts_out->reserve(n_slices);
	normals_out->reserve(n_slices);

	for (unsigned int i = 0; i <= n_slices; i++) {
		float d = step * i;
		RoadVertex c0, c1;
		spline.evaluate(d, &c0, &c1);
		pts_out->push_back(c0.pos);
		normals_out->push_back(c0.normal);
	}
}

void RoadGraph::visualize(VisualizeMode mode) const
{
	Debug::clear();

	const glm::vec3 roadColor(1, 0, 0);
	const glm::vec3 intersectionColor(0, 1, 0);

	std::vector<glm::vec3> ends;
	std::vector<glm::vec3> normalLines;
	for (unsigned int i = 0; i < mNodes.size(); i++)
	{
		const Node* n = mNodes.at(i);

		for (unsigned int j = 0; j < n->edges.size(); j++) {
			const auto& edge = n->edges.at(j);
			auto start = n->data.spline.evaluate(n->data.spline.total_length()).pos;
			auto end = edge.to->data.spline.evaluate(0).pos;
			ends.push_back(start);
			ends.push_back(end + glm::vec3(0, 0.5f, 0));
		}


		std::vector<glm::vec3> pts;
		std::vector<glm::vec3> normals;
		splineToPath(n->data.spline, &pts, &normals);

		if (mode == VISUALIZE_PATHS)
			Debug::drawPath(pts, (!n->data.road.expired()) ? roadColor : intersectionColor);

		normalLines.reserve(normalLines.size() + pts.size() * 2);
		for (unsigned int j = 0; j < pts.size(); j++) {
			normalLines.push_back(pts.at(j));
			normalLines.push_back(pts.at(j) + normals.at(j) * 1.0f);
		}
	}

	if (mode == VISUALIZE_NORMALS)
		Debug::drawLines(normalLines, glm::vec3(0, 0, 1));

	if (ends.size() > 0 && mode == VISUALIZE_CONNECTIONS)
		Debug::drawLines(ends, glm::vec3(0, 1, 1));
}