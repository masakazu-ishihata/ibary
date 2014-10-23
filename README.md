# ibary

rank/select の実装。  
rank は O(1) だけど、select はサボってるので O(log n) なのです。  
勉強で実装したので実用的ではないです。  

## method
### new / clone / free

    ibary *ibary_new(size_t _n);

_n bit のビット配列 _b[0,_n) を定義する。  

    ibary *ibary_clone(ibary *_b);

ビット配列 _b[0,_n) の clone を返す。  

    void ibary_free(ibary *_b);

ビット配列 _b[0,_n) を free する。


### access / rank / select

    uc ibary_access(ibary *_b, ul _i);

_b[_i,_i+1) = _b[i] を返す。  

    ul ibary_rank(ibary *_b, uc _v, ul _i);

ビット配列 _b[0,_i) に含まれる _v の数を返す。  

    ul ibary_select(ibary *_b, uc _v, ul _i);

ビット配列 _b の _i+1 番目の _v の位置を返す。


### irank / iselect

    ul ibary_irank(ibary *_b, uc _v, ul _s, ul _t);

部分ビット配列 _b[_s,_t) に含まれる _v の数を返す。

    ul ibary_iselect(ibary *_b, uc _v, ul _s, ul _t);

部分ビット配列 _b[_s, n) の _i 番目の _v の位置を返す。


### accessors

    ul   ibary_size(ibary *_b);

ビット配列 _b[0,_n) の長さ _n を返す。

    uc   ibary_get(ibary *_b, ul _i);

ビット配列 _b[0,_n) の _i ビット目を取得する。  
(access と等価)

    void ibary_set(ibary *_b, ul _i, uc _v);

ビット配列 _b[0,_n) の _i ビット目を _v にする。

    void ibary_set_num(ibary *_b, ul _n);

ビット配列 _b[0,_n) の値を _n の２進表現にする。

### counters

    ul   ibary_count(ibary *_b, uc _v);

ビット列 _b[0,_n) 中の _v の数を返す。

    ul   ibary_count2(ibary *_a, ibary *_b, uc _v, uc _u);

ビット列 _a[0,_n), _b[0,_n) において _a[i] = _v, _b[i] = _u なる i の数を返す。

    ul   ibary_count_pair(ibary *_a, ibary *_b, uc _v);


### operators

    void ibary_operator(ibary *_a, ibary *_b, ui (*opt)(ui, ui));

ビット配列 _a を _a opt _b へ変更する。

### distances

    double ibary_jaccard(ibary *_a, ibary *_b);

ビット配列 _a, _b の Jaccard 係数を返す。  
Jaccard(a, b) = |a & b| / |a v b|。  

    double ibary_cosine(ibary *_a, ibary *_b);

ビット配列 _a, _b の Cosine 類似度を返す。  
Cosine(a, b) = |a & b| / sqrt(|a||b|)。  

    int ibary_hamming(ibary *_a, ibary *_b);

ビット配列 _a, _b の Hamming 距離を返す。  
Hamming(a, b) = |a, b|_1 (L1 norm)。  

### util

    void ibary_clear(ibary *_b);

ビット配列 _b[0,_n) の要素をすべて 0 に初期化する。

    void ibary_copy(ibary *_a, ibary *_b);

ビット配列 _a[0,_n) にビット配列 _b[0,_n) をコピーする。

    ul  *ibary_indices(ibary *_b, uc _v);

I[i] = ibary_select(_b, _v, i) となる配列を返す。

    int  ibary_equal(ibary *_a, ibary *_b);

ビット配列 _a[0,_n), _b[0,_n) を比較する。

### string

    void ibary_string(ibary *_b, char *_str);

ビット配列 _b[0,_n) を文字列として _s にコピーする。

    void ibary_substring(ibary *_b, ul _s, ul _t, char *_str);

ビット配列 _b[_s,_t) を文字列として _str にコピーする。

    void ibary_bit2str(ui _b, char *_str);

数 _n を文字列として _str にコピーする。


## 使い方

基本的には new して set でコツコツ値を入れたら access/rank/select するだけ。  
詳細は [main.c][main] を参照。

[main]: https://github.com/masakazu-ishihata/ibary/blob/master/main.c "main.c"

