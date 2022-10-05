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
    map<string, vector<string>> adjList_;
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
vector<vector<string>>  ALAP(Graph in_Graph,vector<string> dest_vec);

void ML_RCS(Graph in_Graph,vector<string> dest_vec,map<string,int> resource,
vector<vector<string>> boolean_function,map<string,int> hash_index);


void MR_LCS(Graph in_Graph,vector<string> dest_vec,map<string,int> resource,
vector<vector<string>> boolean_function,map<string,int> hash_index,int input_latency);

int main(int argc, char *argv[])
{

    string line;
    string Filemode = argv[1];  // two mode -l and -r
    string Filename = argv[2];


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


    //EDA PA2不需要
    // string in_node = "";
    // cout << "Please input a node:";
    // cin >> in_node;
    // while (in_node.compare("0")!=0)
    // {
    //     string predecessor;
    //     string successor;
    //     if(new_graph.adjList_[in_node].empty()){
    //         predecessor = predecessor +"-";
    //     }
    //     else{
    //         for(auto &pre:new_graph.adjList_[in_node]){
    //             predecessor = predecessor + pre +",";
    //         }
    //     }
    //     if(predecessor.back()==',')
    //         predecessor.pop_back();
    //     cout << "predecessor:" << predecessor <<endl;


    //     for(int i=0;i<dest_vec.size();i++){
    //         for(auto &suc:new_graph.adjList_[dest_vec[i]]){
    //             if(in_node==suc){
    //                 successor = successor + dest_vec[i] + ",";
    //                 break;
    //             }
    //         }
    //     }
    //     if(successor.empty()){
    //         successor = successor + "-";
    //     }
    //     if(successor.back()==',')
    //         successor.pop_back();
    //     cout << "successor:" << successor <<endl;
    //     cout << "Please input a node:";
    //     in_node.clear();
    //     cin >> in_node;
    // }
    

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


        result = result +dest_vec[des]+"=";
        for(int i=0;i<boolean_function[hash_index[dest_vec[des]]].size();i++){
            if(temp[i][temp[i].size()-1] == '0') //因為temp的最後一個位元是決定dest ouput 是1or0
                result = result + "(";
            for(int j=0;j<temp[i].size()-1;j++){
                if(temp[i][j]=='1'){
                    result = result + index_forbool[j] + "";
                }
                else if(temp[i][j]=='-') continue;
                else if(temp[i][j]=='0'){
                    result = result + index_forbool[j] + "'";
                }            
            }
            if(temp[i][temp[i].size()-1] == '0')
                result = result + ")'";
            if(i==boolean_function[hash_index[dest_vec[des]]].size()-1) continue;
            result = result + "+";
        }
        result = result + "\n";
    }



    // string Filemode = argv[1];  // two mode -l and -r
    // string Filename = argv[2];
    // string And_or_Latency  = argv[3];
    // string Or_con   = argv[4];
    // string Not_con  = argv[5];
    
    if(Filemode=="-l"){
        string And_or_Latency  = argv[3];
        string Or_con   = argv[4];
        string Not_con  = argv[5];
        cout << "Resource-constrained Scheduling" << endl;
        map<string,int> resource;
        resource.insert(make_pair("and",stoi(And_or_Latency)));
        resource.insert(make_pair("or",stoi(Or_con)));
        resource.insert(make_pair("not",stoi(Not_con)));
        ML_RCS(new_graph,dest_vec,resource,boolean_function,hash_index);
        cout << "END" << endl;
    }
    else if (Filemode=="-r"){
        string And_or_Latency  = argv[3];
        cout << "Latency-constrained Scheduling" << endl;
        map<string,int> resource;
        resource.insert(make_pair("and",1));
        resource.insert(make_pair("or",1));
        resource.insert(make_pair("not",1));
        MR_LCS(new_graph,dest_vec,resource,boolean_function,hash_index,stoi(And_or_Latency));
        cout << "END" << endl;
    }


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
    int asap_latency=0;
    vector<vector <string>> ASAP_OUT;
    vector<string> node_vec;        //放所有的node
    vector<string> no_pre_vec;      //放那些沒有pre的node
    for(auto &i:in_Graph.adjList_){     
        node_vec.push_back(i.first);
        for(auto &j:i.second){
            node_vec.push_back(j);
        }
    }

    //delete duplicate node
    sort( node_vec.begin(), node_vec.end() );
    node_vec.erase( unique( node_vec.begin(), node_vec.end() ), node_vec.end() );  

    int flag = 1;   //0 是找不到任何點要work了

    //asap algo start
    while(flag){     //flag = 0 就中斷
        asap_latency++; //calculate latency
        flag = 0;

        no_pre_vec.clear();
        for(auto &i:node_vec){          //traversal graph 把沒有先代的node 放到 no_pre_vec
            if(in_Graph.adjList_[i].empty()){
                vector<string>::iterator it;
                it = find(no_pre_vec.begin(),no_pre_vec.end(),i);
                if(it != no_pre_vec.end())
                    continue;
                no_pre_vec.push_back(i);
            }
        }

        for(auto &i:no_pre_vec){    //將node_vector裡面的沒有先代的去掉
            vector<string>::iterator it;
            it = find(node_vec.begin(),node_vec.end(),i);
            if(it!=node_vec.end())
                it = node_vec.erase(it);
        }

        // cout << "Nonpredecessor:";
        // for(auto &i:no_pre_vec){
        //     cout << i << " ";
        // }
        // cout << endl;

        for(auto &nonpre_node:no_pre_vec){      //將no_pre_vec裡的node 從graph中 remove
            for(auto &dest_node:dest_vec){
                vector<string>::iterator it;    //initial iterator
                it = find(in_Graph.adjList_[dest_node].begin(),in_Graph.adjList_[dest_node].end(),nonpre_node); //找到欲刪除的位置
                if(it!=in_Graph.adjList_[dest_node].end()){
                    //找到位置就刪掉
                    it = in_Graph.adjList_[dest_node].erase(it);
                    flag = 1;   //如果有delte就設1好讓他下個迴圈可以執行
               }
            }
        }
        ASAP_OUT.push_back(no_pre_vec);
        //in_Graph.print_graph(dest_vec);
    }
    for(int i=0;i<asap_latency;i++){
        cout << "cycle " << i+1 <<" complete:";
        for(auto &node:ASAP_OUT[i]){
            cout<<node<<" ";
        }
        cout << endl;
    }
    cout << "Complete ASAP latency:" << asap_latency << endl;
}

vector<vector<string>> ALAP(Graph in_Graph,vector<string> dest_vec){
    vector<string> node_vec;        //放所有的node
    vector<string> no_suc_vec;      //放沒有successor的node
    vector<vector <string>> ALAP_OUT;

    int latency = 1;

    for(auto &i:in_Graph.adjList_){     
        node_vec.push_back(i.first);
        for(auto &j:i.second){
            node_vec.push_back(j);
        }
    }
    //delete duplicate node
    sort( node_vec.begin(), node_vec.end() );
    node_vec.erase( unique( node_vec.begin(), node_vec.end() ), node_vec.end() ); 

    // for(auto &i:node_vec){
    //     cout << i << " ";
    // }
    // cout << endl;

    //in_Graph.print_graph(dest_vec);

    //cout << "----------------" <<endl;

    while (!node_vec.empty())
    {
        latency = latency + 1; 
        no_suc_vec.clear();
        for(auto &node:node_vec){
            int suc_flag=0;     //1等於有子代 0等於沒有子代
            for(auto &dest:dest_vec){
               vector<string>::iterator it;
               it = find(in_Graph.adjList_[dest].begin(),in_Graph.adjList_[dest].end(),node);
               if(it!=in_Graph.adjList_[dest].end()){
                    suc_flag=1;
                    //break;
                }
            }
            if(suc_flag==0){
                no_suc_vec.push_back(node);
            }
        }
        //cout <<"latency "<<latency<< " nonsucessor: ";
        // for(auto &i:no_suc_vec){
        //     cout << i << " ";
        // }
        //cout << endl;

        node_vec.clear();
        for(auto &i:no_suc_vec){
            for(auto &j:in_Graph.adjList_[i]){
                node_vec.push_back(j);
            }
            in_Graph.adjList_.erase(i);
        }
        sort( node_vec.begin(), node_vec.end() );
        node_vec.erase( unique( node_vec.begin(), node_vec.end() ), node_vec.end() ); 
        //in_Graph.print_graph(dest_vec);    
        ALAP_OUT.push_back(no_suc_vec);
    }

    // for(auto &i:ALAP_OUT){
    //     for(auto &j:i){
    //         cout << j << " ";
    //     }
    //     cout << endl;
    // }

    return ALAP_OUT;
}


void ML_RCS(Graph in_Graph,vector<string> dest_vec,map<string,int> resource,
vector<vector<string>> boolean_function,map<string,int> hash_index){

    vector<string> node_vec;        //放所有的node
    vector<string> no_pre_vec;      //放那些沒有pre的node
    map<string,vector<int>> node_content; //vector[0] = latency vector[1] = and vector[2] = or vector[1] = not 


    for(auto &i:in_Graph.adjList_){     
        node_vec.push_back(i.first);
        for(auto &j:i.second){
            node_vec.push_back(j);
        }
    }
    sort( node_vec.begin(), node_vec.end() );
    node_vec.erase( unique( node_vec.begin(), node_vec.end() ), node_vec.end() );


    //count dest node operation
    for(int des=0;des<dest_vec.size();des++){
        vector<string> index_forbool;           //紀錄src node 以及 booleanfunction
        string result="";                       //紀錄每一個dest的boolean function 用這個下去計算and or not

        for(auto &s:in_Graph.adjList_[dest_vec[des]]){  //將欲處理的dest的src放入indexforbool 之後可以取出使用在result
            index_forbool.push_back(s);
        }

        vector<vector<char>> temp;
        for(auto &lines:boolean_function[hash_index[dest_vec[des]]]){
            vector<char> temp2;
            for(auto &ch:lines){
                temp2.push_back(ch);
            }
            temp.push_back(temp2);
        }


        for(int i=0;i<boolean_function[hash_index[dest_vec[des]]].size();i++){
            for(int j=0;j<temp[i].size()-1;j++){
                if(temp[i][j]=='1'){
                    result = result + index_forbool[j];
                }
                else if(temp[i][j]=='-') continue;
                else if(temp[i][j]=='0'){
                    result = result + index_forbool[j] + "'";
                }            
            }
            if(i==boolean_function[hash_index[dest_vec[des]]].size()-1) continue;
            result = result + "+";
        }


        //cout << result << endl;


        //計算每個dest node所需要的and or not
        for(auto &node:node_vec){
            if(node==dest_vec[des]){
                node_content[node].resize(4);
                size_t found = result.find("+");
                if(found != string::npos){
                    node_content[node][2] = node_content[node][2] + 1;
                }
                else{
                    found = result.find("'");
                    if(found != string::npos){
                        node_content[node][3] = node_content[node][3] + 1;
                    }
                    else{
                        node_content[node][1] = node_content[node][1] + 1;
                    }
                }
                //cout << node << " " << node_content[node][1]<< " " << node_content[node][2] << " " << node_content[node][3] << endl;
            }
        }
    }


    //RUN RCS
    int level = 1;
    int and_count = 0;
    int or_count = 0;
    int not_count = 0;

    vector<string> and_ready;
    vector<string>  or_ready;
    vector<string>  not_ready;
    vector<string>  done_set;

    int flag = 1;   //0 是找不到任何點要work了
    

    // in_Graph.print_graph(dest_vec);
    // cout << endl;


    for(auto &i:node_vec){          //traversal graph 把沒有先代的node 放到 no_pre_vec
        if(in_Graph.adjList_[i].empty()){
            vector<string>::iterator it;
            it = find(no_pre_vec.begin(),no_pre_vec.end(),i);
            if(it != no_pre_vec.end())
                continue;
            no_pre_vec.push_back(i);
        }
    }
    for(auto &i:no_pre_vec){    //將node_vector裡面的沒有先代的去掉
        vector<string>::iterator it;
        it = find(node_vec.begin(),node_vec.end(),i);
        if(it!=node_vec.end())
            it = node_vec.erase(it);
    }
    for(auto &nonpre_node:no_pre_vec){      
        for(auto &dest_node:dest_vec){
            vector<string>::iterator it;    //initial iterator
            it = find(in_Graph.adjList_[dest_node].begin(),in_Graph.adjList_[dest_node].end(),nonpre_node); //找到欲刪除的位置
            if(it!=in_Graph.adjList_[dest_node].end()){
                //找到位置就刪掉
                it = in_Graph.adjList_[dest_node].erase(it);
            }
        }
    }

    while(flag){     //flag = 0 就中斷
        flag = 0;
        no_pre_vec.clear();
        done_set.clear();
        and_count=0;
        or_count=0;
        not_count=0;
        string output_str="";

        for(auto &i:node_vec){          //traversal graph 把沒有先代的node 放到 no_pre_vec
            if(in_Graph.adjList_[i].empty()){
                vector<string>::iterator it;
                it = find(no_pre_vec.begin(),no_pre_vec.end(),i);
                if(it != no_pre_vec.end())
                    continue;
                no_pre_vec.push_back(i);
            }
        }

        for(auto &i:no_pre_vec){    //將node_vector裡面的沒有先代的去掉
            vector<string>::iterator it;
            it = find(node_vec.begin(),node_vec.end(),i);
            if(it!=node_vec.end())
                it = node_vec.erase(it);
        }

        // // 去計算哪些operation可以做

        if(!no_pre_vec.empty()){
            for(auto &i:no_pre_vec){
                if(node_content[i][1]==1){      //加進 and ready set
                   and_ready.push_back(i);
                   //cout <<"and ready "<< i << endl;
                    continue;
                }
                else if(node_content[i][2]==1){     //加進 or ready set
                    or_ready.push_back(i);
                    //cout <<"or ready "<< i << endl;
                    continue;
                }
                else if(node_content[i][3]==1){     //加進 not ready set
                    not_ready.push_back(i);
                    //cout <<"not ready "<< i << endl;
                    continue;
                }
            }
        }

        output_str=output_str+"{";
        while(and_count<resource["and"]){
            if(and_ready.empty()){
                break;
            }
            done_set.push_back(and_ready.front());
            output_str = output_str +" "+and_ready.front();
            pop_front(and_ready);
            and_count++;
        }
        output_str=output_str+" }";

        output_str=output_str+"{";
        while(or_count<resource["or"]){
            if(or_ready.empty()){
                break;
            }
            done_set.push_back(or_ready.front());
            output_str = output_str +" "+or_ready.front();
            pop_front(or_ready);
            or_count++;
        }
        output_str=output_str+" }";

        output_str=output_str+"{";
        while(not_count<resource["not"]){
            if(not_ready.empty()){
                break;
            }
            done_set.push_back(not_ready.front());
            output_str = output_str +" "+not_ready.front();
            pop_front(not_ready);
            and_count++;
        }
        output_str=output_str+" }";
        
        for(auto &done_node:done_set){      
            for(auto &dest_node:dest_vec){
                vector<string>::iterator it;    //initial iterator
                it = find(in_Graph.adjList_[dest_node].begin(),in_Graph.adjList_[dest_node].end(),done_node); //找到欲刪除的位置
                if(it!=in_Graph.adjList_[dest_node].end()){
                    //找到位置就刪掉
                    it = in_Graph.adjList_[dest_node].erase(it);
                    flag = 1;   //如果有delte就設1好讓他下個迴圈可以執行
               }
            }
        }
        if(!(and_ready.empty()) || !(or_ready.empty()) || !(not_ready.empty())){
            flag = 1;
        }
        cout << level <<": " << output_str <<endl;
        level = level + 1;
    }

    cout <<"AND: " <<resource["and"] << endl;
    cout <<"OR: " <<resource["or"] << endl;
    cout <<"NOT: " <<resource["not"] << endl;
}

void MR_LCS(Graph in_Graph,vector<string> dest_vec,map<string,int> resource,
vector<vector<string>> boolean_function,map<string,int> hash_index,int input_latency){

    vector<vector<string>> ALAP_output;
    map<string,int> node_deadline;

    ALAP_output = ALAP(in_Graph,dest_vec);

    reverse(ALAP_output.begin(),ALAP_output.end());

    //紀錄每個node的deadline
    for(int i=0;i<ALAP_output.size();i++){
        for(int j=0;j<ALAP_output[i].size();j++){
            node_deadline.insert(make_pair(ALAP_output[i][j],i));
        }
    }

    vector<string> node_vec;        //放所有的node
    vector<string> no_pre_vec;      //放那些沒有pre的node
    map<string,vector<int>> node_content; //vector[0] = latency vector[1] = and vector[2] = or vector[1] = not 


    for(auto &i:in_Graph.adjList_){     
        node_vec.push_back(i.first);
        for(auto &j:i.second){
            node_vec.push_back(j);
        }
    }
    sort( node_vec.begin(), node_vec.end() );
    node_vec.erase( unique( node_vec.begin(), node_vec.end() ), node_vec.end() );


    //count dest node operation
    for(int des=0;des<dest_vec.size();des++){
        vector<string> index_forbool;           //紀錄src node 以及 booleanfunction
        string result="";                       //紀錄每一個dest的boolean function 用這個下去計算and or not

        for(auto &s:in_Graph.adjList_[dest_vec[des]]){  //將欲處理的dest的src放入indexforbool 之後可以取出使用在result
            index_forbool.push_back(s);
        }

        vector<vector<char>> temp;
        for(auto &lines:boolean_function[hash_index[dest_vec[des]]]){
            vector<char> temp2;
            for(auto &ch:lines){
                temp2.push_back(ch);
            }
            temp.push_back(temp2);
        }


        for(int i=0;i<boolean_function[hash_index[dest_vec[des]]].size();i++){
            for(int j=0;j<temp[i].size()-1;j++){
                if(temp[i][j]=='1'){
                    result = result + index_forbool[j];
                }
                else if(temp[i][j]=='-') continue;
                else if(temp[i][j]=='0'){
                    result = result + index_forbool[j] + "'";
                }            
            }
            if(i==boolean_function[hash_index[dest_vec[des]]].size()-1) continue;
            result = result + "+";
        }


        //cout << result << endl;


        //計算每個dest node所需要的and or not
        for(auto &node:node_vec){
            if(node==dest_vec[des]){
                node_content[node].resize(4);
                size_t found = result.find("+");
                if(found != string::npos){
                    node_content[node][2] = node_content[node][2] + 1;
                }
                else{
                    found = result.find("'");
                    if(found != string::npos){
                        node_content[node][3] = node_content[node][3] + 1;
                    }
                    else{
                        node_content[node][1] = node_content[node][1] + 1;
                    }
                }
                //cout << node << " " << node_content[node][1]<< " " << node_content[node][2] << " " << node_content[node][3] << endl;
            }
        }
    }


    //RUN LCS
    int level = 1;
    int and_count = 0;
    int or_count = 0;
    int not_count = 0;

    vector<string> and_ready;
    vector<string>  or_ready;
    vector<string>  not_ready;
    vector<string>  done_set;

    int flag = 1;   //0 是找不到任何點要work了


    //先把input node 處理掉
    for(auto &i:node_vec){          //traversal graph 把沒有先代的node 放到 no_pre_vec
        if(in_Graph.adjList_[i].empty()){
            vector<string>::iterator it;
            it = find(no_pre_vec.begin(),no_pre_vec.end(),i);
            if(it != no_pre_vec.end())
                continue;
            no_pre_vec.push_back(i);
        }
    }
    for(auto &i:no_pre_vec){    //將node_vector裡面的沒有先代的去掉
        vector<string>::iterator it;
        it = find(node_vec.begin(),node_vec.end(),i);
        if(it!=node_vec.end())
            it = node_vec.erase(it);
    }
    for(auto &nonpre_node:no_pre_vec){      
        for(auto &dest_node:dest_vec){
            vector<string>::iterator it;    //initial iterator
            it = find(in_Graph.adjList_[dest_node].begin(),in_Graph.adjList_[dest_node].end(),nonpre_node); //找到欲刪除的位置
            if(it!=in_Graph.adjList_[dest_node].end()){
                //找到位置就刪掉
                it = in_Graph.adjList_[dest_node].erase(it);
            }
        }
    }


    while(flag){     //flag = 0 就中斷
        flag = 0;
        no_pre_vec.clear();
        done_set.clear();
        and_count=0;
        or_count=0;
        not_count=0;
        string output_str="";
        //cout<< (input_latency-((int)ALAP_output.size()-1)) << endl;
        //if user input latency - ASAP latency <=0  break, nosolution
        if( (input_latency-((int)ALAP_output.size()-1)) <=0 ){
            cout << "No solution" << endl;
            break;
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

        for(auto &i:no_pre_vec){    //將node_vector裡面的沒有先代的去掉
            vector<string>::iterator it;
            it = find(node_vec.begin(),node_vec.end(),i);
            if(it!=node_vec.end())
                it = node_vec.erase(it);
        }

        // // 去計算哪些operation可以做

        if(!no_pre_vec.empty()){
            for(auto &i:no_pre_vec){
                if(node_content[i][1]==1){      //加進 and ready set
                   and_ready.push_back(i);
                   //cout <<"and ready "<< i << endl;
                    continue;
                }
                else if(node_content[i][2]==1){     //加進 or ready set
                    or_ready.push_back(i);
                    //cout <<"or ready "<< i << endl;
                    continue;
                }
                else if(node_content[i][3]==1){     //加進 not ready set
                    not_ready.push_back(i);
                    //cout <<"not ready "<< i << endl;
                    continue;
                }
            }
        }

        output_str=output_str+"{";


        while(and_count<resource["and"]){
            if(and_ready.empty()){
                break;
            }
            done_set.push_back(and_ready.front());
            output_str = output_str +" "+and_ready.front();
            pop_front(and_ready);
            and_count++;

            //check 下一個要進來的操作是不是快到deadline了 資源夠不夠用
            if( !(and_ready.empty()) ){
                int slack = node_deadline[and_ready.front()] - level;
                if(slack==0){
                    if(and_count>=resource["and"]){
                        resource["and"] = resource["and"] + 1;
                    }
                }
            }

        }

        output_str=output_str+" }";

        output_str=output_str+"{";
        while(or_count<resource["or"]){
            if(or_ready.empty()){
                break;
            }
            done_set.push_back(or_ready.front());
            output_str = output_str +" "+or_ready.front();
            pop_front(or_ready);
            or_count++;

            //check 下一個要進來的操作是不是快到deadline了 資源夠不夠用
            if( !(and_ready.empty()) ){
                int slack = node_deadline[and_ready.front()] - level;
                if(slack==0){
                    if(and_count>=resource["and"]){
                        resource["and"] = resource["and"] + 1;
                    }
                }
            }
        }
        output_str=output_str+" }";

        output_str=output_str+"{";
        while(not_count<resource["not"]){
            if(not_ready.empty()){
                break;
            }
            done_set.push_back(not_ready.front());
            output_str = output_str +" "+not_ready.front();
            pop_front(not_ready);
            and_count++;

            //check 下一個要進來的操作是不是快到deadline了 資源夠不夠用
            if( !(and_ready.empty()) ){
                int slack = node_deadline[and_ready.front()] - level;
                if(slack==0){
                    if(and_count>=resource["and"]){
                        resource["and"] = resource["and"] + 1;
                    }
                }
            }
        }
        output_str=output_str+" }";
        
        for(auto &done_node:done_set){      
            for(auto &dest_node:dest_vec){
                vector<string>::iterator it;    //initial iterator
                it = find(in_Graph.adjList_[dest_node].begin(),in_Graph.adjList_[dest_node].end(),done_node); //找到欲刪除的位置
                if(it!=in_Graph.adjList_[dest_node].end()){
                    //找到位置就刪掉
                    it = in_Graph.adjList_[dest_node].erase(it);
                    flag = 1;   //如果有delte就設1好讓他下個迴圈可以執行
               }
            }
        }
        if(!(and_ready.empty()) || !(or_ready.empty()) || !(not_ready.empty())){
            flag = 1;
        }
        cout << level <<": " << output_str <<endl;
        level = level + 1;
    }

    cout <<"AND: " <<resource["and"] << endl;
    cout <<"OR: " <<resource["or"] << endl;
    cout <<"NOT: " <<resource["not"] << endl;
}