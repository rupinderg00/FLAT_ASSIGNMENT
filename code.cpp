// 19CS10050 Rupinder Goyal
#include <iostream>
#include <fstream>
#include <string.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

// This function calculates the 2 raised to the power i.
unsigned int power(unsigned int i){
    if (i==0) return 1;
    unsigned int x = power(i/2);
    x= x*x;
    if (i%2==1) x = x*2;
    return x;
}

//  Given an array arr, this function find i in the array
//      Here right the index of the last element.
int binarySearch(unsigned int *arr, int i, int right){
    int left = 0;
    while (left<=right){
        int mid = (left+right)/2;
        if (arr[mid] == i) return mid;
        else if (arr[mid]>i) right = mid-1;
        else left = mid+1;
    }
    return -1;
}

class NFA
{
    int n, m, startState, finalState;
    unsigned int **delta;

public:
    NFA();
    void readNFA(char *);
    void print();
    friend class DFA;
};

NFA::NFA()
{
    this->startState = 0;
    this->finalState = 0;
}

void NFA::readNFA(char *path)
{
    //opening the required file
    ifstream file;
    file.open(path);
    if (!file.is_open())
    {
        cout << "FILE NOT FOUND!!\n\n";
        return;
    }

    //taking input from file
    file >> this->n;
    file >> this->m;

    // allocating memory and initializing delta function with 0
    this->delta = (unsigned int **)malloc(n * sizeof(unsigned int *));
    for (int i = 0; i < n; i++)
    {
        this->delta[i] = (unsigned int *)malloc(m * sizeof(unsigned int));
        for (int j = 0; j < m; j++)
        {
            this->delta[i][j] = 0;
        }
    }

    int x = 0;
    // reading the start state
    file >> x;
    while (x != -1)
    {
        this->startState |= (1U << x);
        file >> x;
    }

    // reading the final state
    file >> x;
    while (x != -1)
    {
        this->finalState |= (1U << x);
        file >> x;
    }

    // reading the delta function
    int start, input, output;
    file >> start;
    while (start != -1)
    {
        file >> input >> output;
        this->delta[start][input] |= (1U << output);
        file >> start;
    }
}

void NFA::print()
{
    cout << "+++Input NFA\n";
    cout << "\tNumber of states: " << this->n << endl;

    cout << "\tInput alphabets: {";
    for (int i = 0; i < m; i++)
    {
        cout << i;
        if (i != m - 1)
            cout << ",";
    }
    cout << "}\n";

    // Printing the start states
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if ((this->startState) & (1U << i))
            count++;
    }
    cout << "\tStartStates: {";
    for (int i = 0; i < n; i++)
    {
        if ((this->startState) & (1U << i))
        {
            cout << i;
            count--;
            if (count)
                cout << ",";
        }
    }
    cout << "}\n";

    // Printing the final states
    count = 0;
    for (int i = 0; i < n; i++)
    {
        if ((this->finalState) & (1U << i))
            count++;
    }
    cout << "\tFinalStates: {";
    for (int i = 0; i < n; i++)
    {
        if ((this->finalState) & (1U << i))
        {
            cout << i;
            count--;
            if (count)
                cout << ",";
        }
    }
    cout << "}\n";

    // Printing the transition Function
    cout << "\tTransition Function:\n";
    for (int start = 0; start < this->n; start++)
    {
        for (int inp = 0; inp < this->m; inp++)
        {
            cout << "\t\tDelta(" << start << "," << inp << "): {";
            count = 0;
            for (int i = 0; i < n; i++)
            {
                if ((this->delta[start][inp]) & (1U << i))
                    count++;
            }
            for (int i = 0; i < n; i++)
            {
                if ((this->delta[start][inp]) & (1U << i))
                {
                    cout << i;
                    count--;
                    if (count)
                        cout << ",";
                }
            }
            cout << "}\n";
        }
    }
}

class DFA{
    unsigned int n,m,_startState;
    unsigned int * _finalState;
    int _totalFinalStates;
    unsigned int ** _delta;
    void dfs(unsigned int * &, int &, unsigned int);
    public:
    DFA(NFA &);
    DFA(){};
    void subsetcons(NFA &);
    void print();
    void collapse(DFA &, bool **);
    unsigned int* findreachable(int &);
    void  rmunreachable(DFA &, unsigned int *, int &);
    bool ** findequiv();
};

DFA::DFA(NFA &nfa){

    this->n = power(nfa.n);
    this->m = nfa.m;
    this->_startState = nfa.startState;
    this->_finalState = (unsigned int *)malloc(this->n/32 * sizeof(unsigned int));
    for (unsigned int i=0;i<this->n/32;i++){
        this->_finalState[i]=0;
    }
    this->_delta = (unsigned int **)malloc(this->n * sizeof(unsigned int *));
    for (unsigned int i = 0; i < this->n; i++)
    {
        this->_delta[i] = (unsigned int *)malloc(this->m * sizeof(unsigned int));
        for (int j = 0; j < this->m; j++)
        {
            this->_delta[i][j] = 0;
        }
    }
    this->_totalFinalStates=0;
}

void DFA::subsetcons(NFA &nfa){

//  Creating the delta function for the DFA
    for (unsigned int i=0;i<this->n;i++){
        for (int j=0;j<32;j++){
            if ( i & (1U<<j) ){ 
                for (int k=0;k<m;k++){
                    this->_delta[i][k] |= nfa.delta[j][k];
                }
            }
        }
    }

// Creating the final states;
    for (unsigned int i=0; i< this->n;i++){
        if ( (nfa.finalState & i) >0){
            this->_finalState[i/32] |= (1U<<(i%32));
            this->_totalFinalStates++;
        }
    }
}

void DFA::print(){
    cout<<"\tNumber of States: "<<this->n<<endl;
    cout << "\tInput alphabets: {";
    for (int i = 0; i < this->m; i++)
    {
        cout << i;
        if (i != m - 1)
            cout << ",";
    }
    cout << "}\n";
    cout<<"\tStart State: "<<this->_startState<<endl;
    cout<<"\t"<<this->_totalFinalStates<<" Final States\n";
    if (this->_totalFinalStates <=100){
        cout << "\tFinalStates: {";
        int count = this->_totalFinalStates;
        for (int i=0; i<this->n;i++){
            if (this->_finalState[i/32] & (1U<< (i%32))){
                cout<<i;
                count--;
                if (count) cout<<", ";
                else break;
            }
        }
        cout << "}\n";
    }
    cout<<"\tTransition Function: ";
    if (this->n>100) cout<<"Skipped\n\n";
    else {
        cout<<endl;
        cout<<"a\\p|";
        for (int i=0;i<this->n;i++){
            cout<<"  "<<i;
        }
        cout<<"\n---+";
        for (int i=0;i<this->n;i++) cout<<"---";
        cout<<endl;
        for (int j=0;j< this->m;j++){
            cout<<" "<<j<<" |";
            for (unsigned int i=0;i<this->n;i++){
                if ( (this->_delta[i][j]>9 && i>9) || (this->_delta[i][j]<10 && i<10)) cout<<"  ";
                else if((this->_delta[i][j]>9 && i<=9)) cout<<" ";
                else cout<<"   ";
                cout<<this->_delta[i][j];
            }
            cout<<endl;
        }
    }
}

void DFA::dfs(unsigned int* &vis, int &count, unsigned int s){
    vis[s/32] |= (1U<<(s%32));
    count++;
    for (int i=0;i<this->m;i++){
        unsigned int t = this->_delta[s][i];
        if ( (vis[t/32] & (1U<<(t%32))) ==0 )
            dfs(vis,count, t);
    }
}

unsigned int* DFA::findreachable(int &count){
    unsigned int *vis;
    vis = (unsigned int *) malloc(this->n/32 * sizeof(unsigned int));
    for (unsigned int i=0; i< this->n/32;i++){
        vis[i]=0;
    }
    unsigned int *reducedStates;
    dfs(vis,count,this->_startState);
    reducedStates = (unsigned int *)malloc(count*sizeof(unsigned int));
    cout<<"+++ Reachable states: {";
    int tempCount=0;
    for (unsigned int i=0;i< this->n;i++){
        if ( (vis[i/32] & (1U<<(i%32))) ){
            cout<<i;
            reducedStates[tempCount] = i;
            tempCount++;
            if (tempCount!=count) cout<<", ";
            if (tempCount==count) break;
        }
    }
    cout<<"}\n";
    return reducedStates;
}

void DFA::rmunreachable(DFA &oldDFA, unsigned int * reducedStates, int &count){
    this->n = count;
    this->m = oldDFA.m;
    this->_totalFinalStates = 0;
    this->_startState = binarySearch(reducedStates,oldDFA._startState, count-1);
    this->_delta = (unsigned int **)malloc(this->n * sizeof(unsigned int *));
    for (unsigned int i = 0; i < this->n; i++)
    {
        this->_delta[i] = (unsigned int *)malloc(this->m * sizeof(unsigned int));
        for (int j = 0; j < this->m; j++)
        {
            this->_delta[i][j] = 0;
        }
    }
    this->_finalState = (unsigned int *)malloc((n+31)/32 * sizeof(unsigned int));
    for (int i=0;i< (n+31)/32;i++){
        this->_finalState[i]=0;
    }

// Stroring the transformed DFA delta
    for (int i=0;i<this->n;i++){
        for (int j=0;j<this->m;j++){
            int x = oldDFA._delta[reducedStates[i]][j];
            this->_delta[i][j] = binarySearch(reducedStates,x,count-1);
        }
    }

// Storing the final states 
    for (int i=0;i<this->n;i++){
        int x = reducedStates[i];
        if (oldDFA._finalState[x/32] & (1U<<(x%32))) {
            this->_totalFinalStates++;
            this->_finalState[i/32] |= (1U<< (i%32) );
        }
    }
}

bool ** DFA::findequiv(){
    bool **equivStates = (bool **)malloc(this->n*sizeof(bool *));
    for (int i=0;i<this->n;i++){
        equivStates[i] = (bool *)malloc(this->n* sizeof(bool));
        for (int j=0;j<this->n;j++) equivStates[i][j]= false;
    }
    for (int i=0;i<this->n;i++){
        for (int j=0;j<i;j++){
            if ( ( ( this->_finalState[i/32] & (1U<< (i%32)) )) &&  ( ( this->_finalState[j/32] & (1U<< (j%32)) ) == 0) ){
                equivStates[i][j] = true;
            }
            else if ( ( ( this->_finalState[i/32] & (1U<< (i%32)) ) ==0) &&  ( ( this->_finalState[j/32] & (1U<< (j%32)) ) ) ){
                equivStates[i][j] = true;
            }
        }
    }
    bool found = true;
    while (found == true){
        found = false;
        for (int i=0;i<this->n;i++){
            for (int j=0;j<i;j++){
                if (equivStates[i][j]==true) continue;
                for (int l=0;l<this->m;l++){
                    int a = this->_delta[i][l];
                    int b = this->_delta[j][l];
                    if (b==a) continue;
                    if (b>a) swap(a,b);
                    if (equivStates[a][b] == true) {
                        equivStates[i][j] = true;
                        found = true;
                    }
                }
            }
        }
    }
    return equivStates;
}

void DFA::collapse(DFA &dfa, bool ** equivStates){
    int *parent = (int *)malloc(dfa.n*sizeof(int));
    int *groupId = (int *)malloc(dfa.n*sizeof(int));
    int *groupHead;
    for (int i=0;i<dfa.n;i++) parent[i] = i;
    int count =0;
    for (int i=dfa.n-1;i>=0;i--){
        if (parent[i]!=i) continue;
        count++;
        for (int j=0;j<i;j++){
            if (equivStates[i][j] == false) parent[j] = i;
        }
    }
    groupHead = (int *) malloc( (count + 1) * sizeof(int ));
    cout<<"\n+++Equivalent States:\n";
    int temp_count =0;
    for (int i=0;i<dfa.n;i++){
        if (parent[i] == -1 || parent[parent[i]] == -1) continue;
        cout<<"Group "<<temp_count<<": {";
        for (int j=i;j<dfa.n;j++){
            if (parent[j]==parent[i]) {
                cout<<j;
                groupId[j] = temp_count;
                if (j==parent[i]) break;
                else cout<<",";
            }
        }
        parent[parent[i]] = -1;
        cout<<"}\n";
        temp_count++;
    }
    for (int i=0;i<dfa.n;i++) groupHead[groupId[i]] = i;
    // for (int i=0;i<dfa.n;i++) cout<<groupId[i]<<" ";
    this->n = count;
    this->m = dfa.m;
    this->_startState = groupId[dfa._startState];
    this->_finalState = (unsigned int *) malloc ( (this->n + 31)/32 * sizeof(unsigned int));
    this->_delta = (unsigned int **) malloc ( count * sizeof(unsigned int *));
    this->_totalFinalStates = 0;
    for (int i=0;i<dfa.n;i++){
        if ( dfa._finalState[i/32] & ( 1U<<(i%32))){
            int x = groupId[i];
            if ( this->_finalState[x/32] & (1U<<(x%32))) continue;
            this->_totalFinalStates++;
            this->_finalState[x/32] |= (1U<<(x%32));
        }
    }
    for (int i=0;i<this->n;i++){
        this->_delta[i] = (unsigned int *) malloc (this->m * sizeof(unsigned int));
        for (int j=0;j<this->m;j++){
            this->_delta[i][j] = groupId[dfa._delta[groupHead[i]][j]];
        }
    }
}

int main()
{
    auto start = high_resolution_clock::now();

    NFA n;
    char *s = strdup("testSample.txt");
    n.readNFA(s);
    n.print();
    DFA dfa(n);
    dfa.subsetcons(n);
    cout<<"\n\n+++ Converted DFA\n";
    dfa.print();
    int count = 0;
    unsigned int * reducedStates = dfa.findreachable(count);
    DFA newDFA;
    newDFA.rmunreachable(dfa,reducedStates, count);
    cout<<"\n\n+++Reduced DFA after removing unreachable states\n";
    newDFA.print();
    bool **equivStates = newDFA.findequiv(); 
    DFA collapsedDFA;
    collapsedDFA.collapse(newDFA, equivStates);
    cout<<"\n\n+++Reduced DFA after collapsing equivalent states\n";
    collapsedDFA.print();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken by function: "<< duration.count() << " seconds" << endl; 

}

