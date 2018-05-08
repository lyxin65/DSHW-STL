namespace Treap {
    struct Node {
        int r, lc, rc, i, val, sz;
    } T[MAXN];

    int pool[MAXN], pooln, pooltot;

#define r(x) T[x].r
#define lc(x) T[x].lc
#define rc(x) T[x].rc
#define id(x) T[x].i
#define val(x) T[x].val
#define sz(x) T[x].sz

    inline int newnode(int i, int v) {
        register int o;
        if(pooln) {
            o = pool[pooln];
            pool[pooln--] = 0;
        } else {
            o = ++pooltot;
        }
        r(o) = rand(); lc(o) = rc(o) = 0;
        id(o) = i; val(o) = v; sz(o) = 1;
        return o;
    }

    inline void delnode(int &o) {
        pool[++pooln] = o; o = 0;
    }

    inline void update(int o) {
        sz(o) = sz(lc(o)) + sz(rc(o)) + 1;
    }

    inline void left_rotate(int &o) {
        int k = rc(o);
        rc(o) = lc(k);
        lc(k) = o;
        update(o);
        update(k);
        o = k;
    }

    inline void right_rotate(int &o) {
        int k = lc(o);
        lc(o) = rc(k);
        rc(k) = o;
        update(o);
        update(k);
        o = k;
    }
    
    void insert(int &o, int i, int v) {
        if(o == 0) {
            o = newnode(i, v);
        } else if(v < val(o)) {
            insert(lc(o), i, v);
            if(r(o) < r(lc(o)))
                right_rotate(o);
        } else {
            insert(rc(o), i, v);
            if(r(o) < r(rc(o)))
                left_rotate(o);
        }
        update(o);
    }

    int kth(int o, int k) {
        if(o == 0 || k > sz(o)) return -1;
        int s = sz(lc(o)) + 1;
        if(k > s) return kth(rc(o), k - s);
        else if(k > sz(lc(o))) return id(o);
        else return kth(lc(o), k);
    }

    void mergeto(int &a, int &b) {
        if(b == 0) return;
        mergeto(a, lc(b));
        mergeto(a, rc(b));
        insert(a, id(b), val(b));
        delnode(b);
    }

    void prt(int o) {
        if(o == 0) return;
        prt(lc(o));
        fly("id:%d val:%d\n", id(o), val(o));
        prt(rc(o));
    }
}
