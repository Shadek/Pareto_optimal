#ifndef DOMINANCE_RELATION_H_INCLUDED
#define DOMINANCE_RELATION_H_INCLUDED

class dominance_graph{
public:
    int dominance_value[OBJECT_SIZE][OBJECT_SIZE];
    void resetAll(){
        memset(dominance_value,-1,sizeof(dominance_value));
    }
    void set_dominance(int obj1, int obj2,int value){
        if(value == INDIFFERENT){
            dominance_value[obj1][obj2] = dominance_value[obj2][obj1] = INDIFFERENT;
        }else{
            dominance_value[obj1][obj2] = value;
            dominance_value[obj2][obj1] = 1^value; //value is opposite in other direction
        }
    }
    int get_dominance(int obj1, int obj2){
        return dominance_value[obj1][obj2];
    }
};

#endif // DOMINANCE_RELATION_H_INCLUDED
