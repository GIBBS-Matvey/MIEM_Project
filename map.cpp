#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <fstream>

size_t col = 1000;
size_t rows = 1000;


class Obstacle {
public:
    virtual size_t get_weight() const = 0;
};


class Usual_Area : public Obstacle {
public:
    size_t get_weight() const override {
        return 1;
    }
};


class Forest : public Obstacle {
public:
    size_t get_weight() const override {
        return 5;
    }
};


class Stone : public Obstacle {
public:
    size_t get_weight() const override {
        return -1;
    }
};


class Movement {
private:
    bool status;
    double weight;
public:
    Movement(bool status, size_t weight) : status(status), weight(weight) {}

    bool get_status() const {
        return status;
    }

    double get_weight() const {
        return weight;
    }

    void set_status(bool new_status) {
        status = new_status;
    }

    void set_weight(double new_weight) {
        weight = new_weight;
    }
};


class Position {
private:
    int x;
    int y;
    Obstacle* obs_type;

public:
    Position() : x(0), y(0), obs_type(nullptr) {}

    Position(double x, double y, Obstacle* obs_type = nullptr) : x(x), y(y), obs_type(obs_type) {}

    size_t get_hash() const {
        return x * col + y;
    }

    int get_x() const {
        return x;
    }

    int get_y() const {
        return y;
    }

    Obstacle* get_obs() const {
        return obs_type;
    }

    friend bool operator==(const Position& p_1, const Position& p_2) {
        return (p_1.x == p_2.x && p_1.y == p_2.y);
    }

    bool is_diagonal(const Position& other) const {
        if (abs(x - other.x) == 1 && abs(y - other.y) == 1) {
            return true;
        } else {
            return false;
        }
    }

    std::pair<Position, Position> get_two_com_nei(const Position& other) const {
        std::pair<Position, Position> com_nei;
        if (other.y < y && other.x < x) {
            com_nei.first = Position(x, y - 1);
            com_nei.second = Position(x - 1, y);
        }
        else if (other.y > y && other.x < x) {
            com_nei.first = Position(x - 1, y);
            com_nei.second = Position(x, y + 1);
        }
        else if (other.y > y && other.x > x) {
            com_nei.first = Position(x, y + 1);
            com_nei.second = Position(x + 1, y);
        }
        else if (other.y < y && other.x > x) {
            com_nei.first = Position(x, y - 1);
            com_nei.second = Position(x + 1, y);
        }

        return com_nei;
    }
};


namespace std {
    template<>
    class hash<Position> {
    public:
        size_t operator()(const Position& pos) const {
            return pos.get_hash();
        }
    };
}


class Map {
private:
    std::unordered_set<Position> obs_set;

public:
    Map(const std::vector<std::vector<char>>& matrix) {
        for (int i = 0; i < matrix.size(); ++i) {
            for (int j = 0; j < matrix[0].size(); ++j) {
                if (matrix[i][j] == '*') {
                    obs_set.insert(Position(i, j, new Forest));
                }
                else if (matrix[i][j] == '@') {
                    obs_set.insert(Position(i, j, new Stone));
                }
            }
        }
    }

    // (todo) for square around position "to"
    Movement get_weight(const Position& from, const Position& to) {
        Movement move(true, 1);

        auto it = obs_set.find(to);

        if (from.is_diagonal(to)) {
            std::pair<Position, Position> nei = from.get_two_com_nei(to);
            Position com_nei_1 = nei.first;
            Position com_nei_2 = nei.second;


            auto nei_it_1 = obs_set.find(com_nei_1);
            auto nei_it_2 = obs_set.find(com_nei_2);

            if (nei_it_1 != obs_set.end() && nei_it_2 != obs_set.end() &&
            (nei_it_1->get_obs()->get_weight() == -1) && (nei_it_2->get_obs()->get_weight() == -1)) {

                move.set_status(false);
            }

            if (it != obs_set.end()) {
                move.set_weight(pow(2, 0.5) * double(it->get_obs()->get_weight()));
            }
        } else {
            if (it != obs_set.end()) {    /// if there is an obstacle
                if (it->get_obs()->get_weight() == -1) {    /// there is a stone
                    move.set_status(false);
                }

                move.set_weight(it->get_obs()->get_weight());
            }
        }


        return move;
    }

    void print() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < col; ++j) {
                auto it = obs_set.find(Position(i, j));
                if (it != obs_set.end()) {
                    if (it->get_obs()->get_weight() == -1) {
                        std::cout << '@' << ' ';
                    }
                    else if (it->get_obs()->get_weight() == 5) {
                        std::cout << '*' << ' ';
                    }
                } else {
                    std::cout << '_' << ' ';
                }
            }
            std::cout << std::endl;
        }
    }
};


int main() {
    std::ifstream fin("C:\\Users\\PC\\CLionProjects\\untitled\\input.txt");
    fin >> rows >> col;
    std::vector<std::vector<char>> matrix;
    matrix.resize(rows, std::vector<char>());
    for (auto& line : matrix) {
        line.resize(col);
    }
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < col; ++j) {
            fin >> matrix[i][j];
        }
    }

    Map map(matrix);
    map.print();

    std::vector<std::pair<Position, Position>> pos_checking = {{{0, 2}, {0, 3}},    /// impossible
                                                               {{0, 4}, {0, 5}},    /// 5
                                                               {{1, 3}, {0, 2}},    /// impossible
                                                               {{0, 0}, {0, 1}},    /// 1
                                                               {{2, 3}, {1, 4}},    /// 5 * 1.44
                                                               {{1, 7}, {0, 6}},    /// 1
                                                               {{1, 7}, {2, 6}}};   /// impossible
   for (const auto& [from, to] : pos_checking) {
       Movement movement = map.get_weight(from, to);
       if (movement.get_status()) {
           std::cout << movement.get_weight() << '\n';
       } else {
           std::cout << "impossible" << '\n';
       }
   }
   return 0;
}

