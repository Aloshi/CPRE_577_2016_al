#include <RoadGraph.h>
#include <openglpp/Debug.h>

#include <utility>  // std::pair

std::pair<int, int> roadLanes(int lane, int end1, int end2)
{
	int in = !end2;
	int out = !end1;

	lane = lane - (lane % 2);
	return std::pair<int, int>(lane + in, lane + out);
}

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

				// TODO take road end into account!
				assert(inRoad->lanes() == outRoad->lanes());
				for (int lane = 0; lane < inRoad->lanes(); lane++) {
					RoadGraphNodeData data;
					data.intersection = intersection;
					data.spline = intersection->generateSpline(i, j, lane);

					const auto lanes = roadLanes(lane, inRoadEnd, outRoadEnd);
					Node* intersectionNode = graph.insert(data);
					roadNodes[inRoad][lanes.first]->edges.push_back(Edge(intersectionNode));
					intersectionNode->edges.push_back(roadNodes[outRoad][lanes.second]);

					// next one will be flipped
					std::swap(inRoad, outRoad);
					std::swap(inRoadEnd, outRoadEnd);
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

	const unsigned int n_slices = (int)ceil(spline.total_length() / 10.0f);
	pts_out->reserve(n_slices);
	normals_out->reserve(n_slices);
	for (unsigned int i = 0; i <= n_slices; i++) {
		float d = spline.total_length() * ((float)i) / n_slices;
		RoadVertex c0, c1;
		spline.evaluate(d, &c0, &c1);
		pts_out->push_back(c0.pos);
		normals_out->push_back(c0.normal);
	}
}

void RoadGraph::visualize() const
{
	const glm::vec3 roadColor(1, 0, 0);
	const glm::vec3 intersectionColor(0, 1, 0);

	for (unsigned int i = 0; i < mNodes.size(); i++)
	{
		const Node* n = mNodes.at(i);

		std::vector<glm::vec3> pts;
		std::vector<glm::vec3> normals;
		splineToPath(n->data.spline, &pts, &normals);

		Debug::drawPath(pts, (!n->data.road.expired()) ? roadColor : intersectionColor);

		std::vector<glm::vec3> normalLines;
		normalLines.resize(pts.size() * 2);
		for (unsigned int j = 0; j < pts.size(); j++) {
			normalLines[j*2+0] = pts.at(j);
			normalLines[j*2+1] = normalLines[j*2] + normals.at(j);
		}
		Debug::drawLines(normalLines, glm::vec3(0, 0, 1));
	}
}