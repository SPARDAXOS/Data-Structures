#include "Utility.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <queue>


#ifndef GRAPH_H
#define GRAPH_H

class GraphNode {
public:
	enum class ConnectionDirection { LEFT, RIGHT, UP, DOWN };

public:
	GraphNode() noexcept = default;
	~GraphNode() = default;

	GraphNode(const GraphNode& other) = default;
	GraphNode& operator=(const GraphNode& other) noexcept = default;

	GraphNode(GraphNode&& other) noexcept = default;
	GraphNode& operator=(GraphNode&& other) noexcept = default;

	bool operator==(const GraphNode& other) const noexcept {
		if (this->m_ID != other.m_ID)
			return false;
		if (this->m_Position != other.m_Position)
			return false;
		return true;
	}

public:
	constexpr inline void ClearPathfindingData() noexcept {
		m_ParentNode = nullptr;
		m_FCost = 0.0f;
		m_GCost = 0.0f; 
		m_HCost = 0.0f;
	}
	constexpr inline void CalculateFCost() noexcept {
		m_FCost = m_GCost + m_HCost;
	}
	inline void CalculateGCost(const GraphNode& startingNode) noexcept {
		m_GCost += Distance(m_Position, startingNode.m_Position);
	}
	inline void AddGCost(float value) noexcept {
		m_GCost += value;
	}
	inline void CalculateHCost(const GraphNode& endNode) noexcept {
		m_HCost = Distance(m_Position, endNode.m_Position);
	}

public:
	int m_ID{ -1 };
	bool m_Visited{ false };
	Vector2 m_Position{ 0.0f, 0.0f };
	//std::vector<std::pair<GraphNode&, ConnectionDirection>> m_Neighbours;

	Container<GraphNode*> m_Neighbours;

public:
	GraphNode* m_ParentNode = nullptr;
	float m_FCost{ 0.0f };
	float m_GCost{ 0.0f };
	float m_HCost{ 0.0f };
};



class Graph final {
public:
	enum class TraversalMethod { BREADTH_FIRST, DEPTH_FIRST };


public:
	Graph() noexcept = default;
	~Graph() = default;

	Graph(const Graph& other) = default;
	Graph& operator=(const Graph& other) noexcept = default;

	Graph(Graph&& other) noexcept = default;
	Graph& operator=(Graph&& other) noexcept = default;


public:
	inline GraphNode& AddNode(Vector2 position) {
		GraphNode NewNode;
		NewNode.m_ID = m_IDCounter;
		NewNode.m_Position = position;
		m_IDCounter++;
		return m_Nodes.emplace_back(NewNode);;
	}
	GraphNode* FindNode(Vector2 position)  noexcept {
		for (auto& GraphNode : m_Nodes) {
			if (GraphNode.m_Position.x == position.x && GraphNode.m_Position.y == position.y)
				return &GraphNode;
		}

		return nullptr;
	}

	void Traverse(Vector2 startingPosition, TraversalMethod method) {
		GraphNode* TargetNode = FindNode(startingPosition);
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
	Container<GraphNode> FindPath(GraphNode& start, GraphNode& target, bool visualization = false)  noexcept {

		Container<GraphNode> Path;
		if (start == target || m_Nodes.size() == 0) {
			Path.emplace_back(start);
			return Path;
		}


		ClearAllLists();
		ClearAllNodes();

		//Prepare first node
		start.CalculateGCost(start);
		start.CalculateHCost(target);
		start.CalculateFCost();
		m_OpenList.push_back(&start);

		bool PathFound = false;
		while (!PathFound) {

			GraphNode* CurrentNode = GetNextInOpenList();
			if (!CurrentNode) {
				std::cout << "Error - Invalid Graphnode" << std::endl;
				return Path;
			}

			if (*CurrentNode == target) {
				PathFound = true;
				break;
			}

			m_ClosedList.push_back(CurrentNode); //Need to get a ref to the actual thing!
			RemoveFromOpenedList(*CurrentNode);
			for (uint32 index = 0; index < CurrentNode->m_Neighbours.size(); index++) {
				auto AdjacentGraphNode = CurrentNode->m_Neighbours[index];
				if (IsInOpenList(*AdjacentGraphNode) || IsInClosedList(*AdjacentGraphNode))
					continue;
				else {
					AdjacentGraphNode->m_ParentNode = CurrentNode;
					AdjacentGraphNode->CalculateGCost(*AdjacentGraphNode->m_ParentNode);
					AdjacentGraphNode->AddGCost(AdjacentGraphNode->m_ParentNode->m_GCost);
					AdjacentGraphNode->CalculateHCost(target);
					AdjacentGraphNode->CalculateFCost();
					m_OpenList.emplace_back(AdjacentGraphNode);
				}
			}
		}

		if (PathFound) {
			GraphNode* CurrentGraphNode = &target;
			Path.emplace_back(*CurrentGraphNode);
			while (CurrentGraphNode->m_ParentNode) {
				CurrentGraphNode = CurrentGraphNode->m_ParentNode;
				Path.emplace_back(*CurrentGraphNode);
			}

			if (visualization) {
				float LastY = 0;
				float LastX = 0;
				for (auto& node : m_Nodes) {
					if (LastY != node.m_Position.y) {
						std::cout << std::endl;
						if (LastX + 1 == node.m_Position.x)
							std::cout << "O";
						else {
							float current = 0;
							while (current != node.m_Position.x) {
								std::cout << "X";
								current++;
								//If looped to new line!
							}
						}
					}
					else if (LastX + 1 == node.m_Position.x || LastX == node.m_Position.x) {
						std::cout << "O";
					}
					else if (LastX + 1 != node.m_Position.x) {
						float current = LastX + 1;
						while (current != node.m_Position.x) {
							std::cout << "X";
							current++;
							//If looped to new line!
						}
					}

					LastY = node.m_Position.y;
					LastX = node.m_Position.x;
				}

				std::cout << "\n" << std::endl;
			}
		}

		return Path;
	}

private:
	void DepthFirst(GraphNode& target) const {
		target.m_Visited = true;
		std::cout << "Visited Node " << target.m_ID << std::endl;
		for (auto& neighbour : target.m_Neighbours) {
			if (!neighbour->m_Visited)
				DepthFirst(*neighbour);
		}
	}
	void BreadthFirst(GraphNode& startingNode) const {
		std::queue<GraphNode*> Nodes;
		startingNode.m_Visited = true;
		Nodes.push(&startingNode);
		GraphNode* CurrentNode = &startingNode;
		while (Nodes.size() != 0) {
			for (auto& neighbour : CurrentNode->m_Neighbours) {
				if (!neighbour->m_Visited) {
					neighbour->m_Visited = true;
					Nodes.push(neighbour);
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
		GraphNode* PreviousNode = nullptr;
		std::vector<bool> NodesOnLine(line.size());

		bool CanConnect = false;
		int ElementsCounter = 0;
		float X = 0;

		for (auto character : line) {
			if (character == 'o' || character == 'S' || character == 'G') {
				GraphNode* NewNode = nullptr;
				if (CanConnect) {
					PreviousNode = &m_Nodes.back();
					NewNode = &AddNode({ X , m_LineCursor });
					NewNode->m_Neighbours.push_back(PreviousNode);
					PreviousNode->m_Neighbours.push_back(NewNode);
				}
				else
					NewNode = &AddNode({ X , m_LineCursor });
				
				if (character == 'S')
					m_Start = NewNode;
				else if (character == 'G')
					m_Target = NewNode;

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
				auto UpperNode = FindNode({ static_cast<float>(i), m_LineCursor - 2 });
				auto LowerNode = FindNode({ static_cast<float>(i), m_LineCursor - 1 });

				if (UpperNode && LowerNode) {
					UpperNode->m_Neighbours.push_back(LowerNode);
					LowerNode->m_Neighbours.push_back(UpperNode);
				}
			}
		}
	}
	void SetupPreviousLineConnections(std::vector<bool> upper, std::vector<bool> previous) {
		if (previous.size() == 0)
			return;

		for (unsigned int i = 0; i < upper.size(); i++) {
			if (upper.at(i) && previous.at(i)) {
				auto UpperNode = FindNode({ static_cast<float>(i), m_LineCursor - 2 });
				auto LastNode = FindNode({ static_cast<float>(i), m_LineCursor - 3 });

				if (UpperNode && LastNode) {
					UpperNode->m_Neighbours.push_back(LastNode);
					LastNode->m_Neighbours.push_back(UpperNode);
				}
			}
		}
	}

private:
	inline GraphNode* GetNextInOpenList() noexcept {
		float LowestFCost = -1.0f;
		GraphNode* TargetNode = nullptr;

		for (auto& element : m_OpenList) {
			if (LowestFCost == -1.0f) {
				LowestFCost = element->m_GCost;
				TargetNode = element;
			}
			else if (LowestFCost > element->m_FCost) {
				LowestFCost = element->m_FCost;
				TargetNode = element;
			}
		}

		return TargetNode;
	}
	inline void RemoveFromOpenedList(GraphNode target) noexcept {
		for (uint32 i = 0; i < m_OpenList.size(); i++) {
			if (*m_OpenList[i] == target) {
				m_OpenList.erase(m_OpenList.begin() + i);
				return;
			}
		}
	}
	inline bool IsInOpenList(const GraphNode& target) const noexcept {
		for (auto& element : m_OpenList) {
			if (*element == target)
				return true;
		}

		return false;
	}
	inline bool IsInClosedList(const GraphNode& target) const noexcept {
		for (auto& element : m_ClosedList) {
			if (*element == target)
				return true;
		}

		return false;
	}
	inline void ClearVisitedNodes() noexcept {
		for (auto& node : m_Nodes)
			node.m_Visited = false;
	}
	inline void ClearAllLists() noexcept {
		if (m_OpenList.size() > 0)
			m_OpenList.clear();

		if (m_ClosedList.size() > 0)
			m_ClosedList.clear();
	}
	inline void ClearAllNodes() noexcept {
		if (m_Nodes.size() == 0)
			return;

		for (auto& node : m_Nodes)
			node.ClearPathfindingData();
	}

public:
	GraphNode* m_Start = nullptr;
	GraphNode* m_Target = nullptr;

private:
	Container<GraphNode*> m_OpenList;
	Container<GraphNode*> m_ClosedList;

private:
	Container<GraphNode> m_Nodes;
	int m_IDCounter = 0;
	float m_LineCursor = 0.0f;
};
#endif // !GRAPH_H

