#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <queue>


#ifndef GRAPH_H
#define GRAPH_H

struct Vector2 { float x, y; };

class Graph final {
public:
	enum class ConnectionDirection { LEFT, RIGHT, UP, DOWN };
	enum class TraversalMethod { BREADTH_FIRST, DEPTH_FIRST };
	struct Node {
		int m_ID{ -1 };
		bool m_Visited{ false };
		Vector2 m_Position{ 0.0f, 0.0f };
		std::vector<std::pair<Node&, ConnectionDirection>> m_Neighbours;
	};

public:
	Graph() noexcept = default;
	~Graph() = default;

	Graph(const Graph& other) = default;
	Graph& operator=(const Graph& other) noexcept = default;

	Graph(Graph&& other) noexcept = default;
	Graph& operator=(Graph&& other) noexcept = default;

public:
	inline Node& AddNode(Vector2 position) {
		Node NewNode;
		NewNode.m_ID = m_IDCounter;
		NewNode.m_Position = position;
		m_IDCounter++;
		return m_Nodes.emplace_back(NewNode);;
	}
	Node* Find(Vector2 position) noexcept {
		for (auto& Node : m_Nodes) {
			if (Node.m_Position.x == position.x && Node.m_Position.y == position.y)
				return &Node;
		}

		return nullptr;
	}

	void Traverse(Vector2 startingPosition, TraversalMethod method) {
		Node* TargetNode = Find(startingPosition);
		if (!TargetNode) {
			printf("Failed to find node at X: %.3f Y: %.3f", startingPosition.x, startingPosition.y);
			return;
		}

		ClearVisitedNodes();
		if (method == TraversalMethod::BREADTH_FIRST)
			BreadthFirst(*TargetNode);
		else if (method == TraversalMethod::DEPTH_FIRST)
			DepthFirst(*TargetNode);
	}
	void LoadGraph(const std::string_view directory) {
		std::string UpperLine;
		std::string LowerLine;

		std::ifstream TargetFile(directory.data(), std::ios::in);
		if (TargetFile.is_open()) {

			TargetFile.seekg(0, std::ios_base::end);
			m_Nodes.reserve(TargetFile.tellg());
			TargetFile.seekg(0, std::ios_base::beg);

			while (!TargetFile.eof()) {
				getline(TargetFile, UpperLine);
				getline(TargetFile, LowerLine);
				CheckForNodes(UpperLine, LowerLine);
			}
			TargetFile.close();
		}
	}

private:
	void DepthFirst(Node& target) const {
		target.m_Visited = true;
		std::cout << "Visited Node " << target.m_ID << std::endl;
		for (auto& neighbour : target.m_Neighbours) {
			if (!neighbour.first.m_Visited)
				DepthFirst(neighbour.first);
		}
	}
	void BreadthFirst(Node& startingNode) const {
		std::queue<Node*> Nodes;
		startingNode.m_Visited = true;
		Nodes.push(&startingNode);
		Node* CurrentNode = &startingNode;
		while (Nodes.size() != 0) {
			for (auto& neighbour : CurrentNode->m_Neighbours) {
				if (!neighbour.first.m_Visited) {
					neighbour.first.m_Visited = true;
					Nodes.push(&neighbour.first);
				}
			}

			std::cout << "Visited Node " << CurrentNode->m_ID << std::endl;
			Nodes.pop();
			if (Nodes.size() > 0)
				CurrentNode = Nodes.front();
		}
	}

private:
	void CheckForNodes(const std::string_view upperLine, const std::string_view lowerLine) {
		std::vector<bool> UpperLineMappings;
		std::vector<bool> LowerLineMappings;
		static std::vector<bool> LastLineMappings;

		//Upper Line Setup
		UpperLineMappings = CheckLineForNodes(upperLine);
		m_LineCursor++;

		//Lower Line Setup
		LowerLineMappings = CheckLineForNodes(lowerLine);
		m_LineCursor++;

		//Crash if both lines dont have the same characters count!
		assert(UpperLineMappings.size() == LowerLineMappings.size() && "Character count between lines mismatch!");

		//Vertical Connections
		SetupVerticalConnections(UpperLineMappings, LowerLineMappings);

		//Connection With Earlier Line - Connect Upper Line With Earlier Bottom
		SetupPreviousLineConnections(UpperLineMappings, LastLineMappings);

		LastLineMappings = LowerLineMappings;
	}
	std::vector<bool> CheckLineForNodes(const std::string_view line) {
		Node* PreviousNode = nullptr;
		std::vector<bool> NodesOnLine(line.size());

		bool CanConnect = false;
		int ElementsCounter = 0;
		float X = 0;

		for (auto character : line) {
			if (character == 'o') {
				Node* NewNode = nullptr;
				if (CanConnect) {
					PreviousNode = &m_Nodes.back();
					NewNode = &AddNode({ X , m_LineCursor });
					NewNode->m_Neighbours.push_back({ *PreviousNode, ConnectionDirection::LEFT });
					PreviousNode->m_Neighbours.push_back({ *NewNode, ConnectionDirection::RIGHT });
				}
				else
					NewNode = &AddNode({ X , m_LineCursor });

				CanConnect = true;
				NodesOnLine.at(ElementsCounter) = true;
			}
			else
				CanConnect = false;

			X++;
			ElementsCounter++;
		}
		return NodesOnLine;
	}
	void SetupVerticalConnections(std::vector<bool> upper, std::vector<bool> lower) {
		for (unsigned int i = 0; i < upper.size(); i++) {
			if (upper.at(i) && lower.at(i)) {
				auto UpperNode = Find({ static_cast<float>(i), m_LineCursor - 2 });
				auto LowerNode = Find({ static_cast<float>(i), m_LineCursor - 1 });

				if (UpperNode && LowerNode) {
					UpperNode->m_Neighbours.push_back({ *LowerNode, ConnectionDirection::DOWN });
					LowerNode->m_Neighbours.push_back({ *UpperNode, ConnectionDirection::UP });
				}
			}
		}
	}
	void SetupPreviousLineConnections(std::vector<bool> upper, std::vector<bool> previous) {
		if (previous.size() == 0)
			return;

		for (unsigned int i = 0; i < upper.size(); i++) {
			if (upper.at(i) && previous.at(i)) {
				auto UpperNode = Find({ static_cast<float>(i), m_LineCursor - 2 });
				auto LastNode = Find({ static_cast<float>(i), m_LineCursor - 3 });

				if (UpperNode && LastNode) {
					UpperNode->m_Neighbours.push_back({ *LastNode, ConnectionDirection::UP });
					LastNode->m_Neighbours.push_back({ *UpperNode, ConnectionDirection::DOWN });
				}
			}
		}
	}

private:
	inline void ClearVisitedNodes() noexcept {
		for (auto& node : m_Nodes)
			node.m_Visited = false;
	}

private:
	std::vector<Node> m_Nodes;
	int m_IDCounter = 0;
	float m_LineCursor = 0.0f;
};
#endif // !GRAPH_H

