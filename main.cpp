#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cstdlib>
using namespace std;
using namespace std::chrono;

struct Task{
    string name;
    int difficulty;
    int duration;
    int deadline;
    vector<int>dependencies;
    int priorityScore;
};

class Node{
public:
    int data;
    Node* next;
    Node(int val){ data = val; next = NULL; }
};

class Queue{
    Node* front;
    Node* back;
public:
    Queue(){ front = back = NULL; }
    void push(int x){
        Node* n = new Node(x);
        if(!front){ front = back = n; return; }
        back->next = n;
        back = n;
    }
    int pop(){
        if(!front) return -1;
        Node* temp = front;
        int val = front->data;
        front = front->next;
        delete temp;
        return val;
    }
    bool empty(){ return !front; }
};

class maxHeap{
public:
    int size, capacity;
    int* taskIndex;
    double* priority;
    maxHeap(int cap=100){
        capacity=cap; size=0;
        taskIndex = new int[cap];
        priority = new double[cap];
    }
    int parent(int i){ return (i-1)/2; }
    int leftChild(int i){ return 2*i+1; }
    int rightChild(int i){ return 2*i+2; }

    void insert(int idx, double pri){
        if(size==capacity){ cout<<"heap full!\n"; return; }
        taskIndex[size]=idx; priority[size]=pri;
        int i=size; size++;
        while(i!=0 && priority[parent(i)]<priority[i]){
            swap(priority[i], priority[parent(i)]);
            swap(taskIndex[i], taskIndex[parent(i)]);
            i = parent(i);
        }
    }

    void maxHeapify(int i){
        int largest=i, l=leftChild(i), r=rightChild(i);
        if(l<size && priority[l]>priority[largest]) largest=l;
        if(r<size && priority[r]>priority[largest]) largest=r;
        if(largest!=i){
            swap(priority[i], priority[largest]);
            swap(taskIndex[i], taskIndex[largest]);
            maxHeapify(largest);
        }
    }

    void removeMax(){
        if(size<=0){ cout<<"heap empty!\n"; return; }
        taskIndex[0]=taskIndex[size-1];
        priority[0]=priority[size-1];
        size--;
        maxHeapify(0);
    }

    bool empty(){ return size==0; }
    ~maxHeap(){ delete[] taskIndex; delete[] priority; }
};

//dummy dataset
void generateDummyTasks(vector<Task>& tasks, int n=500){
    vector<string> subjects = {"Math","Physics","Chemistry","Programming","Data Structures",
        "Discrete Math","English","Islamic Studies","OOP","DSA Theory","Linear Algebra",
        "Computer Architecture","Communication Skills","Statistics","Software Engineering",
        "AI","Machine Learning","Compiler Construction","Digital Logic","Operating Systems"};

    tasks.resize(n);
    for(int i=0;i<n;i++){
        tasks[i].name = subjects[rand()%subjects.size()];
        tasks[i].difficulty = rand()%10+1;
        tasks[i].duration = rand()%5+1;
        tasks[i].deadline = rand()%30+1;

        int maxDeps = min(i, rand()%3);
        vector<int> used;
        for(int j=0;j<maxDeps;j++){
            int dep;
            do { dep = rand()%i; } while(find(used.begin(), used.end(), dep)!=used.end());
            used.push_back(dep);
            tasks[i].dependencies.push_back(dep);
        }
    }
}

int computePriorityScore(Task t){
    return (1.0/t.deadline + t.difficulty);
}

//graph for topological sort 
void buildGraph(const vector<Task>& tasks, vector<vector<int>>& graph, vector<int>& indegree){
    int n = tasks.size();
    graph.assign(n, vector<int>());
    indegree.assign(n,0);
    for(int i=0;i<n;i++){
        for(int dep: tasks[i].dependencies){
            graph[dep].push_back(i);
            indegree[i]++;
        }
    }
}

vector<int> topologicalSort(const vector<Task>&tasks, vector<vector<int>>& graph, vector<int>& indegree){
    Queue q; int n = tasks.size();
    for(int i=0;i<n;i++) if(indegree[i]==0) q.push(i);

    vector<int> topoOrder;
    while(!q.empty()){
        int node = q.pop();
        topoOrder.push_back(node);
        for(int neigh: graph[node]){
            indegree[neigh]--;
            if(indegree[neigh]==0) q.push(neigh);
        }
    }
    if(topoOrder.size()!=n) cout<<"Cycle detected!\n";
    return topoOrder;
}

//heap scheduling
vector<vector<int>> scheduleTasks(vector<Task>&tasks, vector<int>& topoOrder, int H=8){
    int n=tasks.size(); vector<bool> completed(n,false); vector<vector<int>> dailyPlan;
    int tasksRemaining=n;

    while(tasksRemaining>0){
        int hoursLeft=H; vector<int> today; maxHeap heap(n);
        for(int idx: topoOrder){
            if(completed[idx]) continue;
            bool canDo=true;
            for(int dep: tasks[idx].dependencies)
                if(!completed[dep]){ canDo=false; break; }
            if(canDo) heap.insert(idx, tasks[idx].priorityScore);
        }

        while(!heap.empty() && hoursLeft>0){
            int idx = heap.taskIndex[0]; int dur = tasks[idx].duration;
            if(dur<=hoursLeft){
                today.push_back(idx); hoursLeft-=dur;
                completed[idx]=true; tasksRemaining--;
                heap.removeMax();
            } else break;
        }
        dailyPlan.push_back(today);
    }
    return dailyPlan;
}

//merge sort
void merge(vector<Task>& tasks, int l, int m, int r){
    int n1 = m-l+1, n2=r-m;
    vector<Task> L(n1), R(n2);
    for(int i=0;i<n1;i++) L[i]=tasks[l+i];
    for(int i=0;i<n2;i++) R[i]=tasks[m+1+i];
    int i=0,j=0,k=l;
    while(i<n1 && j<n2){
        if(L[i].priorityScore<=R[j].priorityScore) tasks[k++]=L[i++];
        else tasks[k++]=R[j++];
    }
    while(i<n1) tasks[k++]=L[i++];
    while(j<n2) tasks[k++]=R[j++];
}
void mergeSort(vector<Task>& tasks, int l, int r){
    if(l<r){
        int m=l+(r-l)/2;
        mergeSort(tasks,l,m);
        mergeSort(tasks,m+1,r);
        merge(tasks,l,m,r);
    }
}

//earliest deadline first
bool compareDeadline(const Task& a, const Task& b){ return a.deadline < b.deadline; }
vector<Task> runEDF(vector<Task>& tasks){
    vector<Task> edfTasks = tasks;
    sort(edfTasks.begin(), edfTasks.end(), compareDeadline);
    return edfTasks;
}

void printOutput(const vector<Task>& tasks, const vector<vector<int>>& dailyPlan){
    cout<<"-----FINAL DAILY SCHEDULE-----\n";
    for(int day=0;day<dailyPlan.size();day++){
        cout<<"Day "<<day+1<<": ";
        for(int i=0;i<dailyPlan[day].size();i++){
            cout<<tasks[dailyPlan[day][i]].name;
            if(i!=dailyPlan[day].size()-1) cout<<", ";
        }
        cout<<endl;
    }
}

int main(){
    vector<Task> tasks;
    generateDummyTasks(tasks,500);
    for(auto &t: tasks) t.priorityScore = computePriorityScore(t);

    //topological sort + heap scheduling
    vector<vector<int>> graph; vector<int> indegree;
    buildGraph(tasks,graph,indegree);
    double topoTime = 0;
    vector<vector<int>> dailyPlan;
    auto start = high_resolution_clock::now();
    vector<int> topoOrder = topologicalSort(tasks,graph,indegree);
    dailyPlan = scheduleTasks(tasks, topoOrder);
    auto end = high_resolution_clock::now();
    topoTime = duration<double,milli>(end-start).count();
    printOutput(tasks,dailyPlan);
    cout<<"Topological Sort + Heap Scheduling Time: "<<topoTime<<" ms\n";

    //merge sort
    vector<Task> mergeTasks = tasks;
    start = high_resolution_clock::now();
    mergeSort(mergeTasks,0,mergeTasks.size()-1);
    end = high_resolution_clock::now();
    double mergeTime = duration<double,milli>(end-start).count();
    cout<<"Merge Sort Time: "<<mergeTime<<" ms\n";

    //earliest deadline first
    start = high_resolution_clock::now();
    vector<Task> edfTasks = runEDF(tasks);
    end = high_resolution_clock::now();
    double edfTime = duration<double,milli>(end-start).count();
    cout<<"Earliest Deadline First Time: "<<edfTime<<" ms\n";

    //summary table
    cout<<"\n----------------------------------------\n";
    cout<<"Algorithm                    Time (ms)\n";
    cout<<"----------------------------------------\n";
    cout<<"Topological Sort + Heap      "<<topoTime<<"\n";
    cout<<"Merge Sort                   "<<mergeTime<<"\n";
    cout<<"Earliest Deadline First      "<<edfTime<<"\n";
    cout<<"----------------------------------------\n";

    return 0;
}
