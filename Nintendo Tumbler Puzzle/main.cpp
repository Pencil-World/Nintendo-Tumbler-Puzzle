#include<algorithm>
#include<array>
#include<deque>
#include<fstream>
#include<iostream>
#include<map>
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
	bool isUp = false;
	array<short, 3> hidden;

	void printUpper() {

	}

	void printHidden() {
		for (auto item : hidden)
			cout << item << "   ";
		cout << endl;
	}

	void printLower() {

	}
public:
	array<array<short, 2>, 5> upper, lower;
	Edge* edge = nullptr; // the past move which created the current configuration
	bool isLeaf = true;

	Config(array<array<short, 2>, 5> upper, array<array<short, 2>, 5> lower, array<short, 3> hidden) : upper(upper), lower(lower), hidden(hidden) {}
	Config(Config const& og, Edge* _edge) : upper(og.upper), lower(og.lower), hidden(og.hidden), isUp(og.isUp), edge(_edge) {
		switch (edge->move) {
			case Move::UpperLeft:
				ranges::rotate(upper, upper.begin() + 1);
				return;
			case Move::UpperRight:
				ranges::rotate(upper, upper.end() - 1);
				return;
			case Move::LowerLeft:
				ranges::rotate(lower, lower.begin() + 1);
				return;
			case Move::LowerRight:
				ranges::rotate(lower, lower.end() - 1);
				return;
			case Move::Switch:
				isUp = !isUp;
				array<array<short, 2>, 5>::iterator _upper = upper.begin(), _lower = lower.begin();
				for (short& item : hidden) {
					if (isUp) {
						swap(item, (*_lower)[1]);
						swap(item, (*_lower)[0]);
						swap(item, (*_upper)[1]);
						swap(item, (*_upper)[0]);
					} else {
						swap(item, (*_upper)[0]);
						swap(item, (*_upper)[1]);
						swap(item, (*_lower)[0]);
						swap(item, (*_lower)[1]);
					}

					_upper += 2;
					_lower += 2;
				}
		}
	}

	void print() {
		cout << endl;
		if (!isUp) {
			for (auto item : hidden)
				cout << item << "   ";
			cout << endl;
		}
	}
};

inline bool operator< (const Config& lhs, const Config& rhs) { return lhs.upper < rhs.upper&& lhs.lower < rhs.lower; }
inline bool operator> (const Config& lhs, const Config& rhs) { return  operator< (rhs, lhs); }

// emulates find_first_of
map<Config, bool>::iterator find(map<Config, bool>& a, map<Config, bool> const& b) {
	for (auto const& [element, isLeaf] : a) {
		map<Config, bool>::iterator it;
		if (isLeaf && b.contains(element))
			return it;
	}

	return a.end();
}

int main() {
	Config begin = Config(	{ { {{5, 4}}, {{4, 1}}, {{2, 5}}, {{5, 2}}, {{2, 4}} } },
							{ { {{3, 1}}, {{1, 3}}, {{4, 2}}, {{0, 1}}, {{0, 0}} } },
							{ {	3,					3,					5} });
	Config end = Config(	{ { {{5, 5}}, {{1, 1}}, {{4, 4}}, {{3, 3}}, {{2, 2}} } },
							{ { {{5, 5}}, {{1, 1}}, {{4, 4}}, {{3, 3}}, {{2, 2}} } },
							{ {	0,					0,					0} });
	map<Config, bool>::iterator center;

	// meet-in-the-middle: one set works forward to the solution, one set works backward from the solution
	map<Config, bool> forward = {	{ Config(begin, new Edge(Move::UpperLeft)), true },
									{ Config(begin, new Edge(Move::UpperRight)), true },
									{ Config(begin, new Edge(Move::LowerLeft)), true},
									{ Config(begin, new Edge(Move::LowerRight)), true },
									{ Config(begin, new Edge(Move::Switch)), true } };
	map<Config, bool> backward = {	{ Config(end, new Edge(Move::UpperLeft)), true },
									{ Config(end, new Edge(Move::UpperRight)), true },
									{ Config(end, new Edge(Move::LowerLeft)), true },
									{ Config(end, new Edge(Move::LowerRight)), true },
									{ Config(end, new Edge(Move::Switch)), true } };

	// unroll for loop?
	for (bool flipflop = true; (center = find(forward, backward)) == forward.end(); flipflop = !flipflop) {
		map<Config, bool>* curr = flipflop ? &forward : &backward; // set automatically terminates repeated paths
		for (auto& [element, isLeaf] : *curr) {
			if (isLeaf) {
				Edge* edge = element.edge;
				isLeaf = false;
				if (edge->move != Move::UpperRight)
					forward.emplace(element, new Edge(Move::UpperLeft, edge));
				if (edge->move != Move::UpperLeft)
					forward.emplace(element, new Edge(Move::UpperRight, edge));
				if (edge->move != Move::LowerRight)
					forward.emplace(element, new Edge(Move::LowerLeft, edge));
				if (edge->move != Move::LowerLeft)
					forward.emplace(element, new Edge(Move::LowerRight, edge));
				if (edge->move != Move::Switch)
					forward.emplace(element, new Edge(Move::Switch, edge));
			} else {
				//delete
			}
		}
	}

	deque<Move> graph;
	for (Edge* edge = forward.find(center->first)->first.edge; edge; edge = edge->parent)
		graph.push_front(edge->move);
	for (Edge* edge = backward.find(center->first)->first.edge; edge; edge = edge->parent)
		graph.push_back(edge->move);


	//for (auto const& element : graph)
	//	cout << element << endl;

	system("pause");
	return 0;
}