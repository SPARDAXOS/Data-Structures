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
	inline void CalculateHCost(const GraphNode& endNode) noexcept {
		m_HCost = Distance(m_Position, endNode.m_Position);
	}
	constexpr inline void AddGCost(float value) noexcept {
		m_GCost += value;
	}

public:
	int m_ID{ -1 };
	bool m_Visited{ false };
	Vector2 m_Position{ 0.0f, 0.0f };
	std::vector<std::pair<GraphNode*, ConnectionDirection>> m_Neighbours;

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
	inline void LoadFromFile(const std::string_view directory) {
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
	inline Merigold::Container<GraphNode> Traverse(GraphNode& start, GraphNode& target, TraversalMethod method) noexcept {
		if (start == target || m_Nodes.size() == 0)
			return {};

		Merigold::Container<GraphNode> Buffer;
		ClearVisitedNodes();
		if (method == TraversalMethod::BREADTH_FIRST)
			BreadthFirst(start, target, Buffer);
		else if (method == TraversalMethod::DEPTH_FIRST)
			DepthFirst(start, target, Buffer);

		return Buffer;
	}
	inline Merigold::Container<GraphNode> FindPath(GraphNode& start, GraphNode& target, bool visualization = false)  noexcept {
		Merigold::Container<GraphNode> Path;
		if (start == target || m_Nodes.size() == 0) {
			Path.emplace_back(start);
			return Path;
		}

		ClearAllLists();
		ClearAllNodes();

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

			m_ClosedList.push_back(CurrentNode);
			RemoveFromOpenedList(*CurrentNode);
			for (uint32 index = 0; index < CurrentNode->m_Neighbours.size(); index++) {
				auto AdjacentGraphNode = CurrentNode->m_Neighbours[index].first;
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
				//Awfull but should be enough just to get a quick visualization on the path

				std::cout << "\n" << std::endl;
				std::cout << "Path Found: " << std::endl;

				int CursorX = 0;
				float LastX = 0;
				bool IsPathNode = false;

				for (auto& node : m_Nodes) {
					IsPathNode = false;
					for (auto& element : Path) {
						if (node == element) {
							IsPathNode = true;
							break;
						}
					}

					if (CursorX >= 20) {
						CursorX = 0;
						LastX = -1.0f;
						std::cout << std::endl;
					}
					if (LastX == node.m_Position.x || LastX + 1 == node.m_Position.x) {
						if (!IsPathNode)
							std::cout << "O";
						else
							std::cout << " ";
					}
					else {
						float Current = LastX + 1;
						while (Current != node.m_Position.x) {
							std::cout << "X";
							Current++;
							CursorX++;
							if (CursorX >= 20) {
								CursorX = 0;
								Current = 0.0f;
								LastX = 0.0f;
								std::cout << std::endl;
							}
						}

						if (!IsPathNode)
							std::cout << "O";
						else
							std::cout << " ";
					}
						
					LastX = node.m_Position.x;
					CursorX++;
				}

				std::cout << "\n" << std::endl;
			}
		}
		return Path;
	}

private:
	inline bool DepthFirst(GraphNode& start, GraphNode& target, Merigold::Container<GraphNode>& buffer) const {

		start.m_Visited = true;
		buffer.emplace_back(start);

		if (start == target)
			return true;

		for (auto& neighbour : start.m_Neighbours) {
			if (!neighbour.first->m_Visited) {
				auto Results = DepthFirst(*neighbour.first, target, buffer);
				if (Results)
					return true;
			}
		}

		return false;
	}
	inline void BreadthFirst(GraphNode& start, GraphNode& target, Merigold::Container<GraphNode>& buffer) const {

		std::queue<GraphNode*> Nodes;

		start.m_Visited = true;
		Nodes.push(&start);
		buffer.emplace_back(start);

		GraphNode* CurrentNode = &start;
		while (Nodes.size() != 0) {
			
			for (auto& neighbour : CurrentNode->m_Neighbours) {
				if (!neighbour.first->m_Visited) {
					buffer.emplace_back(*neighbour.first);
					if (*neighbour.first == target)
						return;

					neighbour.first->m_Visited = true;
					Nodes.push(neighbour.first);
				}
			}

			Nodes.pop();
			if (Nodes.size() > 0)
				CurrentNode = Nodes.front();
		}
	}

private:
	inline void CheckForNodes(const std::string_view upperLine, const std::string_view lowerLine) {
		std::vector<bool> UpperLineMappings; //Uses template specialization for <bool> to store each bool in a bit instead of 1 byte
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
	inline std::vector<bool> CheckLineForNodes(const std::string_view line) {
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
					NewNode->m_Neighbours.push_back({ PreviousNode, GraphNode::ConnectionDirection::LEFT });
					PreviousNode->m_Neighbours.push_back({ NewNode, GraphNode::ConnectionDirection::RIGHT });
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
	inline void SetupVerticalConnections(std::vector<bool> upper, std::vector<bool> lower) {
		for (unsigned int i = 0; i < upper.size(); i++) {
			if (upper.at(i) && lower.at(i)) {
				auto UpperNode = FindNode({ static_cast<float>(i), m_LineCursor - 2 });
				auto LowerNode = FindNode({ static_cast<float>(i), m_LineCursor - 1 });

				if (UpperNode && LowerNode) {
					UpperNode->m_Neighbours.push_back({ LowerNode, GraphNode::ConnectionDirection::DOWN });
					LowerNode->m_Neighbours.push_back({ UpperNode, GraphNode::ConnectionDirection::UP });
				}
			}
		}
	}
	inline void SetupPreviousLineConnections(std::vector<bool> upper, std::vector<bool> previous) {
		if (previous.size() == 0)
			return;

		for (unsigned int i = 0; i < upper.size(); i++) {
			if (upper.at(i) && previous.at(i)) {
				auto UpperNode = FindNode({ static_cast<float>(i), m_LineCursor - 2 });
				auto LastNode = FindNode({ static_cast<float>(i), m_LineCursor - 3 });

				if (UpperNode && LastNode) {
					UpperNode->m_Neighbours.push_back({ LastNode,  GraphNode::ConnectionDirection::UP });
					LastNode->m_Neighbours.push_back({ UpperNode, GraphNode::ConnectionDirection::DOWN });
				}
			}
		}
	}

private:
	inline GraphNode& AddNode(Vector2 position) {
			GraphNode NewNode;
			NewNode.m_ID = m_IDCounter;
			NewNode.m_Position = position;
			m_IDCounter++;
			return m_Nodes.emplace_back(NewNode);;
		}
	inline GraphNode* FindNode(Vector2 position) noexcept {
			for (auto& GraphNode : m_Nodes) {
				if (GraphNode.m_Position.x == position.x && GraphNode.m_Position.y == position.y)
					return &GraphNode;
			}

			return nullptr;
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
	Merigold::Container<GraphNode*> m_OpenList;
	Merigold::Container<GraphNode*> m_ClosedList;

private:
	Merigold::Container<GraphNode> m_Nodes;
	int m_IDCounter = 0;
	float m_LineCursor = 0.0f;
};
#endif // !GRAPH_H

