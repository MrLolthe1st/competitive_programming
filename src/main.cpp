#include <segtree.h>
#include <iostream>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
	int t;
	cin >> t;
	while (t--) {
		int n, k;
		cin >> n >> k;
		vector<int> a(n);
		for (auto& e : a) cin >> e, --e;
		auto mn = [](int a, int b) { return a < b ? a : b;  };
		TSegTree st(mn, a);
		vector<bool> was(k);
		vector<pair<int, int>> ans(k);
		for (int i = 0; i < n; ++i) {
			if (was[a[i]]) continue;
			was[a[i]] = 1;
			st.UpdateAt(a[i], i);
			ans[a[i]].first = st.Query(a[i], k - 1, mn, [](int x) {return x; });
		}
		was.assign(k, 0);
		for (int i = n - 1; i > -1; --i) {
			if (was[a[i]]) continue;
			was[a[i]] = 1;
			st.UpdateAt(a[i], -i);
			ans[a[i]].second = -st.Query(a[i], k - 1, mn, [](int x) {return x; });
		}
		for (int i = 0; i < k; ++i) {
			if (!was[i]) {
				cout << "0 ";
				continue;
			}
			cout << (1 + ans[i].second - ans[i].first) * 2 << " ";
		}
		cout << "\n";
	}
    /*
    std::vector<int> v(100);
    for (int i = 0; i < 100; ++i) {
        v[i] = -25+(rand() % 50);
    }
    auto sm = [](int a, int b){return a+b;};
    TSegTree s(sm, v);
    while (1) {
        if (rand() % 2) {
            size_t l = rand() % v.size(), r = std::min(l + (rand() % v.size()), v.size() - 1);
            auto s1 = s.Query(l, r, sm, MapIdenity<int>());
            int s2 = 0;
            while (l <= r) {
                s2 += v[l];
                ++l;
            }
            if (s1 != s2) {
                exit(-1);
            }
        } else {
            size_t i = rand() % v.size();
            int nw = -25+(rand() % 50);
            s.UpdateAt(i, nw);
            v[nw] = 1;
        }
    }*/
}