/*
	An implementation to check if the given decomposition follows lossless join property.
	@author: Aashay Palliwar, palliwar.aashay@gmail.com
*/

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <set>
#include <map>

using namespace std;

//Utility function declarations
void removeSpaces(string &str);
void separateAtComma(string str, set<string> &s);
void separateIntoTwo(string str, string &a, string &b);
bool hasDash(string line);
bool has(string line, char alph);
void printSet(const set<string> &s);
void printFD(set<pair<set<string>,set<string>>> &fdset);
set<string> getClosure(set<string> X, set<string> attributes, set<pair<set<string>,set<string>>> &fdset);
void minimize(set<pair<set<string>,set<string>>> &fdset, set<string> attributes);
set<string> findKey(set<pair<set<string>,set<string>>> &fdset, set<string> attributes);
bool isSubsetOf(set<string> a, set<string> b);
set<pair<set<string>,set<string>>> getNewFD(set<string> subset, set<string> attr, set<pair<set<string>,set<string>>> fdset);
void subtractSets(set<string> a, set<string> b, set<string> &c);
void uniteSets(set<string> a, set<string> b, set<string> &c);

class Relation {
  private:
  set<string> attributes;
  set<pair<set<string>,set<string>>> fds;
  set<set<string>> decompositions;
  set<string> key;

  public:
  void printRelInfo();
  set<string> getKey();
  set<string> getAttributes();
  set<pair<set<string>,set<string>>> getFDS();
  set<set<string>> getDecompositions();
  void setDecompositions(set<set<string>> decompositions);
  Relation(set<string> attributes, set<set<string>> decompositions, set<pair<set<string>,set<string>>> fds);
};

Relation::Relation(set<string> attributes, set<set<string>> decompositions, set<pair<set<string>,set<string>>>fds) {

  //Check if decompositions are valid
  for(auto decomposition: decompositions) {
    if(!includes( attributes.begin(), attributes.end(),
      decomposition.begin(), decomposition.end())){
      cout<<"ERROR: All decompositions must be subset of the relation"<<endl;
      exit(1);
    }
  }

  //Check if fds are valid
  for(auto tuple : fds) {
    if(!includes( attributes.begin(), attributes.end(),
      tuple.first.begin(), tuple.first.end())){
      cout<<"ERROR: All functional dependencies must be defined on the relation"<<endl;
      exit(1);
    }
    if(!includes( attributes.begin(), attributes.end(),
      tuple.second.begin(), tuple.second.end())){
      cout<<"ERROR: All functional dependencies must be defined on the relation"<<endl;
      exit(1);
    }
  }

  this->attributes = attributes;
  this->decompositions = decompositions;
  minimize(fds, attributes);
  this->fds = fds;
  this->key = findKey(fds, attributes);
}

set<string> Relation::getAttributes() {
  return this->attributes;
}
  
set<pair<set<string>,set<string>>> Relation::getFDS() {
  return this->fds;
}

set<set<string>> Relation::getDecompositions() {
  return this->decompositions;
}

set<string> Relation::getKey() {
  return this->key;
}

void Relation::setDecompositions(set<set<string>> decompositions) {
  this->decompositions = decompositions;
}

void Relation::printRelInfo(){
  cout<<"---------------"<<endl;
  cout<<"Attributes:"<<endl;
  printSet(attributes);
  cout<<endl<<"Key - ";
  printSet(key);
  cout<<endl<<"FDs"<<endl;
  printFD(fds);
  cout<<"---------------"<<endl;
}

class s_matrix {
  private:
  string ** core;
  map<string, int> amap;
  map<set<string>, int> dmap;
  int rows;
  int columns;
  set<set<string>> decompositions;
  set<string> attributes;

  public:
  s_matrix(set<set<string>> decompositions, set<string> attributes);
  set<int> getRowsWithSameX(set<string> X);
  void setRowsForY(set<string> Y, set<int> row_indices, int &modified);
  bool hasAtypeRow();
  void printState();
};

void s_matrix::printState() {

  cout<<"Following decompositions are present with indices as mentioned:"<<endl;
  for(auto decomp: decompositions) {
    printSet(decomp);
    cout<<" - "<<dmap[decomp]<<endl;
  }

  cout<<"Column number for attributes are as follows-"<<endl;
  for(auto attr: attributes) {
    cout<<attr<<" - "<<amap[attr]<<endl;
  }

  cout<<"S Matrix is as follows"<<endl;
  for(int i = 0; i<rows; i++) {
    for(int j = 0; j<columns; j++){
      cout<<core[i][j]<<" ";
    }
    cout<<endl;
  }

}

bool s_matrix::hasAtypeRow(){
  
  bool hasIt = false;
  for(int i = 0; i<rows; i++) {
    for(int j = 0; j<columns; j++) {
      if(has(core[i][j], 'b')) {
        hasIt = false;
        break; 
      } else hasIt = true;
    }
    if(hasIt) break;
  }
  return hasIt;

}

void s_matrix::setRowsForY(set<string> Y, set<int> row_indices, int &modified) {

  for(auto y: Y) {
    string sym;
    for(auto row: row_indices) {
      sym = core[row][amap[y]];
      if(has(sym, 'a')) {
        break;
      } 
    }
    for(auto row: row_indices) {
      if(core[row][amap[y]] != sym) {
        modified = 1;
      }
      core[row][amap[y]] = sym;
    }
  }

}


set<int> s_matrix::getRowsWithSameX(set<string> X) {

  map<string, set<int>> m;
  for(int i = 0; i<rows; i++) {
    string content = "";
    for(auto x:X){
      content += core[i][amap[x]];
    }
    m[content].insert(i);
  }

  for(auto s: m){
    if(s.second.size()>1){
      return s.second;
    }
  }

  set<int> empty_set;
  return empty_set;

}


s_matrix::s_matrix(set<set<string>> decompositions, set<string> attributes) {
  this->decompositions = decompositions;
  this->attributes = attributes;
  rows = decompositions.size();
  columns = attributes.size();
  int i = 0;
  for(auto attr : attributes) {
    amap[attr] = i;
    i++;
  }

  i = 0;
  for(auto decomp: decompositions) {
    dmap[decomp] = i;
    i++;
  }

  core = new string*[rows];
  for(int i = 0; i<rows; i++) {
    core[i] = new string[columns];
  }

  for(auto decomp: decompositions) {
    for(auto attr: attributes){
      if(decomp.find(attr) != decomp.end()) {
        core[dmap[decomp]][amap[attr]] = "a" + to_string(amap[attr]);
      } else {
        core[dmap[decomp]][amap[attr]] = "b" + to_string(dmap[decomp]) + to_string(amap[attr]);
      }
    }
  }
}




int main(int argc, char **argv) {

  string fileName = argv[1];
  string line;
  ifstream file;
  file.open(argv[1]);

  set<string> attributes;
  set<set<string>> decompositions;
  set<pair<set<string>,set<string>>> fds;

  //Taking input from testcase file
  if (file.is_open()) {

    getline(file, line);
    transform(line.begin(), line.end(), line.begin(), ::toupper);
    removeSpaces(line);
    separateAtComma(line, attributes);

    while (getline(file, line)) {
      
      removeSpaces(line);
      transform(line.begin(), line.end(), line.begin(), ::toupper);

      if(hasDash(line)) {
        string a, b;
        separateIntoTwo(line, a, b);
        set<string> x, y;
        separateAtComma(a, x);
        separateAtComma(b, y);
        fds.insert(make_pair(x,y));
      } else {
        set<string> decomp;
        separateAtComma(line, decomp);
        decompositions.insert(decomp);
      }
    }
    file.close();
  } else {
    cout<<"File failed to open"<<endl;
  }

  Relation * r = new Relation(attributes, decompositions, fds);
  r->printRelInfo();

  s_matrix * s = new s_matrix(r->getDecompositions(), r->getAttributes());

  int modified = 1;
  while(modified != 0) {
    modified = 0;
    for(auto dep: r->getFDS()) {
      set<int> rowsToChange = s->getRowsWithSameX(dep.first);
      if(rowsToChange.size()) {
        s->setRowsForY(dep.second, rowsToChange, modified);
      }
    }
  }

  if(s->hasAtypeRow()) {
    cout<<"SATISFIES LJ"<<endl;
  } else {
    cout<<"FAILS LJ"<<endl;
  }

  cout<<"---------------"<<endl;
  s->printState();

  return 0;
}

//Utility function definitions
void removeSpaces(string &str) { 
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
}

void separateAtComma(string str, set<string> &s) {
  stringstream ss(str); 
  while (ss.good()) {
      string substr;
      getline(ss, substr, ',');
      s.insert(substr);
  }
}

void separateIntoTwo(string str, string &a, string &b) {

  str.erase(remove(str.begin(), str.end(), '>'), str.end());
  stringstream ss(str);
  string temp[2]; 
  int i = 0;
  while (ss.good() && i<2) {
      getline(ss, temp[i], '-');
      i++;
  }
  a = temp[0];
  b = temp[1];
}

bool hasDash(string line) {
  return line.find('-') != string::npos;
}

bool has(string line, char alph) {
  return line.find(alph) != string::npos;
}


void printSet(const set<string> &s) {
  set<string>::iterator itr2;
  for(itr2 = s.begin(); itr2!= s.end(); itr2++){
    cout<<*itr2<<" ";
  }
}

void printFD(set<pair<set<string>,set<string>>> &fdset) {
  for(auto tuple: fdset) {
    for(auto attr : tuple.first) {
      cout<<attr<<" ";
    }
    cout<<"-> ";
    for(auto attr : tuple.second) {
      cout<<attr<<" ";
    }
    cout<<endl;
  }
}

set<string> getClosure(set<string> X, set<string> attributes, set<pair<set<string>,set<string>>> &fdset){
  if(!includes( attributes.begin(), attributes.end(),
      X.begin(), X.end())) {
    cout<<"CLOSURE ERROR: The attribute doesn't exist"<<endl;
    exit(1);
  }

  set<string> Xp = X;
  set<string> oldX;
  while(oldX != Xp) {
    oldX = Xp;
    for(auto dep: fdset) {
      if(includes( Xp.begin(), Xp.end(),
      dep.first.begin(), dep.first.end())) {
        for(auto element: dep.second) {
          Xp.insert(element);
        }
      }
    }
  }

  return Xp;
}


void minimize(set<pair<set<string>,set<string>>> &fdset, set<string> attributes) {

  //Making RHS of FD a single attribute  
  set<pair<set<string>,set<string>>> unfurled_set;
  for(auto tuple : fdset){
    for(auto element: tuple.second){
      set<string> temp = {element};
      unfurled_set.insert(make_pair(tuple.first, temp));
    }
  }
  fdset = unfurled_set;

  //Remove extraneous attributes
  int flag = 1;
  while(flag != 0) {
    flag = 0;
    set<pair<set<string>,set<string>>> temp_fdset = fdset;

    for(auto dep: fdset) {
      if(dep.first.size() > 1) {
        for(auto attr: dep.first) {
          set<string> part1 = {attr};
          set<string> part2 = dep.first;
          part2.erase(attr);
          set<string> closure = getClosure(part2, attributes, fdset);
          if(closure.find(attr) != closure.end()) {
            temp_fdset.erase(dep);
            temp_fdset.insert(make_pair(part2, dep.second));
            flag = 1;
            break;
          }          
        }
      }
      if(flag == 1) break;
    }
    fdset = temp_fdset;
  }

  //Remove dependencies achievable by transitivity
  flag = 1;
  while(flag != 0) {
    flag = 0;
    set<pair<set<string>,set<string>>> temp_fdset = fdset;
    for(auto dep: fdset) {
     set<pair<set<string>,set<string>>> temp = fdset;
     temp.erase(dep);
     set<string> closure = getClosure(dep.first, attributes, temp);
     if(includes( closure.begin(), closure.end(),
      dep.second.begin(), dep.second.end())) {
        temp_fdset.erase(dep);
        flag = 1;
      }
    }
    fdset = temp_fdset;
  }

}

set<string> findKey(set<pair<set<string>,set<string>>> &fdset, set<string> attributes) {

  set<string> key = attributes;
  int flag = 1;
  while(flag != 0) {
    flag = 0;
    for(auto attr: key) {
      set<string> temp = key;
      temp.erase(attr);
      set<string> closure = getClosure(temp, attributes, fdset);
      if(closure == attributes) {
        key.erase(attr);
        flag = 1;
        break;
      }
    } 
  }
  return key;
}

bool isSubsetOf(set<string> a, set<string> b) {
  return includes(b.begin(), b.end(), a.begin(), a.end());
}


set<pair<set<string>,set<string>>> getNewFD(set<string> subset_attr, set<string> attr, set<pair<set<string>,set<string>>> fdset) {

  if(!includes( attr.begin(), attr.end(),
      subset_attr.begin(), subset_attr.end())) {
    cout<<"ERROR: Invalid decomposition"<<endl;
    exit(1);
  }

  set<pair<set<string>,set<string>>> newFD;

  for(auto dep: fdset) {
    if(isSubsetOf(dep.first, subset_attr) && isSubsetOf(dep.second, subset_attr)) {
      newFD.insert(dep);
    }
  }

  return newFD;
}

void subtractSets(set<string> a, set<string> b, set<string> &c) {
  for(auto elem: b){
    if(a.find(elem) != a.end()){
      a.erase(elem);
    }
  }
  c = a;
}

void uniteSets(set<string> a, set<string> b, set<string> &c) {
  c = a;
  for(auto elem: b){
    c.insert(elem);
  }
}