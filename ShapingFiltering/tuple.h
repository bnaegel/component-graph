//
// Created by Benoit Naegel on 05/01/2018.
//

#ifndef tuple_h
#define tuple_h


// Tuple class
template <class T, int N>
class Tuple {
private:
    T el[N];

public:
    Tuple(){};
    Tuple(const Tuple &v){
        for(int i=0; i<N; ++i) el[i]=v.el[i];
    }
    Tuple(int p) {
        for(int i=0; i<N; ++i) el[i]=p;
    }
    Tuple(const T t[]) {
        for(int i=0; i<N; i++) el[i]=t[i];
    }


    T &operator[](int i){
        return el[i];
    }

    T operator[](int i) const{
        return el[i];
    }

    bool operator==(const Tuple &v){
        for(int i=0; i<N; ++i) {
            if(v.el[i]!=el[i])
                return false;
        }
        return true;
    }

    bool operator==(const Tuple &v)const {
        for(int i=0; i<N; ++i) {
            if(v.el[i]!=el[i])
                return false;
        }
        return true;
    }

    bool operator<=(const Tuple &v) const {
        for(int i=0; i<N; ++i) {
            if(el[i]>v.el[i])
                return false;
        }
        return true;
    }
};

#endif
