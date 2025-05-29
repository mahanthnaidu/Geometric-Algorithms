#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <iomanip>
#include <cmath>
#include <fstream>
using namespace std;

#define EPS 1e-9

struct Node {
    int l, r;
    int maxv = 0, maxpos = 0, lazy = 0;
    Node *left = nullptr, *right = nullptr;
    Node(int l_, int r_) : l(l_), r(r_) {}
    ~Node() {
        delete left;
        delete right;
    }
};

struct SegmentTree {
    Node* root;
    int n;
    SegmentTree(int sz) : n(sz) {
        root = build(1, n);
    }
    Node* build(int l, int r) {
        Node* node = new Node(l, r);
        if (l == r) {
            node->maxpos = l;
            return node;
        }
        int m = (l + r) / 2;
        node->left = build(l, m);
        node->right = build(m + 1, r);
        pull(node);
        return node;
    }
    void push(Node* node) {
        if(node->lazy){
            node->maxv += node->lazy;
            if(node->l != node->r){
                node->left->lazy += node->lazy;
                node->right->lazy += node->lazy;
            }
            node->lazy = 0;
        }
    }
    void pull(Node* node) {
        if (node->left->maxv >= node->right->maxv) {
            node->maxv = node->left->maxv;
            node->maxpos = node->left->maxpos;
        } else {
            node->maxv = node->right->maxv;
            node->maxpos = node->right->maxpos;
        }
    }
    void range_add(Node* node, int ql, int qr, int val) {
        push(node);
        if (qr < node->l || node->r < ql) return;
        if (ql <= node->l && node->r <= qr) {
            node->lazy += val;
            push(node);
            return;
        }
        range_add(node->left, ql, qr, val);
        range_add(node->right, ql, qr, val);
        pull(node);
    }
    void range_add(int ql, int qr, int val) {
        range_add(root, ql, qr, val);
    }
    pair<int, int> get_max() {
        return {root->maxv, root->maxpos};
    }
};

struct Event {
    double x;
    int y1, y2;
    int type; // +1 = insert, -1 = remove
    bool operator<(const Event& other) const {
        return x < other.x || (x == other.x && type > other.type);
    }
};

vector<double> x_vals, y_vals;
map<double, multiset<Event>> x_event_map;
set<pair<double, double>> active_points;

int getYIndex(double y) {
    return lower_bound(y_vals.begin(), y_vals.end(), y) - y_vals.begin() + 1;
}

void update_event_map(double x, double y, int type) {
    double x1 = x - 0.5 - EPS, x2 = x + 0.5 + EPS;
    double y1 = y - 0.5 - EPS, y2 = y + 0.5 + EPS;
    int y_start = getYIndex(y1);
    int y_end = getYIndex(y2);
    if(type == 1){
        x_event_map[x1].insert({x1, y_start, y_end, 1});
        x_event_map[x2].insert({x2, y_start, y_end, -1});
    }else{
        auto it1 = x_event_map[x1].find({x1, y_start, y_end, 1});
        x_event_map[x1].erase(it1);
        auto it2 = x_event_map[x2].find({x2, y_start, y_end, -1});
        x_event_map[x2].erase(it2);
    }
    if(type == 1){
        x_vals.push_back(x1);
        x_vals.push_back(x2);
    }else{
        auto it1 = find(x_vals.begin(), x_vals.end(), x1);
        x_vals.erase(it1);
        auto it2 = find(x_vals.begin(), x_vals.end(), x2);
        x_vals.erase(it2);
    }
}

void recompute(SegmentTree &st) {
    sort(x_vals.begin(), x_vals.end());
    x_vals.erase(unique(x_vals.begin(), x_vals.end()), x_vals.end());

    // Rebuild the segment tree
    delete st.root;
    st.root = st.build(1, y_vals.size());

    int max_count = 0;
    double best_x = 0, best_y = 0;

    for (double x : x_vals) {
        for (auto &e : x_event_map[x]) {
            st.range_add(e.y1, e.y2, e.type);
        }
        auto [cur_max, y_idx] = st.get_max();
        if (cur_max > max_count) {
            max_count = cur_max;
            best_x = x;
            best_y = y_vals[y_idx - 1];
        }
    }
    ofstream fout("out.txt");
    fout << fixed << setprecision(6);
    fout << active_points.size() << "\n";
    for (auto p : active_points) {
        fout << p.first << " " << p.second << "\n";
    }
    fout << max_count << "\n";
    fout << best_x - 0.5 << " " << best_y - 0.5 << "\n";
    fout << best_x + 0.5 << " " << best_y - 0.5 << "\n";
    fout << best_x + 0.5 << " " << best_y + 0.5 << "\n";
    fout << best_x - 0.5 << " " << best_y + 0.5 << "\n";
    fout << "\n";
    fout.close();
    system("python3 plot.py");
}

int main() {
    int n;
    cout << "Give number of initial points:";
    cin >> n;
    cout << "Give points in space seperated manner" << "\n";
    vector<pair<double, double>> input(n);
    for (int i = 0; i < n; ++i) {
        cin >> input[i].first >> input[i].second;
        double y1 = input[i].second - 0.5;
        double y2 = input[i].second + 0.5;
        y_vals.push_back(y1);
        y_vals.push_back(y2);
    }

    sort(y_vals.begin(), y_vals.end());
    y_vals.erase(unique(y_vals.begin(), y_vals.end()), y_vals.end());

    for (auto &[x, y] : input) {
        active_points.insert({x, y});
        update_event_map(x, y, +1);
    }

    SegmentTree st((int)y_vals.size() + 2);
    recompute(st);

    int choice;
    double x, y;
    cout << "Choose among the following actions:\n1)Insert point - input 1 x y\n2)Delete point - input 2 x y\n3)quit - input 3\nGive points in space seperated manner\n";
    while (cin >> choice) {
        if (choice == 1) {
            cin >> x >>y;
            if (!active_points.count({x, y})) {
                active_points.insert({x, y});
                update_event_map(x, y, +1);
            } else {
                cout << "Point already inserted.\n";
            }
        } else if (choice == 2) {
            cin >> x >> y;
            if (active_points.count({x, y})) {
                active_points.erase({x, y});
                update_event_map(x, y, -1);
            } else if (choice == 3) {
                cout << "Point not found.\n";
            }
        } else {
            break;
        }
        recompute(st);
        cout << "Choose among the following actions:\n1)Insert point\n2)Delete point\n3)quit\nGive points in space seperated manner\n";
    }
    return 0;
}
