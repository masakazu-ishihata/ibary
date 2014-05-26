# ibary

rank/select の実装。  
rank は O(1) だけど、select はサボってるので O(log n) なのです。  
勉強で実装したので実用的ではないです。  

## method
### new / free

    ibary *ibary_new(size_t _n);

_n byte のビット配列を定義する。

    void ibary_free(ibary *_b);

ビット配列 _b を free する。

### accessor

    void ibary_set(ibary *_b, int _i, int _v);

ビット配列 _b の _i ビット目を _v にする。

    int  ibary_get(ibary *_b, int _i);

ビット配列 _b の _i ビット目を取得する。


### rank / select

    int ibary_rank(ibary *_b, int _v, int _i);

部分ビット配列 _b[0,_i] に含まれる _v の数を返す。
O(1)。

    int ibary_select(ibary *_b, int _v, int _i);

ビット配列 _b の _i 番目の _v の位置を返す。
O(log n)。


## 使い方

基本的には new して set でコツコツ値を入れたら rank/select するだけ。  
詳細は [main.c][main] を参照。

[main]: https://github.com/masakazu-ishihata/ibary/main.c "main.c"

