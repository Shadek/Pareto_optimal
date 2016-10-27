
#pragma comment(linker,"/STACK:16777216")
#pragma  warning ( disable: 4786)
#include <bits/stdc++.h>
using namespace std;
#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))
#define MX 1000000
#define forl(i,a,b) for ( i = a; i < b; i++)
#define fore(i,a,b) for ( i = a; i <= b; i++)
#define forrev(i,a,b) for ( i = a; i >= b; i--)
#define pb push_back
typedef long long  LL;
#define in(a,b,c) ((a) <= (b) && (b) <= (c))
#define ms(a,b) memset((a),(b),sizeof(a))
#define all(v) (v).begin(),(v).end()
#define pb push_back
#define ull unsigned long long int
typedef vector<int> vi;
typedef pair<int,int> pii;
typedef vector<pii> vpii;

string to_string(long long x) { stringstream ss; ss << x; return ss.str(); }
long long to_int(const string &s) { stringstream ss; ss << s; long long x; ss >> x; return x; }
#define MAX 100005

#include "constants.h"
#include "preference_relation.h"
#include "dominance_relation.h"

vector<int> objects;
int n; //object size
vector<int> criteria;
set<int> remaining_objects; //O?
set<int> notPareto; //Ox
set<int> pareto;

//array of graphs, one graph for each criterion
preference_graph *graphs;
dominance_graph dgraph; //among objects
/**for (x,y) question
return 0 if x>y, x dominates y
1 if x<y
2 for indifference x~y
*/
int CroudResult(){
    //int x_count=0;
    //int y_count=0;
    //int i;
    int indifferent=0;
    int r;
    /*forl(i,0,CROUD_SIZE){
        r = rand()%3;
        x_count += r==0;
        y_count += r==1;
        indifferent += r==2;
    }
    double a = x_count*1.0/CROUD_SIZE;
    double b = y_count*1.0/CROUD_SIZE;
    double c = indifferent*1.0/CROUD_SIZE;
    if(a>b && a>c)
        return PREFERRED_OVER;
    else if(b>a && b>c)
        return NOT_PREFERRED_OVER;
    else
        return INDIFFERENT_ON_C;*/
    r = rand()%3;
    if(r==0)
        return PREFERRED_OVER;
    else if(r==1)
        return NOT_PREFERRED_OVER;
    else
        return INDIFFERENT_ON_C;
}

int getElementAt(set<int> st, int index){
    set<int>::iterator it;
    int i;
    for(it = st.begin(),i=0; it != st.end(); it++,i++){
        if(i == index){
            return *it;
        }
    }
}

vector<int> generate_question_RandomQ(bool isFromremaining_objects){
      int i;
      vector<int> v;
      int totalTryCount = OBJECT_SIZE*OBJECT_SIZE*CRITERIA_SIZE;
      if(isFromremaining_objects && remaining_objects.size() == 1){
          return vector<int>();
      }
      set<int> st;
      if(isFromremaining_objects){ //first try to choose y from remaining_objects(O?)
          st = remaining_objects;
      }else{
          st = notPareto;
      }
      while(totalTryCount--){
          int index = rand()%remaining_objects.size();
          int x = getElementAt(remaining_objects, index); //x must belong to O?

          index = rand()%st.size();
          int y = getElementAt(st,index);
          while(x == y){
               index = rand()%st.size();
               y = getElementAt(st,index);
          }
          bool ok=true;
          forl(i,0,CRITERIA_SIZE){
              if(graphs[i].domination_checkFrom2D(x,y)){
                  ok=false;
                  break;
              }
          }
          if(ok==false)
              continue;
          int selected_criteria = rand()%CRITERIA_SIZE;
          //definition 3(i) of Page 5 in paper
          if(graphs[selected_criteria].doesRelationExist(x,y) || graphs[selected_criteria].doesRelationExist(y,x))
               continue;
          v.pb(y);
          v.pb(x);
          v.pb(selected_criteria);
          return v;
      }
      return vector<int>();
  };

pii getRandomPair(){ //(x,y) pair
    set<int> st;
    set<int>::iterator it;
    int x,y;
    for(it = pareto.begin(); it != pareto.end(); it++){
        st.insert(*it);
    }
    for(it = remaining_objects.begin(); it != remaining_objects.end(); it++){
        st.insert(*it);
    }
    if(st.size() == 1){
        //choose from non-pareto
        for(it = notPareto.begin(); it != notPareto.end(); it++){
            st.insert(*it);
        }
    }

    x = getElementAt(remaining_objects, rand()%remaining_objects.size());
    y = getElementAt(st,rand()%st.size());
    while(x == y){
        y = getElementAt(st,rand()%st.size());
    }
    return make_pair(x,y); //x?y
}
/**
select a criterion on pair (x,y)
return -1 if no criterion is available to pick up
*/
int getCriterion_OnPair(int x, int y){
    int c;
    forl(c,0,CRITERIA_SIZE){
        if(graphs[c].doesRelationExist(x,y) || graphs[c].doesRelationExist(y,x))
               continue;
        return c;
    }
    return -1;
}
//rule 2 page 6
int resolve_contradiction(vector<int> v, int res){
    int criterion = v[2];
    int z,x,y;
    if(res == PREFERRED_OVER){
        z = v[1], x = v[0];
    }else{ //NOT_PREFERRED_OVER, Opposite
        z = v[0], x = v[1];
    }
    int resolved_value=res; //keep croud value if no contradiction
    forl(y,0,OBJECT_SIZE){
        if(y==x || y==z)
            continue;
        if(graphs[criterion].indifference_checkFrom2D(x,y) && graphs[criterion].domination_checkFrom2D(y,z)){
            resolved_value = INDIFFERENT_ON_C;
            break;
        }
    }
    return resolved_value;
}

void print(vector<int> v){
    cout << char('a'+v[0]) << " ? " << char('a'+v[1]) << " over " << v[2] << endl;
}

/**
does y dominates x considering all criteria
*/
int checkIf_OneDominates_Another(int y, int x){
    bool atLeastOne=false;
    bool allCriteriaConsidered = true;
    bool isOpposite = false, all_INDIFFERENT=true;
    int c;
    int dvalue = dgraph.get_dominance(y,x); //earlier calculated value
    if(dvalue != -1){
        if(dvalue == DOMINATE)
            return 1;
        return 0;
    }

    forl(c,0,CRITERIA_SIZE){
        int v1 = graphs[c].getPreferenceValue(y,x);
        int v2 = graphs[c].getPreferenceValue(x,y);
        if(v1 == PREFERRED_OVER){
            atLeastOne = true;
            all_INDIFFERENT = false;
        }else{
            if(v2 == PREFERRED_OVER){
                isOpposite = true;
                all_INDIFFERENT = false; //current object y can't dominate x, because x dominates y by criterion c
            }
            else if(v1 == -1){
                allCriteriaConsidered = false; //no relation has been established yet between x and y by criterion c
                all_INDIFFERENT = false;
            }
        }
    }
    if(all_INDIFFERENT or (atLeastOne && isOpposite)){//indifferent
        dgraph.set_dominance(y,x,INDIFFERENT);
        return 0;
    }
    if(allCriteriaConsidered == false ){
        return NOT_ENOUGH_INFO;
    }
    if(atLeastOne && !isOpposite){
        dgraph.set_dominance(y,x,DOMINATE);
        return 1;
    }
    dgraph.set_dominance(y,x,DOMINATED);
    return 0;
}

bool checkIfNon_pareto_optimal(int x){
    int y;
    forl(y,0,OBJECT_SIZE){
        if(y==x)
            continue;
        int result = checkIf_OneDominates_Another(y,x);
        if(result==1)
            return true;
    }
    return false;
}

bool checkIfPareto_Optimal(int x){
    int y;
    forl(y,0,OBJECT_SIZE){
        if(y==x)
            continue;
        int result = checkIf_OneDominates_Another(y,x);
        if(result==1 || result == NOT_ENOUGH_INFO) //x can't be pareto optimal
            return false;
    }
    return true;
}
bool isSamePair(pii p1, pii p2){
    if(p1.first == p2.first && p1.second == p2.second)
        return true;
    if(p1.first == p2.second && p1.second == p2.first)
        return true;
    return false;
}
void print_pair(pii p){
    cout << char('a'+p.first) << " " << char('a'+p.second) << endl;
}
int main(void)
{
    //freopen("1.txt", "r", stdin);
    //freopen("2.txt", "w", stdout);
    srand(time(NULL));
    //a.foo();
    ofstream questions, pareto_file, nonPareto_file;
    ifstream croud_result;
    int i;
    set<pii> saved_pairs;
    map<int,char> preference_symbol;
    preference_symbol[0] = '<';
    preference_symbol[1] = '>';
    preference_symbol[2] = '~';
    graphs = new preference_graph[CRITERIA_SIZE];
    forl(i,0,CRITERIA_SIZE){
        graphs[i].resetAll();
    }
    dgraph.resetAll();
    forl(i,0,OBJECT_SIZE){
        remaining_objects.insert(i);
    }
    questions.open("questions.txt");
    pareto_file.open("pareto_file.txt");
    nonPareto_file.open("nonPareto_file.txt");
    croud_result.open("croud_result.txt");
    pareto_file << "Pareto Optimal Objects:\n\n";
    nonPareto_file << "Non Pareto Optimal Objects:\n\n";
    pii randomP=make_pair(-1,-1);
    pii prevPair = make_pair(-1,-1);
    int question_cnt=0;
    while(!remaining_objects.empty()){
        question_cnt++;
        vector<int> question;
        //RandomQ
        if(QUESTION_SELECTION_ALGO == 0){
            question = generate_question_RandomQ(true);
            if(question.size() == 0){
                question = generate_question_RandomQ(false); //have to choose Qscan2
            }
        }else if(QUESTION_SELECTION_ALGO == 1){ //RandomP
            if(randomP.first == -1){
                randomP = getRandomPair();
            }
            print_pair(randomP);
            int cr = getCriterion_OnPair(randomP.first, randomP.second);
            if(cr == -1){ //no criteria to choose for current x,y
                randomP = make_pair(-1,-1); //choose a different pair next iteration
                continue;
            }
            question.pb(randomP.first);
            question.pb(randomP.second);
            question.pb(cr);
        }
        print(question);
        int criterion = question[2];
        questions << char('a' + question[0]) << " ? " << char('a' +question[1]) << " over criterion " <<  criterion << endl;
        int res;
        //res = CroudResult();
        //croud_result >> res;
        cin >> res;
        if(res == 0 or res == 1){
            res = resolve_contradiction(question, res);
        }
        if(res == PREFERRED_OVER){ //y>x
            graphs[criterion].set_relation(question[1],question[0], PREFERRED_OVER);
        }else if(res == NOT_PREFERRED_OVER){ //x>y
            graphs[criterion].set_relation(question[0],question[1], PREFERRED_OVER);
        }else{
            graphs[criterion].set_relation(question[0],question[1], INDIFFERENT_ON_C);
        }
        questions << char('a' +question[0]) << preference_symbol[res] << char('a' +question[1]) << " over " << criterion << endl << endl;
        //transitive closure
        graphs[criterion].find_transitive_closure();
        set<int>::iterator it;
        vector<int> toBeRemoved;
        for(it = remaining_objects.begin(); it != remaining_objects.end(); it++){
            int obj = *it;
            if(checkIfNon_pareto_optimal(obj)){
                toBeRemoved.pb(obj);
                nonPareto_file << char('a'+obj) <<  endl;
                notPareto.insert(obj);
            }else if(checkIfPareto_Optimal(obj)){
                toBeRemoved.pb(obj);
                pareto_file << char('a'+obj) << endl;
                pareto.insert(obj);
            }
        }
        int vsize = toBeRemoved.size();
        forl(i,0,vsize){
            remaining_objects.erase(toBeRemoved[i]);
        }
    }
    questions.close();
    pareto_file.close();
    nonPareto_file.close();
    return 0;
}
