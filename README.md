# ibary

rank/select の実装。  
rank は O(1) だけど、select はサボってるので O(log n) なのです。  
勉強で実装したので実用的ではないです。  

## method
### new / free

    ibary *ibary_new(size_t _n);

_n byte のビット配列を定義する。  
つまりビット長 n =  8 * _n である。

    void ibary_free(ibary *_b);

ビット配列 _b を free する。

### accessor

    void ibary_set(ibary *_b, int _i, int _v);

ビット配列 _b の _i ビット目を _v にする。

    void ibary_set_num(ibary *_b, ui _n);

ビット配列 _b の値を _n の二進とする。  
_n は unsigned int なので 4 byte の整数。  

    int  ibary_get(ibary *_b, int _i);

ビット配列 _b の _i ビット目を取得する。


### rank / select

    int ibary_rank(ibary *_b, int _v, int _i);

部分ビット配列 _b[0,_i] に含まれる _v の数を返す。
O(1)。

    int ibary_select(ibary *_b, int _v, int _i);

ビット配列 _b の _i 番目の _v の位置を返す。
O(log n)。

### irank / iselect

    int ibary_rank(ibary *_b, int _v, int _s, int _t);

部分ビット配列 _b[_s,_t] に含まれる _v の数を返す。
O(1)。

    int ibary_select(ibary *_b, int _v, int _s, int _t);

部分ビット配列 _b[_s, -1] の _i 番目の _v の位置を返す。
O(log n)。

#### Jaccard index

    double ibary_jaccard(ibary *_a, ibary *_b);

ビット配列 _a, _b の Jaccard 係数を返す。  
Jaccard(a, b) = |a & b| / |a v b|。  
O(_n)。


## 使い方

基本的には new して set でコツコツ値を入れたら rank/select するだけ。  
詳細は [main.c][main] を参照。

[main]: https://github.com/masakazu-ishihata/ibary/blob/master/main.c "main.c"

