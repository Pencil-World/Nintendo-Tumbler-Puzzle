#include<algorithm>
#include<array>
#include<deque>
#include<fstream>
#include<iostream>
#include<map>
#include<set>
#include<string>
using namespace std;

// black = 0, red = 1, orange = 2, yellow = 3, green = 4, blue = 5
enum class Move { UpperLeft, UpperRight, LowerLeft, LowerRight, Switch };

struct Edge {
	Edge* parent = nullptr;
	Move move;
	Edge(Move _move) : move(_move) {}
	Edge(Move _move, Edge* _parent) : move(_move), parent(_parent) {}
};

class Config {
private:
	void printHidden() const {
		for (auto item : hidden)
			cout << item << "   ";
		cout << endl;
	}
public:
	bool isUp = false;
	array<short, 3> hidden;
	array<array<short, 2>, 5> upper, lower;
	Edge* edge = nullptr; // the past move which created the current configuration
	bool isLeaf = true;

	Config(array<array<short, 2>, 5> upper, array<array<short, 2>, 5> lower, array<short, 3> hidden) : upper(upper), lower(lower), hidden(hidden) {}
	Config(array<short, 3> hidden, array<array<short, 2>, 5> upper, array<array<short, 2>, 5> lower) : upper(upper), lower(lower), hidden(hidden), isUp(true) {}
	Config(Config const& og, Edge* _edge) : upper(og.upper), lower(og.lower), hidden(og.hidden), isUp(og.isUp), edge(_edge) {
		switch (edge->move) {
			case Move::UpperLeft:
				ranges::rotate(upper, upper.begin() + 1);
				break;
			case Move::UpperRight:
				ranges::rotate(upper, upper.end() - 1);
				break;
			case Move::LowerLeft:
				ranges::rotate(lower, lower.begin() + 1);
				break;
			case Move::LowerRight:
				ranges::rotate(lower, lower.end() - 1);
				break;
			case Move::Switch:
				isUp = !isUp;
				for (int index = 0; index < 3; ++index) {
					short& item = hidden[index];
					array<short, 2>& _upper = upper[index * 2];
					array<short, 2>& _lower = lower[index * 2];
					if (isUp) {
						swap(item, _lower[1]);
						swap(item, _lower[0]);
						swap(item, _upper[1]);
						swap(item, _upper[0]);
					} else {
						swap(item, _upper[0]);
						swap(item, _upper[1]);
						swap(item, _lower[0]);
						swap(item, _lower[1]);
					}
				}
		}

		print();
	}

	void print() const {
		if (isUp) printHidden();
		for (int index = 0; index < 4; ++index) {
			const array<array<short, 2>, 5>* curr = index < 2 ? &upper : &lower;
			for (auto const& item : *curr)
				cout << item[index % 2] << " ";
			cout << endl;
		}

		if (!isUp) printHidden();
		cout << endl;
	}
};

inline bool operator< (const Config& lhs, const Config& rhs) { return lhs.isUp < rhs.isUp || (lhs.isUp == rhs.isUp && (lhs.upper < rhs.upper || (lhs.upper == rhs.upper && (lhs.lower < rhs.lower || (lhs.lower == rhs.lower && lhs.hidden < rhs.hidden))))); }
inline bool operator==(const Config& lhs, const Config& rhs) { return lhs.isUp == rhs.isUp && lhs.upper == rhs.upper && lhs.lower == rhs.lower && lhs.hidden == rhs.hidden; }

// emulates find_first_of
map<Config, int>::const_iterator find(map<Config, int>& a, map<Config, int> const& b) {
	for (auto const& [element, isLeaf] : b) {
		map<Config, int>::const_iterator it = a.find(element);
		if (it != a.end()) {
			element.print();
			it->first.print();
			return it;
		}
	}

	return a.end();
}

/*
{ { {{5, 4}}, {{4, 1}}, {{2, 5}}, {{5, 2}}, {{2, 4}} } },
							{ { {{3, 1}}, {{1, 3}}, {{4, 2}}, {{0, 1}}, {{0, 0}} } },
							{ {	3,					3,					5} }
*/

int main() {
	Config begin = Config(	{ {	4,					2,					1} } , 
							{ { {{4, 5}}, {{3, 3}}, {{2, 4}}, {{5, 5}}, {{1, 2}} } },
							{ { {{5, 0}}, {{1, 1}}, {{4, 0}}, {{3, 3}}, {{2, 0}} } });
	Config end = Config(	{ { {{5, 5}}, {{1, 1}}, {{4, 4}}, {{3, 3}}, {{2, 2}} } },
							{ { {{5, 5}}, {{1, 1}}, {{4, 4}}, {{3, 3}}, {{2, 2}} } },
							{ {	0,					0,					0} });
	map<Config, int>::const_iterator center;

	// meet-in-the-middle: one set works forward to the solution, one set works backward from the solution
	map<Config, int> forward = {	{ begin, 2 }, 
									{ Config(begin, new Edge(Move::UpperLeft)), true },
									{ Config(begin, new Edge(Move::UpperRight)), true },
									{ Config(begin, new Edge(Move::LowerLeft)), true},
									{ Config(begin, new Edge(Move::LowerRight)), true },
									{ Config(begin, new Edge(Move::Switch)), true } };
	map<Config, int> backward = {	{ end, 2 }, 
									{ Config(end, new Edge(Move::UpperLeft)), true },
									{ Config(end, new Edge(Move::UpperRight)), true },
									{ Config(end, new Edge(Move::LowerLeft)), true },
									{ Config(end, new Edge(Move::LowerRight)), true },
									{ Config(end, new Edge(Move::Switch)), true } };

	for (bool flipflop = true, flag = false; (center = find(forward, backward)) == forward.end(); flipflop = !flipflop) {
		if (flipflop) flag = !flag;
		map<Config, int>* curr = flipflop ? &forward : &backward; // automatically terminates repeated paths
		for (auto& [element, isLeaf] : *curr) {
			if (isLeaf == flag) {
				cout << "-----" << endl << endl;
				element.print();
				cout << "-----" << endl << endl;

				Edge* edge = element.edge;
				isLeaf = 2;

				if (edge->move != Move::UpperRight) {
					Config temp = Config(element, new Edge(Move::UpperLeft, edge));
					auto it = curr->find(temp);
					if (it != curr->end()) it->first.print();
					(*curr)[temp] = !flag;
				}

				if (edge->move != Move::UpperLeft) {
					Config temp = Config(element, new Edge(Move::UpperRight, edge));
					auto it = curr->find(temp);
					if (it != curr->end()) it->first.print();
					(*curr)[temp] = !flag;
				}

				if (edge->move != Move::LowerRight) {
					Config temp = Config(element, new Edge(Move::LowerLeft, edge));
					auto it = curr->find(temp);
					if (it != curr->end()) it->first.print();
					(*curr)[temp] = !flag;
				}

				if (edge->move != Move::LowerLeft) {
					Config temp = Config(element, new Edge(Move::LowerRight, edge));
					auto it = curr->find(temp);
					if (it != curr->end()) it->first.print();
					(*curr)[temp] = !flag;
				}

				if (edge->move != Move::Switch) { 
					Config temp = Config(element, new Edge(Move::Switch, edge));
					auto it = curr->find(temp);
					if (it != curr->end()) it->first.print();
					(*curr)[temp] = !flag;
				}
			}
		}
	}

	deque<string> graph;
	map<Move, string> table;

	table = { { Move::UpperLeft, "UpperLeft" }, { Move::UpperRight, "UpperRight" }, { Move::LowerLeft, "LowerLeft" }, { Move::LowerRight, "LowerRight" }, { Move::Switch, "Switch" } };
	for (Edge* edge = forward.find(center->first)->first.edge; edge; edge = edge->parent)
		graph.push_front(table[edge->move]);

	table = { { Move::UpperLeft, "UpperRight" }, { Move::UpperRight, "UpperLeft" }, { Move::LowerLeft, "LowerRight" }, { Move::LowerRight, "LowerLeft" }, { Move::Switch, "Switch" } };
	for (Edge* edge = backward.find(center->first)->first.edge; edge; edge = edge->parent)
		graph.push_back(table[edge->move]);

	fstream solution("solution.txt", ios::trunc | ios::out);
	for (string item : graph)
		solution << item << endl;

	solution.close();
	system("pause");
	return 0;
}