/*
	An implementation to output the Boyce–Codd normal form decomposition, of a given relation
  with a set of functional dependencies, which abides the lossless join property  
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

  set<pair<set<string>,set<string>>>  fdset = r->getFDS();

  //Step 1
  set<set<string>> decomps;
  decomps.insert(r->getAttributes());

  //Step 2
  int flag = 1;
  while(flag != 0) {
    flag = 0;
    set<set<string>> temp = decomps;
    for(auto decomp: decomps) {
      set<pair<set<string>,set<string>>> newFD = getNewFD(decomp, attributes, fdset);
      for(auto dep: newFD) {
        set<string> closure = getClosure(dep.first, decomp, newFD);
        if(closure != decomp) {
          set<string> sub_decomp1;
          set<string> sub_decomp2;
          subtractSets(decomp, dep.second, sub_decomp1);
          uniteSets(dep.first, dep.second, sub_decomp2);
          temp.erase(decomp);
          temp.insert(sub_decomp1);
          temp.insert(sub_decomp2);
          flag = 1;
          break;
        }
      }
      if(flag == 1) break;
    }
    decomps = temp;
  }

  r->setDecompositions(decomps);
  cout<<"BCNF LJ Decomposition - "<<endl;
  for(auto decomp: decomps) {
    printSet(decomp);
    cout<<endl;
  }

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