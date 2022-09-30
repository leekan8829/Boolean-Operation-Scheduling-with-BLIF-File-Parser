#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include "assert.h"
#include <algorithm>

using namespace std;

class Graph{
public:
    //map<string, set<string>> adjList_;
    map<string, vector<string>> adjList_;
    //map<dest, set<src>> adjList_;
    Graph(){}

    Graph(vector<string> src,vector<vector<string>> dest){
        for(int i=0;i<src.size();i++){
            adjList_.insert( make_pair(src[i],dest[i]) );
        }
    }

    void insert_edge(string src,string dest){
        adjList_[dest].push_back(src);
    }

    void print_graph(vector<string> dest){
        for(int i=0;i<dest.size();i++){
            cout << "dest: " << dest[i]<< ", src={ ";
            for(auto &s:adjList_[dest[i]]){
                cout << s << " ";
            }
            cout <<"}"<< endl;
        }
    }
};

void split_str(string s,vector<string> &s_vect){
    while (1){
        s_vect.push_back(s.substr(0, s.find(" ")));
        s = s.substr(s.find(" ")+1,s.length());
        if (s.find(" ") == -1){
            s_vect.push_back(s);
            break;
        }
    }
}

template<typename T>
void pop_front(std::vector<T>& vec)
{
    assert(!vec.empty());
    vec.erase(vec.begin());
}

void ASAP(Graph in_Graph,vector<string> dest_vec);
void ALAP(Graph in_Graph,vector<string> dest_vec);

int main(int argc, char *argv[])
{

    string line;
    string Filemode = argv[1];  // two mode -l and -r
    string Filename = argv[2];
    string And_or_Latency  = argv[3];
    string Or_con   = argv[4];
    string Not_con  = argv[5];

    vector<string> blif_everyline_data;
    ifstream myFile;
    myFile.open(Filename);
    //myFile.open("xor.blif");

    string model_;
    string input_;
    string output_;
    string result=""; //存放結果
    vector<string> input_vec;
    vector<string> output_vec;
    vector<string> name_vec; //存放每一行name

    while(getline(myFile, line)){
        blif_everyline_data.push_back(line);
    }
    
    myFile.close();

    model_ = blif_everyline_data[0];
    for(auto &line:blif_everyline_data){
        if(line.find(".inputs")!=-1){
            input_ = line;
            break;
        }
    }
    for(auto &line:blif_everyline_data){
        if(line.find(".outputs")!=-1){
            output_ = line;
            break;
        }
    }

    int first_name = 0;
    for(int i=0;i<blif_everyline_data.size();i++){
        if(blif_everyline_data[i].find(".names")!=-1){
            if(first_name==0){
                first_name = i;//紀錄第一個name的位置 
            }
            name_vec.push_back(blif_everyline_data[i]);
        }        
    }

    //cout << "first name " << first_name <<endl;
    //處理input output字串
    split_str(input_,input_vec);
    pop_front(input_vec);
    split_str(output_,output_vec);
    pop_front(output_vec);


    // ----------test area---------
    // test_Graph(vector<string> src,vector<set<string>> dest)

    Graph new_graph;
    vector<string> dest_vec; //放dest以便print
    for(auto &v:name_vec){
        vector<string> name_test; //暫時放src
        split_str(v,name_test);
        pop_front(name_test);   //把name去掉
        string dest;
        dest = name_test.back();
        name_test.pop_back();
        for(auto &src:name_test){
            new_graph.insert_edge(src,dest);
        }
        dest_vec.push_back(dest);
    }

    //打印graph關係圖
    //new_graph.print_graph(dest_vec);

    //map<dest,vector<boolean function>> boolean_function;
    //store boolean function
    vector<vector<string>> boolean_function;
    vector<string> every_dest_boolean;
    for(int i=first_name+1;i<blif_everyline_data.size();i++){
        if(blif_everyline_data[i].find(".names")!=-1 || blif_everyline_data[i].find(".end")!=-1){
            boolean_function.push_back(every_dest_boolean);
            every_dest_boolean.clear();
            continue; //跳到name下一行
        }
        else{
            every_dest_boolean.push_back(blif_everyline_data[i]);
        }
    }
    //把boolean_funcion的空白以及最後的0/1保存下來
    for(auto &des:boolean_function){
        for(auto &lines:des){
            char temp_str = lines.back();
            lines.pop_back();
            lines.pop_back();
            lines = lines + temp_str;
        }
    }

    //create a hash table for dest and boolean fuction
    map<string,int> hash_index;
    for(int i=0;i<dest_vec.size();i++){
        hash_index.insert(make_pair(dest_vec[i],i));
    }

    string in_node = "";
    cout << "Please input a node:";
    cin >> in_node;
    while (in_node.compare("0")!=0)
    {
        string predecessor;
        string successor;
        if(new_graph.adjList_[in_node].empty()){
            predecessor = predecessor +"-";
        }
        else{
            for(auto &pre:new_graph.adjList_[in_node]){
                predecessor = predecessor + pre +",";
            }
        }
        if(predecessor.back()==',')
            predecessor.pop_back();
        cout << "predecessor:" << predecessor <<endl;


        for(int i=0;i<dest_vec.size();i++){
            for(auto &suc:new_graph.adjList_[dest_vec[i]]){
                if(in_node==suc){
                    successor = successor + dest_vec[i] + ",";
                    break;
                }
            }
        }
        if(successor.empty()){
            successor = successor + "-";
        }
        if(successor.back()==',')
            successor.pop_back();
        cout << "successor:" << successor <<endl;
        cout << "Please input a node:";
        in_node.clear();
        cin >> in_node;
    }
    

    //boolean output
    for(int des=0;des<dest_vec.size();des++){
        // cout << new_graph.adjList_[dest_vec[des]].size() << endl;
        // //每個dest boolean 總共有幾列
        // cout << boolean_function[hash_index[dest_vec[des]]].size() << endl;

        vector<string> index_forbool;   //紀錄src node 以及 booleanfunction
        for(auto &s:new_graph.adjList_[dest_vec[des]]){
            index_forbool.push_back(s);
            // cout << s <<" ";
        }

        //temp存放每一個dest的boolean圖 並且用char存 不是用string
        vector<vector<char>> temp;
        for(auto &lines:boolean_function[hash_index[dest_vec[des]]]){
            vector<char> temp2;
            for(auto &ch:lines){
                temp2.push_back(ch);
            }
            temp.push_back(temp2);
        }


        result = result +dest_vec[des]+"= ";
        for(int i=0;i<boolean_function[hash_index[dest_vec[des]]].size();i++){
            if(temp[i][temp[i].size()-1] == '0') //因為temp的最後一個位元是決定dest ouput 是1or0
                result = result + "( ";
            for(int j=0;j<temp[i].size()-1;j++){
                if(temp[i][j]=='1'){
                    result = result + index_forbool[j] + " ";
                }
                else if(temp[i][j]=='-') continue;
                else if(temp[i][j]=='0'){
                    result = result + index_forbool[j] + "' ";
                }            
            }
            if(temp[i][temp[i].size()-1] == '0')
                result = result + ")' ";
            if(i==boolean_function[hash_index[dest_vec[des]]].size()-1) continue;
            result = result + "+";
        }
        result = result + "\n";
    }

    cout << "This is " << Filemode <<endl;
    cout << "AND_OR_NOT:"<< And_or_Latency << " " << Or_con << " " << Not_con <<endl;
    ASAP(new_graph,dest_vec);
    
    ofstream ofs;
    ofs.open("function.out",ios::out);
    ofs << "Node function:" <<endl;
    ofs << result;
    ofs << "END" <<endl;
    ofs.close();
    return 0;
}

void ASAP(Graph in_Graph,vector<string> dest_vec){
    //找出沒有predecessors的node
    //把它放到一個二維的vector[i][j]
    //i代表他在哪一個latency被完成的

    vector<vector <string>> Non_predecessor;
    vector<string> node_vec;        //放所有的node
    vector<string> no_pre_vec;      //放那些沒有pre的node
    for(auto &i:in_Graph.adjList_){     
        node_vec.push_back(i.first);
        for(auto &j:i.second){
            node_vec.push_back(j);
        }
    }

    for(auto &i:node_vec){          //traversal graph 把沒有先代的node 放到 no_pre_vec
        if(in_Graph.adjList_[i].empty()){
            vector<string>::iterator it;
            it = find(no_pre_vec.begin(),no_pre_vec.end(),i);
            if(it != no_pre_vec.end())
                continue;
            no_pre_vec.push_back(i);
        }
    }

    for(auto &i:no_pre_vec){
        cout << i << " ";
    }
    
    cout << endl;

    in_Graph.print_graph(dest_vec);
    cout << "------Before delete------"<<endl;
    for(auto &nonpre_node:no_pre_vec){      //將no_pre_vec裡的node 從graph中 remove
        for(auto &dest_node:dest_vec){
            int num = in_Graph.adjList_[dest_node].size();
            vector<string>::iterator it;    //initial iterator
            it = find(in_Graph.adjList_[dest_node].begin(),in_Graph.adjList_[dest_node].end(),nonpre_node); //找到欲刪除的位置
            if(it!=in_Graph.adjList_[dest_node].end()){
                //找到位置就刪掉
                it = in_Graph.adjList_[dest_node].erase(it);
            }
        }
    }
    in_Graph.print_graph(dest_vec);
    cout << "------After delete------"<<endl;
}