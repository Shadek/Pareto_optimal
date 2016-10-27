#ifndef PREFERENCE_RELATION_H_INCLUDED
#define PREFERENCE_RELATION_H_INCLUDED

typedef pair<int,int> edge; //(vertex,cost)
//for edge (x,y), cost is defined as the following way
//0 if x dominates y, i.e x>y
//1 if y dominates x, i.e dominated
//2 if indifferent

class preference_graph{
private:
    int a,i,j,sz;
    edge pa;

    public:
    /**
    //-1 for not connected,
    0,1,2 for Dominate, Dominated by and indifferent respectively
    */
    int adj2D[OBJECT_SIZE][OBJECT_SIZE];
    vector<edge> adj[OBJECT_SIZE];
    preference_graph(){

    };
    void resetAll(){
        memset(adj2D,-1,sizeof(adj2D));
        int i;
        forl(i,0,OBJECT_SIZE){
            adj[i].clear();
        }
    }
    void set_a(int _a){
        a = _a;
    };
    int get_a(){
        return a;
    };
    /**
    check if a dominates b through transitive closure i.e a>b
    */
    bool isConnectedByStrictDomination(int a,int b){
        queue<int> pq;
        int i,x,sz;
        pq.push(a);
        bool found = false;
        while(!pq.empty()){
            x = pq.front();
            if(x == b){
                found = true;
                break;
            }
            pq.pop();
            sz = adj[x].size();
            forl(i,0,sz){
                if(adj[x][i].second == PREFERRED_OVER){ //x dominates adj[x][i].first
                    pq.push(adj[x][i].first);
                }
            }
        }
        return found;
    }
    void find_transitive_closure(){
        int i,j;
        forl(i,0,OBJECT_SIZE){
            forl(j,0,OBJECT_SIZE){
                if(i==j)
                    continue;
                if(adj2D[i][j] == -1){
                    if(isConnectedByStrictDomination(i,j)){
                        adj2D[i][j] = PREFERRED_OVER;
                        adj[i].pb(make_pair(j,PREFERRED_OVER));
                    }
                }
            }
        }
    }
    int getPreferenceValue(int a, int b){
        return adj2D[a][b];
    }
    void set_relation(int a, int b, int value){
        if(adj2D[a][b] != -1){//relation exists
            return;
        }
        if(value==PREFERRED_OVER){//a dominates b
            adj2D[a][b] = 0;
            adj2D[b][a] = 1;
            adj[a].pb(make_pair(b,0));
        }else if(value==NOT_PREFERRED_OVER){ //b dominates a
            adj2D[b][a] = 0;
            adj2D[a][b] = 1;
            adj[b].pb(make_pair(a,0));
        }else{
            adj2D[a][b] = adj2D[b][a] = INDIFFERENT_ON_C;
            adj[a].pb(make_pair(b,INDIFFERENT_ON_C));
            adj[b].pb(make_pair(a,INDIFFERENT_ON_C));
        }
    }
    bool indifference_checkFrom2D(int a, int b){
        if(adj2D[a][b] == INDIFFERENT_ON_C)
            return true;
        else
            return false;
    }
    /**
    check if (a dominates b) over certain criterion, i.e a>b
    a criterion is an instance of class preference_graph
    */
    bool domination_checkFrom2D(int a, int b){
        if(adj2D[a][b] == PREFERRED_OVER)
            return true;
        return false;
    };

    bool doesRelationExist(int a, int b){
        return adj2D[a][b] != -1;
    }

    void print_relation(){
        int i,j;
        forl(i,0,OBJECT_SIZE){
            forl(j,0,OBJECT_SIZE){
                cout << " " << adj2D[i][j];
            }
            cout << endl;
        }
    }


};


#endif // PREFERENCE_RELATION_H_INCLUDED
