#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

#define EPS 1e-9

struct SegmentTree {
    int n;
    vector<int> maxv, lazy;
    vector<int> maxpos;

    SegmentTree(int sz) : n(sz), maxv(4 * sz), lazy(4 * sz), maxpos(4 * sz) {}

    void build(int node, int l, int r) {
        if (l == r) {
            maxpos[node] = l;
            return;
        }
        int m = (l + r) / 2;
        build(node * 2, l, m);
        build(node * 2 + 1, m + 1, r);
        pull(node);
    }

    void push(int node, int l, int r) {
        if (lazy[node]) {
            maxv[node] += lazy[node];
            if (l != r) {
                lazy[node * 2] += lazy[node];
                lazy[node * 2 + 1] += lazy[node];
            }
            lazy[node] = 0;
        }
    }

    void pull(int node) {
        if (maxv[node * 2] >= maxv[node * 2 + 1]) {
            maxv[node] = maxv[node * 2];
            maxpos[node] = maxpos[node * 2];
        } else {
            maxv[node] = maxv[node * 2 + 1];
            maxpos[node] = maxpos[node * 2 + 1];
        }
    }

    void range_add(int node, int l, int r, int ql, int qr, int val) {
        push(node, l, r);
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) {
            lazy[node] += val;
            push(node, l, r);
            return;
        }
        int m = (l + r) / 2;
        range_add(node * 2, l, m, ql, qr, val);
        range_add(node * 2 + 1, m + 1, r, ql, qr, val);
        pull(node);
    }

    pair<int, int> get_max() {
        return {maxv[1], maxpos[1]};
    }
};

struct Event {
    double x;
    int y1, y2;
    int type; // +1 for add, -1 for remove
    bool operator<(const Event& other) const {
        return x < other.x || (x == other.x && type > other.type);
    }
};

int main() {
    int n;
    cin >> n;
    vector<pair<double, double>> points(n);
    vector<double> y_vals;

    for (int i = 0; i < n; ++i) {
        cin >> points[i].first >> points[i].second;
        double y1 = points[i].second - 0.5;
        double y2 = points[i].second + 0.5;
        y_vals.push_back(y1);
        y_vals.push_back(y2);
    }

    // Coordinate compression
    sort(y_vals.begin(), y_vals.end());
    y_vals.erase(unique(y_vals.begin(), y_vals.end()), y_vals.end());

    auto getYIndex = [&](double y) {
        return lower_bound(y_vals.begin(), y_vals.end(), y) - y_vals.begin() + 1;
    };

    vector<Event> events;
    for (auto& [x, y] : points) {
        double x1 = x - 0.5 - EPS, x2 = x + 0.5 + EPS;
        double y1 = y - 0.5 - EPS, y2 = y + 0.5 +EPS;
        int y_start = getYIndex(y1);
        int y_end = getYIndex(y2);
        events.push_back({x1, y_start, y_end, +1});
        events.push_back({x2, y_start, y_end, -1});
    }

    sort(events.begin(), events.end());
    int m = y_vals.size();
    SegmentTree st(m + 2);
    st.build(1, 1, m);

    int max_count = 0;
    double best_x = 0.0, best_y = 0.0;

    for (auto& e : events) {
        st.range_add(1, 1, m, e.y1, e.y2, e.type);
        auto [cur_max, y_idx] = st.get_max();
        if (cur_max > max_count) {
            max_count = cur_max;
            best_x = e.x;
            best_y = y_vals[y_idx - 1];
        }
    }

    cout << "\n";
    cout << max_count << "\n";
    cout << fixed << setprecision(6);
    cout << best_x - 0.5 << " " << best_y - 0.5 << "\n";
    cout << best_x + 0.5 << " " << best_y - 0.5 << "\n";
    cout << best_x + 0.5 << " " << best_y + 0.5 << "\n";
    cout << best_x - 0.5 << " " << best_y + 0.5 << "\n";

    return 0;
}
